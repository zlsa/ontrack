
#ifndef CONFIG_H
#define CONFIG_H

/* INCLUDES */
#include "file.h"
#include <stdbool.h>

/* DEFINES */
#define CONFIG_ITEMS_START_SIZE (1)
#define CONFIG_KEY_MAX_SIZE (128)

#define CONFIG_ITEM_TYPE_NULL   (0) // empty
#define CONFIG_ITEM_TYPE_BOOL   (1) // 'true'/'false'/'yes'/'no'/'on'/'off'/1/0
#define CONFIG_ITEM_TYPE_INT    (2) // -3 -2 -1 0 1 2 3
#define CONFIG_ITEM_TYPE_DOUBLE (3) // -4.2 -3.2 -2.2 -1.2 -0.2 0.8
#define CONFIG_ITEM_TYPE_STRING (4) // "foobar"
#define CONFIG_ITEM_TYPE_PATH   (5) // "~/foo/" "$AUREL_DATA/foo/:~/.aurel/"

#define CONFIG_SOURCE_UNKNOWN (0)
#define CONFIG_SOURCE_SYSTEM  (1)
#define CONFIG_SOURCE_USER    (2)

#define CONFIG_ITEM_TYPE(t) (t==CONFIG_ITEM_TYPE_NULL?"null": \
    (t==CONFIG_ITEM_TYPE_BOOL?"bool":                         \
    (t==CONFIG_ITEM_TYPE_INT?"int":                           \
    (t==CONFIG_ITEM_TYPE_DOUBLE?"double":                     \
    (t==CONFIG_ITEM_TYPE_STRING?"string":                     \
    (t==CONFIG_ITEM_TYPE_PATH?"path":"oops"))))))

/* BLOCKS */
struct config_item_b {
  // block stuff...
  int references;

  // CONFIG_SOURCE_SYSTEM or CONFIG_SOURCE_USER
  int source;

  // the key (usually something like 'display.fov')
  char *key;

  // the type of key
  int type;
  
  // has the value been modified since the last read?
  bool modified;

  // the data
  union {
    bool bool_value;
    int int_value;
    double double_value;
    char *string_value;
    char **path_value;
  } value;
};

struct config_b {
  // block stuff...
  int references;

  // the number of items
  int items_allocated;

  // the number of used items
  int items_used;
  struct config_item_b **items;
};

/* FUNCTIONS */
// block
struct config_b *config_new(void);
struct config_b *config_reference(struct config_b *config);
bool config_free(struct config_b *config);

// item block
struct config_item_b *config_item_new(void);
struct config_item_b *config_item_reference(struct config_item_b *item);
bool config_item_free(struct config_item_b *item);

int config_get_item_count(struct config_b *config);
struct config_item_b *config_get_item(struct config_b *config,char *key);
struct config_item_b *config_item_slot(struct config_b *config);
bool config_set_item(struct config_b *config,struct config_item_b *item,bool initial);

// get
int config_get_item_type(struct config_item_b *item);
bool config_get_item_bool(struct config_item_b *item);
int config_get_item_int(struct config_item_b *item);
double config_get_item_double(struct config_item_b *item);
char *config_get_item_string(struct config_item_b *item);
char *config_get_item_tostring(struct config_item_b *item);

// load/dump
int config_load_item(struct config_item_b *item,struct file_b *file);
bool config_load(struct config_b *config,struct file_b *file,int source);

// read/write
bool config_read(struct config_b *config,char *filename,int source);

bool config_test(void);

#endif
