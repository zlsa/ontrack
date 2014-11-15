
#include <GL/glew.h>

#include "log.h"
#include "util.h"
#include "file.h"

#include "shader.h"

#include <string.h>

struct shader_b *shader_new(void) {
  struct shader_b *shader=MALLOC(sizeof(struct shader_b));
  shader->references    = 0;

  shader->vertex        = 0;
  shader->fragment      = 0;

  shader->vertex_source   = NULL;
  shader->fragment_source = NULL;

  shader->vertex_source_length   = 0;
  shader->fragment_source_length = 0;

  shader->uniforms = NULL;
  shader->uniform_names = NULL;
  shader->uniform_allocated = 0;
  shader->uniform_used = 0;

  if((shader->program = glCreateProgram()) == 0) {
    print_gl_error(false);
    log_fatal("could not create shader program.");
  }

  shader->vertex   = glCreateShader(GL_VERTEX_SHADER);
  shader->fragment = glCreateShader(GL_FRAGMENT_SHADER);

  return(shader_reference(shader));
}

struct shader_b *shader_reference(struct shader_b *shader) {
  ASSERT(shader);
  shader->references++;
  return(shader);
}

bool shader_free(struct shader_b *shader) {
  ASSERT(shader);
  shader->references--;
  if(shader->references == 0) {
    if(shader->vertex_source)   FREE(shader->vertex_source);
    if(shader->fragment_source) FREE(shader->fragment_source);

    if(shader->vertex) {
      glDetachShader(shader->program, shader->vertex);
      glDeleteShader(shader->vertex);
    }
    if(shader->fragment) {
      glDetachShader(shader->program, shader->fragment);
      glDeleteShader(shader->fragment);
    }

    if(shader->uniform_allocated > 0) {
      if(shader->uniform_names) FREE(shader->uniform_names);
      if(shader->uniforms) FREE(shader->uniforms);
    }

    if(shader->program) glDeleteProgram(shader->program);
    FREE(shader);
  }
  return(true);
}

bool shader_vertex_file(struct shader_b *shader, struct file_b *file) {
  char *content = file_read_all(file);

  shader->vertex_source        = content;
  shader->vertex_source_length = strlen(content) - 1;

  return(true);
}

bool shader_fragment_file(struct shader_b *shader, struct file_b *file) {
  char *content = file_read_all(file);

  shader->fragment_source        = content;
  shader->fragment_source_length = strlen(content) - 1;

  return(true);
}

bool shader_compile_vertex(struct shader_b *shader) {
  if(shader->vertex_source) {
    glShaderSource(shader->vertex, 1, (const GLchar**)&shader->vertex_source, &shader->vertex_source_length);

    glCompileShader(shader->vertex);

    GLint compile_status;
    glGetShaderiv(shader->vertex, GL_COMPILE_STATUS, &compile_status);

    if(!compile_status) {
      GLint log_length = -1;
      glGetShaderiv(shader->vertex, GL_INFO_LOG_LENGTH, &log_length);

      print_gl_error(false);

      log_warn("could not compile vertex shader");
      if(log_length > 0) {
        char *log = MALLOC(sizeof(char) * log_length);
        glGetShaderInfoLog(shader->vertex, log_length-1, NULL, log);

        log_warn("--[    VERTEX WARNING    ]------------------");
        printf("%s", log);
        log_warn("--[  END VERTEX WARNING  ]------------------");
        FREE(log);
      }

#ifdef DEBUG
      log_warn("--[    VERTEX SHADER SOURCE    ]------------");
      printf("%s", shader->vertex_source);
      log_warn("--[  END VERTEX SHADER SOURCE  ]------------");
#endif

      glDeleteShader(shader->vertex);
      shader->vertex = 0;

      return(false);
    }
    return(true);
  }

  return(true);
}

bool shader_compile_fragment(struct shader_b *shader) {
  if(shader->fragment_source) {
    glShaderSource(shader->fragment, 1, (const GLchar**)&shader->fragment_source, &shader->fragment_source_length);

    glCompileShader(shader->fragment);

    GLint compile_status;
    glGetShaderiv(shader->fragment, GL_COMPILE_STATUS, &compile_status);

    if(!compile_status) {
      GLint log_length = -1;
      glGetShaderiv(shader->fragment, GL_INFO_LOG_LENGTH, &log_length);

      print_gl_error(false);

      log_warn("could not compile fragment shader");
      if(log_length > 0) {
        char *log = MALLOC(sizeof(char) * log_length);
        glGetShaderInfoLog(shader->fragment, log_length-1, NULL, log);
        log_warn("--[   FRAGMENT WARNING   ]------------------");
        printf("%s", log);
        log_warn("--[ END FRAGMENT WARNING ]------------------");
        FREE(log);
      }

#ifdef DEBUG
      log_warn("--[   FRAGMENT SHADER SOURCE   ]------------");
      printf("%s", shader->fragment_source);
      log_warn("--[ END FRAGMENT SHADER SOURCE ]------------");
#endif

      glDeleteShader(shader->fragment);
      shader->fragment = 0;

      return(false);
    }
    return(true);
  }

  return(true);
}

