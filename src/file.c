
#include "util.h"
#include "log.h"

#include "file.h"

#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <sys/time.h>

/* BLOCK */

struct file_b *file_new(void) {
  struct file_b *file=MALLOC(sizeof(struct file_b));
  file->references=0;

  file->fp=NULL;
  file->filename=NULL;
  file->mode=FILE_MODE_READ;
  file->open=false;

  file->bufp=-1;

  file->line=1;
  file->eof=false;
  file->err=0;

  return(file_reference(file));
}

struct file_b *file_reference(struct file_b *file) {
  ASSERT(file);
  file->references++;
  return(file);
}

bool file_free(struct file_b *file) {
  ASSERT(file);
  file->references--;
  if(file->references <= 0) {
    if(file->open) {
      // auto-closing file
      file_close(file);
    }
    if(file->filename) FREE(file->filename);
    FREE(file);
    return(true);
  }
  return(false);
}

/* OPEN/CLOSE */

// returns false on error
bool file_open(struct file_b *file,char *filename,int mode) {
  ASSERT(file);
  ASSERT(filename);
  if(file->open) {
    log_warn("attempted to open already-open file '%s'",file->filename);
    return(false);
  }
  if(file->filename) FREE(file->filename);
  file->filename=MALLOC(strlen(filename)+1);
  strncpy(file->filename,filename,strlen(filename)+1);

  char *cmode="r";
  if(mode == FILE_MODE_READ) cmode="r";
  else if(mode == FILE_MODE_READ) cmode="w";
  else if(mode == FILE_MODE_APPEND) cmode="a";
  else log_warn("while opening '%s': unexpected mode ID '%d', using FILE_MODE_READ (%d)",file->filename,mode,FILE_MODE_READ);
  file->mode=mode;
  file->line=1;

  file->fp=fopen(file->filename,cmode);

  if(!file->fp) {
    file->err=errno;
    FREE(file->filename);
    file->filename=NULL;
    return(false);
  }
  file->open=true;
  return(true);
}

bool file_close(struct file_b *file) {
  ASSERT(file);
  if(!file->open) {
    log_warn("attempted to close already-closed file");
    return(false);
  }
  if(file->fp) {
    if(fclose(file->fp) == EOF) {
      log_never("could not close '%s', will pretend we did...",file->filename);
    }
  } else {
    log_never("file '%s' is supposed to be open but is not...",file->filename);
    return(false);
  }

  file->fp=NULL;
  FREE(file->filename);
  file->filename=NULL;
  file->line=-1;
  file->open=false;
  file->eof=false;
  file->err=0;
  return(true);
}

/* ERRORS */

char *file_error(struct file_b *file) {
  ASSERT(file);
  if(file->err == 0) return("no error");
  char *warning=NULL;
  switch(file->err) {
  case EACCES:
    warning="permission denied";
    break;
  case EBUSY:
    warning="device or resource busy";
    break;
  case ENOENT:
    warning="no such file or directory";
    break;
  case ENOMEM:
    warning="no memory";
    break;
  case ENOSPC:
    warning="no space left on device";
    break;
  default:
    warning=strerror(file->err);
    break;
  }
  return(warning);
}

/* READING */

char *file_read_all(struct file_b *file) {
  ASSERT(file);
  if(!file->open) {
    log_warn("attempted to read from closed file");
    return(NULL);
  }
  int read  = 0;
  int chunk = CHUNK_START;
  int size  = CHUNK_START;
  int temp;
  char *buffer = MALLOC(CHUNK_START+1);
  while(true) {
    temp = fread(buffer+read, sizeof(char), chunk, file->fp);
    read+=temp;
    if(temp == 0) {
      buffer[read] = '\0';
      break;
    }
    if(read >= size-2) {
      chunk  *= 2;
      size   += chunk;
      buffer  = REALLOC(buffer, size);
    }
  }
  return(buffer);
}

