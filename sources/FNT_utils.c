#include <FNT_utils.h>

void FNT_Update_Grid_Indecies(char* _buffer_text, int* _alphabet_outline, int* _ssbo_indecies, MATH_ivec2 _grid_size, int _starting_index, MATH_ivec2 _offset)
{

	//_buffer_text = "adjd\nhe\nfhfk\nnd\n";
	//printf("%s\n", _buffer_text);
	int lines_number = TXT_Get_Lines_Number(_buffer_text);
	if (lines_number > _grid_size.y)
		lines_number = _grid_size.y;

	//printf("l n : %d\ng s : %d\n", lines_number, _grid_size.y);
	int _lines[lines_number];

	for (int i = 0, line_start = 0; i < lines_number; i++)
	{
		int tmp = TXT_Get_Line_Size(_buffer_text + line_start);
		//printf("l s : %d\n", tmp);
		_lines[i] = tmp;
		line_start += tmp + 1;
	}

	//clear grid
	for (int grid_index = 0; grid_index < _grid_size.x * _grid_size.y; grid_index++)
	{
		_ssbo_indecies[grid_index] = -1;
	}

	//return;

	//get index
	int text_index = 0;
	for (int i = 0; i < _offset.y; i++)
	{
		text_index += _lines[i];
	}

	//printf("grid_size : (%d, %d)\n", _grid_size.x, _grid_size.y);
	//setup lines
	for (int cur_line = 0; cur_line < lines_number; cur_line++)
	{
		for (int grid_index = 0, line_index = text_index; _offset.x < _lines[_offset.y + cur_line] && grid_index < _grid_size.x; grid_index++, line_index++)
		{
			//printf("$%c", _buffer_text[line_index + _offset.x]);
			if (!_buffer_text[line_index + _offset.x])
				return ;
			if (_buffer_text[line_index + _offset.x] == '\n')
				break;
			if (_buffer_text[line_index + _offset.x] == '\t')
			{
				grid_index += 4;
				continue;
			}
			if (_buffer_text[line_index + _offset.x] <= ' ')
				continue;
			_ssbo_indecies[grid_index + _grid_size.x * cur_line] = _alphabet_outline[_buffer_text[line_index + _offset.x] - 33] - _starting_index;
			//printf("\n*%c\n", _buffer_text[line_index + _offset.x]);
		}

		//printf("%%%c%%", _buffer_text);
		text_index += _lines[_offset.y + cur_line];
	}
}


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

int* FNT_Init(FNT_rect* _rect, char* _font_path)
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

	FT_Done_Face(face);
	FT_Done_FreeType(library);

	return adr;
}


