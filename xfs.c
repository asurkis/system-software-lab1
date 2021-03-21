#include "xfs.h"
#include "utils.h"
#include <alloca.h>
#include <endian.h>
#include <features.h>
#include <stddef.h>
#include <string.h>
#include <sys/stat.h>

fm_xfs_err_t fm_xfs_init(fm_xfs_t *fm, char const *device_path) {
  fm->f = fopen(device_path, "r");
  if (!fm->f)
    return FM_XFS_ERR_DEVICE_NOT_FOUND;
  if (fread(&fm->sb, sizeof(xfs_sb_t), 1, fm->f) != 1) {
    fclose(fm->f);
    return FM_XFS_ERR_DEVICE;
  }
  if (be32toh(fm->sb.sb_magicnum) != XFS_SB_MAGIC)
    return FM_XFS_ERR_MAGIC;
  fm->ino_current_dir = be64toh(fm->sb.sb_rootino);
  return FM_XFS_ERR_NONE;
}

fm_xfs_err_t fm_xfs_free(fm_xfs_t *fm) {
  if (fclose(fm->f))
    return FM_XFS_ERR_DEVICE;
  return FM_XFS_ERR_NONE;
}

static xfs_ino_t inode_number_(xfs_dir2_inou_t u, int is64) {
  if (is64) {
    return be64toh(*(__uint64_t *)u.i8.i);
  } else {
    return be32toh(*(__uint32_t *)u.i4.i);
  }
}

typedef fm_xfs_err_t (*dir_iterfun_t)(fm_xfs_t *fm, xfs_dinode_core_t *core,
                                      void *dfork, void *self,
                                      callback_t callback);

typedef struct fm_xfs_dir_entry {
  ino_t inumber;
  char name[FILENAME_BUFLEN];
  __uint8_t namelen;
  __uint8_t ftype;
} fm_xfs_dir_entry_t;

static fm_xfs_err_t fm_xfs_dir_iter_local_(fm_xfs_t *fm,
                                           xfs_dinode_core_t *core, void *dfork,
                                           void *self, callback_t callback) {
  xfs_dir2_sf_t *sf = dfork;
  xfs_ino_t parent = inode_number_(sf->hdr.parent, sf->hdr.i8count);
  fm_xfs_dir_entry_t entry;

  entry.namelen = 1;
  entry.name[0] = '.';
  entry.name[1] = '\0';
  entry.inumber = fm->ino_current_dir;
  entry.ftype = XFS_DIR3_FT_DIR;
  if (!(*callback)(self, &entry))
    return FM_XFS_ERR_NONE;

  entry.namelen = 2;
  entry.name[0] = '.';
  entry.name[1] = '.';
  entry.name[2] = '\0';
  entry.inumber = parent;
  entry.ftype = XFS_DIR3_FT_DIR;
  if (!(*callback)(self, &entry))
    return FM_XFS_ERR_NONE;

  xfs_dir2_sf_entry_t *iter =
      (xfs_dir2_sf_entry_t *)((char *)&sf->hdr.parent +
                              (sf->hdr.count ? sizeof(sf->hdr.parent.i4)
                                             : sizeof(sf->hdr.parent.i8)));

  for (int i = 0; i < sf->hdr.count || i < sf->hdr.i8count; ++i) {
    entry.namelen = iter->namelen;
    memcpy(entry.name, iter->name, entry.namelen);
    entry.name[entry.namelen] = 0;

    xfs_dir2_sf_entry_footer_t *footer =
        (xfs_dir2_sf_entry_footer_t *)((char *)iter +
                                       offsetof(xfs_dir2_sf_entry_t, name) +
                                       iter->namelen);

    entry.ftype = footer->ftype;
    entry.inumber = inode_number_(footer->inumber, 0);
    if (!(*callback)(self, &entry))
      return 0;

    iter =
        (xfs_dir2_sf_entry_t *)((char *)footer +
                                offsetof(xfs_dir2_sf_entry_footer_t, inumber) +
                                (sf->hdr.count ? sizeof(xfs_dir2_ino4_t)
                                               : sizeof(xfs_dir2_ino8_t)));
  }
  return FM_XFS_ERR_NONE;
}

