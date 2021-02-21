#include "argparse.h"
#include "devicelist.h"
#include "xfs.h"
#include <stdio.h>

void sample_xfs(const char *device_path) {
  FILE *f = fopen(device_path, "r");

  xfs_sb_t sb;
  fread(&sb, sizeof(xfs_sb_t), 1, f);
  betoh_xfs_sb(&sb);
  printf("Magicnum 0x%08x\n", sb.sb_magicnum);

  printf("\nSeeking 0x%x\n", sb.sb_inodesize * sb.sb_rootino);
  fseek(f, sb.sb_inodesize * sb.sb_rootino, SEEK_SET);
  xfs_dinode_core_t di;
  fread(&di, sizeof(xfs_dinode_core_t), 1, f);
  betoh_xfs_dinode_core(&di);
  printf("Magic 0x%04x\n", di.di_magic);

  fclose(f);
}

int main(int argc, char **argv) {
  struct command_line_args args;
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
  }
 sample_xfs("/hdd/homework/system-software/testfs");
  return 0;
}
