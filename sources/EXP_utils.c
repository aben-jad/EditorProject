#include <EXP_utils.h>

void EXP_Update_Grid_Indecies(char** _buffer_lines, int* _alphabet_outline, int* _ssbo_indecies, MATH_ivec2 _grid_size, int _starting_index)
{
	for (int grid_index = 0; grid_index < _grid_size.x * _grid_size.y; grid_index++)
	{
		_ssbo_indecies[grid_index] = -1;
	}

	for (int cur_line = 0, grid_index = 0; _buffer_lines[cur_line] != NULL; cur_line++)
	{
		int starting_char = 0;
		while (_buffer_lines[cur_line][starting_char])
		{
			_ssbo_indecies[grid_index] = _alphabet_outline[_buffer_lines[cur_line][starting_char] - 33]- _starting_index;
			grid_index++;
			starting_char++;
		}
		int tmp = grid_index / _grid_size.x;
		grid_index = (tmp+1) * _grid_size.x;
	}
}

void EXP_Init_Uniforms(EXP_setup* _setup_p)
{
	float aspect_ratio = ((float)(_setup_p -> vp.bottom - _setup_p -> vp.top)) / ((float)(_setup_p -> vp.right - _setup_p -> vp.left));
	((_setup_p -> unifs).val).ar = aspect_ratio;

	glUseProgram(_setup_p -> sh_prg);

	((_setup_p -> unifs).loc).ar = glGetUniformLocation(_setup_p -> sh_prg, "u_aspect");
	((_setup_p -> unifs).loc).grid = glGetUniformLocation(_setup_p -> sh_prg, "u_grid");
	((_setup_p -> unifs).loc).line_pos = glGetUniformLocation(_setup_p -> sh_prg, "u_line");

	(_setup_p -> unifs).val.line_pos = 0.0f;

	int winWidth = (_setup_p -> vp.right - _setup_p -> vp.left);
	int winHeight = (_setup_p -> vp.bottom - _setup_p -> vp.top);
	MATH_ivec2 grid_size = (MATH_ivec2)_setup_p -> grid_data.grid_size;
	MATH_ivec2 cellule_size = {winWidth / grid_size.x, winHeight / grid_size.y};

	GLint cel_size_location = glGetUniformLocation(_setup_p -> sh_prg, "u_cel_size");
	glUniform2i(cel_size_location, cellule_size.x, cellule_size.y);

	GLint upm_location = glGetUniformLocation(_setup_p -> sh_prg, "u_upm");
	glUniform2f(upm_location, _setup_p -> font_buf.glyph_box.xMax - _setup_p -> font_buf.glyph_box.xMin, _setup_p -> font_buf.glyph_box.yMax - _setup_p -> font_buf.glyph_box.yMin);


	glUniform2i(((_setup_p -> unifs).loc).grid, grid_size.x, grid_size.y);
	glUniform1f(((_setup_p -> unifs).loc).ar, ((_setup_p -> unifs).val).ar);
	glUniform1f(((_setup_p -> unifs).loc).line_pos, ((_setup_p -> unifs).val).line_pos);
}

void EXP_Handle_Input(EXP_setup* _setup_p, WPARAM _w_param)
{
	if (_w_param == VK_DOWN)
		((_setup_p->unifs).val).line_pos += 1.0f;
	else if (_w_param == VK_UP)
		((_setup_p->unifs).val).line_pos -= 1.0f;
	else if (_w_param == VK_LEFT)
	{
		//printf("bf : %s\n", (_setup_p -> cur_dir).path);
		char* tmp = strrchr( _setup_p -> cur_dir.path, '\\');
		if (tmp == NULL)	return;

		DWORD dir_size = (tmp-_setup_p -> cur_dir.path);
		(_setup_p -> cur_dir).path[dir_size] = '/';
		(_setup_p -> cur_dir).path[dir_size + 1] = '*';
		(_setup_p -> cur_dir).path[dir_size + 2] = 0;
		//printf("af : %s\n", (_setup_p -> cur_dir).path);
		//
		((_setup_p->unifs).val).line_pos = 0.0f;

		EXP_Close(_setup_p);
		EXP_Open(_setup_p);

	}
	else if (_w_param == VK_RIGHT)
	{
		char* path = (_setup_p -> cur_dir).path;
		char* drc = (_setup_p -> cur_dir).items[(_setup_p->cur_dir).cur];
		int len = strlen(path);
		path[len - 2] = '\\';
		strcpy(path + len - 1, drc);
		len = strlen(path);
		//printf("%s$$\n", path);
		DWORD attributes = GetFileAttributes(path);
		if (attributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			(_setup_p -> cur_dir).path[len] = '/';
			(_setup_p -> cur_dir).path[len + 1] = '*';
			(_setup_p -> cur_dir).path[len + 2] = 0;


			((_setup_p->unifs).val).line_pos = 0.0f;
			
			EXP_Close(_setup_p);
			EXP_Open(_setup_p);
			//printf("af : %s\n", path);
			//printf("%s is a directory.\n", path);
		}
		else
		{
			path[len - 2] = '/';
			path[len - 1] = '*';
		}
	}

	float cv = (_setup_p->unifs).val.line_pos;
	//float gy = _setup_p -> grid_data.grid_size.y;
	float gy = _setup_p -> cur_dir.size;
	glUseProgram(_setup_p->sh_prg);
	(_setup_p->unifs).val.line_pos = (cv == -1.0f) ? 0.0f : (cv == gy) ? gy - 1.0f : cv;
	(_setup_p->cur_dir).cur = (_setup_p->unifs).val.line_pos;
	glUniform1f(((_setup_p -> unifs).loc).line_pos, ((_setup_p -> unifs).val).line_pos);
}

