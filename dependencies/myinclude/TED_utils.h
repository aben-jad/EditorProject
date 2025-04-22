#ifndef __TED_UTILS_H__
#define __TED_UTILS_H__

#include <OGL_utils.h>
typedef struct
{
	GLint ar;
	GLint grid;
	GLint cur_pos;
} TED_uni_locs;

typedef struct
{
	float ar;
	float grid;
	MATH_ivec2 cur_pos;
} TED_uni_vals;

typedef struct
{
	TED_uni_locs loc;
	TED_uni_vals val;
} TED_uniforms;

typedef struct
{
	RECT vp;
	GLuint sh_prg;
	OGL_font_buffer font_buf;
	FNT_grid_data grid_data;
	TED_uniforms unifs;
} TED_setup;

void TED_Init(TED_setup* _setup_p);
void TED_Open(TED_setup _setup);
void TED_Close();
void TED_Render(TED_setup _setup_p);
void TED_Handle_Input(TED_setup* _setup_p, WPARAM _w_param);

#endif
