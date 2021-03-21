#include "ui.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TELL_ERR_(type)                                                        \
  case type:                                                                   \
    fputs(#type, stderr);                                                      \
    break;

static void tell_err_(fm_xfs_err_t code) {
  switch (code) {
  case FM_XFS_ERR_NONE:
    break;
    TELL_ERR_(FM_XFS_ERR_NOT_SUPPORTED)
    TELL_ERR_(FM_XFS_ERR_DEVICE_NOT_FOUND)
    TELL_ERR_(FM_XFS_ERR_DEVICE)
    TELL_ERR_(FM_XFS_ERR_OUT_DEVICE)
    TELL_ERR_(FM_XFS_ERR_MAGIC)
    TELL_ERR_(FM_XFS_ERR_FORMAT)
    TELL_ERR_(FM_XFS_ERR_FILENAME_NOT_FOUND)
    TELL_ERR_(FM_XFS_ERR_NOT_A_DIRECTORY)
  }
}

static int cmd_ls_(fm_xfs_t *fm) {
  tell_err_(fm_xfs_ls(fm));
  return 1;
}

static int cmd_cd_(fm_xfs_t *fm) {
  char *path;
  scanf("%ms", &path);
  tell_err_(fm_xfs_cd(fm, path, strlen(path)));
  free(path);
  return 1;
}

static int cmd_cp_(fm_xfs_t *fm) {
  char *from, *to;
  scanf("%ms %ms", &from, &to);
  tell_err_(fm_xfs_cp(fm, from, to));
  free(from);
  free(to);
  return 1;
}

int next_command(fm_xfs_t *fm) {
  printf("%d>", fm->ino_current_dir);
  char *cmd;
  scanf("%ms", &cmd);

  int retcode = 1;
  if (strcmp("ls", cmd) == 0)
    retcode = cmd_ls_(fm);
  else if (strcmp("cd", cmd) == 0)
    retcode = cmd_cd_(fm);
  else if (strcmp("cp", cmd) == 0)
    retcode = cmd_cp_(fm);
  else if (strcmp("exit", cmd) == 0)
    retcode = 0;
  else
    printf("Unknown command '%s'\n", cmd);

  scanf("%*[^\n]");
  free(cmd);
  return retcode;
}
