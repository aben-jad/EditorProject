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
	char* frag = read_file("C:\\Users\\adnane\\Desktop\\C-Projects\\Editor\\sources\\Shaders\\frag.shader");
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

int virtual_grid(int _x, int _y, int _u)
{
	int l_s = _x % _u;
	int r_s = _u - l_s;

	int u_s = _y % _u;
	int d_s = _u - u_s;

	int lr_d = (l_s < r_s) ? l_s : r_s;
	int ud_d = (u_s < d_s) ? u_s : d_s;

	return (lr_d < ud_d) ? lr_d : ud_d;
}

void construct_i64(void* _p, unsigned int _a1, unsigned int _a2, unsigned int _a3, unsigned int _a4, unsigned int _a5, unsigned int _a6)
{
	unsigned int a = _a1 & 0x3FF; // Mask to 10 bits
	unsigned int b = _a2 & 0x3FF;
    	unsigned int c = _a3 & 0x3FF;

	unsigned int packed = (c << 20) | (b << 10) | a;
	((unsigned int*)_p)[0] = packed;
	//int cur = 0;
	//int B8 = ipow(2, 10) -1;
	//int B10 = ipow(2, 10) -1;
	//int B20 = ipow(2, 20) -1;
	//int B30 = ipow(2, 30) -1;
	//int B32 = -1;

	//int a1 = _a1;
	//int a2 = (_a2 << 10) & (B20-B10);
	//int a3 = (_a3 << 20) & (B30-(B20+B10));
	//int a4 = ((_a4 & 3) << 30);
	//int a42 = (_a4 >> 2) & B8;
	//int a5 = (_a5 << 12);
	//int a6 = (_a6 << 22);

	//((int*)_p)[0] = a1 | a2 | a3 | a4;
	//((int*)_p)[1] = a42 | a5 | a6;
}


void print_i64(void* _p)
{
	int B8 = ipow(2, 8) -1;
	int B10 = ipow(2, 10) -1;
	int B20 = ipow(2, 20) -1;
	int B30 = ipow(2, 30) -1;
	int B32 = -1;

	int x = *(int*)_p;
	int y = ((int*)_p)[1];

	int a = x & B10;
	int b = (x >> 10) & B10;
	int c = (x >> 20) & B10;
	int d = ((x >> 30) & 3) | ((y & B8) << 2);
	int e = (y >> 12) &B10;
	int f = (y >> 22) &B10;


	printf("%d %d %d %d %d %d\n", a, b ,c, d, e, f);
}

