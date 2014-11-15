
#include <GL/glew.h>

#include "util.h"
#include "log.h"

#include "window.h"

#include <string.h>

struct window_b *window_new(void) {
  struct window_b *window=MALLOC(sizeof(struct window_b));
  window->references=0;

  window->window=NULL;

  window->open=false;
  
  window->width=600;
  window->height=400;

  window->fullscreen=false;

  window->title=NULL;

  return(window_reference(window));
}

struct window_b *window_reference(struct window_b *window) {
  ASSERT(window);
  window->references++;
  return(window);
}

bool window_free(struct window_b *window) {
  ASSERT(window);
  window->references--;
  if(window->references == 0) {
    if(window->open) window_close(window);
    if(window->title) FREE(window->title);
    FREE(window);
  }
  return(true);
}

/* OPEN/CLOSE */

bool window_open(struct window_b *window) {
  ASSERT(window);
  ASSERT(window->title);
  if(window->open) {
    log_warn("tried to open an open window");
    return(false);
  }

  GLFWmonitor* monitor=glfwGetPrimaryMonitor();
  if(!window->fullscreen) {
    monitor=NULL;
  } else {
    const GLFWvidmode* mode=glfwGetVideoMode(monitor);
    window_set_size(window,mode->width,mode->height);
    log_info("fullscreen mode selected; setting window size to %dx%d",mode->width,mode->height);
  }

  glfwWindowHint(GLFW_SAMPLES, 16);

  glfwSwapInterval(1);

  window->window=glfwCreateWindow(window->width,window->height,window->title,monitor,NULL);

  if(!window->window) {
    window->window=NULL;
    log_fatal("could not open GLFW window %dx%d",window->width,window->height);
  }

  glfwMakeContextCurrent(window->window);

  GLenum ret = glewInit();

  if(ret != GLEW_OK) {
    log_fatal("could not initialize GLEW: %d", glewGetErrorString(ret));
  }

  if(!GLEW_VERSION_3_3) {
    log_fatal("OpenGL 3.3 or newer required");
  }

  window->open=true;

  return(true);
}

bool window_close(struct window_b *window) {
  ASSERT(window);
  ASSERT(window->window);
  if(!window->open) {
    log_warn("tried to close a closed window");
    return(false);
  }
  glfwDestroyWindow(window->window);
  window->window=NULL;
  window->open=false;
  return(true);
}

/* OPTIONS */

void window_set_fullscreen(struct window_b *window,bool fullscreen) {
  ASSERT(window);
  window->fullscreen=fullscreen;
}

void window_set_size(struct window_b *window,int width,int height) {
  ASSERT(window);
  if(width <= 0) width=1;
  if(height <= 0) height=1;
  window->width=width;
  window->height=height;
  if(window->window) {
    glfwSetWindowSize(window->window,width,height);
  }
}

void window_get_size(struct window_b *window,int *width,int *height) {
  ASSERT(window);
  if(window->window) {
    glfwGetWindowSize(window->window,width,height);
  } else {
    *width=window->width;
    *height=window->height;
  }
}

// caller must free arguments
void window_set_title(struct window_b *window,char *title) {
  ASSERT(window);
  if(window->title) FREE(window->title);
  window->title=MALLOC(strlen(title)+1);
  strncpy(window->title,title,strlen(title)+1);
}

// caller must NOT FREE!!!1!!111!!!
char *window_get_title(struct window_b *window) {
  ASSERT(window);
  return(window->title);
}

/* START/END */

void window_start(void) {
  if(!glfwInit()) log_fatal("could not initialize GLFW");
}

void window_end(void) {
  glfwTerminate();
}
