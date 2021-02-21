#include "XfsTypes.h"
#include <features.h>
#include <endian.h>

static void BeToHostXfsSb(xfs_sb_t *sb) {
  sb->sb_magicnum    = be32toh( sb->sb_magicnum    );
  sb->sb_blocksize   = be32toh( sb->sb_blocksize   );
  sb->sb_dblocks     = be64toh( sb->sb_dblocks     );
  sb->sb_rblocks     = be64toh( sb->sb_rblocks     );
  sb->sb_rextents    = be64toh( sb->sb_rextents    );
  sb->sb_logstart    = be64toh( sb->sb_logstart    );
  sb->sb_rootino     = be64toh( sb->sb_rootino     );
  sb->sb_rbmino      = be64toh( sb->sb_rbmino      );
  sb->sb_rsumino     = be64toh( sb->sb_rsumino     );
  sb->sb_rextsize    = be32toh( sb->sb_rextsize    );
  sb->sb_agblocks    = be32toh( sb->sb_agblocks    );
  sb->sb_agcount     = be32toh( sb->sb_agcount     );
  sb->sb_rbmblocks   = be32toh( sb->sb_rbmblocks   );
  sb->sb_logblocks   = be32toh( sb->sb_logblocks   );
  sb->sb_versionnum  = be16toh( sb->sb_versionnum  );
  sb->sb_sectsize    = be16toh( sb->sb_sectsize    );
  sb->sb_inodesize   = be16toh( sb->sb_inodesize   );
  sb->sb_inopblock   = be16toh( sb->sb_inopblock   );
  sb->sb_icount      = be64toh( sb->sb_icount      );
  sb->sb_ifree       = be64toh( sb->sb_ifree       );
  sb->sb_fdblocks    = be64toh( sb->sb_fdblocks    );
  sb->sb_frextents   = be64toh( sb->sb_frextents   );
  sb->sb_uquotino    = be64toh( sb->sb_uquotino    );
  sb->sb_gquotino    = be64toh( sb->sb_gquotino    );
  sb->sb_qflags      = be16toh( sb->sb_qflags      );
  sb->sb_inoalignmt  = be32toh( sb->sb_inoalignmt  );
  sb->sb_unit        = be32toh( sb->sb_unit        );
  sb->sb_width       = be32toh( sb->sb_width       );
  sb->sb_logsectsize = be16toh( sb->sb_logsectsize );
  sb->sb_logsunit    = be32toh( sb->sb_logsunit    );
  sb->sb_features2   = be32toh( sb->sb_features2   );
}

size_t FreadXfsSb(FILE *f, xfs_sb_t *sb) {
  size_t retval = fread(sb, sizeof(xfs_sb_t), 1, f);
  BeToHostXfsSb(sb);
  return retval;
}
