#ifndef _IDE_UTILS_H_
#define _IDE_UTILS_H_

#include <glad/glad.h>
#include <windows.h>
#include <stdio.h>
#include <OGL_utils.h>
#include <EXP_utils.h>
#include <TED_utils.h>
#include <TRM_utils.h>

typedef struct
{
	BOOL win;
} IDE_leaders;

typedef struct
{
	BOOL is_op;
	RECT view_port;
} IDE_sub_win_data;

typedef enum
{
	NON_FOCUS,
	EXP_FOCUS,
	TED_FOCUS,
	TRM_FOCUS
} IDE_SUB_WINS_ENUM;

typedef struct
{
	IDE_sub_win_data exp;
	IDE_sub_win_data ted;
	IDE_sub_win_data trm;
} IDE_sub_wins_op;

typedef struct
{
	EXP_setup exp;
	TED_setup ted;
	TRM_setup trm;
} IDE_extern_setup;

typedef struct
{
	GLint vp;
	GLint ar;
	GLint exp_pos;
	GLint exp_dim;
} IDE_uni_locs;

typedef struct
{
	float vp;
	float ar;
	MATH_fvec2 exp_pos;
	MATH_fvec2 exp_dim;
} IDE_uni_vals;

typedef struct
{
	IDE_uni_locs loc;
	IDE_uni_vals val;
} IDE_uniforms;

typedef struct
{
	HWND handle;
	WNDCLASSEX class;
	UINT sh_prg;
	RECT dim;
	BOOL is_focus;
} IDE_root_win;

typedef struct
{
	RECT exp;
} IDE_view_ports;

typedef struct
{
	IDE_root_win win;
	OGL_setup ogl_s;
	IDE_leaders leaders;
	IDE_uniforms unifs;
	IDE_sub_wins_op sub_wins;
	OGL_font_buffer font_buf;
	IDE_extern_setup ext;
	IDE_SUB_WINS_ENUM focused_sub_win;
	//void (*current_input_handler)(void*, WPARAM);
} IDE_setup;

HWND IDE_Init(HINSTANCE _h_instance);
void IDE_Render(IDE_setup _ide_setup);
void IDE_Clean_Up(HWND _h_win);
void IDE_Leaders_Management(IDE_leaders* _leaders_p, WPARAM _w_param, BOOL _is_down);
void IDE_Focus_Mode_Enter(IDE_setup* _setup_p);
void IDE_Focus_Mode_Exit (IDE_setup* _setup_p);
void IDE_Init_Uniforms(IDE_setup* _setup_p);
void IDE_Extern_Init(IDE_setup* _setup_p);
void IDE_Handle_Input(IDE_setup* _setup_p, UINT _msg, WPARAM _w_param);
void IDE_EXP_Open(IDE_setup* _setup_p);
void IDE_EXP_Close(IDE_setup* _setup_p);
void IDE_TED_Open(IDE_setup* _setup_p);
void IDE_TED_Close(IDE_setup* _setup_p);
void IDE_TRM_Open(IDE_setup* _setup_p);
void IDE_TRM_Close(IDE_setup* _setup_p);

#endif
