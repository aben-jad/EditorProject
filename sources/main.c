#include <IDE_utils.h>



int WINAPI WinMain(
		HINSTANCE hInstance, 
		HINSTANCE hPrevInstance, 
		PSTR pCmdLine, 
		int nCmdShow
			)
{

	HWND win_handle = IDE_Init(hInstance);

	MSG msg;
	while (GetMessage(&msg, NULL, 0,  0))
	{
		TranslateMessage(&msg); 
		DispatchMessage(&msg);
	}

	IDE_Clean_Up(win_handle);

	return 0;
}
