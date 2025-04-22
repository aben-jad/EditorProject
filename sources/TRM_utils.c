#include <TRM_utils.h>

MATH_fvec2 TRM_Get_Cursor_Position(BUF_strings* _buf_out, char* _buf_in, MATH_ivec2 _grid_dim)
{
	MATH_fvec2 cur = {0.0f, 0.0f};
	while (_buf_out)
	{
		int len = strlen(_buf_out -> str);
		cur.y += len / _grid_dim.x;
		if (_buf_out -> next == NULL)
		{
			cur.x += (len-1) % _grid_dim.x;
		}
		else
		{
			cur.y++;
		}
		_buf_out = _buf_out -> next;
	}

	int len = strlen(_buf_in) + cur.x;

	cur.x = len % _grid_dim.x;
	cur.y += len / _grid_dim.x;

	return cur;
}

void TRM_Update_Grid_Indecies(BUF_strings* _buf_out, char* _buf_in, int* _alphabet_outline, int* _ssbo_indecies, MATH_ivec2 _grid_size, int _starting_index, MATH_ivec2 _off)
{
	for (int grid_index = 0; grid_index < _grid_size.x * _grid_size.y; grid_index++)
	{
		_ssbo_indecies[grid_index] = -1;
	}

	int grid_index = 0;
	for (int cur_line = 0; _buf_out != NULL; cur_line++)
	{
		int starting_char = 0;
		while ((_buf_out -> str)[starting_char])
		{
			if ((_buf_out -> str)[starting_char] >= 32 && (_buf_out -> str)[starting_char] <= 126)
			{
				_ssbo_indecies[grid_index] = _alphabet_outline[(_buf_out -> str)[starting_char] - 33]- _starting_index;
				grid_index++;
			}
			starting_char++;
		}
		if (_buf_out -> next != NULL)
		{
			int tmp = grid_index / _grid_size.x;
			grid_index = (tmp+1) * _grid_size.x;
		}
		_buf_out = _buf_out -> next;
	}

	grid_index--;
	int starting_char = 0;
	while (_buf_in[starting_char])
	{
		_ssbo_indecies[grid_index] = _alphabet_outline[_buf_in[starting_char] - 33]- _starting_index;
		grid_index++;
		starting_char++;
	}
}

void TRM_Init_Uniforms(TRM_setup* _setup_p)
{
	
	float aspect_ratio = ((float)(_setup_p -> vp.bottom - _setup_p -> vp.top)) / ((float)(_setup_p -> vp.right - _setup_p -> vp.left));
	((_setup_p -> unifs).val).ar = aspect_ratio;

	glUseProgram(_setup_p -> sh_prg);

	((_setup_p -> unifs).loc).ar = glGetUniformLocation(_setup_p -> sh_prg, "u_aspect");
	((_setup_p -> unifs).loc).grid = glGetUniformLocation(_setup_p -> sh_prg, "u_grid");
	((_setup_p -> unifs).loc).cur_pos = glGetUniformLocation(_setup_p -> sh_prg, "u_cur");

	//(_setup_p -> unifs).val.cur_pos = (MATH_fvec2){0.0f, 0.0f};

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
	glUniform2f(((_setup_p -> unifs).loc).cur_pos, ((_setup_p -> unifs).val).cur_pos.x, ((_setup_p -> unifs).val).cur_pos.y);
	
}

int shift = 0;

