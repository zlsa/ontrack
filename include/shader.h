
#ifndef SHADER_H
#define SHADER_H

/* INCLUDES */
#include <GLFW/glfw3.h>
#include <stdbool.h>
#include "file.h"

/* DEFINES */
#define SHADER_UNIFORM_CHUNK (4)

/* BLOCKS */
struct shader_b {
  int references;

  // program

  GLuint program;

  // shaders

  GLuint vertex;
  GLuint fragment;

  char *vertex_source;
  char *fragment_source;

  int   vertex_source_length;
  int   fragment_source_length;

  GLuint *uniforms;
  char **uniform_names;
  int uniform_allocated;
  int uniform_used;
  
};

//  time=glGetUniformLocation(shader_program,"time");
//  glUniform1f(time,glfwGetTime());

/* FUNCTIONS */
struct shader_b *shader_new(void);
struct shader_b *shader_reference(struct shader_b *shader);
bool shader_free(struct shader_b *shader);

bool shader_vertex_file(struct shader_b *shader, struct file_b *file);
bool shader_fragment_file(struct shader_b *shader, struct file_b *file);

bool shader_compile_vertex(struct shader_b *shader);
bool shader_compile_fragment(struct shader_b *shader);
bool shader_compile(struct shader_b *shader);

bool shader_use(struct shader_b *shader);
bool shader_draw(struct shader_b *shader);

// uniforms
int shader_uniform_new(struct shader_b *shader, char *name);
int shader_uniform_get(struct shader_b *shader, char *name);
bool shader_uniform_set_float(struct shader_b *shader, char *name, float value);
bool shader_uniform_set_int(struct shader_b *shader, char *name, int value);
bool shader_uniform_set_m4(struct shader_b *shader, char *name, float *value);
#endif
