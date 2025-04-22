#ifndef _FNT_UTILS_H_
#define _FNT_UTILS_H_

#include <ft2build.h>
#include <MATH_utils.h>
#include <TXT_utils.h>
#include <glad/glad.h>

#include FT_FREETYPE_H

typedef struct
{
	int xMin, xMax, yMin, yMax;
} FNT_rect;

typedef struct
{
	GLint indecies_buf;
	MATH_ivec2 grid_size;
	int* grid_indecies;
} FNT_grid_data;


typedef struct
{
	int x, y;
} FNT_grid_size;

void FNT_Update_Grid_Indecies(char* _buffer_text, int* _alphabet_outline, int* _ssbo_indecies, MATH_ivec2 _grid_size, int _starting_index, MATH_ivec2 _offset);
int* FNT_Init(FNT_rect* _rect, char* _font_path);

#endif
