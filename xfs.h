#ifndef _XFS_H_
#define _XFS_H_

#include "xfs_types.h"

typedef struct fm_xfs {
  FILE *f;
  xfs_sb_t sb;
  xfs_ino_t ino_current_dir;
} fm_xfs_t;

typedef enum fm_xfs_err {
  FM_XFS_ERR_NONE = 0,
  FM_XFS_ERR_DEVICE_NOT_FOUND,
  FM_XFS_ERR_DEVICE
} fm_xfs_err_t;

fm_xfs_err_t fm_xfs_init(fm_xfs_t *fm, char const *device_path);
fm_xfs_err_t fm_xfs_free(fm_xfs_t *fm);
fm_xfs_err_t fm_xfs_sample(fm_xfs_t *fm);
fm_xfs_err_t fm_xfs_cd(fm_xfs_t *fm, char const *path);

#endif
