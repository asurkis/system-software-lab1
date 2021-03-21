#include "ui.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int cmd_ls_(fm_xfs_t *fm) {
  switch (fm_xfs_ls(fm)) {
  case FM_XFS_ERR_NONE:
    break;
  case FM_XFS_ERR_NOT_SUPPORTED:
    fputs("ERR: Directory is in unsupported format", stderr);
    break;
  case FM_XFS_ERR_DEVICE_NOT_FOUND:
    break;
  case FM_XFS_ERR_DEVICE:
    fputs("ERR: Couldn't read device block", stderr);
    break;
  case FM_XFS_ERR_MAGIC:
    fputs("ERR: Incorrect magicnum", stderr);
    break;
  case FM_XFS_ERR_FILENAME_NOT_FOUND:
    break;
  case FM_XFS_ERR_NOT_A_DIRECTORY:
    fputs("ERR: Not a directory", stderr);
    break;
  };
  return 1;
}

static int cmd_cd_(fm_xfs_t *fm) {
  char *path;
  scanf("%ms", &path);
  printf("(Seeking path '%s')\n", path);
  switch (fm_xfs_cd(fm, path, strlen(path))) {
  case FM_XFS_ERR_NONE:
    break;
  case FM_XFS_ERR_NOT_SUPPORTED:
    fputs("ERR: Directory is in unsupported format", stderr);
    break;
  case FM_XFS_ERR_DEVICE_NOT_FOUND:
    break;
  case FM_XFS_ERR_DEVICE:
    fputs("ERR: Couldn't read device block", stderr);
    break;
  case FM_XFS_ERR_MAGIC:
    fputs("ERR: Incorrect magicnum", stderr);
    break;
  case FM_XFS_ERR_FILENAME_NOT_FOUND:
    fputs("ERR: Couldn't find file or directory", stderr);
    break;
  case FM_XFS_ERR_NOT_A_DIRECTORY:
    fputs("ERR: Not a directory", stderr);
    break;
  }
  free(path);
  return 1;
}

static int cmd_cp_(fm_xfs_t *fm) {
  /*   char *path1, *path2;
    scanf("%ms %ms", &path1, &path2);
    switch () {
    case FM_XFS_ERR_NONE:
      break;
    case FM_XFS_ERR_NOT_SUPPORTED:
      fputs("ERR: Directory is in unsupported format", stderr);
      break;
    case FM_XFS_ERR_DEVICE_NOT_FOUND:
      break;
    case FM_XFS_ERR_DEVICE:
      fputs("ERR: Couldn't read device block", stderr);
      break;
    case FM_XFS_ERR_MAGIC:
      fputs("ERR: Incorrect magicnum", stderr);
      break;
    case FM_XFS_ERR_FILENAME_NOT_FOUND:
      fputs("ERR: Couldn't find file or directory", stderr);
      break;
    case FM_XFS_ERR_NOT_A_DIRECTORY:
      fputs("ERR: Not a directory", stderr);
      break;
    } */
  return 0;
}

int next_command(fm_xfs_t *fm) {
  printf("%d>", fm->ino_current_dir);
  char *cmd;
  scanf("%ms", &cmd);

  int retcode = 0;
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
