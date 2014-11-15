
#include "util.h"

#include "log.h"
#include "program.h"

extern struct program_b *program;

int memory_blocks=0;
int memory_blocks_total=0;

void *MALLOC(size_t size) {
  if(size == 0) log_warn("allocating zero bytes");
  void *ptr=malloc(size);
  if(!ptr && size) log_fatal("out of memory; requested %d byte%s",size,S(size));
  memory_blocks++;
  memory_blocks_total++;
  return(ptr);
}

void *REALLOC(void *ptr,size_t size) {
  if(size == 0) log_warn("reallocating zero bytes");
  ptr=realloc(ptr,size);
  if(!ptr && size) log_fatal("out of memory; requested a total of %d byte%s",size,S(size));
  return(ptr);
}

void *FREE(void *ptr) {
  assert(ptr);
  free(ptr);
  memory_blocks--;
  return(NULL);
}

void EXIT(int code) {
  if(program) program->exit_code=code;
  exit(code);
}

void print_gl_error(bool exit) {
  GLenum e;
  bool fatal = false;
  int i = 0;
  while((e = glGetError()) != GL_NO_ERROR) {
    switch(e) {
    case GL_INVALID_ENUM:
      log_warn("GL_INVALID_ENUM");
      break;
    case GL_INVALID_VALUE:
      log_warn("GL_INVALID_VALUE");
      break;
    case GL_INVALID_OPERATION:
      log_warn("GL_INVALID_OPERATION");
      break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
      log_warn("GL_INVALID_FRAMEBUFFER_OPERATION");
      break;
    case GL_OUT_OF_MEMORY:
      log_warn("GL_OUT_OF_MEMORY");
      fatal = true;
      break;
    case GL_STACK_OVERFLOW:
      log_warn("GL_STACK_OVERFLOW");
      fatal = true;
      break;
    case GL_STACK_UNDERFLOW:
      log_warn("GL_STACK_UNDERFLOW");
      fatal = true;
      break;
    default:
      log_warn("unknown OpenGL error %d", e);
      break;
    }
    i++;
  }
  if(fatal && exit) {
    log_fatal("encountered OpenGL errors, exiting");
  }
  if(i > 1) {
    log_warn("encountered OpenGL errors, ignoring");
  }
}

void clear_gl_error() {
  while(glGetError() != GL_NO_ERROR);
}