void EXP_Init(EXP_setup* _setup_p)
{
	_setup_p -> cur_dir.path = (char*)malloc(128 * sizeof(char));
	_setup_p -> cur_dir.items = (char**)malloc(128 * sizeof(char*));

	DWORD dir_size = GetCurrentDirectory(128, _setup_p -> cur_dir.path);
	(_setup_p -> cur_dir).path[dir_size] = '/';
	(_setup_p -> cur_dir).path[dir_size + 1] = '*';
	(_setup_p -> cur_dir).path[dir_size + 2] = 0;

	char* vertex_path = "./sources/Shaders/Exp/vertex.shader";
	char* frag_path = "./sources/Shaders/Exp/frag.shader";

	_setup_p -> sh_prg = OGL_Setup_Program(vertex_path, frag_path);

	_setup_p -> grid_data.grid_size = (MATH_ivec2){30, 40};

	_setup_p -> grid_data.grid_indecies = (int*)malloc( _setup_p -> grid_data.grid_size.x * _setup_p -> grid_data.grid_size.y * sizeof(int));

	glGenBuffers(1, &(_setup_p -> grid_data.indecies_buf));
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, _setup_p -> grid_data.indecies_buf);
	glBufferData(GL_SHADER_STORAGE_BUFFER, (_setup_p -> grid_data.grid_size.x) * (_setup_p -> grid_data.grid_size.y) * sizeof(int), NULL, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, _setup_p -> grid_data.indecies_buf);


	EXP_Init_Uniforms(_setup_p);

}

void EXP_Open(EXP_setup* _setup_p)
{
	char** buffer_lines = (_setup_p -> cur_dir).items;
	
	WIN32_FIND_DATA fd;
	HANDLE h = FindFirstFile((_setup_p -> cur_dir).path, &fd);
	BOOL t = TRUE;
	t = FindNextFile(h, &fd);
	t = FindNextFile(h, &fd);
	int start_buf = 0;
	while(t)
	{
		char* ln = fd.cFileName;
		buffer_lines[start_buf]= strdup(ln);
		start_buf++;
		t = FindNextFile(h, &fd);
	}
	FindClose(h);
	(_setup_p -> cur_dir).size = start_buf;
	buffer_lines[start_buf] = NULL;

	//FNT_rect glyph_box;
	//int* alphabet_outline;
	//int starting_index;
	//int ssbo_alphabet_size;
	//GLint ssbo_alphabet_buf;

	int* alphabet_outline = (_setup_p -> font_buf).alphabet_outline;
	int starting_index = (_setup_p -> font_buf).starting_index;
	//MATH_ivec2 grid_size = {8,10};// for now!
	//grid_size = size of all grid!
	//ssbo_grid_indecies = buffer that contain indecies value!
	//int* ssbo_grid_indecies = (_setup_p -> grid_data).grid_indecies;
	MATH_ivec2 offset = {0,0};// for now!

	EXP_Update_Grid_Indecies(buffer_lines, alphabet_outline, (_setup_p -> grid_data).grid_indecies, (_setup_p -> grid_data).grid_size, starting_index);

	//for (int i = 0; buffer_lines[i] != NULL;i++)
	//	free(buffer_lines[i]);
	//free(buffer_lines);
	//for (int grid_index = 0; grid_index < (_setup_p -> grid_data).grid_size.x * (_setup_p -> grid_data).grid_size.y; grid_index++)
	//	printf("%d ", (_setup_p -> grid_data).grid_indecies[grid_index]);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, (_setup_p -> grid_data).indecies_buf);
	glBufferData(GL_SHADER_STORAGE_BUFFER, (_setup_p -> grid_data).grid_size.x * (_setup_p -> grid_data).grid_size.y * sizeof(int), (_setup_p -> grid_data).grid_indecies, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, (_setup_p -> grid_data).indecies_buf);

	//glBufferData(GL_SHADER_STORAGE_BUFFER, grid_size.x * grid_size.y * sizeof(int), ssbo_grid_indecies, GL_DYNAMIC_COPY);
	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo_grid_indecies_buf);

	//update_grid_indecies(editable_file, alphabet_outline, ssbo_grid_indecies, grid_size, starting_index, (ivec2){0, 0}, lines_size, file_lines_max);
	//glBufferData(GL_SHADER_STORAGE_BUFFER, grid_size.x * grid_size.y * sizeof(int), ssbo_grid_indecies, GL_DYNAMIC_COPY);
	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo_grid_indecies_buf);
}

void EXP_Close(EXP_setup* _setup_p)
{
	char** buffer_lines = _setup_p -> cur_dir.items;

	for (int i = 0; buffer_lines[i] != NULL;i++)
		free(buffer_lines[i]);
}

void EXP_Render(EXP_setup _setup)
{
	glViewport(_setup.vp.left, _setup.vp.top, _setup.vp.right, _setup.vp.bottom);
	glUseProgram(_setup.sh_prg);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
}


void EXP_Clean_Up(EXP_setup* _setup_p)
{
	free(_setup_p -> cur_dir.path);
	free(_setup_p -> cur_dir.items);
}
