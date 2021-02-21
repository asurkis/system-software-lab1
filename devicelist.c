#include "devicelist.h"
#include <stdio.h>
#include <stdlib.h>

void print_devices(int HeaderInterval) {
  FILE *f = fopen("/proc/mounts", "r");
  if (!f) {
    return;
  }

  int line = 0;
  int successfulAssigns;
  do {
    char *device, *mountpoint, *filesystem, *mode;
    successfulAssigns = fscanf(f, "%ms%ms%ms%ms%*[^\n]", &device, &mountpoint,
                               &filesystem, &mode);

    if (successfulAssigns == 4) {
      if (HeaderInterval >= 0) {
        if (line == 0) {
          puts("\nDevice\tMount point\tFilesystem\tMode");
        }
        if (++line == HeaderInterval) {
          line = 0;
        }
      }
      printf("%s\t%s\t%s\t%s\n", device, mountpoint, filesystem, mode);
    }

    if (successfulAssigns >= 1) {
      free(device);
    }
    if (successfulAssigns >= 2) {
      free(mountpoint);
    }
    if (successfulAssigns >= 3) {
      free(filesystem);
    }
    if (successfulAssigns >= 4) {
      free(mode);
    }
  } while (successfulAssigns > 0);

  fclose(f);
}