void TRM_RUN(TRM_setup* _setup_p)
{
	PIPE_Write(_setup_p -> pipe_setup.in_write, _setup_p -> io_data.buf_in);
	//char* old_in = strdup(_setup_p -> io_data.buf_in);

	//BUF_Insert_Line(&_setup_p -> io_data.buf_out, old_in, BUF_Get_Count(_setup_p -> io_data.buf_out) - 1);

	Sleep(1000);
	char* buf = PIPE_Read(_setup_p -> pipe_setup.out_read);
	printf("%s", buf);
//return;
	BUF_strings* str_buf = BUF_Text_To_Strings(buf);
//return;
	BUF_Add_Text(_setup_p -> io_data.buf_out, str_buf);
	free(buf);
	ZeroMemory(_setup_p -> io_data.buf_in, TRM_IN_BUF_SIZE);
	_setup_p -> io_data.cmd_cur = 0;
	//BUF_Add_Line(_setup_p -> io_data.buf_out, _setup_p -> io_data.buf_in);
	(_setup_p -> unifs).val.cur_pos = TRM_Get_Cursor_Position(_setup_p -> io_data.buf_out, _setup_p -> io_data.buf_in,  _setup_p -> grid_data.grid_size);

	int* alphabet_outline = _setup_p ->font_buf.alphabet_outline;
	int starting_index = _setup_p -> font_buf.starting_index;

	TRM_Update_Grid_Indecies(_setup_p -> io_data.buf_out, _setup_p -> io_data.buf_in, alphabet_outline, _setup_p -> grid_data.grid_indecies, _setup_p -> grid_data.grid_size, starting_index, (MATH_ivec2){0, 0});
	glUseProgram(_setup_p -> sh_prg);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, _setup_p -> grid_data.indecies_buf);
	glBufferData(GL_SHADER_STORAGE_BUFFER, (_setup_p -> grid_data.grid_size.x) * (_setup_p -> grid_data.grid_size.y) * sizeof(int), _setup_p -> grid_data.grid_indecies, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, _setup_p -> grid_data.indecies_buf);
	glUniform2f(((_setup_p -> unifs).loc).cur_pos, (_setup_p -> unifs).val.cur_pos.x, (_setup_p -> unifs).val.cur_pos.y);
	//PIPE_setup pipe_setup;
	//HANDLE in_write;
	//HANDLE out_read;

	//TRM_stdio_data io_data;
	//char* buf_in;
	//BUF_strings* buf_out;
	//int cmd_cur;

	//printf("%s", _cmd);
	//(_setup_p -> pipe_setup)
}

void TRM_Handle_Input(TRM_setup* _setup_p, UINT _msg, WPARAM _w_param)
{

	//if (_w_param == VK_LSHIFT)
	//	shift = (_msg == WM_KEYDOWN) ? 1 : 0;
	if (_msg == WM_KEYDOWN && _w_param == VK_RETURN)
	{
		_setup_p -> io_data.buf_in[_setup_p -> io_data.cmd_cur] = '\n';
		TRM_RUN(_setup_p);
	}

	if (_msg == WM_CHAR && _w_param > 31 && _w_param < 127)
	{
		//printf("sh : %d\nw_pw: %c\n", shift, _w_param);
		//if (_w_param >= 'a' && _w_param <= 'z' && shift)
		//	_w_param += ('a' - 'A');
		//printf("sh : %d\nw_pw: %c\n", shift, _w_param);
		if (_w_param == '-')
			_w_param = '|';
		_setup_p -> io_data.buf_in[_setup_p -> io_data.cmd_cur] = _w_param;
		_setup_p -> io_data.cmd_cur++;
		int* alphabet_outline = _setup_p ->font_buf.alphabet_outline;
		int starting_index = _setup_p -> font_buf.starting_index;
		(_setup_p -> unifs).val.cur_pos = TRM_Get_Cursor_Position(_setup_p -> io_data.buf_out, _setup_p -> io_data.buf_in, _setup_p -> grid_data.grid_size);
		TRM_Update_Grid_Indecies(_setup_p -> io_data.buf_out, _setup_p -> io_data.buf_in, alphabet_outline, _setup_p -> grid_data.grid_indecies, _setup_p -> grid_data.grid_size, starting_index, (MATH_ivec2){0, 0});
		glUseProgram(_setup_p -> sh_prg);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, _setup_p -> grid_data.indecies_buf);
		glBufferData(GL_SHADER_STORAGE_BUFFER, (_setup_p -> grid_data.grid_size.x) * (_setup_p -> grid_data.grid_size.y) * sizeof(int), _setup_p -> grid_data.grid_indecies, GL_DYNAMIC_COPY);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, _setup_p -> grid_data.indecies_buf);
		glUniform2f(((_setup_p -> unifs).loc).cur_pos, (_setup_p -> unifs).val.cur_pos.x, (_setup_p -> unifs).val.cur_pos.y);
	}
	
	else if (_w_param == VK_BACK && _setup_p -> io_data.cmd_cur)
	{
		_setup_p -> io_data.cmd_cur--;
		_setup_p -> io_data.buf_in[_setup_p -> io_data.cmd_cur] = 0;
		int* alphabet_outline = _setup_p ->font_buf.alphabet_outline;
		int starting_index = _setup_p -> font_buf.starting_index;
		(_setup_p -> unifs).val.cur_pos = TRM_Get_Cursor_Position(_setup_p -> io_data.buf_out,  _setup_p -> io_data.buf_in, _setup_p -> grid_data.grid_size);
		TRM_Update_Grid_Indecies(_setup_p -> io_data.buf_out,	_setup_p -> io_data.buf_in, alphabet_outline, _setup_p -> grid_data.grid_indecies, _setup_p -> grid_data.grid_size, starting_index, (MATH_ivec2){0, 0});
		glUseProgram(_setup_p -> sh_prg);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, _setup_p -> grid_data.indecies_buf);
		glBufferData(GL_SHADER_STORAGE_BUFFER, (_setup_p -> grid_data.grid_size.x) * (_setup_p -> grid_data.grid_size.y) * sizeof(int), _setup_p -> grid_data.grid_indecies, GL_DYNAMIC_COPY);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, _setup_p -> grid_data.indecies_buf);
		glUniform2f(((_setup_p -> unifs).loc).cur_pos, (_setup_p -> unifs).val.cur_pos.x, (_setup_p -> unifs).val.cur_pos.y);
	}

	/*if (_w_param == VK_RIGHT)
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
	//printf("uo\n");*/
}

