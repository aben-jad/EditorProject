#ifndef _FILE_UTILS_H
#define _FILE_UTILS_H
#include <stdio.h>

char *read_file(char* _path);
unsigned int shader_file_exist(char* _path);
char* get_next_line(FILE* _f);
char* get_next_line_with_new_line(FILE* _f);

#endif
