#include "argparse.h"
#include "devicelist.h"
#include "ui.h"
#include "xfs.h"
#include <stdio.h>
#include <sys/stat.h>

static fm_xfs_err_t use_xfs(char *device_path) {
  fm_xfs_t fm;
  FM_XFS_CHKTHROW(fm_xfs_init(&fm, device_path));
  int nxt_cmd_code;
  do {
    nxt_cmd_code = next_command(&fm);
  } while (nxt_cmd_code);
  FM_XFS_CHKTHROW(fm_xfs_free(&fm));
}

int main(int argc, char **argv) {
  struct command_line_args args;
  argparse(&args, argc, argv);

  switch (args.mode) {
  case MODE_INCORRECT:
    print_usage(argv[0]);
    return -1;

  case MODE_LIST:
    print_devices(args.list.header_interval);
    break;

  case MODE_OPEN:
    if (use_xfs(args.open.device_path) != FM_XFS_ERR_NONE)
      return -2;
    break;
  }
  return 0;
}
