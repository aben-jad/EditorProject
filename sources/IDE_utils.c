#include <IDE_utils.h>


BOOL IDE_Is_Ide_Input()
{
	return TRUE;
}


LRESULT CALLBACK IDE_Proc(
			HWND _win_handle, 
			UINT _msg, 
			WPARAM _w_param, 
			LPARAM _l_param
				)
{

	IDE_setup* setup = (IDE_setup*)GetWindowLongPtr(_win_handle, GWLP_USERDATA);

	switch(_msg)
	{
		case WM_PAINT:
			IDE_Render(*setup);
			return 0;
			break;
		case WM_CLOSE:
			PostQuitMessage(0);
			return 0;
			break;
		case WM_CHAR:
		case WM_KEYDOWN:
		case WM_KEYUP:
			IDE_Handle_Input(setup, _msg, _w_param);
			return 0;
			break;
	}

	return DefWindowProc(_win_handle, _msg, _w_param, _l_param);
}

void IDE_Handle_Input(IDE_setup* _setup_p, UINT _msg, WPARAM _w_param)
{
	if ((_msg == WM_KEYUP || _msg == WM_KEYDOWN) && _setup_p -> win.is_focus)
	{
		if (_w_param == VK_LWIN)
			IDE_Leaders_Management(&(_setup_p -> leaders), _w_param, _msg == WM_KEYDOWN);
	}


	if (_msg == WM_KEYDOWN && _w_param == VK_ESCAPE)
	{
		if (!_setup_p -> win.is_focus)
			IDE_Focus_Mode_Enter(_setup_p);
		else if (_setup_p -> leaders.win)
			IDE_Focus_Mode_Exit(_setup_p);

	}
	if(_msg == WM_KEYDOWN && _w_param == 'X')
	{	
		if (_setup_p -> leaders.win)
		{
			if(!(_setup_p -> sub_wins).exp.is_op)
			{
				IDE_EXP_Open(_setup_p);
			}
			else
			{
				IDE_EXP_Close(_setup_p);
			}
		}
	}
	if(_msg == WM_KEYDOWN && _w_param == 'E')
	{	
		if (_setup_p -> leaders.win)
		{
			if(!(_setup_p -> sub_wins).ted.is_op)
			{
				IDE_TED_Open(_setup_p);
			}
			else
			{
				IDE_TED_Close(_setup_p);
			}
		}
	}

	if(_msg == WM_KEYDOWN && _w_param == 'T')
	{	
		if (_setup_p -> leaders.win)
		{
			if(!(_setup_p -> sub_wins).trm.is_op)
			{
				IDE_TRM_Open(_setup_p);
			}
			else
			{
				IDE_TRM_Close(_setup_p);
			}
		}
	}


	if (_msg == WM_KEYDOWN || _msg == WM_CHAR)
	{
		switch (_setup_p -> focused_sub_win)
		{
			case EXP_FOCUS:
				EXP_Handle_Input(&(_setup_p->ext.exp), _w_param);
				break;
			case TED_FOCUS:
				TED_Handle_Input(&(_setup_p->ext.ted), _w_param);
				break;
			case TRM_FOCUS:
				TRM_Handle_Input(&(_setup_p->ext.trm), _msg, _w_param);
				break;
		}
	}
}

void IDE_Leaders_Management(IDE_leaders* _leaders_p, WPARAM _w_param, BOOL _is_down)
{
	if (_is_down)
	{
		_leaders_p -> win = TRUE;
	}
	else
	{
		_leaders_p -> win = FALSE;
	}
}

void IDE_Focus_Mode_Enter(IDE_setup* _setup_p)
{
	HOK_Enable();

	LONG style = WS_MAXIMIZE | WS_POPUP;
	SetWindowLongPtr(_setup_p -> win.handle, GWL_STYLE, (LONG)style);
	_setup_p -> win.is_focus = TRUE;
	ShowWindow(_setup_p -> win.handle, SW_MAXIMIZE);
}

void IDE_Focus_Mode_Exit(IDE_setup* _setup_p)
{
	HOK_Disable();

	(_setup_p -> leaders).win = FALSE;

	LONG style = WS_OVERLAPPEDWINDOW;
	SetWindowLongPtr(_setup_p -> win.handle, GWL_STYLE, (LONG)style);
	_setup_p -> win.is_focus = FALSE;
	//SetWindowPos(_setup_p -> win.handle, NULL, 200, 100, 1200, 800, SWP_NOMOVE);
	ShowWindow(_setup_p -> win.handle, SW_RESTORE);
}

