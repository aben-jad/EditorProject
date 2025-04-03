#ifndef _IDE_UTILS_H_
#define _IDE_UTILS_H_

#include <SDL3/SDL.h>

typedef struct
{
	SDL_Window* sdl_window_p;
	SDL_GLContext sdl_gl_context;
	GLuint ide_shader_program;
	bool ide_is_full_screen;
	bool ide_is_focus;
} IDE_setup;

int IDE_Init(IDE_setup* _ide_setup_p);
void IDE_Render(IDE_setup _ide_setup);
void IDE_Clean_Up(IDE_setup _ide_setup);

#endif
