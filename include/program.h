
#ifndef PROGRAM_H
#define PROGRAM_H

/* INCLUDES */
#include "config.h"
#include "window.h"
#include <stdbool.h>

/* DEFINES */
#define PROGRAM_PARSE_MODE_STRING (0)
#define PROGRAM_PARSE_MODE_LONG   (1)
#define PROGRAM_PARSE_MODE_SHORT  (2)

/* BLOCKS */
struct program_b {
  // block_b
  int references;

  // same as main()'s
  int argc;
  char **argv;

  // only valid after EXIT() has been called
  int exit_code;

  // debug level
  int debug;

  // test mode?
  bool test;

  // verbosity.
  // -1: print out more
  // 0 : normal
  // 1 : do not print out any meta-information or info.
  // 2 : do not print out anything
  int quiet;

  // help mode? (if this is true, program_parse() will call
  // program_usage() and exit.)
  bool help;

  // version mode? (if this is true, program_parse() will call
  // program_version() and exit.)
  bool version;

  // for config variables
  struct config_b *config;

  // for the window
  struct window_b *window;
};

/* FUNCTIONS */
struct program_b *program_new(void);
struct program_b *program_reference(struct program_b *program);
bool program_free(struct program_b *program);

// argparse/help/version
void program_version(void);
void program_usage(void);
bool program_parse(void);

// start/end
bool program_start(void);
void program_end(void);

#endif
