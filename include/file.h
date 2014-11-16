
#ifndef FILE_H
#define FILE_H

/* INCLUDES */
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

/* DEFINES */
#define FILE_MODE_READ   (0)
#define FILE_MODE_WRITE  (1)
#define FILE_MODE_APPEND (2)

#define FILE_UNGETC_BUFFER_SIZE (32)

#define FILE_TOKEN_START_SIZE (16)

#define FILE_PATH_CHUNK_SIZE    (128)

#define istoken(c) (isalpha(c) || c == '.' || c == '_')

/* BLOCKS */
struct file_b {
  // block_b
  int references;

  // file pointer; if !file_b.open, fp is NULL
  FILE *fp;

  // filename; if !file_b.open, filename may be NULL
  char *filename;

  // one of FILE_MODE_READ, FILE_MODE_WRITE, or FILE_MODE_APPEND
  int mode;

  // is the file currently open?
  bool open;

  // ungetc buffer
  int bufp;
  char buffer[FILE_UNGETC_BUFFER_SIZE];

  // line number
  int line;

  // end of file?
  bool eof;

  // last error
  int err;
};

/* FUNCTIONS */
// blocks
struct file_b *file_new(void);
struct file_b *file_reference(struct file_b *file);
bool file_free(struct file_b *file);

// open/close
bool file_open(struct file_b *file,char *filename,int mode);
bool file_close(struct file_b *file);

bool file_mkdirs(char *filename);
char *file_path(char *filename);

// errors
char *file_error(struct file_b *file);

// read
char *file_read_all(struct file_b *file);
uint8_t *file_read_all_binary(struct file_b *file, size_t *length);
char *file_read_all_length(struct file_b *file, size_t *length);
int file_getc(struct file_b *file);
int file_ungetc(struct file_b *file,char c);
int file_peek(struct file_b *file);

// write
int file_write(struct file_b *file, char *data);

// skip
int file_skip_whitespace(struct file_b *file);
int file_skip_whitespace_no_newline(struct file_b *file);
int file_skip_to(struct file_b *file,char stop);
int file_skip_to_ignore_comment(struct file_b *file,char stop,char comment_start);
char *file_read_to(struct file_b *file,char stop);

// tokens
char *file_get_token(struct file_b *file);
char *file_get_bool(struct file_b *file,bool *ptr);
char *file_get_int(struct file_b *file,int *ptr);;
char *file_get_double(struct file_b *file,double *ptr);
int file_get_string(struct file_b *file,char **ptr);

// testing
bool file_test(void);

#endif
