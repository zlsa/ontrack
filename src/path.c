
#include "util.h"
#include "log.h"

#include "path.h"

#include <stdlib.h>
#include <string.h>

struct path_b *path_new(void) {
  struct path_b *path = MALLOC(sizeof(struct path_b));

  path->references = 0;

  path->items           = MALLOC(1);

  path->items_used      = 0;
  path->items_allocated = 0;

  return(path_reference(path));
}

struct path_b *path_reference(struct path_b *path) {
  ASSERT(path);

  path->references++;
  return(path);
}

bool path_free(struct path_b *path) {
  ASSERT(path);

  path->references--;

  if(path->references == 0) {

    int i;

    for(i=0; i<path->items_used; i++) {
      FREE(path->items[i]);
    }

    FREE(path->items);
    FREE(path);
  }

  return(true);
}

// ADD

bool path_add(struct path_b *path, char *pathname) {
  ASSERT(path);

  if(path->items_used + 1 > path->items_allocated) {
    if(path->items_allocated == 0) path->items_allocated  = PATH_CHUNK_SIZE;
    else                     path->items_allocated *= 2;

    path->items = REALLOC(path->items, path->items_allocated * sizeof(char *));
  }

  int len = strlen(pathname);

  path->items[path->items_used] = MALLOC(len + 1);
  strncpy(path->items[path->items_used], pathname, len + 1);

  path->items_used++;

  return(true);
}

char *path_to_string(struct path_b *path) {
  ASSERT(path);

  int i;

  int size = CHUNK_SIZE;
  char *s  = MALLOC(size + 1);
  int len;
  int sp = 0;

  for(i=0; i<path->items_used; i++) {
    len = strlen(path->items[i]);

    if(len < size + 2) {
      size *= 2;
      s     = REALLOC(s, size + 1);
    }

    strncpy(s + sp, path->items[i], len);

    sp += len + 1;

    s[sp - 1] = ':';
  }

  s[sp - 1] = '\0';

  return(s);
}

void path_vomit(struct path_b *path) {
  int i;

  log_vomit("-- START PATH --");

  for(i=0; i<path->items_used; i++) {
    log_vomit("%s", path->items);
  }

  log_vomit("--- END PATH ---");

}
