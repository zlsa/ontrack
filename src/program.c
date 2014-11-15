
#include "log.h"
#include "util.h"
#include "block.h"
#include "window.h"
#include "main.h"

#include "program.h"

#include <stdio.h>

#include <string.h>

/* BLOCK */

extern struct program_b *program;

struct program_b *program_new(void) {
  struct program_b *program=MALLOC(sizeof(struct program_b));
  program->references=0;

  program->argc=0;
  program->argv=NULL;
  program->exit_code=EXIT_SUCCESS;

  program->debug=0;
  program->test=false;

  program->quiet=0;

  program->help=false;
  program->version=false;

  program->config=config_new();
  program->window=window_new();

  return(program_reference(program));
}

struct program_b *program_reference(struct program_b *program) {
  ASSERT(program);
  program->references++;
  return(program);
}

bool program_free(struct program_b *program) {
  ASSERT(program);
  program->references--;
  if(program->references <= 0) {
    if(program->config) config_free(program->config);
    if(program->window) window_free(program->window);
    FREE(program);
    return(true);
  }
  return(false);
}

/* VERSION/USAGE */

void program_version(void) {
  log_info(NAME " version " VERSION);
}

void program_usage(void) {
  printf("Usage: %s <options>\n", program->argv[0]);
  printf("Options:\n");
  printf("     --test             test all subsystems and exit\n");
  printf("     --verbose          log more (repeat for more effect)\n");
  printf("     --debug            log debug information (repeat for more effect)\n");
  printf("  -q --quiet            log less (repeat for more effect)\n");
  printf("  -v --version          show version\n");
  printf("  -h --help             this help\n");
}

/* ARGUMENT PARSING */

bool program_parse(void) {
  int i=0; // the current argument
  char *arg=NULL;
  int mode=PROGRAM_PARSE_MODE_STRING;
  for(i=1;i<program->argc;i++) {
    arg=program->argv[i];
    if((strlen(arg) >= 2) && (arg[0] == '-') && (arg[1] == '-')) {
      mode=PROGRAM_PARSE_MODE_LONG;
      arg+=2;
    } else if((strlen(arg) > 1) && (arg[0] == '-')) {
      mode=PROGRAM_PARSE_MODE_SHORT;
      arg+=1;
    } else {
      mode=PROGRAM_PARSE_MODE_STRING;
    }
    if(mode == PROGRAM_PARSE_MODE_LONG) {
      if(strncmp(arg, "version", strlen(arg)+1) == 0) {
        program->version=true;
      } else if(strncmp(arg, "help", strlen(arg)+1) == 0) {
        program->help=true;
      } else if(strncmp(arg, "verbose", strlen(arg)+1) == 0) {
        program->quiet--;
      } else if(strncmp(arg, "quiet", strlen(arg)+1) == 0) {
        program->quiet++;
      } else if(strncmp(arg, "debug", strlen(arg)+1) == 0) {
        program->debug++;
      } else if(strncmp(arg, "test", strlen(arg)+1) == 0) {
        program->test=true;
      } else {
        log_warn("invalid long option '%s'", arg);
      }
    } else if(mode == PROGRAM_PARSE_MODE_SHORT) {
      unsigned int a;
      char option;
      for(a=0;a<strlen(arg);a++) {
        option=arg[a];
        if(option == 'd') {
          program->debug++;
        } else if(option == 'v') {
          program->version=true;
        } else if(option == 'h') {
          program->help=true;
        } else if(option == 'q') {
          program->quiet++;
        } else {
          log_warn("invalid short option '%c'", option);
        }
      }
    } else {
      log_warn("invalid argument '%s'", arg);
    }
  }
  if(program->version) program_version();
  if(program->help) program_usage();
  if(program->version ||program->help) EXIT(EXIT_SUCCESS);
  if(program->test) {
    log_notice("in testing mode");
  }
  if(program->test && program->quiet > 0) {
    log_notice("in testing mode,  quiet is ignored");
    program->quiet=0;
  }
  if(program->debug && program->quiet > 0) {
    log_notice("in debug mode,  quiet is ignored");
    program->quiet=0;
  }
  return(true);
}

/* INIT */

bool program_start(void) {
  int width       = 1024;
  int height      = 768;
  bool fullscreen = false;
  config_read(program->config, "/usr/share/aurel/config", CONFIG_SOURCE_SYSTEM);
  config_read(program->config, "/home/forest/.config/aurel/config", CONFIG_SOURCE_USER);

  struct config_item_b *item = config_get_item(program->config, "window.width");
  if(item) {
    width=config_get_item_int(item);
  }

  item = config_get_item(program->config, "window.height");
  if(item) {
    height=config_get_item_int(item);
  }

  item = config_get_item(program->config, "window.fullscreen");
  if(item) {
    fullscreen=config_get_item_bool(item);
  }

  window_start();
  window_set_size(program->window, width, height);
  window_set_fullscreen(program->window, fullscreen);
  window_set_title(program->window, "aurel");
  window_open(program->window);

  return(true);
}

void program_end(void) {
  window_end();
}
