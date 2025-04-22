#ifndef __EXP_UTILS_H__
#define __EXP_UTILS_H__

#include <OGL_utils.h>
typedef struct
{
	GLint ar;
	GLint grid;
	GLint line_pos;
} EXP_uni_locs;

typedef struct
{
	float ar;
	float grid;
	float line_pos;
} EXP_uni_vals;

typedef struct
{
	EXP_uni_locs loc;
	EXP_uni_vals val;
} EXP_uniforms;

typedef struct
{
	char* path;
	char** items;
	int cur;
	int size;

} EXP_dir_data;

typedef struct
{
	RECT vp;
	GLuint sh_prg;
	OGL_font_buffer font_buf;
	FNT_grid_data grid_data;
	EXP_uniforms unifs;
	EXP_dir_data cur_dir;
} EXP_setup;

void EXP_Init(EXP_setup* _setup_p);
void EXP_Clean_Up(EXP_setup* _setup_p);
void EXP_Open(EXP_setup* _setup_p);
void EXP_Close(EXP_setup* _setup_p);
void EXP_Render(EXP_setup _setup_p);
void EXP_Handle_Input(EXP_setup* _setup_p, WPARAM _w_param);
void EXP_Update_Grid_Indecies(char** _buffer_lines, int* _alphabet_outline, int* _ssbo_indecies, MATH_ivec2 _grid_size, int _starting_index);

#endif
