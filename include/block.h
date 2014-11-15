
#ifndef BLOCK_H
#define BLOCK_H

/* INCLUDES */
#include <stdbool.h>

/* BLOCKS */
struct block_b {
  int references;
};

/* FUNCTIONS */
struct block_b *block_new(void);
struct block_b *block_reference(struct block_b *block);
bool block_free(struct block_b *block);

#endif
