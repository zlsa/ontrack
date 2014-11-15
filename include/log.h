
#ifndef LOG_H
#define LOG_H

void log_test(const char* format,...);
void log_debug(const char* format,...);
void log_info(const char* format,...);
void log_notice(const char* format,...);
void log_warn(const char* format,...);
void log_never(const char* format,...);
void log_fatal(const char* format,...);

#endif