void TRM_Init(TRM_setup* _setup_p)
{
	char* vertex_path = "./sources/Shaders/Trm/vertex.shader";
	char* frag_path = "./sources/Shaders/Trm/frag.shader";

	_setup_p -> sh_prg = OGL_Setup_Program(vertex_path, frag_path);

	_setup_p -> grid_data.grid_size = (MATH_ivec2){60, 15};

	_setup_p -> io_data.buf_in = calloc(TRM_IN_BUF_SIZE , sizeof(char));
	_setup_p -> io_data.cmd_cur = 0;

	_setup_p -> grid_data.grid_indecies = (int*)malloc( _setup_p -> grid_data.grid_size.x * _setup_p -> grid_data.grid_size.y * sizeof(int));

	glGenBuffers(1, &(_setup_p -> grid_data.indecies_buf));
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, _setup_p -> grid_data.indecies_buf);
	glBufferData(GL_SHADER_STORAGE_BUFFER, (_setup_p -> grid_data.grid_size.x) * (_setup_p -> grid_data.grid_size.y) * sizeof(int), NULL, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, _setup_p -> grid_data.indecies_buf);

	int processID = PIPE_Create_Proc(&(_setup_p -> pipe_setup));
	if (!processID)
	{
        	printf("Failed to start %d\n", GetLastError());
	}

	Sleep(2000);

	char* buf = PIPE_Read(_setup_p -> pipe_setup.out_read);
	_setup_p -> io_data.buf_out = BUF_Text_To_Strings(buf);
	//BUF_Add_Line(_setup_p -> io_data.buf_out, _setup_p -> io_data.buf_in);
	(_setup_p -> unifs).val.cur_pos = TRM_Get_Cursor_Position(_setup_p -> io_data.buf_out,  _setup_p -> io_data.buf_in, _setup_p -> grid_data.grid_size);
	free(buf);

	TRM_Init_Uniforms(_setup_p);

}

void TRM_Clean_Up(TRM_setup* _setup_p)
{
	CloseHandle((_setup_p -> pipe_setup).proc_info.hProcess);
	
	free(_setup_p -> io_data.buf_in);
	free(_setup_p -> grid_data.grid_indecies);
}

void TRM_Open(TRM_setup* _setup_p)
{
	int* alphabet_outline = _setup_p -> font_buf.alphabet_outline;
	int starting_index = _setup_p -> font_buf.starting_index;

	glUseProgram(_setup_p -> sh_prg);

	TRM_Update_Grid_Indecies(_setup_p -> io_data.buf_out,	_setup_p -> io_data.buf_in, alphabet_outline, _setup_p -> grid_data.grid_indecies, _setup_p -> grid_data.grid_size, starting_index, (MATH_ivec2){0, 0});


	glBindBuffer(GL_SHADER_STORAGE_BUFFER, _setup_p -> grid_data.indecies_buf);
	glBufferData(GL_SHADER_STORAGE_BUFFER, _setup_p -> grid_data.grid_size.x * _setup_p -> grid_data.grid_size.y * sizeof(int), _setup_p -> grid_data.grid_indecies, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, _setup_p -> grid_data.indecies_buf);
}

void TRM_Close()
{
}

void TRM_Render(TRM_setup _setup)
{
	glViewport(_setup.vp.left, _setup.vp.top, _setup.vp.right, _setup.vp.bottom);
	glUseProgram(_setup.sh_prg);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
}

