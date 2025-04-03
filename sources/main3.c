#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>
#include <SDL3/SDL.h>
#include <windows.h>

#include <mystructs.h>
#include <mymath.h>
#include <mymacros.h>
#include <fileutils.h>
#include <sdffont.h>
#include <ttfutils.h>

#include <IDE_utils.h>


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


static HHOOK keyboardHook = NULL;
static HWND hWnd;

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
//	printf("%s\n","from here");
    //return CallNextHookEx(NULL, nCode, wParam, lParam);
    if (nCode == HC_ACTION) {
	    //bool b;
	    KBDLLHOOKSTRUCT* kbStruct = (KBDLLHOOKSTRUCT*)lParam;

        	UINT msg = (UINT)wParam;              // WM_KEYDOWN or WM_KEYUP
		printf("%s\n", (wParam == WM_KEYDOWN) ? "down" : "up");
        	WPARAM wParamPost = kbStruct->vkCode; // Virtual key code
        	LPARAM lParamPost = 0;

        	// Build lParam (mimics a real WM_KEYDOWN/WM_KEYUP lParam)
        	lParamPost |= (kbStruct->scanCode & 0xFF) << 16; // Bits 16-23: Scan code
        	lParamPost |= (kbStruct->flags & LLKHF_EXTENDED) ? (1 << 24) : 0; // Bit 24: Extended flag
        	lParamPost |= (kbStruct->flags & LLKHF_UP) ? (1 << 31) : 0;       // Bit 31: Transition (1 for up, 0 for down)
        	lParamPost |= 1; // Bits 0-15: Repeat count
	    PostMessage(hWnd, msg, wParamPost, lParamPost);
	    
        return 1;
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

void setKeyboardInputEnabled(BOOL enabled) {
    if (enabled && keyboardHook != NULL) {
        UnhookWindowsHookEx(keyboardHook);
        keyboardHook = NULL;
    } else if (!enabled && keyboardHook == NULL) {
        keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(NULL), 0);
    }
}

typedef enum  {
    MOD_NONE   = 0,      // 0b0000
    MOD_LALT   = 1 << 0, // 0b0001 (1)
    MOD_LWIN  = 1 << 1, // 0b0010 (2)
    MOD_LCTR = 1 << 2, // 0b0100 (4)
    MOD_LSHF = 1 << 3  // 0b1000 (8)
}Mod ;

void SetupModifiers(Mod* _mod, SDL_Keycode _key, bool _is_down)
{
	Mod cm = MOD_NONE;
	switch(_key)
	{
		case SDLK_LALT:
			cm = MOD_LALT;
			break;
		case SDLK_LGUI:
			cm = MOD_LWIN;
			break;
		case SDLK_LCTRL:
			cm = MOD_LCTR;
			break;
		case SDLK_LSHIFT:
			cm = MOD_LSHF;
			break;
	
	}

	*_mod = (_is_down) ? (*_mod | cm) : (*_mod & ~cm);
}

