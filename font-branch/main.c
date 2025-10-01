#include <stdio.h>
#include <windows.h>
#include <OGL_utils.h>

void Render();

HDC hdc;

typedef struct 
{
	HANDLE h;
	WNDCLASSEX c;
} win_setup;

LRESULT CALLBACK Win_Proc(
			HWND _win_handle, 
			UINT _msg, 
			WPARAM _w_param, 
			LPARAM _l_param
				)
{

	switch(_msg)
	{
		case WM_PAINT:
			Render();
			return 0;
			break;
		case WM_CLOSE:
			PostQuitMessage(0);
			return 0;
			break;
		case WM_CHAR:
		case WM_KEYDOWN:
		case WM_KEYUP:
			//IDE_Handle_Input(setup, _msg, _w_param);
			return 0;
			break;
	}

	return DefWindowProc(_win_handle, _msg, _w_param, _l_param);
}

win_setup Win_Init(HINSTANCE _h_instance)
{
	WNDCLASSEX w_class = (WNDCLASSEX){0};

	w_class.cbSize = sizeof(WNDCLASSEX);
	w_class.lpfnWndProc   = Win_Proc;
	w_class.hInstance     = _h_instance;
	w_class.style = CS_HREDRAW | CS_VREDRAW; // ACTIVATES RELOAD ON REDRAW
	w_class.lpszClassName = "window_class";
	//w_class.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);

	RegisterClassEx(&w_class);

	HWND win_handle = 
		CreateWindowEx(
				0,
			"window_class",                     // Window class
    			"win ide",    // Window text
    			WS_MAXIMIZE | WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
    			NULL,       // Parent window    
    			NULL,       // Menu
    			_h_instance,  // Instance handle
    			NULL        // Additional application data
    				);


	if (win_handle == NULL)
	{
		printf("error in win");
	}

	win_setup win_res = {0};
	win_res.h = win_handle;
	win_res.c = w_class;

	return win_res;
}

void Render()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

	SwapBuffers(hdc);
}

int WINAPI WinMain(
		HINSTANCE hInstance, 
		HINSTANCE hPrevInstance, 
		PSTR pCmdLine, 
		int nCmdShow
			)
{
	win_setup win_setup = Win_Init(hInstance);

	OGL_setup ogl_setup = OGL_Init(win_setup.h);
	hdc = ogl_setup.h_dc;

	printf("en\n");

	char* vertex_src = "./shaders/vertex.glsl";
	char* frag_src = "./shaders/frag.glsl";

	ShowWindow(win_setup.h, SW_MAXIMIZE);
	SetWindowPos(win_setup.h, 0, 570, 180, 800, 800, SWP_NOZORDER);

	GLuint sh_pr = OGL_Setup_Program(vertex_src, frag_src);
	UpdateWindow(win_setup.h);
	
	glUseProgram(sh_pr);
	glViewport(0, 0, 800, 800);
	Render();

	MSG msg;
	while (GetMessage(&msg, NULL, 0,  0))
	{
		TranslateMessage(&msg); 
		DispatchMessage(&msg);
	}

	return 0;
}
