
#include "log.h"
#include "util.h"
#include "block.h"
#include "window.h"
#include "main.h"

#include "program.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* BLOCK */

extern struct program_b *program;

struct program_b *program_new(void) {
  struct program_b *program=MALLOC(sizeof(struct program_b));
  program->references = 0;

  program->argc = 0;
  program->argv = NULL;
  program->exit_code = EXIT_SUCCESS;

  program->debug   = 0;
  program->test    = false;

  program->quiet   = 0;

  program->help    = false;
  program->version = false;

  program->config  = config_new();
  program->window  = window_new();

  program->state   = PROGRAM_STATE_STARTUP;

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
  printf("     --vomit            log ALL THE THINGS (same as --debug --debug)\n");
  printf("  -q --quiet            log less (repeat for more effect)\n");
  printf("  -v --version          show version\n");
  printf("  -h --help             this help\n");
}

/* ARGUMENT PARSING */

bool program_parse(void) {
  int i=0; // the current argument
  char *arg=NULL;
  int mode=PROGRAM_PARSE_MODE_STRING;
  for(i=1; i<program->argc; i++) {
    arg = program->argv[i];
    if((strlen(arg) >= 2) && (arg[0] == '-') && (arg[1] == '-')) {
      mode  = PROGRAM_PARSE_MODE_LONG;
      arg  += 2;
    } else if((strlen(arg) > 1) && (arg[0] == '-')) {
      mode  = PROGRAM_PARSE_MODE_SHORT;
      arg  += 1;
    } else {
      mode  = PROGRAM_PARSE_MODE_STRING;
    }
    if(mode == PROGRAM_PARSE_MODE_LONG) {
      if(strncmp(arg, "version", strlen(arg)+1) == 0) {
        program->version = true;
      } else if(strncmp(arg, "help", strlen(arg)+1) == 0) {
        program->help    = true;
      } else if(strncmp(arg, "verbose", strlen(arg)+1) == 0) {
        program->quiet--;
      } else if(strncmp(arg, "quiet", strlen(arg)+1) == 0) {
        program->quiet++;
      } else if(strncmp(arg, "debug", strlen(arg)+1) == 0) {
        program->debug++;
      } else if(strncmp(arg, "vomit", strlen(arg)+1) == 0) {
        program->debug = 2;
      } else if(strncmp(arg, "test", strlen(arg)+1) == 0) {
        program->test    = true;
      } else {
        log_warn("invalid long option '%s'", arg);
      }
    } else if(mode == PROGRAM_PARSE_MODE_SHORT) {
      unsigned int a;
      char option;
      for(a=0; a<strlen(arg); a++) {
        option = arg[a];
        if(option == 'd') {
          program->debug++;
        } else if(option == 'v') {
          program->version = true;
        } else if(option == 'h') {
          program->help    = true;
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
  if(program->version || program->help) EXIT(EXIT_SUCCESS);
  if(program->test) {
    log_notice("in testing mode");
  }
  if(program->test && program->quiet > 0) {
    log_notice("in testing mode, quiet is ignored");
    program->quiet = 0;
  }
  if(program->debug && program->quiet > 0) {
    log_notice("in debug mode, quiet is ignored");
    program->quiet = 0;
  }
  return(true);
}

/* INIT */

bool program_start(void) {
  int width       = 1024;
  int height      = 768;
  bool fullscreen = false;

  // read system-wide config
  config_read(program->config, "/usr/share/ontrack/config",           CONFIG_SOURCE_SYSTEM);

  // read local config
  char *home             = getenv("HOME");
  char *home_config_path = "/.config/ontrack/config";
  char *home_config      = NULL;

  if(!home) {
    log_warn("environment variable HOME is not defined");
  } else {
    home_config      = MALLOC(strlen(home) + strlen(home_config_path) + 1);
    strncpy(home_config, home, strlen(home));
    strncpy(home_config + strlen(home), home_config_path, strlen(home_config_path) + 1);

    config_read(program->config, home_config, CONFIG_SOURCE_USER);

    FREE(home_config);
  }

  struct config_item_b *item;

  // debug/test options

  if((item = config_get_item(program->config, "option.debug")) != NULL)
    program->debug = config_get_item_int(item);

  if((item = config_get_item(program->config, "option.vomit")) != NULL)
    program->debug = (config_get_item_bool(item) ? 2 : program->debug);

  // window opening

  if((item = config_get_item(program->config, "window.width")) != NULL)
    width = config_get_item_int(item);

  if((item = config_get_item(program->config, "window.height")) != NULL)
    height = config_get_item_int(item);

  if((item = config_get_item(program->config, "window.fullscreen")) != NULL)
    fullscreen = config_get_item_bool(item);

  window_start();
  window_set_size(program->window, width, height);
  window_set_fullscreen(program->window, fullscreen);
  window_set_title(program->window, "onTrack");
  window_open(program->window);

  config_vomit(program->config);

  program->state = PROGRAM_STATE_LOADING;

  program->state = PROGRAM_STATE_RUNNING;

  return(true);
}

void program_end(void) {
  log_vomit("program end");

  window_end();

  if(program->state == PROGRAM_STATE_RUNNING) {
    config_write(program->config, "/home/forest/.config/ontrack/config", CONFIG_SOURCE_USER | CONFIG_SOURCE_RUNTIME);
  }

  program->state = PROGRAM_STATE_SHUTDOWN;
}
