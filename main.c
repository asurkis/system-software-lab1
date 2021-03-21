#include "argparse.h"
#include "devicelist.h"
#include "ui.h"
#include "xfs.h"
#include <stdio.h>
#include <sys/stat.h>

int main(int argc, char **argv) {
  /* struct command_line_args args;
  argparse(&args, argc, argv);

  switch (args.Mode) {
  case MODE_INCORRECT:
    print_usage(argv[0]);
    break;

  case MODE_LIST:
    print_devices(args.List.HeaderInterval);
    break;

  case MODE_OPEN:
    sample_xfs(args.Open.DevicePath);
    break;
  } */
  // printf("\n\nERRCODE = %d\n",
  //        sample_xfs("/hdd/homework/system-software/testfs"));
  fm_xfs_t fm;
  FM_XFS_CHKTHROW(fm_xfs_init(&fm, "/hdd/homework/system-software/testfs"));
  int nxt_cmd_code;
  do {
    nxt_cmd_code = next_command(&fm);
  } while (nxt_cmd_code);
  FM_XFS_CHKTHROW(fm_xfs_free(&fm));
  return 0;
}
