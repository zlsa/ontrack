
#ifndef PATH_H
#define PATH_H

/* INCLUDES */
#include <stdbool.h>

/* DEFINES */

#define PATH_CHUNK_SIZE  (8)

/* PATHS */
struct path_b {
  int references;

  char **items;

  int items_used;
  int items_allocated;
};

/* FUNCTIONS */
struct path_b *path_new(void);
struct path_b *path_reference(struct path_b *path);
bool path_free(struct path_b *path);

// add
bool path_add(struct path_b *path, char *filename);
char *path_to_string(struct path_b *path);

void path_vomit(struct path_b *path);
#endif
