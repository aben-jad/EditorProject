#include <PIPE_utils.h>

	//HANDLE in_write;
	//HANDLE out_read;
	//STARTUPINFO stup_info;
	//PROCESS_INFORMATION proc_info;

//int PIPE_Create_Proc(HANDLE* &(_setup_p -> in_write), HANDLE* &(_setup -> out_read), STARTUPINFO* &(_setup_p -> stup_info), PROCESS_INFORMATION* &(_setup_p -> proc_info))
int PIPE_Create_Proc(PIPE_setup* _setup_p)
{
	SECURITY_ATTRIBUTES attr;

	attr.nLength = sizeof(SECURITY_ATTRIBUTES); 
	attr.bInheritHandle = TRUE; 
	//attr.bInheritHandle = FALSE; 
	attr.lpSecurityDescriptor = NULL; 

	HANDLE h_stdInRead, h_stdOutWrite;
	if (!CreatePipe(&h_stdInRead, &(_setup_p -> in_write), &attr, 0))
		printf("pipe %d\n",  GetLastError());
	if (!CreatePipe(&(_setup_p -> out_read), &h_stdOutWrite, &attr, 0))
		printf("pipe %d\n",  GetLastError());

	if (!SetHandleInformation(_setup_p -> in_write, HANDLE_FLAG_INHERIT, 0))
		printf("handle %d\n",  GetLastError());
	if (!SetHandleInformation(_setup_p -> out_read, HANDLE_FLAG_INHERIT, 0))
		printf("handle %d\n",  GetLastError());


	ZeroMemory(&(_setup_p -> stup_info), sizeof(*&(_setup_p -> stup_info)));
	_setup_p -> stup_info.cb = sizeof(_setup_p -> stup_info);
	_setup_p -> stup_info.hStdInput = h_stdInRead;
	_setup_p -> stup_info.hStdOutput = h_stdOutWrite;
	_setup_p -> stup_info.hStdError = h_stdOutWrite;
	_setup_p -> stup_info.dwFlags |= STARTF_USESTDHANDLES;
	_setup_p -> stup_info.wShowWindow = SW_HIDE;
	ZeroMemory(&(_setup_p -> proc_info), sizeof(_setup_p -> proc_info));

	//char* _path = "C:\\Windows\\System32\\WindowsPowerShell\\v1.0";
	char* _path = "powerShell.exe -NoLogo -NoProfile -NonInteractive -ExecutionPolicy Bypass";
	//char* _path = "powerShell.exe -Command - ";
	int proc_succ = CreateProcess(NULL, _path,  NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &(_setup_p -> stup_info), &(_setup_p -> proc_info));

	CloseHandle(h_stdInRead);
	CloseHandle(h_stdOutWrite);

	return proc_succ;
}

char* PIPE_Read(HANDLE _h)
{
	
	DWORD bytesRead = 0;

	int const BUF_SIZE = 2048;
	char buffer[BUF_SIZE];
	char tmp[BUF_SIZE];
	ReadFile(_h, tmp, BUF_SIZE, &bytesRead, NULL);
	for (int i = 0, j = 0; i < bytesRead; i++)
	{
		if (tmp[i] >= 32 && tmp[0] < 127)
		{
			buffer[j] = tmp[i];
		}
		else
			buffer[j] = '!';
		j++;
	}
	//char tmp[1];
	//while (!ReadFile(_h, tmp, 1, &bytesRead, NULL))
	//{
	//printf("cc %d\n", bytesRead);
	//	if (tmp[0] >= 32 && tmp[0] < 127)
	//	{
	//		buffer[bytesRead] = *tmp;
	//		bytesRead++;
	//	}
	//	
	//}
	//int len = ReadFile(_h, buffer, BUF_SIZE, &bytesRead, NULL);
	//if (len == 0)
	//	printf("faild");
	//else printf("%d", bytesRead);
	
	buffer[bytesRead] = 0;

	return strdup(buffer);
}

void PIPE_Write(HANDLE _h, char* _str)
{
	int len = strlen(_str);

	WriteFile(_h, _str, len, NULL, NULL);
}
