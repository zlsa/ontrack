
#ifndef OBJECT_H
#define OBJECT_H

/* INCLUDES */
#include <GLFW/glfw3.h>
#include <stdbool.h>
#include "shader.h"
//#include "texture.h"

/* DEFINES */
// vertices, normals
#define OBJECT_ARRAY_NUMBER 1

/* BLOCKS */
struct object_b {
  int references;

  struct shader_b *shader;

  //  struct texture_b *textures;
  //  int texture_allocated;
  //  int texture_used;

  GLuint vertex_position_array;

  GLuint vertex_buffer;
  GLuint normal_buffer;

  int vertices;
};

/* FUNCTIONS */
struct object_b *object_new(void);
struct object_b *object_reference(struct object_b *object);
bool object_free(struct object_b *object);

bool object_set_shader(struct object_b *object, struct shader_b *shader);

bool object_draw(struct object_b *object);

#endif
