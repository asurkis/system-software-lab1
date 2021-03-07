#include "xfs.h"
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
  fm->ino_current_dir = be64toh(fm->sb.sb_rootino);
  return FM_XFS_ERR_NONE;
}

fm_xfs_err_t fm_xfs_free(fm_xfs_t *fm) {
  if (fclose(fm->f))
    return FM_XFS_ERR_DEVICE;
  return FM_XFS_ERR_NONE;
}

static xfs_ino_t inode_number(xfs_dir2_inou_t u, int is64) {
  if (is64) {
    return be64toh(*(__uint64_t *)u.i8.i);
  } else {
    return be32toh(*(__uint32_t *)u.i4.i);
  }
}

static void fm_xfs_ls_local_(fm_xfs_t *fm, void *dfork) {
  xfs_dir2_sf_t *sf = dfork;
  xfs_ino_t parent = inode_number(sf->hdr.parent, sf->hdr.i8count);
  printf(".. => %d\n", parent);
  // printf("%d/%d children/64 bit:\n", sf->hdr.count, sf->hdr.i8count);
  xfs_dir2_sf_entry_t *iter =
      (xfs_dir2_sf_entry_t *)((char *)&sf->hdr.parent +
                              (sf->hdr.count ? sizeof(sf->hdr.parent.i4)
                                             : sizeof(sf->hdr.parent.i8)));

  char buf[FILENAME_BUFLEN];
  for (int i = 0; i < sf->hdr.count || i < sf->hdr.i8count; ++i) {
    for (int j = 0; j < iter->namelen; ++j)
      buf[j] = iter->name[j];
    buf[iter->namelen] = 0;

    xfs_dir2_sf_entry_footer_t *footer =
        (xfs_dir2_sf_entry_footer_t *)((char *)iter +
                                       offsetof(xfs_dir2_sf_entry_t, name) +
                                       iter->namelen);
    printf("%s => %d\n", buf, inode_number(footer->inumber, 0));
    iter =
        (xfs_dir2_sf_entry_t *)((char *)footer +
                                offsetof(xfs_dir2_sf_entry_footer_t, inumber) +
                                (sf->hdr.count ? sizeof(xfs_dir2_ino4_t)
                                               : sizeof(xfs_dir2_ino8_t)));
  }
}

static void fm_xfs_ls_extents_(fm_xfs_t *fm, void *dfork) {}

static void fm_xfs_ls_btree_(fm_xfs_t *fm, void *dfork) {}

fm_xfs_err_t fm_xfs_ls(fm_xfs_t *fm) {
  __uint16_t inodesize = be16toh(fm->sb.sb_inodesize);
  void *inode_info = alloca(inodesize);
  fseek(fm->f, inodesize * fm->ino_current_dir, SEEK_SET);
  if (fread(inode_info, inodesize, 1, fm->f) != 1)
    return FM_XFS_ERR_DEVICE;
  xfs_dinode_core_t *di = inode_info;
  void *dfork = (void *)((char *)inode_info + xfs_dinode_size(di));
  printf("di_format = %d\n", di->di_format);
  switch (di->di_format) {
  case XFS_DINODE_FMT_LOCAL:
    fm_xfs_ls_local_(fm, dfork);
    break;
  case XFS_DINODE_FMT_EXTENTS:
    fm_xfs_ls_extents_(fm, dfork);
    break;
  case XFS_DINODE_FMT_BTREE:
    fm_xfs_ls_btree_(fm, dfork);
    break;
  }
}

