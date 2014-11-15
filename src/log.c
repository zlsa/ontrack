
#include "util.h"
#include "program.h"
#include "log.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
 
extern struct program_b *program;

void log_test(const char* format,...) {
  if(program && !program->test) return;
  va_list args;
  fprintf(stdout,"[\033[35m\033[03m TEST   \033[00m]\033[03m ");
  va_start(args,format);
  vfprintf(stdout,format,args);
  va_end(args);
  fprintf(stdout,"\033[00m\n");
}

void log_debug(const char* format,...) {
  if(program && !program->debug) return;
  va_list args;
  fprintf(stdout,"[\033[03m DEBUG  \033[00m]\033[03m ");
  va_start(args,format);
  vfprintf(stdout,format,args);
  va_end(args);
  fprintf(stdout,"\033[00m\n");
}

void log_info(const char* format,...) {
  va_list args;
  fprintf(stdout,"[ INFO   ] ");
  va_start(args,format);
  vfprintf(stdout,format,args);
  va_end(args);
  fprintf(stdout,"\n");
}

void log_notice(const char* format,...) {
  va_list args;
  fprintf(stdout,"[\033[36m NOTICE \033[00m] ");
  va_start(args,format);
  vfprintf(stdout,format,args);
  va_end(args);
  fprintf(stdout,"\n");
}

void log_warn(const char* format,...) {
  va_list args;
  fprintf(stderr,"[\033[33m WARN   \033[00m] ");
  va_start(args,format);
  vfprintf(stderr,format,args);
  va_end(args);
  fprintf(stdout,"\n");
}

void log_never(const char* format,...) {
  va_list args;
  fprintf(stderr,"[\033[01m NEVER  \033[00m] ");
  va_start(args,format);
  vfprintf(stderr,format,args);
  va_end(args);
  fprintf(stderr,"\n");
}

void log_fatal(const char* format,...) {
  va_list args;
  fprintf(stderr,"[\033[31m FATAL  \033[00m] ");
  va_start(args,format);
  vfprintf(stderr,format,args);
  va_end(args);
  fprintf(stderr,"\n");
  EXIT(EXIT_FAILURE);
}
