
#ifndef WORLD_H
#define WORLD_H

/* INCLUDES */
#include <stdbool.h>

/* WORLDS */
struct world_b {
  int references;
};

/* FUNCTIONS */
struct world_b *world_new(void);
struct world_b *world_reference(struct world_b *world);
bool world_free(struct world_b *world);

#endif
