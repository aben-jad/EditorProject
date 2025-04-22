#include <HOK_utils.h>

static HHOOK s_keyboard_hook;
static HWND s_win_handle;

static LRESULT CALLBACK LowLevelKeyboardProc(
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
        PostMessage(s_win_handle, msg, wParamPost, lParamPost);
	//printf("hok\n");
	return 1;
}


void HOK_Init(HWND _win_handle)
{
	s_win_handle = _win_handle;
}

void HOK_Enable()
{
	s_keyboard_hook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(NULL), 0);
	//printf("hoken\n");
}

void HOK_Disable()
{
	UnhookWindowsHookEx(s_keyboard_hook);
	s_keyboard_hook = NULL;
	//printf("hokdis\n");
}
