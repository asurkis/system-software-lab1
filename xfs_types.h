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
typedef __u8 xfs_dir2_sf_off_t[2];
typedef void *xfs_dir2_dataptr_t;
// typedef off_t xfs_dir2_data_off_t;
typedef __uint16_t xfs_dir2_data_off_t;

#define PACKED_STRUCT struct __attribute__((packed))

#define XFS_SB_MAGICNUM 0x58465342

typedef PACKED_STRUCT xfs_sb {
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
}
xfs_sb_t;

typedef PACKED_STRUCT { __uint8_t i[8]; }
xfs_dir2_ino8_t;

typedef PACKED_STRUCT { __uint8_t i[4]; }
xfs_dir2_ino4_t;

typedef union {
  xfs_dir2_ino8_t i8;
  xfs_dir2_ino4_t i4;
} xfs_dir2_inou_t;

typedef PACKED_STRUCT xfs_dir2_sf_hdr {
  __uint8_t count;
  __uint8_t i8count;
  xfs_dir2_inou_t parent;
}
xfs_dir2_sf_hdr_t;

typedef PACKED_STRUCT xfs_dir2_sf_entry {
  __uint8_t namelen;
  xfs_dir2_sf_off_t offset;
  __uint8_t name[1];
  __uint8_t ftype;
  xfs_dir2_inou_t inumber;
}
xfs_dir2_sf_entry_t;

typedef PACKED_STRUCT xfs_dir2_sf_entry_hdr {
  __uint8_t namelen;
  xfs_dir2_sf_off_t offset;
  __uint8_t name[1];
}
xfs_dir2_sf_entry_hdr_t;

enum xfs_dir3_ft {
  XFS_DIR3_FT_UNKNOWN,
  XFS_DIR3_FT_REG_FILE,
  XFS_DIR3_FT_DIR,
  XFS_DIR3_FT_CHRDEV,
  XFS_DIR3_FT_BLKDEV,
  XFS_DIR3_FT_FIFO,
  XFS_DIR3_FT_SOCK,
  XFS_DIR3_FT_SYMLINK,
  XFS_DIR3_FT_WHT
};

typedef PACKED_STRUCT xfs_dir2_sf_entry_footer {
  __uint8_t ftype;
  xfs_dir2_inou_t inumber;
}
xfs_dir2_sf_entry_footer_t;

typedef PACKED_STRUCT xfs_dir2_sf {
  xfs_dir2_sf_hdr_t hdr;
  xfs_dir2_sf_entry_t list[1];
}
xfs_dir2_sf_t;

#define XFS_DIR2_DATA_FD_COUNT 3

typedef PACKED_STRUCT xfs_dir2_data_entry {
  xfs_ino_t inumber;
  __uint8_t namelen;
  __uint8_t name[1];
  __uint8_t ftype;
  xfs_dir2_data_off_t tag;
}
xfs_dir2_data_entry_t;

#define XFS_DIR2_DATA_UNUSED_FREETAG 0xFFFF

typedef PACKED_STRUCT xfs_dir2_data_unused {
  __uint16_t freetag; /*0xffff*/
  xfs_dir2_data_off_t length;
  xfs_dir2_data_off_t tag;
}
xfs_dir2_data_unused_t;

typedef union {
  xfs_dir2_data_entry_t entry;
  xfs_dir2_data_unused_t unused;
} xfs_dir2_data_union_t;

typedef PACKED_STRUCT xfs_dir2_data_free {
  xfs_dir2_data_off_t offset;
  xfs_dir2_data_off_t length;
}
xfs_dir2_data_free_t;

typedef PACKED_STRUCT xfs_dir2_leaf_entry {
  xfs_dahash_t hashval;
  xfs_dir2_dataptr_t address;
}
xfs_dir2_leaf_entry_t;

typedef PACKED_STRUCT xfs_dir2_block_tail {
  __uint32_t count;
  __uint32_t stale;
}
xfs_dir2_block_tail_t;

typedef PACKED_STRUCT xfs_dir2_data_hdr {
  __uint32_t magic;
  xfs_dir2_data_free_t bestfree[XFS_DIR2_DATA_FD_COUNT];
}
xfs_dir2_data_hdr_t;

PACKED_STRUCT xfs_dir3_blk_hdr {
  __be32 magic;
  __be32 crc;
  __be64 blkno;
  __be64 lsn;
  uuid_t uuid;
  __be64 owner;
};

PACKED_STRUCT xfs_dir3_data_hdr {
  PACKED_STRUCT xfs_dir3_blk_hdr hdr;
  xfs_dir2_data_free_t best_free[XFS_DIR2_DATA_FD_COUNT];
  __be32 pad;
};

typedef PACKED_STRUCT xfs_dir2_block {
  xfs_dir2_data_hdr_t hdr;
  xfs_dir2_data_union_t u[1];
  xfs_dir2_leaf_entry_t leaf[1];
  xfs_dir2_block_tail_t tail;
}
xfs_dir2_block_t;

typedef PACKED_STRUCT xfs_timestamp {
  __int32_t t_sec;
  __int32_t t_nsec;
}
xfs_timestamp_t;

typedef enum xfs_dinode_fmt {
  XFS_DINODE_FMT_DEV,
  XFS_DINODE_FMT_LOCAL,
  XFS_DINODE_FMT_EXTENTS,
  XFS_DINODE_FMT_BTREE,
  XFS_DINODE_FMT_UUID,
  XFS_DINODE_FMT_RMAP,
} xfs_dinode_fmt_t;

typedef PACKED_STRUCT xfs_dinode_core {
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
}
xfs_dinode_core_t;

/* 15.3 Data Fork -- начало параграфа */
#define XFS_DINODE_V2_SIZE 100
#define XFS_DINODE_V3_SIZE 176

static inline size_t xfs_dinode_size(xfs_dinode_core_t *di) {
  return di->di_version < 3 ? XFS_DINODE_V2_SIZE : XFS_DINODE_V3_SIZE;
}

typedef enum { XFS_EXT_NORM, XFS_EXT_UNWRITTEN, XFS_EXT_INVALID } xfs_exntst_t;

PACKED_STRUCT xfs_bmbt_rec { __uint64_t l0, l1; };

PACKED_STRUCT xfs_bmbt_irec {
  xfs_fileoff_t br_startoff;
  xfs_fsblock_t br_startblock;
  xfs_filblks_t br_blockcount;
  xfs_exntst_t br_state;
};

// https://elixir.bootlin.com/linux/v5.11.4/source/fs/xfs/libxfs/xfs_bmap_btree.c#L60
static inline void xfs_bmbt_disk_get_all(PACKED_STRUCT xfs_bmbt_rec *rec,
                                         PACKED_STRUCT xfs_bmbt_irec *irec) {
  __uint64_t l0 = be64toh(rec->l0);
  __uint64_t l1 = be64toh(rec->l1);
  irec->br_startoff = (l0 & ((1 << 63) - 1)) >> 9;
  irec->br_startblock = (l0 & ((1 << 9) - 1)) << 43 | (l1 >> 21);
  irec->br_blockcount = l1 & ((1 << 21) - 1);
  if (l0 >> 63)
    irec->br_state = XFS_EXT_UNWRITTEN;
  else
    irec->br_state = XFS_EXT_NORM;
}

#endif
