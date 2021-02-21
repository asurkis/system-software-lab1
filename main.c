#include "CommandLineArgs.h"
#include "DisplayDeviceList.h"
#include "Xfs.h"
#include <stdio.h>

int main(int argc, char **argv) {
  struct CommandLineArgs args;
  ParseArgs(&args, argc, argv);

  switch (args.Mode) {
  case MODE_INCORRECT:
    PrintUsage(argv[0]);
    break;

  case MODE_LIST:
    DisplayDeviceList(args.List.HeaderInterval);
    break;

  case MODE_OPEN: {
    xfs_sb_t sb;
    FILE *f = fopen(args.Open.DevicePath, "r");
    FreadXfsSb(f, &sb);
    fclose(f);
    printf("%08x %c%c%c%c\n", sb.sb_magicnum, (sb.sb_magicnum >> 24) & 0xff,
           (sb.sb_magicnum >> 16) & 0xff, (sb.sb_magicnum >> 8) & 0xff,
           (sb.sb_magicnum >> 0) & 0xff);
    printf("versionnum: %d %x\n", sb.sb_versionnum, sb.sb_versionnum);
  } break;
  }
  return 0;
}