int file_getc(struct file_b *file) {
  ASSERT(file);
  if(!file->open) {
    log_warn("attempted to read from closed file");
    return(EOF);
  }
  int c;
  if(file->bufp >= 0) {
    c=file->buffer[file->bufp--];
  } else {
    errno=0;
    c=getc(file->fp);
  }
  if(c == EOF) {
    if(errno) {
      log_warn("error while reading from '%s': %s",file->filename,strerror(errno));
      file->err=errno;
    } else {
      file->eof=true;
    }
    return(EOF);
  }
  if(c == '\n') file->line++;
  return(c);
}

// returns c, or EOF for failure (should almost never happen)
int file_ungetc(struct file_b *file,char c) {
  ASSERT(file);
  if(!file->open) {
    log_warn("attempted to unget closed file");
    return(EOF);
  }
  if(file->bufp-1 >= FILE_UNGETC_BUFFER_SIZE) {
    log_never("ungot beyond FILE_UNGETC_BUFFER_SIZE (%d)",FILE_UNGETC_BUFFER_SIZE);
    return(EOF);
  }
  if(c == EOF) {
    log_never("tried to unget EOF");
    file->eof=true;
    return(EOF);
  }
  file->buffer[++(file->bufp)]=c;
  if(c == '\n') file->line--;
  return(c);
}

int file_peek(struct file_b *file) {
  ASSERT(file);
  if(!file->open) {
    log_warn("attempted to peek from closed file");
    return(EOF);
  }
  int c=file_getc(file);
  file_ungetc(file,c);
  return(c);
}

// SKIPPING

int file_skip_whitespace(struct file_b *file) {
  int c;
  int i=0;
  while(((c=file_getc(file)) != EOF) && isspace(c)) {
    i++;
  }
  if(c != EOF) {
    file_ungetc(file,c);
    i--;
  }
  return(i);
}

int file_skip_whitespace_no_newline(struct file_b *file) {
  int c;
  int i=0;
  while(((c=file_getc(file)) != EOF) && isspace(c) && c != '\n') {
    i++;
  }
  if(c != EOF) {
    file_ungetc(file,c);
    i--;
  }
  return(i);
}

// returns the number of extraneous non-whitespace characters until the stop
int file_skip_to(struct file_b *file,char stop) {
  int c;
  int i=0;
  while(((c=file_getc(file)) != EOF) && c != stop) {
    if(!isspace(c)) i++;
  }
  if(c != EOF) {
    file_ungetc(file,c);
    if(!isspace(c)) i--;
  }
  return(i);
}

int file_skip_to_ignore_comment(struct file_b *file,char stop,char comment_start) {
  int c;
  int i=0;
  bool comment=false;
  while(((c=file_getc(file)) != EOF) && c != stop) {
    if(c == comment_start) comment=true;
    if(!isspace(c) && !comment) i++;
  }
  if(c != EOF && !comment) {
    file_ungetc(file,c);
    if(!isspace(c)) i--;
  }
  return(i);
}

// reads until and NOT including 'stop'
char *file_read_to(struct file_b *file,char stop) {
  int size=FILE_TOKEN_START_SIZE;
  int c;
  int i=0;
  char *s=MALLOC(size);
  while(true) {
    c=file_getc(file);
    if(file->eof) break;
    if(i-1 >= size) {
      size*=2;
      s=REALLOC(s,size);
    }
    if(c != stop && c != '#' && !isspace(c)) {
      s[i++]=c;
      continue;
    }
    file_ungetc(file,c);
    break;
  }
  s[i]='\0';
  return(s);
}

// TOKENS

// caller must free return value
char *file_get_token(struct file_b *file) {
  int size=FILE_TOKEN_START_SIZE;
  int c;
  int i=0;
  char *s=MALLOC(size);
  while(true) {
    c=file_getc(file);
    if(file->eof) break;
    if(i-1 >= size) {
      size*=2;
      s=REALLOC(s,size);
    }
    if(istoken(c)) {
      s[i++]=c;
    } else {
      file_ungetc(file,c);
      break;
    }
  }
  s[i]='\0';
  return(s);
}

