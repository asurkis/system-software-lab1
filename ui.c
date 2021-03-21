#include "ui.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int next_ls_(fm_xfs_t *fm) {
  switch (fm_xfs_ls(fm)) {
  case FM_XFS_ERR_NONE:
    break;
  case FM_XFS_ERR_NOT_SUPPORTED:
    fputs("ERR: Directory is in unsupported format", stderr);
    break;
  case FM_XFS_ERR_MAGIC:
  case FM_XFS_ERR_NOT_A_DIRECTORY:
    fputs("ERR: Not a directory", stderr);
    break;
  };
  return 1;
}

static int next_cd_(fm_xfs_t *fm) { return 1; }

static int next_cp_(fm_xfs_t *fm) { return 1; }

int next_command(fm_xfs_t *fm) {
  printf(">");
  char *cmd;
  scanf("%ms", &cmd);

  int retcode = 0;
  if (strcmp("ls", cmd) == 0)
    retcode = next_ls_(fm);
  else if (strcmp("cd", cmd) == 0)
    retcode = next_cd_(fm);
  else if (strcmp("cp", cmd) == 0)
    retcode = next_cp_(fm);
  else if (strcmp("exit", cmd) == 0)
    retcode = 0;
  else
    printf("Unknown command '%s'\n", cmd);
  scanf("%*[^\n]");

  free(cmd);
  return retcode;
}
