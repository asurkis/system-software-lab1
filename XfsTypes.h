#ifndef _XFS_TYPES_H_
#define _XFS_TYPES_H_

#include <stdint.h>
#include <uuid/uuid.h>

typedef uint64_t xfs_ino_t;
typedef int64_t xfs_off_t;
typedef int64_t xfs_daddr_t;
typedef uint32_t xfs_agnumber_t;
typedef uint32_t xfs_agblock_t;
typedef uint32_t xfs_extlen_t;
typedef int32_t xfs_extnum_t;
typedef uint32_t xfs_dablk_t;
typedef uint32_t xfs_dahash_t;
typedef uint64_t xfs_dfsbno_t;
typedef uint64_t xfs_drfsbno_t;
typedef uint64_t xfs_drtbno_t;
typedef uint64_t xfs_dfiloff_t;
typedef uint64_t xfs_dfilblks_t;

typedef struct xfs_sb {
  uint32_t sb_magicnum;
  uint32_t sb_blocksize;
  xfs_drfsbno_t sb_dblocks;
  xfs_drfsbno_t sb_rblocks;
  xfs_drtbno_t sb_rextents;
  uuid_t sb_uuid;
  xfs_dfsbno_t sb_logstart;
  xfs_ino_t sb_rootino;
  xfs_ino_t sb_rbmino;
  xfs_ino_t sb_rsumino;
  xfs_agblock_t sb_rextsize;
  xfs_agblock_t sb_agblocks;
  xfs_agnumber_t sb_agcount;
  xfs_extlen_t sb_rbmblocks;
  xfs_extlen_t sb_logblocks;
  uint16_t sb_versionnum;
  uint16_t sb_sectsize;
  uint16_t sb_inodesize;
  uint16_t sb_inopblock;
  char sb_fname[12];
  uint8_t sb_blocklog;
  uint8_t sb_sectlog;
  uint8_t sb_inodelog;
  uint8_t sb_inopblog;
  uint8_t sb_agblklog;
  uint8_t sb_rextslog;
  uint8_t sb_inprogress;
  uint8_t sb_imax_pct;
  uint64_t sb_icount;
  uint64_t sb_ifree;
  uint64_t sb_fdblocks;
  uint64_t sb_frextents;
  xfs_ino_t sb_uquotino;
  xfs_ino_t sb_gquotino;
  uint16_t sb_qflags;
  uint8_t sb_flags;
  uint8_t sb_shared_vn;
  xfs_extlen_t sb_inoalignmt;
  uint32_t sb_unit;
  uint32_t sb_width;
  uint8_t sb_dirblklog;
  uint8_t sb_logsectlog;
  uint16_t sb_logsectsize;
  uint32_t sb_logsunit;
  uint32_t sb_features2;
} xfs_sb_t;

#endif
