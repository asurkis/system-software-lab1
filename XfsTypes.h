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

typedef struct xfs_agi {
  uint32_t agi_magicnum;
  uint32_t agi_versionnum;
  uint32_t agi_seqno;
  uint32_t agi_length;
  uint32_t agi_count;
  uint32_t agi_root;
  uint32_t agi_level;
  uint32_t agi_freecount;
  uint32_t agi_newino;
  uint32_t agi_dirino;
  uint32_t agi_unlinked[64];
} xfs_agi_t;

typedef struct xfs_btree_sblock {
  uint32_t bb_magic;
  uint16_t bb_level;
  uint16_t bb_numrecs;
  uint32_t bb_leftsib;
  uint32_t bb_rightsib;
} xfs_btree_sblock_t;

typedef struct xfs_btree_sblock xfs_inobt_block_t;

typedef struct xfs_inobt_rec {
  uint32_t ir_startino;
  uint32_t ir_freecount;
  uint64_t ir_free;
} xfs_inobt_rec_t;

typedef struct xfs_inobt_key {
  uint32_t ir_startino;
} xfs_inobt_key_t;
typedef uint32_t xfs_inobt_ptr_t;

typedef struct xfs_timestamp {
  int32_t t_sec;
  int32_t t_nsec;
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
  __uint8_t di_pad[8];
  __uint16_t di_flushiter;
  xfs_timestamp_t di_atime;
  xfs_timestamp_t di_mtime;
  xfs_timestamp_t di_ctime;
  xfs_fsize_t di_size;
  xfs_drfsbno_t di_nblocks;
  xfs_extlen_t di_extsize;
  xfs_extnum_t di_nextents;
  xfs_aextnum_t di_anextents;
  __uint8_t di_forkoff;
  __int8_t di_aformat;
  __uint32_t di_dmevmask;
  __uint16_t di_dmstate;
  __uint16_t di_flags;
  __uint32_t di_gen;
} xfs_dinode_core_t;

union {
  xfs_bmdr_block_t di_bmbt;
  xfs_bmbt_rec_t di_bmx[1];
  xfs_dir2_sf_t di_dir2sf;
  char di_c[1];
  xfs_dev_t di_dev;
  uuid_t di_muuid;
  char di_symlink[1];
} di_u;

union {
  xfs_bmdr_block_t di_abmbt;
  xfs_bmbt_rec_t di_abmx[1];
  xfs_attr_shortform_t di_attrsf;
} di_a;

void BeToHostXfsSb(xfs_sb_t *sb);
void BeToHostXfsAgi(xfs_agi_t *agi);
void BeToHostXfsBtreeSblock(xfs_btree_sblock_t *bb);
void BeToHostXfsInobtRec(xfs_inobt_rec_t *ir);
void BeToHostXfsInobtKey(xfs_inobt_key_t *ir);

#endif
