#version 460 core

layout (location = 0) in vec4 pos;
layout (location = 1) in vec2 in_text_uv;


out vec2 uv;

void main()
{
	gl_Position = pos;
	uv = in_text_uv;
}
