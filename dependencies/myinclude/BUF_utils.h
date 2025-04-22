#ifndef __BUF_UTILS_H__
#define __BUF_UTILS_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct BUF_strings
{
	char* str;
	struct BUF_strings* next;
};

typedef struct BUF_strings BUF_strings;

void BUF_Init(BUF_strings* _root, char* _str);
void BUF_Add_Line(BUF_strings* _root, char* _str);
void BUF_Add_Text(BUF_strings* _root, BUF_strings* _new);
void BUF_Insert_Line(BUF_strings** _root, char* _str, int _index);
int  BUF_Get_Count(BUF_strings* _root);
void BUF_Traverse(BUF_strings* _root);
void BUF_Clean_Up(BUF_strings* _root);
BUF_strings* BUF_Text_To_Strings(char* _txt);

#endif
