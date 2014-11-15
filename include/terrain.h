
#ifndef TERRAIN_H
#define TERRAIN_H

/* INCLUDES */
#include <stdbool.h>

#include "object.h"

/* TERRAINS */
struct terrain_b {
  int references;

  struct object_b *object;
};

/* FUNCTIONS */
struct terrain_b *terrain_new(void);
struct terrain_b *terrain_reference(struct terrain_b *terrain);
bool terrain_free(struct terrain_b *terrain);

bool terrain_generate_object(struct terrain_b *terrain);

bool terrain_draw(struct terrain_b *terrain);

#endif
