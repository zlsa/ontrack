
#include "util.h"

#include "block.h"

struct block_b *block_new(void) {
  struct block_b *block=MALLOC(sizeof(struct block_b));
  block->references=0;
  return(block_reference(block));
}

struct block_b *block_reference(struct block_b *block) {
  ASSERT(block);
  block->references++;
  return(block);
}

bool block_free(struct block_b *block) {
  ASSERT(block);
  block->references--;
  return(true);
}
