#include "xfs.h"
#include "utils.h"
#include <alloca.h>
#include <endian.h>
#include <features.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

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

static fm_xfs_err_t fm_xfs_iter_extents_(fm_xfs_t *fm, xfs_dinode_core_t *core,
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
      if (!(*callback)(self, data))
        return FM_XFS_ERR_NONE;
    }
  }
  return FM_XFS_ERR_NONE;
}

static fm_xfs_err_t fm_xfs_iter_btree_(fm_xfs_t *fm, xfs_dinode_core_t *core,
                                       void *dfork, void *self,
                                       callback_t callback) {
  return FM_XFS_ERR_NOT_SUPPORTED;
}

typedef fm_xfs_err_t (*iterfun_t)(fm_xfs_t *fm, xfs_dinode_core_t *core,
                                  void *dfork, void *self, callback_t callback);

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

typedef struct fm_xfs_dir_iter_block_self {
  fm_xfs_t *fm;
  void *entry_self;
  callback_t entry_callback;
  fm_xfs_err_t err;
} fm_xfs_dir_iter_block_self_t;

static int fm_xfs_dir_iter_block_(void *self_, void *data) {
  fm_xfs_dir_iter_block_self_t *self = self_;
  struct xfs_dir3_data_hdr *dir3_header = data;
  if (be32toh(dir3_header->hdr.magic) != XFS_DIR3_BLOCK_MAGIC &&
      be32toh(dir3_header->hdr.magic) != XFS_DIR3_DATA_MAGIC) {
    self->err = FM_XFS_ERR_MAGIC;
    return 0;
  }

  fm_xfs_dir_entry_t entry;
  xfs_dir2_data_union_t *iter = dir3_header->entries;
  while ((char *)iter + sizeof(xfs_dir2_data_union_t) <=
             (char *)data + be32toh(self->fm->sb.sb_blocksize) &&
         iter->unused.freetag != XFS_DIR2_DATA_UNUSED_FREETAG) {
    entry.namelen = iter->entry.namelen;
    memcpy(entry.name, iter->entry.name, entry.namelen);
    entry.name[iter->entry.namelen] = 0;
    entry.inumber = be64toh(iter->entry.inumber);
    entry.ftype = iter->entry.name[entry.namelen];

    if (!(*self->entry_callback)(self->entry_self, &entry)) {
      self->err = FM_XFS_ERR_NONE;
      return 0;
    }

    size_t offset = sizeof(xfs_dir2_data_entry_t) - sizeof(char) +
                    iter->entry.namelen * sizeof(char);
    offset +=
        sizeof(ino_t) - ((offset + sizeof(ino_t) - 1) % sizeof(ino_t) + 1);
    iter = (xfs_dir2_data_union_t *)((char *)iter + offset);
  }
  return 1;
}

static fm_xfs_err_t fm_xfs_dir_iter_extents_(fm_xfs_t *fm,
                                             xfs_dinode_core_t *core,
                                             void *dfork, void *self,
                                             callback_t callback) {
  fm_xfs_dir_iter_block_self_t block_self;
  block_self.fm = fm;
  block_self.entry_self = self;
  block_self.entry_callback = callback;
  block_self.err = FM_XFS_ERR_NONE;

  FM_XFS_CHKTHROW(fm_xfs_iter_extents_(fm, core, dfork, &block_self,
                                       fm_xfs_dir_iter_block_));
  return block_self.err;
}

