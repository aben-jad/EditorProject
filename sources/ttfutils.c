#include <stdio.h>
#include <stdlib.h>
#include <ft2build.h>
#include <mystructs.h>
#include FT_FREETYPE_H

int init_ttf(char* _fontpath, FT_Library* _library, FT_Face* _face, FT_BBox* _box)
{


	int error = FT_Init_FreeType( _library );
	if (error)
	{
		printf("... an error occurred during library initialization ...");
	}


	error = FT_New_Face(*_library, _fontpath, 0, _face);
	if ( error == FT_Err_Unknown_File_Format )
	{
		  printf("... the font file could be opened and read, but it appears"
			    "... that its font format is unsupported");
	}
	else if ( error )
	{
		  printf("... another error code means that the font file could not"
			    "... be opened or read, or that it is broken...");
	}

	*_box = (*_face)->bbox;

	return error;
}

int load_glyph(FT_Face _face, char _c, FT_Outline** _outline)
{

	FT_UInt glyph_index = FT_Get_Char_Index(_face, _c);
	FT_Error error = FT_Load_Glyph(_face, glyph_index, FT_LOAD_NO_SCALE);
	FT_GlyphSlot slot = _face->glyph;
	*_outline = &slot->outline;

	return error;
}

void init_all_characters(int** _memadr, irect* _rect, char* _font_path)
{
	FT_Library  library;
	FT_Face face;


	FT_BBox bbox;
	FT_Error error = init_ttf(_font_path, &library, &face, &bbox);
	_rect->xMin = bbox.xMin;
	_rect->yMin = bbox.yMin;
	_rect->xMax = bbox.xMax;
	_rect->yMax = bbox.yMax;

	FT_Outline* outline;
	int alphapet_size = 0;

	char start_char = 33;
	char end_char = 127;

	for (char i =start_char; i < end_char; i++)
	{
		load_glyph(face, i, &outline);
		alphapet_size += outline -> n_points +1;
	}


	int* adr = (int*)malloc((alphapet_size + (end_char - start_char +1)) * sizeof(int));

	int current_char_index = end_char - start_char + 1;
	for (char char_index = start_char; char_index < end_char; char_index++)
	{
		load_glyph(face, char_index, &outline);

		adr[char_index - start_char] = current_char_index;
		adr[current_char_index] = 0;

		for (int i = 0; i < outline->n_contours; i++)
		{
			adr[current_char_index] += (outline->contours[i] << 6*i);
		}

		for (int i =0; i < outline->n_points; i++)
		{
			FT_Vector point = outline->points[i];
			char tag = outline->tags[i];
			adr[current_char_index + 1 + i] = (point.x-bbox.xMin) + ((point.y-bbox.yMin) << 15) + (tag << 30);
		}
		current_char_index += outline -> n_points + 1;
	}
	adr[end_char-start_char] = current_char_index;

	*_memadr = adr;

	FT_Done_Face(face);
	FT_Done_FreeType(library);
}
