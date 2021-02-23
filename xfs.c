#include "xfs.h"
#include <alloca.h>
#include <endian.h>
#include <features.h>
#include <sys/stat.h>

fm_xfs_err_t fm_xfs_init(fm_xfs_t *fm, char const *device_path) {
  fm->f = fopen(device_path, "r");
  if (!fm->f)
    return FM_XFS_ERR_DEVICE_NOT_FOUND;
  if (fread(&fm->sb, sizeof(xfs_sb_t), 1, fm->f) != 1) {
    fclose(fm->f);
    return FM_XFS_ERR_DEVICE;
  }
  dtoh_xfs_sb(&fm->sb);
  fm->ino_current_dir = fm->sb.sb_rootino;
  return FM_XFS_ERR_NONE;
}

fm_xfs_err_t fm_xfs_free(fm_xfs_t *fm) {
  if (fclose(fm->f))
    return FM_XFS_ERR_DEVICE;
  return FM_XFS_ERR_NONE;
}

static void fm_xfs_ls_local_(fm_xfs_t *fm, void *dfork) {
  xfs_dir2_sf_t *sf = dfork;
  printf("Parent inode: %d\n", sf->hdr.parent);
  printf("%d children (%d 64-bit):\n", sf->hdr.count, sf->hdr.i8count);
  for (int i = 0; i < sf->hdr.count; ++i) {
    xfs_dir2_sf_entry_t *entry = &sf->list[i];
    char buf[256];
    for (int j = 0; j < entry->namelen; ++j)
      buf[j] = entry->name[j - 4];
    buf[entry->namelen] = 0;
    __uint64_t n = ((__uint64_t)(entry->inumber.i4.i[0]) << 0) |
                   ((__uint64_t)(entry->inumber.i4.i[1]) << 8) |
                   ((__uint64_t)(entry->inumber.i4.i[2]) << 16) |
                   ((__uint64_t)(entry->inumber.i4.i[3]) << 24);
    printf("%s => %d\n", buf, entry->inumber);
  }
}

static void fm_xfs_ls_extents_(fm_xfs_t *fm, void *dfork) {}

static void fm_xfs_ls_btree_(fm_xfs_t *fm, void *dfork) {}

fm_xfs_err_t fm_xfs_ls(fm_xfs_t *fm) {
  fprintf(stderr, "%d %d\n", fm->sb.sb_inodesize, sizeof(xfs_dinode_core_t));
  void *inode_info = alloca(fm->sb.sb_inodesize);
  fseek(fm->f, fm->sb.sb_inodesize * fm->ino_current_dir, SEEK_SET);
  if (fread(inode_info, fm->sb.sb_inodesize, 1, fm->f) != 1)
    return FM_XFS_ERR_DEVICE;
  xfs_dinode_core_t *di = inode_info;
  dtoh_xfs_dinode_core(di);
  fputs("qwerqwer\n", stderr);
  fprintf(stderr, "%x\n", di->di_magic);
  fputs("asdfasdf\n", stderr);
  void *dfork = (void *)((char *)inode_info + xfs_dinode_size(di));
  fputs("zxcvzxcv\n", stderr);
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

fm_xfs_err_t fm_xfs_sample(fm_xfs_t *fm) {
  fm_xfs_ls(fm);
}

fm_xfs_err_t fm_xfs_cd(fm_xfs_t *fm, char const *path) {}
