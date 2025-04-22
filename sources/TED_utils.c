#include <TED_utils.h>



void TED_Init_Uniforms(TED_setup* _setup_p)
{
	float aspect_ratio = ((float)(_setup_p -> vp.bottom - _setup_p -> vp.top)) / ((float)(_setup_p -> vp.right - _setup_p -> vp.left));
	((_setup_p -> unifs).val).ar = aspect_ratio;

	glUseProgram(_setup_p -> sh_prg);

	((_setup_p -> unifs).loc).ar = glGetUniformLocation(_setup_p -> sh_prg, "u_aspect");
	((_setup_p -> unifs).loc).grid = glGetUniformLocation(_setup_p -> sh_prg, "u_grid");
	((_setup_p -> unifs).loc).cur_pos = glGetUniformLocation(_setup_p -> sh_prg, "u_cur");

	(_setup_p -> unifs).val.cur_pos = (MATH_ivec2){0, 0};

	int winWidth = (_setup_p -> vp.right - _setup_p -> vp.left);
	int winHeight = (_setup_p -> vp.bottom - _setup_p -> vp.top);
	MATH_ivec2 grid_size = (MATH_ivec2)_setup_p -> grid_data.grid_size;
	MATH_ivec2 cellule_size = {winWidth / grid_size.x, winHeight / grid_size.y};

	GLint cel_size_location = glGetUniformLocation(_setup_p -> sh_prg, "u_cel_size");
	glUniform2i(cel_size_location, cellule_size.x, cellule_size.y);

	//_setup_p -> font_buf.;

	GLint upm_location = glGetUniformLocation(_setup_p -> sh_prg, "u_upm");
	glUniform2f(upm_location, _setup_p -> font_buf.glyph_box.xMax - _setup_p -> font_buf.glyph_box.xMin, _setup_p -> font_buf.glyph_box.yMax - _setup_p -> font_buf.glyph_box.yMin);


	glUniform2i(((_setup_p -> unifs).loc).grid, grid_size.x, grid_size.y);
	glUniform1f(((_setup_p -> unifs).loc).ar, ((_setup_p -> unifs).val).ar);
	glUniform1f(((_setup_p -> unifs).loc).cur_pos, ((_setup_p -> unifs).val).cur_pos.x);
}

void TED_Handle_Input(TED_setup* _setup_p, WPARAM _w_param)
{
	if (_w_param == VK_RIGHT)
		((_setup_p->unifs).val).cur_pos.x += 1.0f;
	else if (_w_param == VK_LEFT)
		((_setup_p->unifs).val).cur_pos.x -= 1.0f;
	else if (_w_param == VK_DOWN)
		((_setup_p->unifs).val).cur_pos.y += 1.0f;
	else if (_w_param == VK_UP)
		((_setup_p->unifs).val).cur_pos.y -= 1.0f;

	MATH_ivec2 cv = (_setup_p->unifs).val.cur_pos;
	MATH_ivec2 gv = _setup_p -> grid_data.grid_size;
	(_setup_p->unifs).val.cur_pos.x = (cv.x == -1.0f) ? 0.0f : (cv.x == gv.x) ? gv.x - 1.0f : cv.x;
	(_setup_p->unifs).val.cur_pos.y = (cv.y == -1.0f) ? 0.0f : (cv.y == gv.y) ? gv.y - 1.0f : cv.y;
	//printf();
	glUseProgram(_setup_p -> sh_prg);
	glUniform2f(((_setup_p -> unifs).loc).cur_pos,  ((_setup_p -> unifs).val).cur_pos.x, ((_setup_p -> unifs).val).cur_pos.y);
	//printf("uo\n");
}

