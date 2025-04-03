#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mystring.h>
#include <myglobals.h>

char* read_file(char* _path)
{
	FILE* fp = fopen(_path, "r");
	if (fp == NULL)
	{
		printf("file doesn't exist anymore! %s\n", _path);
		fclose(fp);

		return NULL;
	}
	int len = 0;
	char c;
	while (fread(&c, sizeof(char), 1, fp))
		len++;

	fseek(fp, SEEK_SET, SEEK_SET);

	char* arr = (char*)malloc(sizeof(char) * (len + 1));
	arr[len] = 0;

	fread(arr, sizeof(char), len, fp);
	fclose(fp);

	return arr;
}

char* open_file_to_edit(char* _path, FILE** _fp, int* _file_size)
{
	*_fp = fopen(_path, "r+");
	if (*_fp == NULL)
	{
		printf("file doesn't exist anymore! %s\n", _path);

		return NULL;
	}

	int BUFFER_SIZE = 4 * 1024 * 1024;

	char* mem = (char*)malloc(BUFFER_SIZE * sizeof(char));

	*_file_size = fread(mem, sizeof(char), BUFFER_SIZE, *_fp);
	memset(mem + (*_file_size), 0, BUFFER_SIZE - (*_file_size));

	return mem;
}

unsigned int shader_file_exist(char* _path)
{
	FILE* fp = fopen(_path, "r");

	fclose(fp);

	return (fp != NULL);
}

char* get_next_line(FILE* _f)
{ 
	char tmp;
	char* buffer;

	int line_size = 0;
	if (fread(&tmp, sizeof(char), 1, _f) == 0)
		return NULL;
	line_size++;

	while(tmp != '\n')
	{
		fread(&tmp, sizeof(char), 1, _f);
		line_size++;
	}

	fseek(_f, -(line_size+1), SEEK_CUR);
	buffer = (char*)malloc((line_size) * sizeof(char));
	fread(buffer, sizeof(char), line_size, _f);
	buffer[line_size-1] = 0;

	return buffer;
}

char* get_next_line_with_new_line(FILE* _f)
{ 
	char tmp;
	char* buffer;

	int line_size = 0;
	if (fread(&tmp, sizeof(char), 1, _f) == 0)
		return NULL;
	line_size++;

	while(tmp != '\n')
	{
		fread(&tmp, sizeof(char), 1, _f);
		line_size++;
	}

	fseek(_f, -(line_size+1), SEEK_CUR);
	buffer = (char*)malloc((line_size+1) * sizeof(char));
	fread(buffer, sizeof(char), line_size, _f);
	buffer[line_size] = 0;

	return buffer;
}
