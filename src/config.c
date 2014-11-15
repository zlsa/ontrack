
#include "log.h"
#include "util.h"
#include "file.h"

#include "config.h"

#include <string.h>
#include <ctype.h>

////////////////////////////////////////
// BLOCK
////////////////////////////////////////

extern int memory_blocks;

struct config_b *config_new(void) {
  struct config_b *config = MALLOC(sizeof(struct config_b));
  config->references      = 0;
  config->items_allocated = CONFIG_ITEMS_START_SIZE;
  config->items_used      = 0;
  config->items           = MALLOC(sizeof(struct config_item_b *)*config->items_allocated);

  int i;
  for(i=0; i<config->items_allocated; i++) {
    config->items[i] = NULL;
  }

  return(config_reference(config));
}

struct config_b *config_reference(struct config_b *config) {
  ASSERT(config);
  config->references++;
  return(config);
}

bool config_free(struct config_b *config) {
  ASSERT(config);
  config->references--;
  if(config->references <= 0) {
    int i;
    for(i=0; i<config->items_used; i++) {
      if(config->items[i]) {
        config_item_free(config->items[i]);
        config->items[i] = NULL;
      }
    }
    FREE(config->items);
    FREE(config);
    return(true);
  }
  return(false);
}

////////////////////////////////////////
// ITEM BLOCK
////////////////////////////////////////

struct config_item_b *config_item_new(void) {
  struct config_item_b *item=MALLOC(sizeof(struct config_item_b));
  item->references = 0;
  item->key        = NULL;
  item->source     = CONFIG_SOURCE_UNKNOWN;
  item->type       = CONFIG_ITEM_TYPE_NULL;
  item->modified   = false;
  item->value.string_value = NULL;

  return(config_item_reference(item));
}

struct config_item_b *config_item_reference(struct config_item_b *item) {
  ASSERT(item);
  item->references++;
  return(item);
}

bool config_item_free(struct config_item_b *item) {
  ASSERT(item);
  item->references--;

  if(item->references <= 0) {
    if(item->key) FREE(item->key);
    if(item->type == CONFIG_ITEM_TYPE_STRING && item->value.string_value) {
      FREE(item->value.string_value);
    }
    FREE(item);
    return(true);
  }

  return(false);
}

////////////////////////////////////////
// CONFIG GET/SET
////////////////////////////////////////

int config_get_item_count(struct config_b *config) {
  return(config->items_used);
}

struct config_item_b *config_get_item(struct config_b *config,char *key) {
  ASSERT(config);
  int i;
  for(i=0;i<config->items_used;i++) {
    if(config->items[i]) {
      if(!strcmp(config->items[i]->key,key)) {
        return(config->items[i]);
      }
    }
  }
  return(NULL);
}

struct config_item_b *config_item_slot(struct config_b *config) {
  if(config->items_used-1 >= config->items_allocated) {
    //    log_notice("expanding config item list from %d to %d",config->items_allocated,config->items_allocated*2);
    config->items_allocated*=2;
    config->items=REALLOC(config->items,sizeof(struct config_item_b *)*config->items_allocated);
    int i;
    for(i=config->items_used;i<config->items_allocated-1;i++) {
      config->items[i]=NULL;
    }
  }
  config->items[config->items_used-1]=config_item_new();
  return(config->items[config->items_used-1]);
}

// returns true if key already exists
bool config_set_item(struct config_b *config, struct config_item_b *item) {
  ASSERT(config);
  ASSERT(item);

  bool existed = true;

  struct config_item_b *current = config_get_item(config,item->key);
  if(!current) {
    existed = false;
    config->items_used++;
    current      = config_item_slot(config);
    current->key = MALLOC(strlen(item->key)+1);
    strncpy(current->key,item->key,strlen(item->key)+1);
  }

  if((current->type != item->type) && existed) {
    log_warn("type of '%s' changed from %s to %s", item->key, CONFIG_ITEM_TYPE(current->type), CONFIG_ITEM_TYPE(item->type));
  }

  if(current->type == CONFIG_ITEM_TYPE_STRING && current->value.string_value)
    FREE(current->value.string_value);

  current->type = item->type;
  switch(current->type) {
  case CONFIG_ITEM_TYPE_STRING:
    current->value.string_value = MALLOC(strlen(item->value.string_value)+1);
    strncpy(current->value.string_value,item->value.string_value,strlen(item->value.string_value)+1);
    break;
  case CONFIG_ITEM_TYPE_DOUBLE:
    current->value.double_value = item->value.double_value;
    break;
  case CONFIG_ITEM_TYPE_BOOL:
    current->value.bool_value   = item->value.bool_value;
    break;
  case CONFIG_ITEM_TYPE_INT:
    current->value.int_value    = item->value.int_value;
    break;
  }
  if(existed) {
    current->modified = true;
    return(true);
  }
  return(false);
}

