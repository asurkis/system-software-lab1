#ifndef _XFS_TYPES_H_
#define _XFS_TYPES_H_

#include <stdio.h>
#include <sys/types.h>
#include <uuid/uuid.h>

typedef __uint8_t __u8;
typedef __uint16_t __be16, __le16;
typedef __uint32_t __be32, __le32;
typedef __uint64_t __be64, __le64;

typedef __uint64_t xfs_ino_t;
typedef __int64_t xfs_off_t;
typedef __int64_t xfs_daddr_t;
typedef __uint32_t xfs_agnumber_t;
typedef __uint32_t xfs_agblock_t;
typedef __uint32_t xfs_extlen_t;
typedef __int32_t xfs_extnum_t;
typedef __int16_t xfs_aextnum_t;
typedef __uint32_t xfs_dablk_t;
typedef __uint32_t xfs_dahash_t;
typedef __uint64_t xfs_fsblock_t;
typedef __uint64_t xfs_rfsblock_t;
typedef __uint64_t xfs_rtblock_t;
typedef __uint64_t xfs_fileoff_t;
typedef __uint64_t xfs_filblks_t;
typedef __int64_t xfs_fsize_t;

typedef __int64_t xfs_lsn_t;

typedef struct xfs_sb {
  __uint32_t sb_magicnum;
  __uint32_t sb_blocksize;
  xfs_rfsblock_t sb_dblocks;
  xfs_rfsblock_t sb_rblocks;
  xfs_rtblock_t sb_rextents;
  uuid_t sb_uuid;
  xfs_fsblock_t sb_logstart;
  xfs_ino_t sb_rootino;
  xfs_ino_t sb_rbmino;
  xfs_ino_t sb_rsumino;
  xfs_agblock_t sb_rextsize;
  xfs_agblock_t sb_agblocks;
  xfs_agnumber_t sb_agcount;
  xfs_extlen_t sb_rbmblocks;
  xfs_extlen_t sb_logblocks;
  __uint16_t sb_versionnum;
  __uint16_t sb_sectsize;
  __uint16_t sb_inodesize;
  __uint16_t sb_inopblock;
  char sb_fname[12];
  __uint8_t sb_blocklog;
  __uint8_t sb_sectlog;
  __uint8_t sb_inodelog;
  __uint8_t sb_inopblog;
  __uint8_t sb_agblklog;
  __uint8_t sb_rextslog;
  __uint8_t sb_inprogress;
  __uint8_t sb_imax_pct;
  __uint64_t sb_icount;
  __uint64_t sb_ifree;
  __uint64_t sb_fdblocks;
  __uint64_t sb_frextents;
  xfs_ino_t sb_uquotino;
  xfs_ino_t sb_gquotino;
  __uint16_t sb_qflags;
  __uint8_t sb_flags;
  __uint8_t sb_shared_vn;
  xfs_extlen_t sb_inoalignmt;
  __uint32_t sb_unit;
  __uint32_t sb_width;
  __uint8_t sb_dirblklog;
  __uint8_t sb_logsectlog;
  __uint16_t sb_logsectsize;
  __uint32_t sb_logsunit;
  __uint32_t sb_features2;
  __uint32_t sb_bad_features2;

  /* version 5 superblock fields start here */
  __uint32_t sb_features_compat;
  __uint32_t sb_features_ro_compat;
  __uint32_t sb_features_incompat;
  __uint32_t sb_features_log_incompat;
  __uint32_t sb_crc;
  xfs_extlen_t sb_spino_align;
  xfs_ino_t sb_pquotino;
  xfs_lsn_t sb_lsn;
  uuid_t sb_meta_uuid;
  xfs_ino_t sb_rrmapino;
} xfs_sb_t;

typedef struct xfs_timestamp {
  __int32_t t_sec;
  __int32_t t_nsec;
} xfs_timestamp_t;

typedef struct xfs_dinode_core {
  __uint16_t di_magic;
  __uint16_t di_mode;
  __int8_t di_version;
  __int8_t di_format;
  __uint16_t di_onlink;
  __uint32_t di_uid;
  __uint32_t di_gid;
  __uint32_t di_nlink;
  __uint16_t di_projid;
  __uint16_t di_projid_hi;
  __uint8_t di_pad[6];
  __uint16_t di_flushiter;
  xfs_timestamp_t di_atime;
  xfs_timestamp_t di_mtime;
  xfs_timestamp_t di_ctime;
  xfs_fsize_t di_size;
  xfs_rfsblock_t di_nblocks;
  xfs_extlen_t di_extsize;
  xfs_extnum_t di_nextents;
  xfs_aextnum_t di_anextents;
  __uint8_t di_forkoff;
  __int8_t di_aformat;
  __uint32_t di_dmevmask;
  __uint16_t di_dmstate;
  __uint16_t di_flags;
  __uint32_t di_gen;
  /* di_next_unlinked is the only non-core field in the old dinode */
  __be32 di_next_unlinked;
  /* version 5 filesystem (inode version 3) fields start here */
  __le32 di_crc;
  __be64 di_changecount;
  __be64 di_lsn;
  __be64 di_flags2;
  __be32 di_cowextsize;
  __u8 di_pad2[12];
  xfs_timestamp_t di_crtime;
  __be64 di_ino;
  uuid_t di_uuid;
} xfs_dinode_core_t;

void dtoh_xfs_sb(xfs_sb_t *sb);
void dtoh_xfs_timestamp(xfs_timestamp_t *t);
void dtoh_xfs_dinode_core(xfs_dinode_core_t *di);

#endif