void TED_Init(TED_setup* _setup_p)
{
	char* vertex_path = "./sources/Shaders/Ted/vertex.shader";
	char* frag_path = "./sources/Shaders/Ted/frag.shader";

	_setup_p -> sh_prg = OGL_Setup_Program(vertex_path, frag_path);

	_setup_p -> grid_data.grid_size = (MATH_ivec2){80, 50};

	//_setup_p -> grid_data.grid_indecies = (int*)malloc( _setup_p -> grid_data.grid_size.x * _setup_p -> grid_data.grid_size.y * sizeof(int));

	//glGenBuffers(1, &(_setup_p -> grid_data.indecies_buf));
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, _setup_p -> grid_data.indecies_buf);
	//glBufferData(GL_SHADER_STORAGE_BUFFER, (_setup_p -> grid_data.grid_size.x) * (_setup_p -> grid_data.grid_size.y) * sizeof(int), NULL, GL_DYNAMIC_COPY);
	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, _setup_p -> grid_data.indecies_buf);


	TED_Init_Uniforms(_setup_p);

}

void TED_Open(TED_setup _setup)
{
	//char* editable_file = (char*)malloc(1024*sizeof(char));
	//
	//WIN32_FIND_DATA fd;
	//HANDLE h = FindFirstFile(".\\*.*", &fd);
	//BOOL t = TRUE;
	//int start_buf = 0;
	//while(t)
	//{
	//	char* ln = fd.cFileName;
	//	int sz = strlen(ln);
	//	strcpy(editable_file + start_buf, ln);
	//	start_buf += sz;
	//	editable_file[start_buf] = '\n';
	//	start_buf++;
	//	//printf("%s\n", fd.cFileName);
	//	t = FindNextFile(h, &fd);
	//}
	////printf("%s",editable_file);
	//FindClose(h);
	//editable_file[start_buf-1] = 0;

	//FNT_rect glyph_box;
	//int* alphabet_outline;
	//int starting_index;
	//int ssbo_alphabet_size;
	//GLint ssbo_alphabet_buf;

	int* alphabet_outline = _setup.font_buf.alphabet_outline;
	int starting_index = _setup.font_buf.starting_index;
	//MATH_ivec2 grid_size = {8,10};// for now!
	//grid_size = size of all grid!
	//ssbo_grid_indecies = buffer that contain indecies value!
	//int* ssbo_grid_indecies = _setup.grid_data.grid_indecies;
	MATH_ivec2 offset = {0,0};// for now!

	//FNT_Update_Grid_Indecies(editable_file, alphabet_outline, _setup.grid_data.grid_indecies, _setup.grid_data.grid_size, starting_index, (MATH_ivec2){0, 0});

	//for (int grid_index = 0; grid_index < _setup.grid_data.grid_size.x * _setup.grid_data.grid_size.y; grid_index++)
	//	printf("%d ", _setup.grid_data.grid_indecies[grid_index]);

	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, _setup.grid_data.indecies_buf);
	//glBufferData(GL_SHADER_STORAGE_BUFFER, _setup.grid_data.grid_size.x * _setup.grid_data.grid_size.y * sizeof(int), _setup.grid_data.grid_indecies, GL_DYNAMIC_COPY);
	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, _setup.grid_data.indecies_buf);

	//glBufferData(GL_SHADER_STORAGE_BUFFER, grid_size.x * grid_size.y * sizeof(int), ssbo_grid_indecies, GL_DYNAMIC_COPY);
	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo_grid_indecies_buf);

	//update_grid_indecies(editable_file, alphabet_outline, ssbo_grid_indecies, grid_size, starting_index, (ivec2){0, 0}, lines_size, file_lines_max);
	//glBufferData(GL_SHADER_STORAGE_BUFFER, grid_size.x * grid_size.y * sizeof(int), ssbo_grid_indecies, GL_DYNAMIC_COPY);
	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo_grid_indecies_buf);
}

void TED_Close()
{
}

void TED_Render(TED_setup _setup)
{
	glViewport(_setup.vp.left, _setup.vp.top, _setup.vp.right, _setup.vp.bottom);
	glUseProgram(_setup.sh_prg);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
}