////////////////////////////////////////
// SOURCE
////////////////////////////////////////

int config_get_item_source(struct config_item_b *item) {
  ASSERT(item);

  return(item->source);
}

int config_set_item_source(struct config_item_b *item, int source) {
  ASSERT(item);

  if(item->source != source) item->modified = true;
  item->source     = source;

  return(source);
}

////////////////////////////////////////
// VALUES
////////////////////////////////////////

int config_get_item_type(struct config_item_b *item) {
  ASSERT(item);
  return(item->type);
}

bool config_get_item_bool(struct config_item_b *item) {
  ASSERT(item);
  if(config_get_item_type(item) != CONFIG_ITEM_TYPE_BOOL) return(false);
  return(item->value.bool_value);
}

int config_get_item_int(struct config_item_b *item) {
  ASSERT(item);
  if(config_get_item_type(item) != CONFIG_ITEM_TYPE_INT) return(0);
  return(item->value.int_value);
}

double config_get_item_double(struct config_item_b *item) {
  ASSERT(item);
  if(config_get_item_type(item) != CONFIG_ITEM_TYPE_DOUBLE) return(0.0);
  return(item->value.double_value);
}

char *config_get_item_string(struct config_item_b *item) {
  ASSERT(item);
  if(config_get_item_type(item) != CONFIG_ITEM_TYPE_STRING) return(NULL);
  return(item->value.string_value);
}

char *config_get_item_to_string(struct config_item_b *item) {
  ASSERT(item);

  char *s;
  int type = config_get_item_type(item);
  bool bool_value;

  switch(type) {
  case CONFIG_ITEM_TYPE_BOOL:
    bool_value = config_get_item_bool(item);

    s = MALLOC(strlen(BTOS(bool_value)) + 1);
    strncpy(s, BTOS(bool_value), strlen(BTOS(bool_value)) + 1);
    break;
  case CONFIG_ITEM_TYPE_INT:
    s = MALLOC(INT_ENOUGH + 2);
    snprintf(s, INT_ENOUGH + 1, "%d", config_get_item_int(item));
    break;
  default:
    log_never("no handler for %s type",CONFIG_ITEM_TYPE(type));
    EXIT(EXIT_FAILURE);
    break;
  }
  return(s);
}

/* LOAD/DUMP */