void IDE_TED_Open(IDE_setup* _setup_p)
{
	glUseProgram(_setup_p -> win.sh_prg);

	RECT win_rect;
	GetClientRect(_setup_p -> win.handle, &win_rect);
	//RECT exp_rect = {win_rect.left, win_rect.top, win_rect.left + (win_rect.right - win_rect.left) / 5, win_rect.bottom};
	//MATH_ivec2 vp = {win_rect.right - win_rect.left, 0.5f};
	MATH_ivec2 vp = {2.5f, 0.5f};

	//((_setup_p -> unifs).val).ar = aspect_ratio;
	//glViewport(_setup.vp.left, _setup.vp.top, _setup.vp.right, _setup.vp.bottom);
	//((_setup_p -> unifs).val).vp = 1.0f;

	//RECT r = (RECT)_setup_p -> ext.exp.vp;
	//((_setup_p -> unifs).loc).ar = glGetUniformLocation(_setup_p -> win.sh_prg, "u_aspect");

	//glUniform1f(((_setup_p -> unifs).loc).ar, ((_setup_p -> unifs).val).ar);
	//glUniform1f(((_setup_p -> unifs).loc).vp, ((_setup_p -> unifs).val).vp);
	////glUniform2f(((_setup_p -> unifs).loc).vp, ((_setup_p -> unifs).val).exp_pos.x, ((_setup_p -> unifs).val).exp_pos.y);
	//glUniform2f(((_setup_p -> unifs).loc).exp_pos, vp.x, vp.y);

	//GLint st = glGetUniformLocation(_setup_p -> win.sh_prg, "u_exp_start");
	//GLint en = glGetUniformLocation(_setup_p -> win.sh_prg, "u_exp_end");

	//RECT rt = (RECT)_setup_p -> ext.ted.vp;
	//float t = (float)(rt.right - rt.left) / (float)(win_rect.right - win_rect.left);
	//glUniform2f(st, t+.004f, 0.f);
	//glUniform2f(en, t+.004f, 1.f);

	TED_Open(_setup_p -> ext.ted);
	_setup_p -> focused_sub_win = TED_FOCUS;
	(_setup_p -> sub_wins).ted.is_op = TRUE;
}

void IDE_TED_Close(IDE_setup* _setup_p)
{
	glUseProgram(_setup_p -> win.sh_prg);

	//((_setup_p -> unifs).val).ar = aspect_ratio;
	//((_setup_p -> unifs).val).vp = -10.0f;

	////((_setup_p -> unifs).loc).ar = glGetUniformLocation(_setup_p -> win.sh_prg, "u_aspect");
	//((_setup_p -> unifs).loc).vp = glGetUniformLocation(_setup_p -> win.sh_prg, "u_view_port");


	//{
	//GLint st = glGetUniformLocation(_setup_p -> win.sh_prg, "u_exp_start");
	//GLint en = glGetUniformLocation(_setup_p -> win.sh_prg, "u_exp_end");

	//glUniform2f(st, .002f, 0.f);
	//glUniform2f(en, .002f, 1.f);
	//}
	//
	////glUniform1f(((_setup_p -> unifs).loc).ar, ((_setup_p -> unifs).val).ar);
	//glUniform1f(((_setup_p -> unifs).loc).vp, ((_setup_p -> unifs).val).vp);
	TED_Close();
	_setup_p -> sub_wins.ted.is_op = FALSE;
	if (_setup_p -> sub_wins.exp.is_op)
		_setup_p -> focused_sub_win = EXP_FOCUS;
	else if (_setup_p -> sub_wins.trm.is_op)
		_setup_p -> focused_sub_win = TRM_FOCUS;
	else
		_setup_p -> focused_sub_win = NON_FOCUS;
	
}

