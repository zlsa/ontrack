
#ifndef UTIL_H
#define UTIL_H

/* INCLUDES */
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>

/* DEFINES */
#define S(n) (n==1?"":"s")
#define SS(n,s,m) (n==1?s:m)
#define ASSERT(e) (assert(e))

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#define BTOS(b) (b?"true":"false")
// boolean to fail
#define BTOF(b) (b?"   ":"=> ")

#define RAD_TO_DEG(rad) ((rad / M_PI) * 180.0)
#define DEG_TO_RAD(deg) ((deg / 180.0) * M_PI)

#define CHUNK_START            (256)

#define RETURN_SUCCESS         (0)
#define RETURN_FAILURE_EOF     (1)
#define RETURN_FAILURE_PARSE   (2)
#define RETURN_FAILURE_NEWLINE (3)

#define PRINT_GL_ERROR(exit) (print_gl_error(exit))

/* FUNCTIONS */
void *MALLOC(size_t size);
void *REALLOC(void *ptr,size_t size);
void *FREE(void *ptr);

void EXIT(int code);

void print_gl_error(bool exit);
void clear_gl_error();

#endif