// if error, returns pointer to string
char *file_get_bool(struct file_b *file,bool *ptr) {
  char *s=file_read_to(file,'\n');
  if((!strcmp(s,"true")) ||
     (!strcmp(s,"yes")) ||
     (!strcmp(s,"on")) ||
     (!strcmp(s,"1"))) *ptr=true;
  else if((!strcmp(s,"false")) ||
          (!strcmp(s,"no")) ||
          (!strcmp(s,"off")) ||
          (!strcmp(s,"0"))) *ptr=false;
  else if(!strcmp(s,"aladeen")) *ptr=(rand()>(RAND_MAX/4)?true:false);
  else return(s);
  FREE(s);
  return(NULL);
}

// if error, returns pointer to string
char *file_get_int(struct file_b *file,int *ptr) {
  char *s=file_read_to(file,'\n');
  char *temp;
  *ptr=strtol(s,&temp,0);
  if(s[0] != '\0' && temp[0] == '\0') {
    FREE(s);
    return(NULL);
  } else {
    return(s);
  }
}

// if error, returns pointer to string
char *file_get_double(struct file_b *file,double *ptr) {
  char *s=file_read_to(file,'\n');
  char *temp;
  *ptr=strtod(s,&temp);
  if(s[0] != '\0' && temp[0] == '\0') {
    FREE(s);
    return(NULL);
  } else {
    return(s);
  }
}

// returns number of characters read
int file_get_string(struct file_b *file,char **ptr) {
  int size=FILE_TOKEN_START_SIZE;
  int c;
  int i=0;
  char *s=MALLOC(size);

  bool escape=false;

  while(true) {
    c=file_getc(file);
    if(file->eof) break;
    if(i-1 >= size) {
      size*=2;
      s=REALLOC(s,size);
    }
    if(c == '\\') {
      escape=true;
      continue;
    }
    if(c == '"' && !escape) {
      //      file_ungetc(file,c);
      break;
    }
    if(escape) {
      if(c == '\n') continue;
      else if(c == 'n') c='\n';
      else if(c == '"') c='"';
      escape=false;
    }
    s[i++]=c;
  }
  s[i]='\0';
  *ptr=s;
  return(i);
}

/* TESTING */

bool file_test(void) {
  log_test("   == FILE ====================================================");

  bool passed=true;
  bool status=false;

  struct file_b *file=file_new();
  if(file) status=true;
  if(!status) passed=false;
  log_test("%sstruct file_b *file=file_new();",BTOF(status));

  status=file_open(file,"test/file",FILE_MODE_READ);
  log_test("%sfile_open(file,\"test/file\",FILE_MODE_READ);",BTOF(status));

  if(status) {

    int c=file_getc(file);
    status=(c=='1');
    if(!status) passed=false;
    log_test("%sfile_getc(file);",BTOF(status));

    c=file_getc(file);
    status=(c=='2');
    if(!status) passed=false;
    log_test("%sfile_getc(file);",BTOF(status));

    status=file_ungetc(file,c);
    if(!status) passed=false;
    log_test("%sfile_ungetc(file,c);",BTOF(status));
    
    c=file_getc(file);
    status=(c=='2');
    if(!status) passed=false;
    log_test("%sfile_getc(file);",BTOF(status));
    
    char *token=file_get_token(file);
    if(!token) {
      passed=false;
      log_test("%sfile_get_token(file);",BTOF(false));
    } else {
      status=(strncmp(token,"this_is_a_token",strlen(token))==0);
      if(!status) passed=false;
      log_test("%sfile_get_token(file);",BTOF(status));
      FREE(token);
    }
    
    status=file_free(file);
    log_test("%sfile_free(file);",BTOF(status));
    if(!status) passed=false;

  } else {
    passed=false;
  }

  if(passed) log_test("%sfile passed",BTOF(passed));
  else log_test("%sfile failed",BTOF(passed));
  return(passed);
}