int config_load_item(struct config_item_b *item,struct file_b *file) {
  char *type=NULL;
  char *key=NULL;
  file_skip_whitespace(file);
  if(file->eof) goto eof;

  if(file_peek(file) == '#') { // comment is this
    file_skip_to(file,'\n');
    return(RETURN_FAILURE_NEWLINE);
  }

  type=file_get_token(file);
  if(file->eof) goto eof;

  file_skip_whitespace_no_newline(file);
  if(file->eof) goto eof;

  key=file_get_token(file);
  if(file->eof) goto eof;

  file_skip_whitespace_no_newline(file);
  if(file->eof) goto eof;

  int c=file_getc(file);
  if(c != '=') {
    if(c == EOF) goto eof;
    if(c == '\n') log_warn("expected an equals sign '=', not a newline, on line %d of '%s'; discarding",file->line,file->filename);
    else if(isspace(c)) log_warn("expected an equals sign '=', not whitespace, on line %d of '%s'; discarding",file->line,file->filename);
    else log_warn("expected an equals sign '=', not '%c', on line %d of '%s'; discarding",c,file->line,file->filename);
    goto parse;
  }

  file_skip_whitespace_no_newline(file);
  if(file->eof) goto eof;

  if(strlen(key) == 0 && strlen(type) == 0) {
    FREE(key);
    FREE(type);
    return(RETURN_FAILURE_NEWLINE);
  }

  if(strcmp(type,"bool") == 0) {
    item->type=CONFIG_ITEM_TYPE_BOOL;
  } else if(strcmp(type,"int") == 0) {
    item->type=CONFIG_ITEM_TYPE_INT;
  } else if(strcmp(type,"double") == 0) {
    item->type=CONFIG_ITEM_TYPE_DOUBLE;
  } else if(strcmp(type,"string") == 0) {
    item->type=CONFIG_ITEM_TYPE_STRING;
  } else if(strcmp(type,"path") == 0) {
    item->type=CONFIG_ITEM_TYPE_PATH;
  } else {
    log_warn("unknown type '%s' on line %d of '%s', discarding; note that the type is required and must be all lowercase",type,file->line,file->filename);
    goto parse;
  }

  char *s;
  switch(item->type) {
  case CONFIG_ITEM_TYPE_BOOL:
    s=file_get_bool(file,&item->value.bool_value);
    if(s) {
      log_warn("'%s' is not a valid boolean value on line %d of '%s'; expected one of 'true'/'false', 'yes'/'no', 'on'/'off', or '1'/0', discarding %s '%s'",s,file->line,file->filename,type,key);
      FREE(s);
      goto parse;
    }
    break;
  case CONFIG_ITEM_TYPE_INT:
    s=file_get_int(file,&item->value.int_value);
    if(s) {
      log_warn("'%s' is not a valid integer value on line %d of '%s', discarding %s '%s'",s,file->line,file->filename,type,key);
      FREE(s);
      goto parse;
    }
    break;
  case CONFIG_ITEM_TYPE_DOUBLE:
    s=file_get_double(file,&item->value.double_value);
    if(s) {
      log_warn("'%s' is not a valid double value on line %d of '%s', discarding %s '%s'",s,file->line,file->filename,type,key);
      FREE(s);
      goto parse;
    }
    break;
  case CONFIG_ITEM_TYPE_STRING:
    c=file_getc(file);
    if(c != '"') {
      if(c == EOF) {
        log_warn("expected a string, got end of file on line %d of '%s'; discarding %s '%s'",c,file->line,file->filename,type,key);
        goto eof;
      }
      if(c == '\n') log_warn("expected a double quote '\"', not a newline, on line %d of '%s'; discarding %s '%s'",file->line,file->filename,type,key);
      else if(isspace(c)) log_warn("expected a double quote '\"', not whitespace, on line %d of '%s'; discarding %s '%s'",file->line,file->filename,type,key);
      else log_warn("expected a double quote '\"', not '%c', on line %d of '%s'; discarding %s '%s'",c,file->line,file->filename,type,key);
      goto parse;
    }
    file_get_string(file,&item->value.string_value);
    break;
  default:
    log_never("somebody added a new type '%s' with no way to parse it on line %d of '%s', discarding %s '%s'",type,file->line,file->filename,type,key);
    goto parse;
    break;
  }

  int i=0;
  if((i=file_skip_to_ignore_comment(file,'\n','#')) != 0) {
    log_warn("%d extraneous character%s at the end of line %d of '%s', discarding %s '%s'",i,S(i),file->line,file->filename,type,key);
  }

  goto done;
 eof:
  if(type) FREE(type);
  if(key) FREE(key);
  return(RETURN_FAILURE_EOF);
 parse:
  if(type) FREE(type);
  if(key) FREE(key);
  //  item->type=CONFIG_KEY_TYPE_NULL;
  file_skip_to(file,'\n');
  return(RETURN_FAILURE_PARSE);
 done:
  item->key=key;
  if(type) FREE(type);
  return(RETURN_SUCCESS);
}

