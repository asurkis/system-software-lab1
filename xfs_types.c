#include "xfs_types.h"
#include <endian.h>
#include <features.h>

void betoh_xfs_sb(xfs_sb_t *sb) {
  sb->sb_magicnum = be32toh(sb->sb_magicnum);
  sb->sb_blocksize = be32toh(sb->sb_blocksize);
  sb->sb_dblocks = be64toh(sb->sb_dblocks);
  sb->sb_rblocks = be64toh(sb->sb_rblocks);
  sb->sb_rextents = be64toh(sb->sb_rextents);
  sb->sb_logstart = be64toh(sb->sb_logstart);
  sb->sb_rootino = be64toh(sb->sb_rootino);
  sb->sb_rbmino = be64toh(sb->sb_rbmino);
  sb->sb_rsumino = be64toh(sb->sb_rsumino);
  sb->sb_rextsize = be32toh(sb->sb_rextsize);
  sb->sb_agblocks = be32toh(sb->sb_agblocks);
  sb->sb_agcount = be32toh(sb->sb_agcount);
  sb->sb_rbmblocks = be32toh(sb->sb_rbmblocks);
  sb->sb_logblocks = be32toh(sb->sb_logblocks);
  sb->sb_versionnum = be16toh(sb->sb_versionnum);
  sb->sb_sectsize = be16toh(sb->sb_sectsize);
  sb->sb_inodesize = be16toh(sb->sb_inodesize);
  sb->sb_inopblock = be16toh(sb->sb_inopblock);
  sb->sb_icount = be64toh(sb->sb_icount);
  sb->sb_ifree = be64toh(sb->sb_ifree);
  sb->sb_fdblocks = be64toh(sb->sb_fdblocks);
  sb->sb_frextents = be64toh(sb->sb_frextents);
  sb->sb_uquotino = be64toh(sb->sb_uquotino);
  sb->sb_gquotino = be64toh(sb->sb_gquotino);
  sb->sb_qflags = be16toh(sb->sb_qflags);
  sb->sb_inoalignmt = be32toh(sb->sb_inoalignmt);
  sb->sb_unit = be32toh(sb->sb_unit);
  sb->sb_width = be32toh(sb->sb_width);
  sb->sb_logsectsize = be16toh(sb->sb_logsectsize);
  sb->sb_logsunit = be32toh(sb->sb_logsunit);
  sb->sb_features2 = be32toh(sb->sb_features2);
}

void betoh_xfs_timestamp(xfs_timestamp_t *t) {
  t->t_sec = be32toh(t->t_sec);
  t->t_nsec = be32toh(t->t_nsec);
}

void betoh_xfs_dinode_core(xfs_dinode_core_t *di) {
  di->di_magic = be16toh(di->di_magic);
  di->di_mode = be16toh(di->di_mode);
  di->di_onlink = be16toh(di->di_onlink);
  di->di_uid = be32toh(di->di_uid);
  di->di_gid = be32toh(di->di_gid);
  di->di_nlink = be32toh(di->di_nlink);
  di->di_projid = be16toh(di->di_projid);
  di->di_flushiter = be16toh(di->di_flushiter);
  betoh_xfs_timestamp(&di->di_atime);
  betoh_xfs_timestamp(&di->di_mtime);
  betoh_xfs_timestamp(&di->di_ctime);
  di->di_size = be64toh(di->di_size);
  di->di_nblocks = be64toh(di->di_nblocks);
  di->di_extsize = be32toh(di->di_extsize);
  di->di_nextents = be32toh(di->di_nextents);
  di->di_anextents = be16toh(di->di_anextents);
  di->di_dmevmask = be32toh(di->di_dmevmask);
  di->di_dmstate = be16toh(di->di_dmstate);
  di->di_flags = be16toh(di->di_flags);
  di->di_gen = be32toh(di->di_gen);
}
