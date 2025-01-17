#include "argparse.h"
#include <stdio.h>
#include <string.h>

void print_usage(char *exe_path) {
  puts("Usage:");
  printf("\t%s list [header interval]"
         "\n\t\t-- list mounted devices"
         "\n\t\t   header interval values:"
         "\n\t\t     <0 -- disabled (default)"
         "\n\t\t      0 -- display only the first header"
         "\n\t\t      N -- display header for each Nth line\n",
         exe_path);
  printf("\t%s open <device>\n\t\t-- open an xfs filesystem\n", exe_path);
}

void argparse(struct command_line_args *out, int argc, char **argv) {
  if (argc >= 2 && strcmp(argv[1], "list") == 0) {
    out->mode = MODE_LIST;
    out->list.header_interval = -1;
    if (argc == 3) {
      if (sscanf(argv[2], "%d", &out->list.header_interval) != 1) {
        out->mode = MODE_INCORRECT;
      }
    }
    if (argc > 3) {
      out->mode = MODE_INCORRECT;
    }
  } else if (argc == 3 && strcmp(argv[1], "open") == 0) {
    out->mode = MODE_OPEN;
    out->open.device_path = argv[2];
  } else {
    out->mode = MODE_INCORRECT;
  }
}
