#ifndef __PIPE_UTILS_H__
#define __PIPE_UTILS_H__

#include <OGL_utils.h>
#include <windows.h>

typedef struct
{
	HANDLE in_write;
	HANDLE out_read;
	STARTUPINFO stup_info;
	PROCESS_INFORMATION proc_info;
} PIPE_setup;

//int PIPE_Create_Proc(HANDLE* _h_stdInWrite, HANDLE* _h_stdOutRead, STARTUPINFO* _siStartInfo, PROCESS_INFORMATION* _piProcInfo);
int PIPE_Create_Proc(PIPE_setup* _setup_p);
char* PIPE_Read(HANDLE _h);
void PIPE_Write(HANDLE _h, char* _str);

//void PIPE_Init(PIPE_setup* _setup_p);
//void PIPE_Clean_Up(PIPE_setup* _setup_p);
//void PIPE_Open(PIPE_setup _setup);
//void PIPE_Close();
//void PIPE_Render(PIPE_setup _setup_p);
//void PIPE_Handle_Input(PIPE_setup* _setup_p, UINT _msg, WPARAM _w_param);

#endif

