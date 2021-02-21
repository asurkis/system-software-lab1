#ifndef _ARGUMENTS_H_
#define _ARGUMENTS_H_

struct CommandLineArgs {
  enum ExecutionMode {
    MODE_INCORRECT = 0,
    MODE_LIST,
    MODE_OPEN
  } Mode;
  union {
    struct {
      int HeaderInterval;
    } List;
    struct {
      char *DevicePath;
    } Open;
  };
};

void PrintUsage(char *ExePath);
void ParseArgs(struct CommandLineArgs *out, int argc, char **argv);

#endif
