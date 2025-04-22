#include <OGL_utils.h>

OGL_setup OGL_Init(HWND _h_win)
{
	OGL_setup set = OGL_Init_Loader(_h_win);

	OGL_Init_Vertices();

	return set;
}

OGL_font_buffer OGL_Setup_Font_Buffer(char* _fontpath)
{
	FNT_rect glyph_box;
	int* alphabet_outline = FNT_Init(&glyph_box, _fontpath);

	int starting_index = alphabet_outline[0];
	int ssbo_alphabet_size = alphabet_outline[starting_index - 1];

	GLint ssbo_alphabet_buf;
	glGenBuffers(1, &ssbo_alphabet_buf);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_alphabet_buf);
	glBufferData(GL_SHADER_STORAGE_BUFFER, ssbo_alphabet_size * sizeof(int), alphabet_outline + starting_index, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo_alphabet_buf);

	return (OGL_font_buffer){glyph_box, 
				alphabet_outline, 
				starting_index, 
				ssbo_alphabet_size};
	
	

	//GLint ssbo_alphabet_buf;
	//GLint ssbo_grid_indecies_buf;
	//glGenBuffers(1, &ssbo_grid_indecies_buf);
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_grid_indecies_buf);


}

void OGL_Init_Vertices()
{
	float ide_vertices[] = { -1.0f, -1.0f, 0.0f, 1.0f, 
			  -1.0f, 1.0f, 0.0, 0.0f,
			  1.0f, 1.0f, 1.0f, 0.0f,
			  1.0f, -1.0f, 1.0f, 1.0f };

	unsigned int ide_indices[] = {
	    0, 1, 2,  // First triangle
	    0, 2, 3   // Second triangle
	};
		
	GLuint ide_arr_verts;
	GLuint ide_buf_verts;
	GLuint ide_buf_indcs;

	glGenVertexArrays(1, &ide_arr_verts);
	glGenBuffers(1, &ide_buf_verts);
	glGenBuffers(1, &ide_buf_indcs);

	glBindVertexArray(ide_arr_verts);
	glBindBuffer(GL_ARRAY_BUFFER, ide_buf_verts);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ide_buf_indcs);

	glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float), ide_vertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(float), ide_indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4,0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);


}

OGL_setup OGL_Init_Loader(HWND _h_win)
{
	HDC hdc = GetDC(_h_win);

	PIXELFORMATDESCRIPTOR pfd;

	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;

	pfd.cDepthBits = 0;//32;
	pfd.cStencilBits = 0;//8;

	int pixelFormat = ChoosePixelFormat(hdc, &pfd);
	SetPixelFormat(hdc, pixelFormat, &pfd );

	HGLRC tempRC = wglCreateContext(hdc);
	wglMakeCurrent(hdc, tempRC);
	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;
	wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");

	const int attribList[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
		WGL_CONTEXT_MINOR_VERSION_ARB, 6,
		WGL_CONTEXT_FLAGS_ARB, 0,
		WGL_CONTEXT_PROFILE_MASK_ARB,
		WGL_CONTEXT_COREPROFILE_BIT_ARB, 0, 
	};
	
	HGLRC hglrc = wglCreateContextAttribsARB(hdc, 0, attribList);
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(tempRC);
	wglMakeCurrent(hdc, hglrc);

	if (!gladLoadGL())
	{
		printf("failed\n");
	}
	return (OGL_setup){hdc, hglrc};
}

GLuint OGL_Setup_Program(char* _vertex_file_path, char* _frag_file_path)
{
	//const char* vertex_source = (const char*)FLS_Read_File(_vertex_file_path);
	//const char* frag_source = (const char*)FLS_Read_File(_frag_file_path);
	
	char st_vertex_file_path[124];
	char st_frag_file_path[124];

	strcpy(st_vertex_file_path, _vertex_file_path);
	strcpy(st_frag_file_path, _frag_file_path);

	const char* vertex_source = (const char*)OGL_Expand_File(st_vertex_file_path);
	const char* frag_source = (const char*)OGL_Expand_File(st_frag_file_path);


	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_source, NULL);
	glCompileShader(vertex_shader);

	GLuint frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag_shader, 1, (const char**)&frag_source, NULL);
	glCompileShader(frag_shader);

	GLuint shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, frag_shader);
	glLinkProgram(shader_program);

	glDeleteShader(vertex_shader);
	glDeleteShader(frag_shader);
	free((void*)vertex_source);
	//printf("%s\n$$\n", frag_source);
	free((void*)frag_source);
	return shader_program;
}

