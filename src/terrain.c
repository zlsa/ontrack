
#include <GL/glew.h>

#include "util.h"
#include "log.h"
#include "shader.h"

#include "terrain.h"

struct terrain_b *terrain_new(void) {
  struct terrain_b *terrain = MALLOC(sizeof(struct terrain_b));
  terrain->references = 0;

  terrain->object     = object_new();

  struct shader_b *shader = shader_new();

  struct file_b *file = file_new();
  char *filename      = "../data/shaders/terrain.vert";

  if(file_open(file, filename, FILE_MODE_READ)) {
    shader_vertex_file(shader, file);
    file_close(file);
  } else {
    log_warn("could not open terrain vert shader '%s' for reading: %s", filename, file_error(file));
  }

  filename = "../data/shaders/terrain.frag";
  if(file_open(file, filename, FILE_MODE_READ)) {
    shader_fragment_file(shader, file);
    file_close(file);
  } else {
    log_warn("could not open terrain frag shader '%s' for reading: %s", filename, file_error(file));
  }

  file_free(file);

  shader_compile(shader);

  shader_uniform_new(shader, "u_Detail");

  object_set_shader(terrain->object, shader);
  
  return(terrain_reference(terrain));
}

struct terrain_b *terrain_reference(struct terrain_b *terrain) {
  ASSERT(terrain);
  terrain->references++;
  return(terrain);
}

bool terrain_free(struct terrain_b *terrain) {
  ASSERT(terrain);
  terrain->references--;
  if(terrain->references == 0) {
    shader_free(terrain->object->shader);
    object_free(terrain->object);
    FREE(terrain);
  }
  return(true);
}

bool terrain_generate_object(struct terrain_b *terrain) {
  float size = 1;
  float resolution = 1.0;
  int number = floor(size / resolution / 2);
  int quad_number = (number+1) * (number+1);
  int vertex_number = quad_number * 4;

  log_info("terrain vertices: %d", vertex_number);
  
  float *vertices = MALLOC(sizeof(float) * vertex_number * 3);

  int x = 0;
  int y = 0;
  int i = 0;

  for(i=0;i<quad_number;i++) {

    vertices[i*12+0]  = (x + 0) * resolution - (size * 0.5);
    vertices[i*12+1]  = 0;
    vertices[i*12+2]  = (y + 0) * resolution - (size * 0.5);

    vertices[i*12+3]  = (x + 0) * resolution - (size * 0.5);
    vertices[i*12+4]  = 0;
    vertices[i*12+5]  = (y + 1) * resolution - (size * 0.5);

    vertices[i*12+6]  = (x + 1) * resolution - (size * 0.5);
    vertices[i*12+7]  = 0;
    vertices[i*12+8]  = (y + 1) * resolution - (size * 0.5);

    vertices[i*12+9]  = (x + 1) * resolution - (size * 0.5);
    vertices[i*12+10] = 0;
    vertices[i*12+11] = (y + 0) * resolution - (size * 0.5);

    x++;
    if(x > number) {
      x=0;
      y++;
    }
  }

  terrain->object->vertices = vertex_number;

  glBindVertexArray(terrain->object->vertex_position_array);

  glBindBuffer(GL_ARRAY_BUFFER, terrain->object->vertex_buffer);

  glBufferData(GL_ARRAY_BUFFER, vertex_number * sizeof(float) * 3, vertices, GL_STATIC_DRAW);

  PRINT_GL_ERROR(true);
  FREE(vertices);
  return(true);
}

bool terrain_draw(struct terrain_b *terrain) {

  shader_use(terrain->object->shader);

  float detail = sin(glfwGetTime()) * 10 + 20;

  shader_uniform_set_float(terrain->object->shader, "u_Detail", detail);

  object_draw(terrain->object);

  return(true);
}