bool shader_compile(struct shader_b *shader) {
  bool success = true;
  success &= shader_compile_vertex(shader);
  if(shader->vertex) {
    glAttachShader(shader->program, shader->vertex);
  }
  success &= shader_compile_fragment(shader);
  if(shader->fragment) {
    glAttachShader(shader->program, shader->fragment);
  }

  glLinkProgram(shader->program);
  
  GLint link_status;
  glGetProgramiv(shader->program, GL_LINK_STATUS, &link_status);

  if(!link_status) {
    GLint log_length = -1;
    glGetProgramiv(shader->program, GL_INFO_LOG_LENGTH, &log_length);

    print_gl_error(false);

    log_warn("could not link shader program");
    if(log_length > 0) {
      char *log = MALLOC(sizeof(char) * log_length);
      glGetProgramInfoLog(shader->program, log_length-1, NULL, log);
      log_warn("--[   PROGRAM WARNING    ]------------------");
      printf("%s", log);
      log_warn("--[ END PROGRAM WARNING  ]------------------");
      FREE(log);
    }

    glDeleteProgram(shader->program);
    shader->program = 0;

    return(false);
  }

  shader_use(shader);
  shader_uniform_new(shader, "u_Time");
  shader_uniform_new(shader, "u_ModelView");
  shader_uniform_new(shader, "u_Projection");

  return(success);
}

// binding

bool shader_use(struct shader_b *shader) {
  ASSERT(shader);
  glUseProgram(shader->program);
  return(true);
}

// call before each frame

bool shader_draw(struct shader_b *shader) {
  if(!shader_use(shader)) return(false);

  GLfloat matrix[16] = {0};

  shader_uniform_set_float(shader, "u_Time", glfwGetTime());

  glGetFloatv(GL_MODELVIEW_MATRIX, matrix);

  shader_uniform_set_m4(shader, "u_ModelView", matrix);

  glGetFloatv(GL_PROJECTION_MATRIX, matrix);
  shader_uniform_set_m4(shader, "u_Projection", matrix);

  return(true);
}

//

int shader_uniform_new(struct shader_b *shader, char *name) {
  ASSERT(shader);
  if(!shader->uniforms) {
    shader->uniforms = MALLOC(sizeof(GLint) * SHADER_UNIFORM_CHUNK);
    shader->uniform_names = MALLOC(sizeof(char *) * SHADER_UNIFORM_CHUNK);
    shader->uniform_allocated = SHADER_UNIFORM_CHUNK;
    shader->uniform_used = 0;
  }
  if(shader->uniform_used == shader->uniform_allocated) {
    shader->uniform_allocated *= 2;
    shader->uniforms = REALLOC(shader->uniforms, sizeof(GLint) * (shader->uniform_allocated));
    shader->uniform_names = REALLOC(shader->uniform_names, sizeof(char *) * (shader->uniform_allocated));
  }

  GLint location = glGetUniformLocation(shader->program, name);

  if(location == -1) {
    print_gl_error(true);
    log_warn("could not find location of uniform variable '%s'", name);
    return(-1);
  } else {
    shader->uniform_names[shader->uniform_used] = name;
    shader->uniforms[shader->uniform_used] = location;
    return(shader->uniform_used++);
  }
  
}

// assumes the shader program is already active
int shader_uniform_get(struct shader_b *shader, char *name) {
  ASSERT(shader);
  int i;
  if(!shader->uniforms) {
    log_warn("no uniforms set on shader");
    return(-1);
  }
  for(i=0;i<shader->uniform_used;i++) {
    if(strcmp(shader->uniform_names[i], name) == 0) return(shader->uniforms[i]);
  }
  //  log_warn("no uniform variable '%s'", name);
  return(-1);
}

bool shader_uniform_set_float(struct shader_b *shader, char *name, float value) {
  ASSERT(shader);
  int uniform = shader_uniform_get(shader, name);
  if(uniform == -1) return(false);
  glUniform1f(uniform, value);
  return(true);
}

bool shader_uniform_set_int(struct shader_b *shader, char *name, int value) {
  ASSERT(shader);
  int uniform = shader_uniform_get(shader, name);
  if(uniform == -1) return(false);
  glUniform1i(uniform, value);
  return(true);
}

bool shader_uniform_set_m4(struct shader_b *shader, char *name, float *value) {
  ASSERT(shader);
  int uniform = shader_uniform_get(shader, name);
  if(uniform == -1) return(false);
  glUniformMatrix4fv(uniform, 1, GL_FALSE, value);
  return(true);
}