void IDE_EXP_Open(IDE_setup* _setup_p)
{
	glUseProgram(_setup_p -> win.sh_prg);

	RECT win_rect;
	GetClientRect(_setup_p -> win.handle, &win_rect);
	//RECT exp_rect = {win_rect.left, win_rect.top, win_rect.left + (win_rect.right - win_rect.left) / 5, win_rect.bottom};
	//MATH_ivec2 vp = {win_rect.right - win_rect.left, 0.5f};
	MATH_ivec2 vp = {2.5f, 0.5f};

	//((_setup_p -> unifs).val).ar = aspect_ratio;
	//glViewport(_setup.vp.left, _setup.vp.top, _setup.vp.right, _setup.vp.bottom);
	((_setup_p -> unifs).val).vp = 1.0f;

	//RECT r = (RECT)_setup_p -> ext.exp.vp;
	//((_setup_p -> unifs).loc).ar = glGetUniformLocation(_setup_p -> win.sh_prg, "u_aspect");

	//glUniform1f(((_setup_p -> unifs).loc).ar, ((_setup_p -> unifs).val).ar);
	glUniform1f(((_setup_p -> unifs).loc).vp, ((_setup_p -> unifs).val).vp);
	//glUniform2f(((_setup_p -> unifs).loc).vp, ((_setup_p -> unifs).val).exp_pos.x, ((_setup_p -> unifs).val).exp_pos.y);
	glUniform2f(((_setup_p -> unifs).loc).exp_pos, vp.x, vp.y);

	GLint st = glGetUniformLocation(_setup_p -> win.sh_prg, "u_exp_start");
	GLint en = glGetUniformLocation(_setup_p -> win.sh_prg, "u_exp_end");

	RECT rt = (RECT)_setup_p -> ext.exp.vp;
	float t = (float)(rt.right - rt.left) / (float)(win_rect.right - win_rect.left);
	glUniform2f(st, t+.004f, 0.f);
	glUniform2f(en, t+.004f, 1.f);

	EXP_Open(&_setup_p -> ext.exp);
	_setup_p -> focused_sub_win = EXP_FOCUS;
	(_setup_p -> sub_wins).exp.is_op = TRUE;
}

void IDE_EXP_Close(IDE_setup* _setup_p)
{
	glUseProgram(_setup_p -> win.sh_prg);

	//((_setup_p -> unifs).val).ar = aspect_ratio;
	((_setup_p -> unifs).val).vp = -10.0f;

	//((_setup_p -> unifs).loc).ar = glGetUniformLocation(_setup_p -> win.sh_prg, "u_aspect");
	((_setup_p -> unifs).loc).vp = glGetUniformLocation(_setup_p -> win.sh_prg, "u_view_port");


	{
	GLint st = glGetUniformLocation(_setup_p -> win.sh_prg, "u_exp_start");
	GLint en = glGetUniformLocation(_setup_p -> win.sh_prg, "u_exp_end");

	glUniform2f(st, .002f, 0.f);
	glUniform2f(en, .002f, 1.f);
	}
	
	//glUniform1f(((_setup_p -> unifs).loc).ar, ((_setup_p -> unifs).val).ar);
	glUniform1f(((_setup_p -> unifs).loc).vp, ((_setup_p -> unifs).val).vp);
	EXP_Close(&_setup_p -> ext.exp);
	_setup_p -> sub_wins.exp.is_op = FALSE;
	if (_setup_p -> sub_wins.ted.is_op)
		_setup_p -> focused_sub_win = TED_FOCUS;
	else if (_setup_p -> sub_wins.trm.is_op)
		_setup_p -> focused_sub_win = TRM_FOCUS;
	else
		_setup_p -> focused_sub_win = NON_FOCUS;
	
}

void IDE_TRM_Open(IDE_setup* _setup_p)
{
	glUseProgram(_setup_p -> win.sh_prg);

	RECT win_rect;
	GetClientRect(_setup_p -> win.handle, &win_rect);
	//RECT exp_rect = {win_rect.left, win_rect.top, win_rect.left + (win_rect.right - win_rect.left) / 5, win_rect.bottom};
	//MATH_ivec2 vp = {win_rect.right - win_rect.left, 0.5f};
	MATH_ivec2 vp = {2.5f, 0.5f};

	//((_setup_p -> unifs).val).ar = aspect_ratio;
	//glViewport(_setup.vp.left, _setup.vp.top, _setup.vp.right, _setup.vp.bottom);
	//((_setup_p -> unifs).val).vp = 1.0f;

	//RECT r = (RECT)_setup_p -> ext.exp.vp;
	//((_setup_p -> unifs).loc).ar = glGetUniformLocation(_setup_p -> win.sh_prg, "u_aspect");

	//glUniform1f(((_setup_p -> unifs).loc).ar, ((_setup_p -> unifs).val).ar);
	//glUniform1f(((_setup_p -> unifs).loc).vp, ((_setup_p -> unifs).val).vp);
	////glUniform2f(((_setup_p -> unifs).loc).vp, ((_setup_p -> unifs).val).exp_pos.x, ((_setup_p -> unifs).val).exp_pos.y);
	//glUniform2f(((_setup_p -> unifs).loc).exp_pos, vp.x, vp.y);

	//GLint st = glGetUniformLocation(_setup_p -> win.sh_prg, "u_exp_start");
	//GLint en = glGetUniformLocation(_setup_p -> win.sh_prg, "u_exp_end");

	//RECT rt = (RECT)_setup_p -> ext.ted.vp;
	//float t = (float)(rt.right - rt.left) / (float)(win_rect.right - win_rect.left);
	//glUniform2f(st, t+.004f, 0.f);
	//glUniform2f(en, t+.004f, 1.f);

	TRM_Open(&_setup_p -> ext.trm);
	_setup_p -> focused_sub_win = TRM_FOCUS;
	(_setup_p -> sub_wins).trm.is_op = TRUE;
}

