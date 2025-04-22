#include <TXT_utils.h>

char* TXT_Get_Next_Line(char* _txt_start)
{
	while (*_txt_start && *_txt_start != '\n')
		_txt_start++;

	if (*_txt_start == '\n')
		return ++_txt_start;
	return NULL;
}

int TXT_Get_Line_Size(char* _line)
{
	int line_size = 0;

	while(_line[line_size] && _line[line_size] != '\n')
	{
		line_size++;
	}

	return line_size;
}

int TXT_Get_Lines_Number(char* _text)
{
	int lines_number = 1;

	while(*_text)
	{
		if (*_text == '\n' && *(_text + 1))
			lines_number++;
		_text++;
	}


	return lines_number;
}


char* TXT_Skip_Spaces(char* _start)
{
	while (*_start == ' ' || *_start == '\t')
		_start++;
	return _start;
}

BOOL TXT_Is_Line_Start_By(char* _start, char* _patern)
{
	_start = TXT_Skip_Spaces(_start);

	if (*_start == 0 || *_start == '\n')
		return FALSE;

	while (*_patern)
	{
		if (*_patern != *_start)
			return FALSE;
		_patern++;
		_start++;
	}

	return TRUE;
}

char* TXT_Find_Char(char *_line, char _c)
{
	while (*_line && *_line != '\n' && *_line != _c)
		_line++;

	if (*_line == _c)
		return _line;

	return NULL;
}
