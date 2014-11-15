
#ifndef WINDOW_H
#define WINDOW_H

/* INCLUDES */
#include <GLFW/glfw3.h>
#include <stdbool.h>

/* BLOCKS */
struct window_b {
  // block stuff...
  int references;

  GLFWwindow *window;

  bool open;

  int width;
  int height;

  bool fullscreen;
  
  char *title;
  
};

/* FUNCTIONS */
// block
struct window_b *window_new(void);
struct window_b *window_reference(struct window_b *window);
bool window_free(struct window_b *window);

// open/close
bool window_open(struct window_b *window);
bool window_close(struct window_b *window);

void window_set_fullscreen(struct window_b *window,bool fullscreen);

void window_set_size(struct window_b *window,int width,int height);
void window_get_size(struct window_b *window,int *width,int *height);

void window_set_title(struct window_b *window,char *title);
char *window_get_title(struct window_b *window);

void window_start(void);
void window_end(void);


#endif
