#ifndef __TRM_UTILS_H__
#define __TRM_UTILS_H__

#include <OGL_utils.h>
#include <PIPE_utils.h>
#include <BUF_utils.h>

#define TRM_IN_BUF_SIZE 128

typedef struct
{
	char* buf_in;
	BUF_strings* buf_out;
	int cmd_cur;
} TRM_stdio_data;

typedef struct
{
	GLint ar;
	GLint grid;
	GLint cur_pos;
} TRM_uni_locs;

typedef struct
{
	float ar;
	float grid;
	MATH_fvec2 cur_pos;
} TRM_uni_vals;

typedef struct
{
	TRM_uni_locs loc;
	TRM_uni_vals val;
} TRM_uniforms;

typedef struct
{
	RECT vp;
	GLuint sh_prg;
	OGL_font_buffer font_buf;
	FNT_grid_data grid_data;
	TRM_stdio_data io_data;
	PIPE_setup pipe_setup;
	TRM_uniforms unifs;
} TRM_setup;

void TRM_Init(TRM_setup* _setup_p);
void TRM_Clean_Up(TRM_setup* _setup_p);
void TRM_Open(TRM_setup* _setup_p);
void TRM_Close();
void TRM_Render(TRM_setup _setup_p);
void TRM_Handle_Input(TRM_setup* _setup_p, UINT _msg, WPARAM _w_param);
void TRM_Update_Grid_Indecies(BUF_strings* _buf_out, char* _buf_in, int* _alphabet_outline, int* _ssbo_indecies, MATH_ivec2 _grid_size, int _starting_index, MATH_ivec2 _off);
void TRM_RUN(TRM_setup* _setup_p);
MATH_fvec2 TRM_Get_Cursor_Position(BUF_strings* _buf_out, char* _buf_in, MATH_ivec2 _grid_dim);

#endif
