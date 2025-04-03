#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>
#include <SDL3/SDL.h>

#include <mystructs.h>
#include <mymath.h>
#include <mymacros.h>
#include <fileutils.h>
#include <sdffont.h>
#include <ttfutils.h>

const char *v_sh = "#version 430 core\n"
	    "layout (location = 0) in vec4 pos;\n"
	    "layout (location = 1) in vec2 in_uv;\n"
	    "out vec2 uv;\n"
	    "void main() {\n"
	    "    gl_Position = pos;\n"
	    "	 uv = in_uv;\n"
	    "}";

#define uint unsigned int
int get_program()
{
	char* frag = read_file("./sources./Shaders/frag.shader");
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &v_sh, NULL);
	glCompileShader(vertexShader);

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, (const char**)&frag, NULL);
	glCompileShader(fragmentShader);

	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	free(frag);
	return shaderProgram;
}

void update_grid_indecies(char* _editable_file, int* _alphabet_outline, int* _ssbo_indecies, ivec2 _grid_size, int _starting_index, ivec2 _offset, int* _lines, int _max)
{

	//clear grid
	for (int grid_index = 0; grid_index < _grid_size.x * _grid_size.y; grid_index++)
	{
		_ssbo_indecies[grid_index] = -1;
	}


	//get index
	int text_index = 0;
	for (int i = 0; i < _offset.y; i++)
	{
		text_index += _lines[i];
	}

	//setup lines
	for (int cur_line = 0; cur_line < _grid_size.y; cur_line++)
	{
		for (int grid_index = 0, line_index = text_index; _offset.x < _lines[_offset.y + cur_line] && grid_index < _grid_size.x; grid_index++, line_index++)
		{
			if (_editable_file[line_index + _offset.x] == '\n')
				break;
			if (_editable_file[line_index + _offset.x] == '\t')
			{
				grid_index += 4;
				continue;
			}
			if (_editable_file[line_index + _offset.x] <= ' ')
				continue;
			_ssbo_indecies[grid_index + _grid_size.x * cur_line] = _alphabet_outline[_editable_file[line_index + _offset.x] - 33] - _starting_index;
		}

		text_index += _lines[_offset.y + cur_line];
	}
}

void insert_char(char* _editable_file,char _key,ivec2 _file_offset, int* _lines_size)
{
	_editable_file[0] = _key;
}

