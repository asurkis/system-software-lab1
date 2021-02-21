#ifndef _ARGUMENTS_H_
#define _ARGUMENTS_H_

struct command_line_args {
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

void print_usage(char *ExePath);
void argparse(struct command_line_args *out, int argc, char **argv);

#endif