char* OGL_Get_File_Name_From_Include(char* _line)
{
	char* st = TXT_Find_Char(_line, '"');
	
	if (st == NULL)
		return NULL;

	st++;
	char* end = TXT_Find_Char(st, '"');
	
	if (end == NULL || end - st == 0)
		return NULL;

	int len = end - st;
	char* str = malloc(sizeof(char) * len);

	for (int i = 0; i < len; i++)
		str[i] = st[i];

	str[len] = 0;

	return str;
}

char* OGL_Expand_File_Name_From_Include(char* _cd, char* _line)
{
	char* st = TXT_Find_Char(_line, '"');
	
	if (st == NULL)
		return NULL;

	st++;
	char* end = TXT_Find_Char(st, '"');
	
	if (end == NULL || end - st == 0)
		return NULL;

	int cd_len = strlen(_cd);

	int sp = 0;

	//if (cd_len != 0 && _cd[cd_len - 1] != '/')
	//	sp++;

	int len = end - st;
	char* str = malloc(sizeof(char) * (len + cd_len + sp));

	for (int i = 0; i < cd_len; i++)
		str[i] = _cd[i];

	int j = cd_len;
	
	//if (cd_len != 0 && _cd[cd_len - 1] != '/')
	//	_cd[j++] = '/';

	for (int i = 0; i < len; i++)
		str[i + j] = st[i];

	str[len + j] = 0;

	return str;
}

int OGL_Includes_Number(char* _text)
{
	char* line = _text;
	char* nxt = TXT_Get_Next_Line(line);
	int res = 0;

	while (nxt)
	{
		int ln = nxt - line;

		if (TXT_Is_Line_Start_By(line, "#include"))
			res++;

		line += ln;
		nxt = TXT_Get_Next_Line(line);
	}
		

	return res;
}

char* OGL_Expand_File(char* _org_file_path)
{
	char* org_file_txt = FLS_Read_File(_org_file_path);
	
	int includes_nbr = OGL_Includes_Number(org_file_txt);
	if (includes_nbr == 0)
		return org_file_txt;


	int org_text_size = strlen(org_file_txt);

	char** include_files_path = (char**)malloc(sizeof(char*) * includes_nbr);
	int* include_lines_sizes = (int*)malloc(sizeof(int) * includes_nbr);
	int* include_files_sizes = (int*)malloc(sizeof(int) * includes_nbr);

	{
	char* start = org_file_txt;
	char* nxt = TXT_Get_Next_Line(start);
	int i = 0;
	char* path = _org_file_path;
	{
	char* coc = strrchr(path, '/');
	if (coc != NULL)
		coc[1] = 0;
	}
	while (nxt)
	{
		int ln = nxt - start;

		if (TXT_Is_Line_Start_By(start, "#include"))
		{
			include_lines_sizes[i] = ln;
			include_files_path[i++] = OGL_Expand_File_Name_From_Include(path, start);
		}

		start += ln;
		nxt = TXT_Get_Next_Line(start);
	}
	}


	for (int i = 0; i < includes_nbr; i++)
	{
		char* fi = FLS_Read_File(include_files_path[i]);

		if (fi != NULL)
			include_files_sizes[i] = strlen(fi);
		else
			include_files_sizes[i] = 0;

		free(include_files_path[i]);
		include_files_path[i] = fi;
	}

	int final_size = org_text_size;
	for (int i = 0; i < includes_nbr; i++)
	{
		if (include_files_sizes[i])
			final_size += include_files_sizes[i] - include_lines_sizes[i];
	}

	char* final_text = (char*)malloc((final_size + 1) * sizeof(char));

	{
	char* start = org_file_txt;
	char* nxt = TXT_Get_Next_Line(start);
	int i = 0;
	char* path = _org_file_path;
	int cur = 0;
	
	while (nxt)
	{
		int ln = nxt - start;

		if (TXT_Is_Line_Start_By(start, "#include"))
		{
			if (include_files_sizes[i])
			{
				memcpy(final_text + cur, include_files_path[i], include_files_sizes[i]);
				cur += include_files_sizes[i];
				free(include_files_path[i]);
			}
			else
			{
				memcpy(final_text + cur, start, ln);
				cur += ln;
			}
			i++;
		}
		else
		{
			memcpy(final_text + cur, start, ln);
			cur += ln;
		}


		start += ln;
		nxt = TXT_Get_Next_Line(start);
	}
	}

	final_text[final_size] = 0;

	free(include_lines_sizes);
	free(include_files_sizes);
	free(org_file_txt);
	free(include_files_path);

	return final_text;
}