static fm_xfs_err_t fm_xfs_dir_iter_block_(fm_xfs_t *fm, int *is_running,
                                           void *data, void *self,
                                           callback_t callback) {
  struct xfs_dir3_data_hdr *dir3_header = data;
  if (be32toh(dir3_header->hdr.magic) != XFS_DIR3_DATA_MAGIC)
    return FM_XFS_ERR_MAGIC;

  fm_xfs_dir_entry_t entry;

  xfs_dir2_data_union_t *iter = dir3_header->entries;
  while ((char *)iter + sizeof(xfs_dir2_data_union_t) <=
             (char *)data + be32toh(fm->sb.sb_blocksize) &&
         iter->unused.freetag != XFS_DIR2_DATA_UNUSED_FREETAG) {
    entry.namelen = iter->entry.namelen;
    memcpy(entry.name, iter->entry.name, entry.namelen);
    entry.name[iter->entry.namelen] = 0;
    entry.inumber = be64toh(iter->entry.inumber);
    entry.ftype = iter->entry.name[entry.namelen];

    if (!(*callback)(self, &entry)) {
      *is_running = 0;
      return FM_XFS_ERR_NONE;
    }

    size_t offset = sizeof(xfs_dir2_data_entry_t) - sizeof(char) +
                    iter->entry.namelen * sizeof(char);
    offset +=
        sizeof(ino_t) - ((offset + sizeof(ino_t) - 1) % sizeof(ino_t) + 1);
    iter = (xfs_dir2_data_union_t *)((char *)iter + offset);
  }
  return FM_XFS_ERR_NONE;
}

static fm_xfs_err_t fm_xfs_dir_iter_extents_(fm_xfs_t *fm,
                                             xfs_dinode_core_t *core,
                                             void *dfork, void *self,
                                             callback_t callback) {
  struct xfs_bmbt_rec *list = dfork;
  struct xfs_bmbt_irec unwrapped;
  void *data = alloca(be32toh(fm->sb.sb_blocksize));

  int is_running = 1;
  xfs_filblks_t read_blocks = 0;
  for (xfs_extnum_t i = 0; is_running && i < be32toh(core->di_nextents); ++i) {
    xfs_bmbt_disk_get_all(&list[i], &unwrapped);
    fseek(fm->f, unwrapped.br_startblock * be32toh(fm->sb.sb_blocksize),
          SEEK_SET);
    for (int j = 0; is_running && unwrapped.br_startoff <= read_blocks &&
                    j < unwrapped.br_blockcount;
         ++j) {
      if (fread(data, be32toh(fm->sb.sb_blocksize), 1, fm->f) != 1)
        return FM_XFS_ERR_DEVICE;
      ++read_blocks;
      FM_XFS_CHKTHROW(
          fm_xfs_dir_iter_block_(fm, &is_running, data, self, callback));
    }
  }
  return FM_XFS_ERR_NONE;
}

static fm_xfs_err_t fm_xfs_dir_iter_btree_(fm_xfs_t *fm,
                                           xfs_dinode_core_t *core, void *dfork,
                                           void *self, callback_t callback) {
  return FM_XFS_ERR_NOT_SUPPORTED;
}

fm_xfs_err_t fm_xfs_dir_iter_(fm_xfs_t *fm, void *self, callback_t callback) {
  __uint16_t inodesize = be16toh(fm->sb.sb_inodesize);
  void *inode_info = alloca(inodesize);
  fseek(fm->f, inodesize * fm->ino_current_dir, SEEK_SET);
  if (fread(inode_info, inodesize, 1, fm->f) != 1)
    return FM_XFS_ERR_DEVICE;
  xfs_dinode_core_t *di = inode_info;
  if (be16toh(di->di_magic) != XFS_DINODE_MAGIC)
    return FM_XFS_ERR_MAGIC;
  void *dfork = (void *)((char *)inode_info + xfs_dinode_size(di));

  dir_iterfun_t iterfun = NULL;
  switch (di->di_format) {
  case XFS_DINODE_FMT_LOCAL:
    iterfun = fm_xfs_dir_iter_local_;
    break;
  case XFS_DINODE_FMT_EXTENTS:
    iterfun = fm_xfs_dir_iter_extents_;
    break;
  case XFS_DINODE_FMT_BTREE:
    iterfun = fm_xfs_dir_iter_btree_;
    break;
  }

  if (!iterfun)
    return FM_XFS_ERR_FORMAT;

  return iterfun(fm, di, dfork, self, callback);
}

static int fm_xfs_dir_entry_ls_callback_(void *self, void *data) {
  fm_xfs_dir_entry_t *entry = data;
  printf("%s => %d\n", entry->name, entry->inumber);
  return 1;
}

fm_xfs_err_t fm_xfs_ls(fm_xfs_t *fm) {
  return fm_xfs_dir_iter_(fm, NULL, fm_xfs_dir_entry_ls_callback_);
}

typedef struct fm_xfs_dir_entry_find_callback_self {
  fm_xfs_dir_entry_t *expected;
  fm_xfs_err_t err;
} fm_xfs_dir_entry_find_callback_self_t;

static int fm_xfs_dir_entry_find_callback_(void *self_, void *data) {
  fm_xfs_dir_entry_find_callback_self_t *self = self_;
  fm_xfs_dir_entry_t *entry = data;
  if (entry->namelen == self->expected->namelen &&
      memcmp(entry->name, self->expected->name, entry->namelen) == 0) {
    self->expected->ftype = entry->ftype;
    self->expected->inumber = entry->inumber;
    return 0;
  }
  return 1;
}

