#include "argparse.h"
#include "devicelist.h"
#include "xfs.h"
#include <stdio.h>
#include <sys/stat.h>

void sample_xfs(const char *device_path) {
  fm_xfs_t fm;
  fm_xfs_init(&fm, device_path);
  fm_xfs_sample(&fm);
  fm_xfs_free(&fm);
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
  sample_xfs("/hdd/homework/system-software/testfs");
  return 0;
}