static fm_xfs_err_t fm_xfs_find_entry_local_(fm_xfs_t *fm, __uint8_t *ftype_out,
                                             ino_t *ino_out, void *dfork,
                                             char const *dirname,
                                             size_t dirnamelen) {
  xfs_dir2_sf_t *sf = dfork;
  xfs_ino_t parent = inode_number(sf->hdr.parent, sf->hdr.i8count);
  if (dirnamelen == 2 && dirname[0] == '.' && dirname[1] == '.') {
    *ftype_out = XFS_DIR3_FT_DIR;
    *ino_out = parent;
    return FM_XFS_ERR_NONE;
  }

  xfs_dir2_sf_entry_t *iter =
      (xfs_dir2_sf_entry_t *)((char *)&sf->hdr.parent +
                              (sf->hdr.count ? sizeof(sf->hdr.parent.i4)
                                             : sizeof(sf->hdr.parent.i8)));

  for (int i = 0; i < sf->hdr.count || i < sf->hdr.i8count; ++i) {
    xfs_dir2_sf_entry_footer_t *footer =
        (xfs_dir2_sf_entry_footer_t *)((char *)iter +
                                       offsetof(xfs_dir2_sf_entry_t, name) +
                                       iter->namelen);

    if (dirnamelen == iter->namelen) {
      int j = 0;
      while (j < dirnamelen && iter->name[j] == dirname[j])
        ++j;
      if (j == dirnamelen) {
        *ftype_out = footer->ftype;
        *ino_out = inode_number(footer->inumber, sf->hdr.i8count);
        return FM_XFS_ERR_NONE;
      }
    }

    iter =
        (xfs_dir2_sf_entry_t *)((char *)footer +
                                offsetof(xfs_dir2_sf_entry_footer_t, inumber) +
                                (sf->hdr.count ? sizeof(xfs_dir2_ino4_t)
                                               : sizeof(xfs_dir2_ino8_t)));
  }
  return FM_XFS_ERR_DIRECTORY_NOT_FOUND;
}

static fm_xfs_err_t fm_xfs_find_entry_extents_(fm_xfs_t *fm,
                                               __uint8_t *ftype_out,
                                               ino_t *ino_out, void *dfork,
                                               char const *dirname,
                                               size_t dirnamelen) {}

static fm_xfs_err_t fm_xfs_find_entry_btree_(fm_xfs_t *fm, __uint8_t *ftype_out,
                                             ino_t *ino_out, void *dfork,
                                             char const *dirname,
                                             size_t dirnamelen) {}

fm_xfs_err_t fm_xfs_find_entry(fm_xfs_t *fm, __uint8_t *ftype_out,
                               ino_t *ino_out, char const *dirname,
                               size_t dirnamelen) {
  __uint16_t inodesize = be16toh(fm->sb.sb_inodesize);
  void *inode_info = alloca(inodesize);
  fseek(fm->f, inodesize * fm->ino_current_dir, SEEK_SET);
  if (fread(inode_info, inodesize, 1, fm->f) != 1)
    return FM_XFS_ERR_DEVICE;
  xfs_dinode_core_t *di = inode_info;
  void *dfork = (void *)((char *)inode_info + xfs_dinode_size(di));
  switch (di->di_format) {
  case XFS_DINODE_FMT_LOCAL:
    return fm_xfs_find_entry_local_(fm, ftype_out, ino_out, dfork, dirname,
                                    dirnamelen);
    break;
  case XFS_DINODE_FMT_EXTENTS:
    return fm_xfs_find_entry_extents_(fm, ftype_out, ino_out, dfork, dirname,
                                      dirnamelen);
    break;
  case XFS_DINODE_FMT_BTREE:
    return fm_xfs_find_entry_btree_(fm, ftype_out, ino_out, dfork, dirname,
                                    dirnamelen);
    break;
  }
}

fm_xfs_err_t fm_xfs_cd(fm_xfs_t *fm, char const *dirname, size_t dirnamelen) {
  __uint8_t ftype;
  ino_t ino;
  fm_xfs_err_t err = fm_xfs_find_entry(fm, &ftype, &ino, dirname, dirnamelen);
  if (err != FM_XFS_ERR_NONE)
    return err;
  if (ftype != XFS_DIR3_FT_DIR)
    return FM_XFS_ERR_NOT_A_DIRECTORY;
  fm->ino_current_dir = ino;
  return FM_XFS_ERR_NONE;
}

fm_xfs_err_t fm_xfs_sample(fm_xfs_t *fm) {
  fm_xfs_ls(fm);
  fm_xfs_cd(fm, "dir1", 4);
  fm_xfs_ls(fm);
  fm_xfs_cd(fm, "..", 2);
  fm_xfs_ls(fm);
}
