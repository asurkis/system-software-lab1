#include "CommandLineArgs.h"
#include "DisplayDeviceList.h"
#include "Xfs.h"
#include <stdio.h>

void SampleXfs(const char *DevicePath) {
  xfs_sb_t sb;
  xfs_agi_t agi;
  xfs_inobt_block_t bt;

  FILE *f = fopen(DevicePath, "r");

  fread(&sb, sizeof(xfs_sb_t), 1, f);
  BeToHostXfsSb(&sb);
  printf("Magicnum 0x%08x\n", sb.sb_magicnum);

  printf("\nSeeking 0x%04x\n", 2 * sb.sb_sectsize);
  fseek(f, 2 * sb.sb_sectsize, SEEK_SET);
  fread(&agi, sizeof(xfs_agi_t), 1, f);
  BeToHostXfsAgi(&agi);
  printf("Magicnum 0x%08x\n", agi.agi_magicnum);

  printf("\nSeeking 0x%x\n", sb.sb_blocksize * agi.agi_root);
  // fseek(f, agi.agi_root, SEEK_SET);

  fclose(f);

  printf("%08x %08x\n", sb.sb_magicnum, agi.agi_magicnum);
}

int main(int argc, char **argv) {
  /*
  struct CommandLineArgs args;
  ParseArgs(&args, argc, argv);

  switch (args.Mode) {
  case MODE_INCORRECT:
    PrintUsage(argv[0]);
    break;

  case MODE_LIST:
    DisplayDeviceList(args.List.HeaderInterval);
    break;

  case MODE_OPEN:
    SampleXfs(args.Open.DevicePath);
    break;
  }
  */
 SampleXfs("/hdd/homework/system-software/testfs");
  return 0;
}
