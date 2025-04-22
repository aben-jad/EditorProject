#ifndef _TXT_UTILS_H_
#define _TXT_UTILS_H_

#include<stdio.h>
#include<stdlib.h>
#include<windows.h>

char* TXT_Get_Next_Line(char* _txt_start);
char* TXT_Skip_Spaces(char* _start);
BOOL TXT_Is_Line_Start_By(char* _start, char* _patern);
char* TXT_Find_Char(char *_line, char _c);
int TXT_Get_Lines_Number(char* _text);
int TXT_Get_Line_Size(char* _line);

#endif
