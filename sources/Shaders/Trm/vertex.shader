#version 460 core

layout (location = 0) in vec4 pos;
layout (location = 1) in vec2 in_text_uv;

uniform float u_aspect;

out vec2 text_uv;

void main()
{
	gl_Position = pos;
	text_uv = in_text_uv;// * vec2(1., u_aspect);
}
