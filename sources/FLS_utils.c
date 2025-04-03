#include <FLS_utils.h>

char* FLS_Read_File(char* _file_path)
{
	FILE* fp = fopen(_file_path, "r");
	if (fp == NULL)
	{
		printf("file doesn't exist anymore! %s\n", _file_path);
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
