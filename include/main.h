
#ifndef MAIN_H
#define MAIN_H

#define VERSION_MAJOR 0
#define VERSION_MINOR 1
#define VERSION_PATCH 1

#define VERSION "" STR(VERSION_MAJOR) "." STR(VERSION_MINOR) "." STR(VERSION_PATCH) ""

#define NAME "onTrack"

void exit_cleanup(void);
void test(void);

#endif
