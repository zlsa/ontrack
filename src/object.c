
#include <GL/glew.h>

#include "util.h"
#include "shader.h"

#include "object.h"

struct object_b *object_new(void) {
  struct object_b *object=MALLOC(sizeof(struct object_b));
  object->references = 0;

  glGenVertexArrays(1, &object->vertex_position_array);

  glGenBuffers(1, &object->vertex_buffer);
  glGenBuffers(1, &object->normal_buffer);

  object->shader   = NULL;

  object->vertices = 0;

  return(object_reference(object));
}

struct object_b *object_reference(struct object_b *object) {
  ASSERT(object);
  object->references++;
  return(object);
}

bool object_free(struct object_b *object) {
  ASSERT(object);
  object->references--;
  if(object->references == 0) {
    if(object->shader) shader_free(object->shader);
    glDeleteVertexArrays(1, &object->vertex_position_array);
    FREE(object);
  }
  return(true);
}

bool object_set_shader(struct object_b *object, struct shader_b *shader) {
  ASSERT(object);
  ASSERT(shader);

  if(object->shader) shader_free(object->shader);
  object->shader = shader_reference(shader);

  return(true);
}

bool object_draw(struct object_b *object) {
  glBindVertexArray(object->vertex_position_array);

  shader_draw(object->shader);

  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, object->vertex_buffer);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);

  glDrawArrays(GL_QUADS, 0, object->vertices);

  glDisableVertexAttribArray(0);

  return(true);
}