void IDE_TRM_Close(IDE_setup* _setup_p)
{
	glUseProgram(_setup_p -> win.sh_prg);

	//((_setup_p -> unifs).val).ar = aspect_ratio;
	//((_setup_p -> unifs).val).vp = -10.0f;

	////((_setup_p -> unifs).loc).ar = glGetUniformLocation(_setup_p -> win.sh_prg, "u_aspect");
	//((_setup_p -> unifs).loc).vp = glGetUniformLocation(_setup_p -> win.sh_prg, "u_view_port");


	//{
	//GLint st = glGetUniformLocation(_setup_p -> win.sh_prg, "u_exp_start");
	//GLint en = glGetUniformLocation(_setup_p -> win.sh_prg, "u_exp_end");

	//glUniform2f(st, .002f, 0.f);
	//glUniform2f(en, .002f, 1.f);
	//}
	//
	////glUniform1f(((_setup_p -> unifs).loc).ar, ((_setup_p -> unifs).val).ar);
	//glUniform1f(((_setup_p -> unifs).loc).vp, ((_setup_p -> unifs).val).vp);
	TRM_Close();
	_setup_p -> sub_wins.trm.is_op = FALSE;
	if (_setup_p -> sub_wins.exp.is_op)
		_setup_p -> focused_sub_win = EXP_FOCUS;
	else if (_setup_p -> sub_wins.ted.is_op)
		_setup_p -> focused_sub_win = TED_FOCUS;
	else
		_setup_p -> focused_sub_win = NON_FOCUS;
	
}

IDE_root_win IDE_Win_Init(HINSTANCE _h_instance)
{
	WNDCLASSEX w_class = (WNDCLASSEX){0};

	w_class.cbSize = sizeof(WNDCLASSEX);
	w_class.lpfnWndProc   = IDE_Proc;
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
    			WS_MAXIMIZE | WS_POPUP,// WS_OVERLAPPEDWINDOW
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

	IDE_root_win win_res = {0};
	win_res.handle = win_handle;
	win_res.class = w_class;

	//LONG style = WS_MAXIMIZE | WS_POPUP;
	////style &= ~(WS_CAPTION | WS_THICKFRAME | WS_BORDER);
	//SetWindowLongPtr(win_handle, GWL_STYLE, (LONG)style);

	return win_res;
}

HWND IDE_Init(HINSTANCE _h_instance)
{
	IDE_setup* setup_p = (IDE_setup*)malloc(sizeof(IDE_setup));

	setup_p -> win = IDE_Win_Init(_h_instance);

	setup_p -> ogl_s = OGL_Init(setup_p -> win.handle);

	char* fontpath = "./fonts/segoeprb.ttf";
	setup_p -> font_buf = OGL_Setup_Font_Buffer(fontpath);
	setup_p -> ext.exp.font_buf = setup_p -> font_buf;
	setup_p -> ext.ted.font_buf = setup_p -> font_buf;
	setup_p -> ext.trm.font_buf = setup_p -> font_buf;

	HOK_Init(setup_p -> win.handle);
	HOK_Enable();
	setup_p -> win.is_focus = TRUE;
	IDE_Extern_Init(setup_p);
	ShowWindow(setup_p -> win.handle, SW_MAXIMIZE);
	char* vertex_path = "./sources/Shaders/Ide/vertex.shader";
	char* frag_path = "./sources/Shaders/Ide/frag.shader";

	setup_p -> win.sh_prg = OGL_Setup_Program(vertex_path, frag_path);
	setup_p -> focused_sub_win = NON_FOCUS;

	IDE_Init_Uniforms(setup_p);
	//IDE_leaders leaders = (IDE_leaders)(setup_p -> leaders);
	setup_p -> leaders.win = FALSE;

	SetWindowLongPtr(setup_p -> win.handle, GWLP_USERDATA, (LONG_PTR)setup_p);

	UpdateWindow(setup_p -> win.handle);
	return setup_p -> win.handle;
}

