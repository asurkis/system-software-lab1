#ifndef _XFS_TYPES_H_
#define _XFS_TYPES_H_

#include <stdint.h>
#include <stdio.h>
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

typedef struct xfs_timestamp {
  int32_t t_sec;
  int32_t t_nsec;
} xfs_timestamp_t;

typedef int64_t xfs_fsize_t;
typedef int16_t xfs_aextnum_t;

typedef struct xfs_dinode_core {
  uint16_t di_magic;
  uint16_t di_mode;
  int8_t di_version;
  int8_t di_format;
  uint16_t di_onlink;
  uint32_t di_uid;
  uint32_t di_gid;
  uint32_t di_nlink;
  uint16_t di_projid;
  uint8_t di_pad[8];
  uint16_t di_flushiter;
  xfs_timestamp_t di_atime;
  xfs_timestamp_t di_mtime;
  xfs_timestamp_t di_ctime;
  xfs_fsize_t di_size;
  xfs_drfsbno_t di_nblocks;
  xfs_extlen_t di_extsize;
  xfs_extnum_t di_nextents;
  xfs_aextnum_t di_anextents;
  uint8_t di_forkoff;
  int8_t di_aformat;
  uint32_t di_dmevmask;
  uint16_t di_dmstate;
  uint16_t di_flags;
  uint32_t di_gen;
} xfs_dinode_core_t;

// union {
//   xfs_bmdr_block_t di_bmbt;
//   xfs_bmbt_rec_t di_bmx[1];
//   xfs_dir2_sf_t di_dir2sf;
//   char di_c[1];
//   xfs_dev_t di_dev;
//   uuid_t di_muuid;
//   char di_symlink[1];
// } di_u;

// union {
//   xfs_bmdr_block_t di_abmbt;
//   xfs_bmbt_rec_t di_abmx[1];
//   xfs_attr_shortform_t di_attrsf;
// } di_a;

void betoh_xfs_sb(xfs_sb_t *sb);
void betoh_xfs_timestamp(xfs_timestamp_t *t);
void betoh_xfs_dinode_core(xfs_dinode_core_t *di);

#endif