int main(int argc, char** argv)
{
	const int TEXT_BUFFER_SIZE = 4 * 1024 * 1024;

	if (argc < 2)
	{
		printf("you need to enter a file path to edit");
		return 1;
	}

	FILE* file;
	int file_size;
	int file_lines_max = 0;
	int file_cursor;
	int current_file_line = 0;
	char* editable_file = open_file_to_edit(argv[1], &file, &file_size);

	if (!file) {
		printf("fopen failed\n");
		return 2;
	}

	if (editable_file == NULL)
	{
		printf("you need to enter a correct file path to edit");
		return 1;
	}

	SDL_Init(SDL_INIT_VIDEO);

	int winWidth = 1024, winHeight = 1024;
	bool focus_lost = false;
	ivec2 screen_cur_pos = {0, 0};
	ivec2 file_cur_pos = {0, 0};
	irect glyph_box;

	float edge = .01f;
	Uint64 timer = 0;

	SDL_Window* window = SDL_CreateWindow("mywindow", winWidth, winHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN);


	if (window == NULL)
	{
		printf("error in win");
		return 0;
	}
	else
		SDL_GetWindowSize(window, &winWidth, &winHeight);

	char* fontpath = "./fonts/segoeprb.ttf";
	int* alphabet_outline;
	init_all_characters(&alphabet_outline, &glyph_box, fontpath);

	int starting_index = alphabet_outline[0];
	int ssbo_alphabet_size = alphabet_outline[starting_index - 1];

	ivec2 grid_aspect_ratio = {5, 3};
	ivec2 grid_size = SCA2i(15, grid_aspect_ratio);
	int* ssbo_grid_indecies = (int*)malloc(grid_size.x*grid_size.y * sizeof(int));
	int* lines_size = (int*)malloc(1024 * sizeof(int));
	ivec2 file_offset = {0, 0};

	for (int line_index = 0, file_buffer_index = 0; line_index < 1024 && file_buffer_index < file_size; line_index++)
	{
		int ls = 0;
		while (editable_file[file_buffer_index + ls] != '\n')
			ls++;
		ls++;
		lines_size[line_index] = ls;
		file_buffer_index += ls;
		file_lines_max++;
	}
	//printf("m : %d\n", file_lines_max);


	for (int i = 0; i < grid_size.x * grid_size.y; i++)
	{
		ssbo_grid_indecies[i] = -1;
	}

	
	ivec2 cellule_size = {winWidth / grid_size.x, winHeight / grid_size.y};

	SDL_GLContext glcontext = SDL_GL_CreateContext(window);
	gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);

	float main_vertices[] = { -1.0f, -1.0f, 0.0f, 1.0f, 
			  -1.0f, 1.0f, 0.0, 0.0f,
			  1.0f, 1.0f, 1.0f, 0.0f,
			  1.0f, -1.0f, 1.0f, 1.0f };

	unsigned int indices[] = {
	    0, 1, 2,  // First triangle
	    0, 2, 3   // Second triangle
	};


	uint main_buf_verts;
	glGenBuffers(1, &main_buf_verts);
	glBindBuffer(GL_ARRAY_BUFFER, main_buf_verts);
	glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float), main_vertices, GL_STATIC_DRAW);

	uint indices_buf;
	glGenBuffers(1, &indices_buf);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_buf);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(float), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4,0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	GLint ssbo_alphabet_buf;
	glGenBuffers(1, &ssbo_alphabet_buf);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_alphabet_buf);



	glBufferData(GL_SHADER_STORAGE_BUFFER, ssbo_alphabet_size * sizeof(int), alphabet_outline + starting_index, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo_alphabet_buf);


	GLint ssbo_grid_indecies_buf;
	glGenBuffers(1, &ssbo_grid_indecies_buf);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_grid_indecies_buf);

	update_grid_indecies(editable_file, alphabet_outline, ssbo_grid_indecies, grid_size, starting_index, (ivec2){0, 0}, lines_size, file_lines_max);
	//printf("Error code: %d\n", errno);
	//printf("ex lp : \n");
	glBufferData(GL_SHADER_STORAGE_BUFFER, grid_size.x * grid_size.y * sizeof(int), ssbo_grid_indecies, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo_grid_indecies_buf);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	GLuint main_shader_program = get_program();
	glUseProgram(main_shader_program);

	//--> get uniforms locations
	GLint grid_location = glGetUniformLocation(main_shader_program, "u_grid");
	GLint cursor_location = glGetUniformLocation(main_shader_program, "u_cursor");
	GLint upm_location = glGetUniformLocation(main_shader_program, "u_upm");
	GLint cel_size_location = glGetUniformLocation(main_shader_program, "u_cel_size");
	GLint edge_location = glGetUniformLocation(main_shader_program, "u_edge");
	GLint timer_location = glGetUniformLocation(main_shader_program, "u_timer");
	//GLint index_location = glGetUniformLocation(main_shader_program, "u_index");
	//--> end

	

	glUniform2i(grid_location, grid_size.x, grid_size.y);
	glUniform2i(cel_size_location, cellule_size.x, cellule_size.y);
	glUniform2i(cursor_location, screen_cur_pos.x, screen_cur_pos.y);
	glUniform2f(upm_location, glyph_box.xMax - glyph_box.xMin, glyph_box.yMax - glyph_box.yMin);
	glUniform1f(edge_location, edge);
	glUniform1f(timer_location, 0.0f);

	while (1)
	{
		timer = SDL_GetTicks();

		SDL_Event event;
		SDL_PollEvent(&event);

		if (event.type == SDL_EVENT_QUIT || (SDL_EVENT_KEY_DOWN && event.key.key == SDLK_C && SDL_GetKeyboardState(NULL)[SDL_SCANCODE_LCTRL]))
			break;


		if (event.window.type == SDL_EVENT_WINDOW_FOCUS_LOST)
			focus_lost = true;
		if (event.window.type == SDL_EVENT_WINDOW_FOCUS_GAINED)
			focus_lost = false;

		if (focus_lost)
			continue;

		if (event.type == SDL_EVENT_KEY_DOWN)
		{
			if (event.key.key >= SDLK_A && event.key.key >= SDLK_Z)
			{
				char key = event.key.key - SDLK_A + 'a';

				insert_char(editable_file, key, file_offset, lines_size);
				update_grid_indecies(editable_file, alphabet_outline, ssbo_grid_indecies, grid_size, starting_index, file_offset, lines_size, file_lines_max);
				glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_grid_indecies_buf);
				glBufferData(GL_SHADER_STORAGE_BUFFER, grid_size.x * grid_size.y * sizeof(int), ssbo_grid_indecies, GL_DYNAMIC_COPY);
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo_grid_indecies_buf);
				glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
			}

			switch (event.key.key)
			{
				case SDLK_UP:
					screen_cur_pos.y--;
					file_cur_pos.y--;
					if (screen_cur_pos.y == -1)
					{
						screen_cur_pos.y++;
						if (file_cur_pos.y > -1)
						{
							file_offset.y = file_cur_pos.y;
							update_grid_indecies(editable_file, alphabet_outline, ssbo_grid_indecies, grid_size, starting_index, file_offset, lines_size, file_lines_max);
							glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_grid_indecies_buf);
							glBufferData(GL_SHADER_STORAGE_BUFFER, grid_size.x * grid_size.y * sizeof(int), ssbo_grid_indecies, GL_DYNAMIC_COPY);
							glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo_grid_indecies_buf);
							glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
						}
						else
							file_cur_pos.y = 0;
					}
					glUniform2i(cursor_location, screen_cur_pos.x, screen_cur_pos.y);
					break;
				case SDLK_DOWN:
					screen_cur_pos.y++;
					file_cur_pos.y++;
					if (screen_cur_pos.y == grid_size.y)
					{
						screen_cur_pos.y--;
						if (file_cur_pos.y < file_lines_max)
						{
							file_offset.y = file_cur_pos.y - grid_size.y + 1;
							update_grid_indecies(editable_file, alphabet_outline, ssbo_grid_indecies, grid_size, starting_index, file_offset, lines_size, file_lines_max);
							glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_grid_indecies_buf);
							glBufferData(GL_SHADER_STORAGE_BUFFER, grid_size.x * grid_size.y * sizeof(int), ssbo_grid_indecies, GL_DYNAMIC_COPY);
							glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo_grid_indecies_buf);
							glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
						}
						else
							file_cur_pos.y = file_lines_max - 1;
					}
					else if (screen_cur_pos.y + file_offset.y == file_lines_max)
					{
						file_cur_pos.y = file_lines_max - 1;
						screen_cur_pos.y--;
					}
					glUniform2i(cursor_location, screen_cur_pos.x, screen_cur_pos.y);
					break;
				case SDLK_RIGHT:
					screen_cur_pos.x++;
					file_cur_pos.x++;
					if (screen_cur_pos.x == grid_size.x || screen_cur_pos.x == lines_size[file_cur_pos.y])
					{
						screen_cur_pos.x --;
						if (file_cur_pos.x < lines_size[file_cur_pos.y])
						{
							file_offset.x = file_cur_pos.x - grid_size.x + 1;
							update_grid_indecies(editable_file, alphabet_outline, ssbo_grid_indecies, grid_size, starting_index, file_offset, lines_size, file_lines_max);
							glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_grid_indecies_buf);
							glBufferData(GL_SHADER_STORAGE_BUFFER, grid_size.x * grid_size.y * sizeof(int), ssbo_grid_indecies, GL_DYNAMIC_COPY);
							glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo_grid_indecies_buf);
							glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
						}
						else
							file_cur_pos.x = lines_size[file_cur_pos.y] - 1;
					}
					glUniform2i(cursor_location, screen_cur_pos.x, screen_cur_pos.y);
					break;
				case SDLK_LEFT:
					screen_cur_pos.x--;
					file_cur_pos.x--;
					if (screen_cur_pos.x == -1)
					{
						screen_cur_pos.x++;
						if (file_cur_pos.x > -1)
						{
							file_offset.x = file_cur_pos.x;
							update_grid_indecies(editable_file, alphabet_outline, ssbo_grid_indecies, grid_size, starting_index, file_offset, lines_size, file_lines_max);
							glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_grid_indecies_buf);
							glBufferData(GL_SHADER_STORAGE_BUFFER, grid_size.x * grid_size.y * sizeof(int), ssbo_grid_indecies, GL_DYNAMIC_COPY);
							glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo_grid_indecies_buf);
							glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
						}
						else
							file_cur_pos.x = 0;
					}
					glUniform2i(cursor_location, screen_cur_pos.x, screen_cur_pos.y);
					break;
				case SDLK_KP_PLUS:
					if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_LCTRL])
					{
						grid_size = ADD2i(grid_size, grid_aspect_ratio);
						cellule_size = (ivec2){winWidth / grid_size.x, winHeight / grid_size.y};
						glUniform2i(grid_location, grid_size.x, grid_size.y);
						glUniform2i(cel_size_location, cellule_size.x, cellule_size.y);
						//update_grid_indecies(editable_file, alphabet_outline, ssbo_grid_indecies, grid_size, starting_index);
					}
					else
					{
						edge = fmaxf(edge - 0.001f, -.5f);
						glUniform1f(edge_location, edge);
					}
					break;
				case SDLK_KP_MINUS:
					if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_LCTRL])
					{
						grid_size = max_iv2(ADD2i(grid_size, SCA2i(-1, grid_aspect_ratio)), grid_aspect_ratio);
						cellule_size = (ivec2){winWidth / grid_size.x, winHeight / grid_size.y};
						glUniform2i(grid_location, grid_size.x, grid_size.y);
						glUniform2i(cel_size_location, cellule_size.x, cellule_size.y);
						//update_grid_indecies(editable_file, alphabet_outline, ssbo_grid_indecies, grid_size, starting_index);
					}
					else
					{
						edge = fminf(edge + 0.001f, .5f);
						glUniform1f(edge_location, edge);
					}
					break;
			}
		}

		Uint64 frame_duration = SDL_GetTicks() - timer;
		uint fps = 16;
		if (frame_duration < fps)
		{
			SDL_Delay(fps-frame_duration);
		}


		glUniform1f(timer_location, (float)((double)timer / 1000.0));

		glClear(GL_COLOR_BUFFER_BIT);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
		SDL_GL_SwapWindow(window);

	}
	free(alphabet_outline);
	free(ssbo_grid_indecies);
	free(lines_size);
	fclose(file);
	free(editable_file);
	SDL_GL_DestroyContext(glcontext);  

	return 0;
	
}
