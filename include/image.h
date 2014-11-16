
#ifndef IMAGE_H
#define IMAGE_H

/* INCLUDES */
#include "file.h"
#include <stdbool.h>

/* DEFINES */
#define IMAGE_ERROR_NONE     (0)
#define IMAGE_ERROR_FILE     (1)
#define IMAGE_ERROR_HEADER   (2)
#define IMAGE_ERROR_DECODE   (3)

/* IMAGES */
struct image_b {
  int references;

  int width;
  int height;

  int error;

  uint8_t *data;
  size_t data_length;
  struct file_b *file;
};

/* FUNCTIONS */
struct image_b *image_new(void);
struct image_b *image_reference(struct image_b *image);
bool image_free(struct image_b *image);

bool image_open(struct image_b *image, char *filename);
bool image_test(void);

#endif
