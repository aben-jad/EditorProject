#include <BUF_utils.h>
#include <io.h>


void BUF_Init(BUF_strings* _root, char* _str)
{
	_root -> str = _str;
	_root -> next = NULL;
}

void BUF_Add_Line(BUF_strings* _root, char* _str)
{
	while (_root -> next)
		_root = _root -> next;

	BUF_strings* new = (BUF_strings*)malloc(sizeof(BUF_strings));
	BUF_Init(new, _str);

	_root -> next = new;

}

void BUF_Add_Text(BUF_strings* _root, BUF_strings* _new)
{
	while (_root -> next)
		_root = _root -> next;


	_root -> next = _new;

}

void BUF_Traverse(BUF_strings* _root)
{
	while (_root)
	{
		printf("%s\n", _root->str);
		_root = _root->next;
	}
}

void BUF_Clean_Up(BUF_strings* _root)
{
	while (_root)
	{
		free(_root->str);
		BUF_strings* tmp = _root;
		_root = _root->next;
		free(tmp);
	}
}

BUF_strings* BUF_Text_To_Strings(char* _txt)
{
	BUF_strings* strings = (BUF_strings*)malloc(sizeof(BUF_strings));
	strings -> str = NULL;
	strings -> next = NULL;

	//printf("-----------------------------enter----------------------\n");
	//printf("txt : %s\n-------end\n", _txt);
	int i = 0;
	char* nx = _txt;
	while (*nx)
	{
		char* old = nx;
		int len = 0;
		nx = strchr(nx, '\n');
		if (nx == NULL)
			len = strlen(old);
		else
		{
			//printf("ln\n");
			len = nx - old;
		}
		//printf("$$en %d : \n", len);
		printf("%d , %d\n", *old, len);
		char* str = (char*)malloc((len) * sizeof(char));
		if (str == NULL)
			printf("NUUL\n");
		//_write(1, str, len);
		memcpy(str, old, len);
		str[len] = 0;
		//printf("%s , %d$$\n", str , len);
		if (strings -> str == NULL)
			BUF_Init(strings, str);
		else
			BUF_Add_Line(strings, str);
		nx = old + len;
		if (*nx == '\n')
			nx++;
	}

	return strings;
}

void BUF_Insert_Line(BUF_strings** _root, char* _str, int _index)
{
	BUF_strings* new = (BUF_strings*)malloc(sizeof(BUF_strings));
	BUF_Init(new, _str);

	int i = 0;
	while ((*_root) && i < _index)
	{
		_root = &(*_root) -> next;
		i++;
	}

	BUF_strings* old = *_root;
	(*_root) = new;
	new  -> next = old;
}

int  BUF_Get_Count(BUF_strings* _root)
{
	int len = 0;
	while (_root)
	{
		len++;
		_root = _root -> next;
	}

	return len;
}