bool SDLCALL MyMessageHook(void *userdata, MSG *msg) {
	printf("cc\n");
    //if (msg->message == WM_KEYDOWN) {
    //    if (msg->wParam == VK_LWIN || msg->wParam == VK_RWIN) {
    //        printf("Blocked Windows key press!\n");
    //        return false; // Block the message, no OS response
    //    }
    //}
    return false; // Let other messages pass through
}
int main(int argc, char** argv)
{
	IDE_setup ide_setup;
	IDE_Init(&ide_setup);
        //hook2 = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc2, GetModuleHandle(NULL), 0);
        //hook1 = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc1, GetModuleHandle(NULL), 0);
	//hWnd = (HWND)SDL_GetPointerProperty(SDL_GetWindowProperties(ide_setup.sdl_window_p), SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
        //keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(NULL), 0);
	SDL_SetWindowsMessageHook(MyMessageHook, NULL);

	//Mod modifier = MOD_NONE;
	//setKeyboardInputEnabled(ide_setup.ide_is_full_screen);
	//char* fontpath = "./fonts/segoeprb.ttf";
	//int* alphabet_outline;
	//init_all_characters(&alphabet_outline, &glyph_box, fontpath);

	//int starting_index = alphabet_outline[0];
	//int ssbo_alphabet_size = alphabet_outline[starting_index - 1];

	//ivec2 grid_aspect_ratio = {5, 3};
	//ivec2 grid_size = SCA2i(15, grid_aspect_ratio);
	//int* ssbo_grid_indecies = (int*)malloc(grid_size.x*grid_size.y * sizeof(int));
	//int* lines_size = (int*)malloc(1024 * sizeof(int));
	//ivec2 file_offset = {0, 0};

	//for (int line_index = 0, file_buffer_index = 0; line_index < 1024 && file_buffer_index < file_size; line_index++)
	//{
	//	int ls = 0;
	//	while (editable_file[file_buffer_index + ls] != '\n')
	//		ls++;
	//	ls++;
	//	lines_size[line_index] = ls;
	//	file_buffer_index += ls;
	//	file_lines_max++;
	//}
	//printf("m : %d\n", file_lines_max);


	//for (int i = 0; i < grid_size.x * grid_size.y; i++)
	//{
	//	ssbo_grid_indecies[i] = -1;
	//}

	//
	//ivec2 cellule_size = {winWidth / grid_size.x, winHeight / grid_size.y};



	//uint main_buf_verts;
	//glGenBuffers(1, &main_buf_verts);
	//glBindBuffer(GL_ARRAY_BUFFER, main_buf_verts);
	//glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float), main_vertices, GL_STATIC_DRAW);

	//uint indices_buf;
	//glGenBuffers(1, &indices_buf);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_buf);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(float), indices, GL_STATIC_DRAW);

	//glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4,0);
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(2 * sizeof(float)));
	//glEnableVertexAttribArray(1);

	//GLint ssbo_alphabet_buf;
	//glGenBuffers(1, &ssbo_alphabet_buf);
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_alphabet_buf);



	//glBufferData(GL_SHADER_STORAGE_BUFFER, ssbo_alphabet_size * sizeof(int), alphabet_outline + starting_index, GL_DYNAMIC_COPY);
	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo_alphabet_buf);


	//GLint ssbo_grid_indecies_buf;
	//glGenBuffers(1, &ssbo_grid_indecies_buf);
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_grid_indecies_buf);

	//update_grid_indecies(editable_file, alphabet_outline, ssbo_grid_indecies, grid_size, starting_index, (ivec2){0, 0}, lines_size, file_lines_max);
	////printf("Error code: %d\n", errno);
	////printf("ex lp : \n");
	//glBufferData(GL_SHADER_STORAGE_BUFFER, grid_size.x * grid_size.y * sizeof(int), ssbo_grid_indecies, GL_DYNAMIC_COPY);
	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo_grid_indecies_buf);

	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	//GLuint main_shader_program = get_program();
	//glUseProgram(main_shader_program);

	////--> get uniforms locations
	//GLint grid_location = glGetUniformLocation(main_shader_program, "u_grid");
	//GLint cursor_location = glGetUniformLocation(main_shader_program, "u_cursor");
	//GLint upm_location = glGetUniformLocation(main_shader_program, "u_upm");
	//GLint cel_size_location = glGetUniformLocation(main_shader_program, "u_cel_size");
	//GLint edge_location = glGetUniformLocation(main_shader_program, "u_edge");
	//GLint timer_location = glGetUniformLocation(main_shader_program, "u_timer");
	////GLint index_location = glGetUniformLocation(main_shader_program, "u_index");
	////--> end

	//

	//glUniform2i(grid_location, grid_size.x, grid_size.y);
	//glUniform2i(cel_size_location, cellule_size.x, cellule_size.y);
	//glUniform2i(cursor_location, screen_cur_pos.x, screen_cur_pos.y);
	//glUniform2f(upm_location, glyph_box.xMax - glyph_box.xMin, glyph_box.yMax - glyph_box.yMin);
	//glUniform1f(edge_location, edge);
	//glUniform1f(timer_location, 0.0f);

	while (1)
	{
		//timer = SDL_GetTicks();
		//
		//MSG msgtest;
		//PeekMessageA(&msgtest, hWnd, 0, 0, PM_NOREMOVE);
		//if (msgtest.wParam == VK_LWIN)
            	//	printf("%d pm\n", GetKeyState(VK_LWIN));

		SDL_Event event;
		SDL_PollEvent(&event);

		//if (event.type == SDL_EVENT_QUIT || (SDL_EVENT_KEY_DOWN && event.key.key == SDLK_C && SDL_GetKeyboardState(NULL)[SDL_SCANCODE_LCTRL]))
		if (event.type == SDL_EVENT_QUIT)
			break;

		if (event.window.type == SDL_EVENT_WINDOW_FOCUS_LOST)
			ide_setup.ide_is_focus = false;
		if (event.window.type == SDL_EVENT_WINDOW_FOCUS_GAINED)
			ide_setup.ide_is_focus = true;

		if(!ide_setup.ide_is_focus)
			continue;

		if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP)
		{
			printf("%s\n", (event.type == SDL_EVENT_KEY_DOWN) ? "ld" : (event.type == SDL_EVENT_KEY_UP) ? "lu" : "00");
			//SetupModifiers(&modifier, event.key.key, event.type != SDL_EVENT_KEY_UP);
			//printf("%d\n", modifier);
		}



		//if (event.type == SDL_EVENT_KEY_DOWN)
		//{
		//	printf("%d\n", event.key.mod);
		//	switch (event.key.mod)
		//	{
		//		//case SDLK_A ... SDLK_Z : 
		//		//	printf("%c\n", 'a' + (event.key.key - SDLK_A));
		//		//	break;
		//		case SDL_KMOD_LCTRL : 
		//			printf("left\n");
		//			break;
		//		case SDL_KMOD_LSHIFT : 
		//			printf("right\n");
		//			break;
		//	}
		//}
	

		//if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE && ((event.key.mod & SDL_KMOD_LGUI) == SDL_KMOD_LGUI) && ide_setup.ide_is_full_screen)
			//printf("yes\n" );
		//if (event.type == SDL_EVENT_KEY_DOWN)// && event.key.key == SDLK_A)
		//{
		//	//printf("%d\n", event.key.scancode);
		//	bool bKey = (SDL_GetKeyboardState(NULL)[event.key.scancode]);
		//		printf("yyay%d\n", bKey);
		//	//printf("%d , %d\n", event.key.key, event.key.mod);
		//}
			//&& (ide_setup.ide_is_full_screen == false
				//|| (event.key.mod == SDL_KMOD_LGUI && ide_setup.ide_is_full_screen)))
			//printf("%u\n", (Uint16)event.key.mod);
		//if ((event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE)
		//	&& (ide_setup.ide_is_full_screen == false
		//		|| (((event.key.mod & SDL_KMOD_LGUI) == SDL_KMOD_LGUI) && ide_setup.ide_is_full_screen)))
		//{
		//	printf("here we go\n");
		//	ide_setup.ide_is_full_screen = !ide_setup.ide_is_full_screen;
		//	SDL_SetWindowFullscreen(ide_setup.sdl_window_p, ide_setup.ide_is_full_screen);
		//	setKeyboardInputEnabled(ide_setup.ide_is_full_screen);
		//}
			//printf("%d", event.key.key);
		//if (focus_lost)
		//	continue;

		//if (event.type == SDL_EVENT_KEY_DOWN)
		//{
		//	if (event.key.key >= SDLK_A && event.key.key >= SDLK_Z)
		//	{
		//		char key = event.key.key - SDLK_A + 'a';

		//		insert_char(editable_file, key, file_offset, lines_size);
		//		update_grid_indecies(editable_file, alphabet_outline, ssbo_grid_indecies, grid_size, starting_index, file_offset, lines_size, file_lines_max);
		//		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_grid_indecies_buf);
		//		glBufferData(GL_SHADER_STORAGE_BUFFER, grid_size.x * grid_size.y * sizeof(int), ssbo_grid_indecies, GL_DYNAMIC_COPY);
		//		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo_grid_indecies_buf);
		//		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		//	}

		//	switch (event.key.key)
		//	{
		//		case SDLK_UP:
		//			screen_cur_pos.y--;
		//			file_cur_pos.y--;
		//			if (screen_cur_pos.y == -1)
		//			{
		//				screen_cur_pos.y++;
		//				if (file_cur_pos.y > -1)
		//				{
		//					file_offset.y = file_cur_pos.y;
		//					update_grid_indecies(editable_file, alphabet_outline, ssbo_grid_indecies, grid_size, starting_index, file_offset, lines_size, file_lines_max);
		//					glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_grid_indecies_buf);
		//					glBufferData(GL_SHADER_STORAGE_BUFFER, grid_size.x * grid_size.y * sizeof(int), ssbo_grid_indecies, GL_DYNAMIC_COPY);
		//					glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo_grid_indecies_buf);
		//					glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		//				}
		//				else
		//					file_cur_pos.y = 0;
		//			}
		//			glUniform2i(cursor_location, screen_cur_pos.x, screen_cur_pos.y);
		//			break;
		//		case SDLK_DOWN:
		//			screen_cur_pos.y++;
		//			file_cur_pos.y++;
		//			if (screen_cur_pos.y == grid_size.y)
		//			{
		//				screen_cur_pos.y--;
		//				if (file_cur_pos.y < file_lines_max)
		//				{
		//					file_offset.y = file_cur_pos.y - grid_size.y + 1;
		//					update_grid_indecies(editable_file, alphabet_outline, ssbo_grid_indecies, grid_size, starting_index, file_offset, lines_size, file_lines_max);
		//					glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_grid_indecies_buf);
		//					glBufferData(GL_SHADER_STORAGE_BUFFER, grid_size.x * grid_size.y * sizeof(int), ssbo_grid_indecies, GL_DYNAMIC_COPY);
		//					glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo_grid_indecies_buf);
		//					glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		//				}
		//				else
		//					file_cur_pos.y = file_lines_max - 1;
		//			}
		//			else if (screen_cur_pos.y + file_offset.y == file_lines_max)
		//			{
		//				file_cur_pos.y = file_lines_max - 1;
		//				screen_cur_pos.y--;
		//			}
		//			glUniform2i(cursor_location, screen_cur_pos.x, screen_cur_pos.y);
		//			break;
		//		case SDLK_RIGHT:
		//			screen_cur_pos.x++;
		//			file_cur_pos.x++;
		//			if (screen_cur_pos.x == grid_size.x || screen_cur_pos.x == lines_size[file_cur_pos.y])
		//			{
		//				screen_cur_pos.x --;
		//				if (file_cur_pos.x < lines_size[file_cur_pos.y])
		//				{
		//					file_offset.x = file_cur_pos.x - grid_size.x + 1;
		//					update_grid_indecies(editable_file, alphabet_outline, ssbo_grid_indecies, grid_size, starting_index, file_offset, lines_size, file_lines_max);
		//					glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_grid_indecies_buf);
		//					glBufferData(GL_SHADER_STORAGE_BUFFER, grid_size.x * grid_size.y * sizeof(int), ssbo_grid_indecies, GL_DYNAMIC_COPY);
		//					glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo_grid_indecies_buf);
		//					glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		//				}
		//				else
		//					file_cur_pos.x = lines_size[file_cur_pos.y] - 1;
		//			}
		//			glUniform2i(cursor_location, screen_cur_pos.x, screen_cur_pos.y);
		//			break;
		//		case SDLK_LEFT:
		//			screen_cur_pos.x--;
		//			file_cur_pos.x--;
		//			if (screen_cur_pos.x == -1)
		//			{
		//				screen_cur_pos.x++;
		//				if (file_cur_pos.x > -1)
		//				{
		//					file_offset.x = file_cur_pos.x;
		//					update_grid_indecies(editable_file, alphabet_outline, ssbo_grid_indecies, grid_size, starting_index, file_offset, lines_size, file_lines_max);
		//					glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_grid_indecies_buf);
		//					glBufferData(GL_SHADER_STORAGE_BUFFER, grid_size.x * grid_size.y * sizeof(int), ssbo_grid_indecies, GL_DYNAMIC_COPY);
		//					glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo_grid_indecies_buf);
		//					glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		//				}
		//				else
		//					file_cur_pos.x = 0;
		//			}
		//			glUniform2i(cursor_location, screen_cur_pos.x, screen_cur_pos.y);
		//			break;
		//		case SDLK_KP_PLUS:
		//			if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_LCTRL])
		//			{
		//				grid_size = ADD2i(grid_size, grid_aspect_ratio);
		//				cellule_size = (ivec2){winWidth / grid_size.x, winHeight / grid_size.y};
		//				glUniform2i(grid_location, grid_size.x, grid_size.y);
		//				glUniform2i(cel_size_location, cellule_size.x, cellule_size.y);
		//				//update_grid_indecies(editable_file, alphabet_outline, ssbo_grid_indecies, grid_size, starting_index);
		//			}
		//			else
		//			{
		//				edge = fmaxf(edge - 0.001f, -.5f);
		//				glUniform1f(edge_location, edge);
		//			}
		//			break;
		//		case SDLK_KP_MINUS:
		//			if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_LCTRL])
		//			{
		//				grid_size = max_iv2(ADD2i(grid_size, SCA2i(-1, grid_aspect_ratio)), grid_aspect_ratio);
		//				cellule_size = (ivec2){winWidth / grid_size.x, winHeight / grid_size.y};
		//				glUniform2i(grid_location, grid_size.x, grid_size.y);
		//				glUniform2i(cel_size_location, cellule_size.x, cellule_size.y);
		//				//update_grid_indecies(editable_file, alphabet_outline, ssbo_grid_indecies, grid_size, starting_index);
		//			}
		//			else
		//			{
		//				edge = fminf(edge + 0.001f, .5f);
		//				glUniform1f(edge_location, edge);
		//			}
		//			break;
		//	}
		//}

		//Uint64 frame_duration = SDL_GetTicks() - timer;
		//uint fps = 16;
		//if (frame_duration < fps)
		//{
		//	SDL_Delay(fps-frame_duration);
		//}


		//glUniform1f(timer_location, (float)((double)timer / 1000.0));

		IDE_Render(ide_setup);
		//glClear(GL_COLOR_BUFFER_BIT);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
		//SDL_GL_SwapWindow(window);

	}

	//setKeyboardInputEnabled(ide_setup.ide_is_full_screen);
        UnhookWindowsHookEx(keyboardHook);
	keyboardHook = NULL;
        //UnhookWindowsHookEx(hook1);
        //UnhookWindowsHookEx(hook2);
	IDE_Clean_Up(ide_setup);

	//free(alphabet_outline);
	//free(ssbo_grid_indecies);
	//free(lines_size);
	//fclose(file);
	//free(editable_file);
	//SDL_GL_DestroyContext(glcontext);  

	return 0;
	
}