int main(int argc, char** argv)
{
	SDL_Init(SDL_INIT_VIDEO);

	int winWidth = 1024, winHeight = 1024;
	bool focus_lost = false;
	ivec2 cur_pos = {0, 0};
	irect glyph_box;

	float edge = .01f;
	Uint64 timmer = 0;

	SDL_Window* window = SDL_CreateWindow("mywindow", winWidth, winHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN);

	uint ss_buffer_size = 134217728;

	if (window == NULL)
	{
		printf("error in win");
		return 0;
	}
	else
		SDL_GetWindowSize(window, &winWidth, &winHeight);

	int* characters_points;
	init_all_characters(&characters_points, &glyph_box);

	int currentIndex = 0;

	ivec2 grid_aspect_ratio = {2, 1};
	ivec2 grid_size = SCA2i(12, grid_aspect_ratio);
	//printf("%d %d\n", grid_size.x, grid_size.y);
	int* ss_buffer_indecies = (int*)malloc(grid_size.x*grid_size.y * sizeof(int));
	for (int i = 0; i < grid_size.x * grid_size.y; i++)
	{
		ss_buffer_indecies[i] = -1;
	}
	
	//memset(ss_buffer_indecies, 1, grid_size.x*grid_size.y * sizeof(int));
	
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

	GLint ssbo_celdata;
	glGenBuffers(1, &ssbo_celdata);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_celdata);

	char* hola = "hello from my new editor";
	//char* hola = "abcd";
	int str_len = strlen(hola);
	unsigned int* cellData = calloc(ss_buffer_size, 1);
	int cell_data_index = 0;
	for(int i = 0; i < str_len; i++)
	{
		if (hola[i] <= ' ' || hola[i] >= '~')
		{
			ss_buffer_indecies[i] = -1;
			continue;
		}
		int offset = characters_points[hola[i] - 33];
		int next = characters_points[hola[i] - 32];
		int char_curve = (next - offset)  * sizeof(int);
		memcpy(cellData +cell_data_index, characters_points + offset, char_curve);
		ss_buffer_indecies[i] = cell_data_index;
		cell_data_index += char_curve;
	}

	for (int i = 0; i < grid_size.x * grid_size.y; i++)
		printf("%d ", ss_buffer_indecies[i]);

	//glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, grid_size.x * grid_size.y * sizeof(int), ss_buffer_indecies);
	//glBufferSubData(GL_SHADER_STORAGE_BUFFER, grid_size.x * grid_size.y * sizeof(int), (ss_buffer_size - grid_size.x * grid_size.y), cellData);

	glBufferData(GL_SHADER_STORAGE_BUFFER, ss_buffer_size, cellData, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo_celdata);

	GLint ssbo_celindecies;
	glGenBuffers(1, &ssbo_celindecies);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_celindecies);

	glBufferData(GL_SHADER_STORAGE_BUFFER, grid_size.x * grid_size.y * sizeof(int), ss_buffer_indecies, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo_celindecies);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	GLuint main_shader_program = get_program();
	glUseProgram(main_shader_program);

	//--> get uniforms locations
	GLint grid_location = glGetUniformLocation(main_shader_program, "u_grid");
	GLint cursor_location = glGetUniformLocation(main_shader_program, "u_cursor");
	//GLint cpc_location = glGetUniformLocation(main_shader_program, "u_cpc");
	GLint upm_location = glGetUniformLocation(main_shader_program, "u_upm");
	GLint cel_size_location = glGetUniformLocation(main_shader_program, "u_cel_size");
	GLint edge_location = glGetUniformLocation(main_shader_program, "u_edge");
	GLint index_location = glGetUniformLocation(main_shader_program, "u_index");
	//GLint indices_location = glGetUniformLocation(main_shader_program, "u_indecies");
	//--> end

	

	glUniform2i(grid_location, grid_size.x, grid_size.y);
	glUniform2i(cel_size_location, cellule_size.x, cellule_size.y);
	glUniform2i(cursor_location, cur_pos.x, cur_pos.y);
	glUniform2f(upm_location, glyph_box.xMax - glyph_box.xMin, glyph_box.yMax - glyph_box.yMin);
	//glUniform1i(cpc_location, curve_per_cellule);
	glUniform1f(edge_location, edge);
	glUniform1i(index_location, currentIndex);
	//glUniform1iv(indices_location, grid_size.x* grid_size.y, ss_buffer_indecies);

	GLint maxSSBOSize;
	glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &maxSSBOSize);
	printf("%d\n", maxSSBOSize);

	while (1)
	{
		timmer = SDL_GetTicks();

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

			switch (event.key.key)
			{
				case SDLK_UP:
					cur_pos.y--;
					glUniform2i(cursor_location, cur_pos.x, cur_pos.y);
					break;
				case SDLK_DOWN:
					cur_pos.y++;
					glUniform2i(cursor_location, cur_pos.x, cur_pos.y);
					break;
				case SDLK_RIGHT:
					//currentIndex = (currentIndex + 1) % curve_per_cellule;
					//glUniform1i(index_location, currentIndex);
					cur_pos.x++;
					glUniform2i(cursor_location, cur_pos.x, cur_pos.y);
					break;
				case SDLK_LEFT:
					//currentIndex = (currentIndex - 1 + curve_per_cellule) % curve_per_cellule;
					//glUniform1i(index_location, currentIndex);
					cur_pos.x--;
					glUniform2i(cursor_location, cur_pos.x, cur_pos.y);
					break;
				case SDLK_KP_PLUS:
					if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_LCTRL])
					{
						grid_size = ADD2i(grid_size, grid_aspect_ratio);
						cellule_size = (ivec2){winWidth / grid_size.x, winHeight / grid_size.y};
						glUniform2i(grid_location, grid_size.x, grid_size.y);
						glUniform2i(cel_size_location, cellule_size.x, cellule_size.y);
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
					}
					else
					{
						edge = fminf(edge + 0.001f, .5f);
						glUniform1f(edge_location, edge);
					}
					break;
			}
		}

		Uint64 frame_duration = SDL_GetTicks() - timmer;
		uint fps = 16;
		if (frame_duration < fps)
		{
			//printf("%d\n", frame_duration);
			SDL_Delay(fps-frame_duration);
		}


		glClear(GL_COLOR_BUFFER_BIT);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
		SDL_GL_SwapWindow(window);

	}
	free(characters_points);
	free(ss_buffer_indecies);
	SDL_GL_DestroyContext(glcontext);  

	return 0;
	
}
