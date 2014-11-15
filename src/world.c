
#include "util.h"

#include "world.h"

struct world_b *world_new(void) {
  struct world_b *world=MALLOC(sizeof(struct world_b));
  world->references=0;

  return(world_reference(world));
}

struct world_b *world_reference(struct world_b *world) {
  ASSERT(world);
  world->references++;
  return(world);
}

bool world_free(struct world_b *world) {
  ASSERT(world);
  world->references--;
  if(world->references == 0) {
    FREE(world);
  }
  return(true);
}
