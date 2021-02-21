#include "CommandLineArgs.h"
#include "Xfs.h"

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

  case MODE_OPEN:
    break;
  }
  return 0;
}