fm_xfs_err_t fm_xfs_find_entry(fm_xfs_t *fm, fm_xfs_dir_entry_t *expected) {
  fm_xfs_dir_entry_find_callback_self_t self;
  self.expected = expected;
  self.err = FM_XFS_ERR_NONE;
  FM_XFS_CHKTHROW(fm_xfs_dir_iter_(fm, &self, fm_xfs_dir_entry_find_callback_));
  return self.err;
}

fm_xfs_err_t fm_xfs_cd(fm_xfs_t *fm, char const *dirname, size_t dirnamelen) {
  if (dirnamelen == 1 && dirname[0] == '/') {
    fm->ino_current_dir = be64toh(fm->sb.sb_rootino);
    return FM_XFS_ERR_NONE;
  }
  fm_xfs_dir_entry_t dir;
  memcpy(dir.name, dirname, dirnamelen);
  dir.namelen = dirnamelen;
  FM_XFS_CHKTHROW(fm_xfs_find_entry(fm, &dir));
  if (dir.ftype != XFS_DIR3_FT_DIR)
    return FM_XFS_ERR_NOT_A_DIRECTORY;
  fm->ino_current_dir = dir.inumber;
  return FM_XFS_ERR_NONE;
}

static fm_xfs_err_t
fm_xfs_print_file_extents_(fm_xfs_t *fm, xfs_dinode_core_t *core, void *dfork) {
  struct xfs_bmbt_rec *list = dfork;
  struct xfs_bmbt_irec unwrapped;
  void *data = alloca(be32toh(fm->sb.sb_blocksize));
  printf("File size: %d\n", be64toh(core->di_size));
  for (xfs_extnum_t i = 0; i < be32toh(core->di_nextents); ++i) {
    xfs_bmbt_disk_get_all(&list[i], &unwrapped);
    printf("%d %d %d %d\n", unwrapped.br_startoff, unwrapped.br_startblock,
           unwrapped.br_blockcount, unwrapped.br_state);
    size_t s = be32toh(fm->sb.sb_blocksize) - unwrapped.br_startoff;
    printf("Byte offset: %d\n",
           unwrapped.br_startblock * be32toh(fm->sb.sb_blocksize) +
               unwrapped.br_startoff);
    fseek(fm->f,
          unwrapped.br_startblock * be32toh(fm->sb.sb_blocksize) +
              unwrapped.br_startoff,
          SEEK_SET);
    for (int j = 0; j < unwrapped.br_blockcount; ++j) {
      printf("Reading %d bytes\n", s);
      if (fread(data, s, 1, fm->f) != 1)
        return FM_XFS_ERR_DEVICE;
      fwrite(data, s, 1, stderr);
      fflush(stderr);
      s = be32toh(fm->sb.sb_blocksize);
    }
  }
  return FM_XFS_ERR_NONE;
}

static fm_xfs_err_t
fm_xfs_print_file_btree_(fm_xfs_t *fm, xfs_dinode_core_t *core, void *dfork) {
  return FM_XFS_ERR_NONE;
}

static fm_xfs_err_t fm_xfs_print_file_(fm_xfs_t *fm, ino_t file) {
  __uint16_t inodesize = be16toh(fm->sb.sb_inodesize);
  void *inode_info = alloca(inodesize);
  fseek(fm->f, inodesize * file, SEEK_SET);
  if (fread(inode_info, inodesize, 1, fm->f) != 1)
    return FM_XFS_ERR_DEVICE;
  xfs_dinode_core_t *di = inode_info;
  void *dfork = (void *)((char *)inode_info + xfs_dinode_size(di));
  switch (di->di_format) {
  case XFS_DINODE_FMT_EXTENTS:
    return fm_xfs_print_file_extents_(fm, di, dfork);
  case XFS_DINODE_FMT_BTREE:
    return fm_xfs_print_file_btree_(fm, di, dfork);
  }
}

fm_xfs_err_t fm_xfs_cp(fm_xfs_t *fm, char const *from, char const *to) {}

#define CHKERR(expr)                                                           \
  do {                                                                         \
    FM_XFS_CHKTHROW(expr);                                                     \
    printf("==== %s: %d ====\n", __FILE__, __LINE__);                          \
  } while (0);

fm_xfs_err_t fm_xfs_sample(fm_xfs_t *fm) {
  __uint8_t ftype;
  ino_t ino;

  CHKERR(FM_XFS_ERR_NONE);
  // CHKERR(fm_xfs_ls(fm));
  // CHKERR(fm_xfs_find_entry(fm, &ftype, &ino, "file2.txt", 9));
  // CHKERR(fm_xfs_print_file_(fm, ino));
  // CHKERR(fm_xfs_cd(fm, "dir1", 4));
  // CHKERR(fm_xfs_ls(fm));
  // CHKERR(fm_xfs_cd(fm, "..", 2));
  // CHKERR(fm_xfs_ls(fm));
  CHKERR(fm_xfs_cd(fm, "dir2", 4));
  CHKERR(fm_xfs_ls(fm));
  return FM_XFS_ERR_NONE;
}
