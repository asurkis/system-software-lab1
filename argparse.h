#ifndef _ARGUMENTS_H_
#define _ARGUMENTS_H_

struct command_line_args {
  enum ExecutionMode { MODE_INCORRECT = 0, MODE_LIST, MODE_OPEN } mode;
  union {
    struct {
      int header_interval;
    } list;
    struct {
      char *device_path;
    } open;
  };
};

void print_usage(char *exe_path);
void argparse(struct command_line_args *out, int argc, char **argv);

#endif
