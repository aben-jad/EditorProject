#include <glad/glad.h>
#include <SDL3/SDL.h>
#include <IDE_utils_sdl.h>
#include <OGL_utils.h>
#include <stdio.h>

int IDE_Init(IDE_setup* _ide_setup_p)
{
	SDL_Init(SDL_INIT_VIDEO);

	int winWidth = 1024, winHeight = 1024;

	_ide_setup_p->sdl_window_p = SDL_CreateWindow("mywindow", winWidth, winHeight, SDL_WINDOW_OPENGL);// | SDL_WINDOW_FULLSCREEN);



	if (_ide_setup_p->sdl_window_p == NULL)
	{
		printf("error in win");
		return 1;
	}
	else
		SDL_GetWindowSize(_ide_setup_p->sdl_window_p, &winWidth, &winHeight);

	_ide_setup_p->sdl_gl_context = SDL_GL_CreateContext(_ide_setup_p->sdl_window_p);
	gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);

	_ide_setup_p -> ide_is_focus = true;
	_ide_setup_p -> ide_is_full_screen = true;

	float ide_vertices[] = { -1.0f, -1.0f, 0.0f, 1.0f, 
			  -1.0f, 1.0f, 0.0, 0.0f,
			  1.0f, 1.0f, 1.0f, 0.0f,
			  1.0f, -1.0f, 1.0f, 1.0f };

	unsigned int ide_indices[] = {
	    0, 1, 2,  // First triangle
	    0, 2, 3   // Second triangle
	};
		
	char* vertex_path = "./sources/Shaders/Ide/vertex.shader";
	char* frag_path = "./sources/Shaders/Ide/frag.shader";

	GLuint ide_buf_verts;
	glGenBuffers(1, &ide_buf_verts);
	glBindBuffer(GL_ARRAY_BUFFER, ide_buf_verts);
	glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float), ide_vertices, GL_STATIC_DRAW);

	GLuint ide_indices_buf;
	glGenBuffers(1, &ide_indices_buf);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ide_indices_buf);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(float), ide_indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4,0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	_ide_setup_p -> ide_shader_program = OGL_Setup_Program(vertex_path, frag_path);

	return 0;
}

void IDE_Render(IDE_setup _ide_setup)
{
	//SDL_SetWindowFullscreen(_ide_setup.sdl_window_p, false);
	glUseProgram(_ide_setup.ide_shader_program);
	glClear(GL_COLOR_BUFFER_BIT);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
	SDL_GL_SwapWindow(_ide_setup.sdl_window_p);
}

void IDE_Clean_Up(IDE_setup _ide_setup)
{
	SDL_GL_DestroyContext(_ide_setup.sdl_gl_context);  
	SDL_DestroyWindow(_ide_setup.sdl_window_p);
	SDL_Quit();
}