void IDE_Extern_Init(IDE_setup* _setup_p)
{
	RECT win_rect;
	GetClientRect(_setup_p -> win.handle, &win_rect);

	RECT exp_rect = {win_rect.left + 3, win_rect.top+2, win_rect.left + (win_rect.right - win_rect.left) / 5, win_rect.bottom-4};
	RECT ted_rect = {win_rect.left + 8  + (win_rect.right - win_rect.left) / 5, win_rect.top+2, win_rect.right - (win_rect.right - win_rect.left) / 5, win_rect.bottom-4};
	//RECT trm_rect = {win_rect.left + 8  + (win_rect.right - win_rect.left) / 5, win_rect.bottom + 2 + (win_rect.bottom -win_rect.top) / 5, win_rect.right, win_rect.bottom-4};
	RECT trm_rect = {win_rect.left + 8  + (win_rect.right - win_rect.left) / 5, win_rect.top, win_rect.right - (win_rect.left + 8  + (win_rect.right - win_rect.left) / 5), win_rect.bottom / 4};
	//printf("top : %d\nbtm : %d\n", win_rect.top, win_rect.bottom);

	_setup_p -> sub_wins.exp.is_op = FALSE;
	_setup_p -> sub_wins.ted.is_op = FALSE;
	_setup_p -> sub_wins.trm.is_op = FALSE;

	_setup_p -> ext.exp.vp = (RECT){exp_rect.left, exp_rect.top, exp_rect.right, exp_rect.bottom};
	_setup_p -> ext.ted.vp = (RECT){ted_rect.left, ted_rect.top, ted_rect.right, ted_rect.bottom};
	_setup_p -> ext.trm.vp = (RECT){trm_rect.left, trm_rect.top, trm_rect.right, trm_rect.bottom};

	EXP_Init(&(_setup_p -> ext.exp));
	TED_Init(&(_setup_p -> ext.ted));
	TRM_Init(&(_setup_p -> ext.trm));

	//printf("l : %d, t : %d, r : %d, b : %d\n", ted_rect.left, ted_rect.top, ted_rect.right, ted_rect.bottom);
}

void IDE_Init_Uniforms(IDE_setup* _setup_p)
{
	RECT win_rect;
	GetClientRect(_setup_p -> win.handle, &win_rect);
	float aspect_ratio = ((float)(win_rect.right-win_rect.left)) / ((float)(win_rect.bottom - win_rect.top));
	((_setup_p -> unifs).val).ar = aspect_ratio;
	((_setup_p -> unifs).val).vp = -10.0f;

	glUseProgram(_setup_p -> win.sh_prg);

	((_setup_p -> unifs).loc).ar = glGetUniformLocation(_setup_p -> win.sh_prg, "u_aspect");
	((_setup_p -> unifs).loc).vp = glGetUniformLocation(_setup_p -> win.sh_prg, "u_view_port");
	((_setup_p -> unifs).loc).exp_pos = glGetUniformLocation(_setup_p -> win.sh_prg, "u_exp_pos");
	((_setup_p -> unifs).loc).exp_dim = glGetUniformLocation(_setup_p -> win.sh_prg, "u_exp_dim");

	glUniform1f(((_setup_p -> unifs).loc).ar, ((_setup_p -> unifs).val).ar);
	glUniform1f(((_setup_p -> unifs).loc).vp, ((_setup_p -> unifs).val).vp);
}

void IDE_Render(IDE_setup _ide_setup)
{
	//printf("ide_ren ");
	RECT win_rect;
	GetClientRect(_ide_setup.win.handle, &win_rect);
	glViewport(0, 0, win_rect.right, win_rect.bottom);
	glUseProgram(_ide_setup.win.sh_prg);
	glClear(GL_COLOR_BUFFER_BIT);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

	if (_ide_setup.sub_wins.exp.is_op)
	{
		//printf("exp_ren ");
		EXP_Render(_ide_setup.ext.exp);
	}

	if (_ide_setup.sub_wins.ted.is_op)
	{
		//printf("ted_ren ");
		TED_Render(_ide_setup.ext.ted);
	}

	if (_ide_setup.sub_wins.trm.is_op)
	{
		//printf("exp_ren ");
		TRM_Render(_ide_setup.ext.trm);
	}

	SwapBuffers(_ide_setup.ogl_s.h_dc);
}

void IDE_Clean_Up(HWND _h_win)
{
	IDE_setup* setup = (IDE_setup*)GetWindowLongPtr(_h_win, GWLP_USERDATA);

	HOK_Disable();

	EXP_Clean_Up(&(setup->ext).exp);
	TRM_Clean_Up(&(setup->ext).trm);

	DestroyWindow(_h_win);
	wglMakeCurrent(NULL, NULL);
        ReleaseDC(_h_win, setup -> ogl_s.h_dc);

	wglDeleteContext (setup -> ogl_s.h_rc);

	free(setup -> font_buf.alphabet_outline);
	free(setup);
}