static fm_xfs_err_t fm_xfs_dir_iter_btree_(fm_xfs_t *fm,
                                           xfs_dinode_core_t *core, void *dfork,
                                           void *self, callback_t callback) {
  fm_xfs_dir_iter_block_self_t block_self;
  block_self.fm = fm;
  block_self.entry_self = self;
  block_self.entry_callback = callback;
  block_self.err = FM_XFS_ERR_NONE;

  FM_XFS_CHKTHROW(
      fm_xfs_iter_btree_(fm, core, dfork, &block_self, fm_xfs_dir_iter_block_));
  return block_self.err;
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

  iterfun_t iterfun = NULL;
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

fm_xfs_err_t fm_xfs_find_entry_(fm_xfs_t *fm, fm_xfs_dir_entry_t *expected) {
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
  FM_XFS_CHKTHROW(fm_xfs_find_entry_(fm, &dir));
  if (dir.ftype != XFS_DIR3_FT_DIR)
    return FM_XFS_ERR_NOT_A_DIRECTORY;
  fm->ino_current_dir = dir.inumber;
  return FM_XFS_ERR_NONE;
}

typedef struct fm_xfs_cp_file_block_self {
  __uint32_t blocksize;
  FILE *stream;
  fm_xfs_err_t err;
} fm_xfs_cp_file_block_self_t;

static int fm_xfs_cp_file_block_(void *self_, void *data) {
  fm_xfs_cp_file_block_self_t *self = self_;
  if (fwrite(data, self->blocksize, 1, self->stream) != 1) {
    self->err = FM_XFS_ERR_OUT_DEVICE;
    return 0;
  }
  self->err = FM_XFS_ERR_NONE;
  return 1;
}

static fm_xfs_err_t fm_xfs_cp_file_(fm_xfs_t *fm, fm_xfs_dir_entry_t *what) {
  __uint16_t inodesize = be16toh(fm->sb.sb_inodesize);
  void *inode_info = alloca(inodesize);
  fseek(fm->f, inodesize * what->inumber, SEEK_SET);
  if (fread(inode_info, inodesize, 1, fm->f) != 1)
    return FM_XFS_ERR_DEVICE;
  xfs_dinode_core_t *di = inode_info;
  if (be16toh(di->di_magic) != XFS_DINODE_MAGIC)
    return FM_XFS_ERR_MAGIC;
  void *dfork = (void *)((char *)inode_info + xfs_dinode_size(di));

  iterfun_t iterfun = NULL;
  switch (di->di_format) {
  case XFS_DINODE_FMT_EXTENTS:
    iterfun = fm_xfs_iter_extents_;
    break;
  case XFS_DINODE_FMT_BTREE:
    iterfun = fm_xfs_iter_btree_;
    break;
  }

  if (!iterfun)
    return FM_XFS_ERR_FORMAT;

  FILE *f = fopen(what->name, "wb");
  if (!f)
    return FM_XFS_ERR_OUT_DEVICE;
  fm_xfs_cp_file_block_self_t self;
  self.blocksize = be32toh(fm->sb.sb_blocksize);
  self.stream = f;
  self.err = FM_XFS_ERR_NONE;
  fm_xfs_err_t iter_err = iterfun(fm, di, dfork, &self, fm_xfs_cp_file_block_);
  fclose(f);
  FM_XFS_CHKTHROW(iter_err);
  return self.err;
}

static int is_self_or_parent_(char const *name) {
  return strcmp(".", name) == 0 || strcmp("..", name) == 0;
}

static fm_xfs_err_t fm_xfs_cp_entry_(fm_xfs_t *fm, fm_xfs_dir_entry_t *what);

typedef struct fm_xfs_cp_dir_entry_callback_self {
  fm_xfs_t *fm;
  fm_xfs_err_t err;
} fm_xfs_cp_dir_entry_callback_self_t;

static int fm_xfs_cp_dir_entry_callback_(void *self_, void *data) {
  fm_xfs_cp_dir_entry_callback_self_t *self = self_;
  fm_xfs_dir_entry_t *entry = data;
  if (is_self_or_parent_(entry->name))
    return 1;
  self->err = fm_xfs_cp_entry_(self->fm, entry);
  return self->err == FM_XFS_ERR_NONE;
}

static fm_xfs_err_t fm_xfs_cp_dir_(fm_xfs_t *fm, fm_xfs_dir_entry_t *what) {
  char *old_path = getcwd(NULL, 0);
  if (!is_self_or_parent_(what->name)) {
    if (mkdir(what->name, 0777) || chdir(what->name)) {
      free(old_path);
      return FM_XFS_ERR_OUT_DEVICE;
    }
  }
  xfs_ino_t old = fm->ino_current_dir;
  fm->ino_current_dir = what->inumber;

  fm_xfs_cp_dir_entry_callback_self_t self;
  self.fm = fm;
  self.err = FM_XFS_ERR_NONE;
  fm_xfs_dir_iter_(fm, &self, fm_xfs_cp_dir_entry_callback_);

  fm->ino_current_dir = old;
  if (!chdir(old_path)) {
    free(old_path);
    return FM_XFS_ERR_OUT_DEVICE;
  }
  free(old_path);
  return self.err;
}

static fm_xfs_err_t fm_xfs_cp_entry_(fm_xfs_t *fm, fm_xfs_dir_entry_t *what) {
  switch (what->ftype) {
  case XFS_DIR3_FT_REG_FILE:
    fm_xfs_cp_file_(fm, what);
    break;
  case XFS_DIR3_FT_DIR:
    fm_xfs_cp_dir_(fm, what);
    break;
  default:
    return FM_XFS_ERR_NOT_SUPPORTED;
  }
  return FM_XFS_ERR_NONE;
}

fm_xfs_err_t fm_xfs_cp(fm_xfs_t *fm, char const *from, char const *to) {
  char *old_path = getcwd(NULL, 0);
  if (!is_self_or_parent_(to)) {
    if (mkdir(to, 0777) || chdir(to)) {
      free(old_path);
      return FM_XFS_ERR_OUT_DEVICE;
    }
  }

  fm_xfs_dir_entry_t entry;
  entry.namelen = strlen(from);
  strcpy(entry.name, from);
  fm_xfs_find_entry_(fm, &entry);
  fm_xfs_cp_entry_(fm, &entry);

  if (chdir(old_path)) {
    free(old_path);
    return FM_XFS_ERR_OUT_DEVICE;
  }
  free(old_path);
  return FM_XFS_ERR_NONE;
}
