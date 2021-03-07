#include "argparse.h"
#include "devicelist.h"
#include "xfs.h"
#include <stdio.h>
#include <sys/stat.h>

fm_xfs_err_t sample_xfs(const char *device_path) {
  fm_xfs_err_t err;
  fm_xfs_t fm;
  err = fm_xfs_init(&fm, device_path);
  if (err != FM_XFS_ERR_NONE)
    return err;
  err = fm_xfs_sample(&fm);
  if (err != FM_XFS_ERR_NONE)
    return err;
  err = fm_xfs_free(&fm);
  if (err != FM_XFS_ERR_NONE)
    return err;
}

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
  printf("\n\nERRCODE = %d\n",
         sample_xfs("/hdd/homework/system-software/testfs"));
  return 0;
}
