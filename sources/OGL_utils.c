#include <OGL_utils.h>
#include <FLS_utils.h>


GLuint OGL_Setup_Program(char* _vertex_file_path, char* _frag_file_path)
{
	const char* vertex_source = (const char*)FLS_Read_File(_vertex_file_path);
	const char* frag_source = (const char*)FLS_Read_File(_frag_file_path);

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
	free((void*)frag_source);
	return shader_program;
}