int config_dump_item(struct config_item_b *item, struct file_b *file) {
  ASSERT(item);
  ASSERT(file);

  switch(item->type) {
  case CONFIG_ITEM_TYPE_NULL:
    log_warn("null config item");
    return(RETURN_SUCCESS);
  case CONFIG_ITEM_TYPE_BOOL:
    file_write(file, "bool   ");
    break;
  case CONFIG_ITEM_TYPE_INT:
    file_write(file, "int    ");
    break;
  case CONFIG_ITEM_TYPE_DOUBLE:
    file_write(file, "double ");
    break;
  case CONFIG_ITEM_TYPE_STRING:
    file_write(file, "string ");
    break;
  case CONFIG_ITEM_TYPE_PATH:
    file_write(file, "path   ");
    break;
  }

  char *value = config_get_item_to_string(item);

  file_write(file, item->key);
  file_write(file, " = ");
  file_write(file, value);
  file_write(file, "\n");

  FREE(value);

  return(RETURN_SUCCESS);
}

// LOAD/DUMP

bool config_load(struct config_b *config,struct file_b *file,int source) {
  bool errors=false;
  struct config_item_b *item;
  int status;
  while(true) {
    item   = config_item_new();
    status = config_load_item(item,file);
    if(status != RETURN_SUCCESS) {
      if(status != RETURN_FAILURE_NEWLINE && status != RETURN_FAILURE_EOF) {
        errors=true;
      }
      config_item_free(item);
      if(file->eof) break;
      continue;
    }
    config_set_item(config, item);
    config_item_free(item);
    if(file->eof) break;
  }
  if(config && source) {
  }
  return(!errors);
}

bool config_dump(struct config_b *config, struct file_b *file, int source) {
  bool errors = false;

  int i;
  int status;

  for(i=0; i<config->items_used; i++) {
    if(config->items[i]) {
      if(source & config->items[i]->source || true) {
        status &= config_dump_item(config->items[i], file);
      }
    }
  }

  errors = !status;

  return(!errors);
}

////////////////////////////////////////
// READ/WRITE
////////////////////////////////////////

// returns false if errors encountered
bool config_read(struct config_b *config,char *filename,int source) {
  struct file_b *file = file_new();

  if(!file_open(file, filename, FILE_MODE_READ)) {
    log_notice("could not load config file '%s': %s", filename, file_error(file));
    file_free(file);
    return(false);
  }

  bool errors = config_load(config, file, source);
  file_close(file);
  file_free(file);
  return(errors);
}

// writes all changed variables
bool config_write(struct config_b *config, char *filename, int source) {

  struct file_b *file = file_new();

  file_mkdirs(filename);

  if(!file_open(file, filename, FILE_MODE_WRITE)) {
    log_notice("could not dump config file '%s': %s", filename, file_error(file));
    file_free(file);
    return(false);
  }

  bool errors = config_dump(config, file, source);

  file_close(file);
  file_free(file);

  return(errors);
}

bool config_test(void) {
  log_test("   == CONFIG ==================================================");
  bool passed=true;
  bool status=false;

  struct config_b *config=config_new();
  if(config) status=true;
  if(!status) passed=false;
  log_test("%sstruct config_b *config=config_new();",BTOF(status));

  status=config_read(config,"test/config",CONFIG_SOURCE_USER);
  if(!status) passed=false;
  log_test("%sconfig_read(config,\"test/config\",CONFIG_SOURCE_USER);",BTOF(status));

  status=false;
  int count=config_get_item_count(config);
  if(count == 5) status=true;
  if(!status) passed=false;
  log_test("%sconfig_get_item_count(config);",BTOF(status));

  status=false;
  struct config_item_b *item=config_get_item(config,"test_bool");
  if(item) {
    status=true;
      //    log_test("%sstruct config_item_b *item=config_get_item(config,\"bool_test\");",BTOF(status));
  }
  if(!status) passed=false;
  log_test("%sstruct config_item_b *item=config_get_item(config,\"bool_test\");",BTOF(status));

  status=config_free(config);
  if(!status) passed=false;
  log_test("%sconfig_free(config);",BTOF(status));

  if(passed) log_test("%sconfig passed",BTOF(passed));
  else log_test("%sconfig failed",BTOF(passed));
  return(passed);
}
