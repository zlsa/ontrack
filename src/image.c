
#include "util.h"
#include "image.h"
#include "log.h"

#include <webp/decode.h>

struct image_b *image_new(void) {
  struct image_b *image = MALLOC(sizeof(struct image_b));

  image->references =  0;

  image->width      = -1;
  image->width      = -1;

  image->error      = IMAGE_ERROR_NONE;

  image->data       = NULL;
  image->file       = file_new();

  return(image_reference(image));
}

struct image_b *image_reference(struct image_b *image) {
  ASSERT(image);

  image->references++;

  return(image);
}

bool image_free(struct image_b *image) {
  ASSERT(image);

  image->references--;

  if(image->references == 0) {
    file_free(image->file);
    if(image->data)
      FREE(image->data);
    FREE(image);
  }

  return(true);
}

////////////////////////////////////////
// OPEN
////////////////////////////////////////

bool image_open(struct image_b *image, char *filename) {
  ASSERT(image);
  ASSERT(filename);

  if(image->file->open)
    file_close(image->file);

  if(!file_open(image->file, filename, FILE_MODE_READ)) {
    image->error = IMAGE_ERROR_FILE;
    return(false);
  }

  image->data = file_read_all_binary(image->file, &image->data_length);

  VP8StatusCode error = WebPGetInfo(image->data, (size_t) &image->data_length, &image->width, &image->height);

  if(error != 0) {
    image->error = IMAGE_ERROR_HEADER;

    log_warn("%d", error);

    switch(error) {
    case VP8_STATUS_OUT_OF_MEMORY:
      log_warn("VP8_STATUS_OUT_OF_MEMORY while reading %s", filename);
      break;
    case VP8_STATUS_INVALID_PARAM:
      log_warn("VP8_STATUS_INVALID_PARAM while reading %s", filename);
      break;
    case VP8_STATUS_BITSTREAM_ERROR:
      log_warn("VP8_STATUS_BITSTREAM_ERROR while reading %s", filename);
      break;
    case VP8_STATUS_UNSUPPORTED_FEATURE:
      log_warn("VP8_STATUS_UNSUPPORTED_FEATURE while reading %s", filename);
      break;
    case VP8_STATUS_SUSPENDED:
      log_warn("VP8_STATUS_SUSPENDED while reading %s", filename);
      break;
    case VP8_STATUS_USER_ABORT:
      log_warn("VP8_STATUS_USER_ABORT while reading %s", filename);
      break;
    case VP8_STATUS_NOT_ENOUGH_DATA:
      log_warn("VP8_STATUS_NOT_ENOUGH_DATA while reading %s", filename);
      break;
    default:
      log_never("WebP status %d while reading %s", filename);
      break;
    }
    return(false);
  }

  return(true);
}

bool image_test(void) {
  log_test("   == IMAGE ===================================================");

  bool status;
  bool passed;

  struct image_b *image = image_new();
  char *filename = "test/image.webp";

  if(image) status = true;
  log_test("%sstruct image_b *image = image_new()", BTOF(status));
  if(!status) passed = false;

  status = image_open(image, filename);
  log_test("%simage_open(image, \"%s\")", BTOF(status), filename);
  if(!status) passed = false;

  if(status) {

    log_test("%s// image size: %dx%d", BTOF(true), image->width, image->height);

  }

  if(passed) log_test("%simage passed", BTOF(passed));
  else       log_test("%simage failed", BTOF(passed));

  image_free(image);

  return(passed);
}
