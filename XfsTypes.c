#include "XfsTypes.h"
#include <endian.h>
#include <features.h>

void BeToHostXfsSb(xfs_sb_t *sb) {
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

void BeToHostXfsAgi(xfs_agi_t *agi) {
  agi->agi_magicnum = be32toh(agi->agi_magicnum);
  agi->agi_versionnum = be32toh(agi->agi_versionnum);
  agi->agi_seqno = be32toh(agi->agi_seqno);
  agi->agi_length = be32toh(agi->agi_length);
  agi->agi_count = be32toh(agi->agi_count);
  agi->agi_root = be32toh(agi->agi_root);
  agi->agi_level = be32toh(agi->agi_level);
  agi->agi_freecount = be32toh(agi->agi_freecount);
  agi->agi_newino = be32toh(agi->agi_newino);
  agi->agi_dirino = be32toh(agi->agi_dirino);
  for (int i = 0; i < 64; ++i)
    agi->agi_unlinked[i] = be32toh(agi->agi_unlinked[i]);
}

void BeToHostXfsBtreeSblock(xfs_btree_sblock_t *bb) {
  bb->bb_magic = be32toh(bb->bb_magic);
  bb->bb_level = be16toh(bb->bb_level);
  bb->bb_numrecs = be16toh(bb->bb_numrecs);
  bb->bb_leftsib = be32toh(bb->bb_leftsib);
  bb->bb_rightsib = be32toh(bb->bb_rightsib);
}

void BeToHostXfsInobtRec(xfs_inobt_rec_t *ir) {
  ir->ir_startino = be32toh(ir->ir_startino);
  ir->ir_freecount = be32toh(ir->ir_freecount);
  ir->ir_free = be64toh(ir->ir_free);
}

void BeToHostXfsInobtKey(xfs_inobt_key_t *ir) {
  ir->ir_startino = be32toh(ir->ir_startino);
}
