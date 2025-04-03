#include <stdio.h>
#include <windows.h>

HHOOK g_keyboard_hook;
HWND g_win_handle;

LRESULT CALLBACK LowLevelKeyboardProc(
				int _n_code,
				WPARAM _w_param,
				LPARAM _l_param
					)
{
	KBDLLHOOKSTRUCT* kbStruct = (KBDLLHOOKSTRUCT*)_l_param;
	UINT msg = (UINT)_w_param;              // WM_KEYDOWN or WM_KEYUP
	WPARAM wParamPost = kbStruct->vkCode; // Virtual key code
	LPARAM lParamPost = 0;
	
	// Build lParam (mimics a real WM_KEYDOWN/WM_KEYUP lParam)
	lParamPost |= (kbStruct->scanCode & 0xFF) << 16; // Bits 16-23: Scan code
	lParamPost |= (kbStruct->flags & LLKHF_EXTENDED) ? (1 << 24) : 0; // Bit 24: Extended flag
	lParamPost |= (kbStruct->flags & LLKHF_UP) ? (1 << 31) : 0;       // Bit 31: Transition (1 for up, 0 for down)
	lParamPost |= 1; // Bits 0-15: Repeat count
        PostMessage(g_win_handle, msg, wParamPost, lParamPost);
	return 1;
}

BOOL g_leader_key_pressed = FALSE;

LRESULT CALLBACK WindowProc(
			HWND _win_handle, 
			UINT _msg, 
			WPARAM _w_param, 
			LPARAM _l_param
				)
{

	switch(_msg)
	{
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
			break;
		}
		case WM_KEYUP:
		{
			if (_w_param == VK_LWIN)
			{
				g_leader_key_pressed = FALSE;
				return 0;
			}
			break;
		}

		case WM_KEYDOWN:
		{
			switch(_w_param)
			{
				case VK_LWIN:
				{
					if (IsZoomed(_win_handle))
						g_leader_key_pressed = TRUE;
					break;
				}
				case VK_ESCAPE:
				{
					//if (GetKeyState(VK_LWIN) & 0x8000)
					//{
					//	MessageBox(_win_handle, "true", "DEB", MB_OK);
					//}
					if (IsZoomed(_win_handle) == FALSE)
					{
						g_keyboard_hook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(NULL), 0);
						ShowWindow(_win_handle, SW_MAXIMIZE);
					}
					//else if (GetKeyState(VK_LWIN) & 0x8000)
					else if (g_leader_key_pressed)
					{
						g_leader_key_pressed = FALSE;
						UnhookWindowsHookEx(g_keyboard_hook);
						g_keyboard_hook = NULL;
						ShowWindow(_win_handle, SW_RESTORE);
					}
					break;
				}
				case 'A' ... 'Z':
				{
					char str[2];
					str[0] = _w_param;
					str[1] = 0;
					MessageBox(_win_handle, str, "mb", MB_OK);
					break;
				}
			}
			return 0;
			break;
		}
	}

	return DefWindowProc(_win_handle, _msg, _w_param, _l_param);
}

int WINAPI WinMain(
		HINSTANCE hInstance, 
		HINSTANCE hPrevInstance, 
		PSTR pCmdLine, 
		int nCmdShow
			)
{
	// Register the window class.
	WNDCLASS win_class = {0};
	
	win_class.lpfnWndProc   = WindowProc;
	win_class.hInstance     = hInstance;
	win_class.lpszClassName = "window_class";

	RegisterClass(&win_class);

	g_win_handle = 
		CreateWindow(
			"window_class",                     // Window class
    			"win test",    // Window text
    			WS_OVERLAPPEDWINDOW,            // Window style
    			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
    			NULL,       // Parent window    
    			NULL,       // Menu
    			hInstance,  // Instance handle
    			NULL        // Additional application data
    				);

	//g_keyboard_hook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(NULL), 0);

	if (g_win_handle == NULL)
	{
	    return 0;
	}

	ShowWindow(g_win_handle, SW_SHOW);
	//UpdateWindow(g_win_handle);
	MSG msg;
	while (GetMessage(&msg, NULL, 0,  0))
	{
		TranslateMessage(&msg); 
		DispatchMessage(&msg);
	}

	UnhookWindowsHookEx(g_keyboard_hook);
	g_keyboard_hook = NULL;

	return 0;
}
