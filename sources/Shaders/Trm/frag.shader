#version 460 core

uniform ivec2 u_grid;
uniform ivec2 u_cel_size;
uniform vec2 u_upm;
uniform vec2 u_cur;
uniform float u_aspect;

in vec2 text_uv;

out vec4 FragColor;

layout(std430, binding=0) buffer CellBuffer {
	uint alphabet_data[]; 
};

layout(std430, binding=2) buffer bufferIndecies {
	int indecies[];
};
//
#include "../Utils/sdf.glsl"
//
#include "../Utils/utils.glsl"
//
////construct_char depend on alphabet_data , u_upm!!
#include "../Utils/font.glsl"


void main()
{
	//float u_line = 0.0;
	float cur_d = sdBox(text_uv * u_grid - (vec2(.5, .5) + u_cur), vec2(.5, .5));
	float cur_a = smoothstep(.1, -.1, cur_d);
	float cur_fnl_a = smoothstep(.1, -.1, abs(cur_d) - .01);

	float test_d = sdBox(text_uv*u_grid - vec2(.5, .5), vec2(.4, .2));
	float test_a = smoothstep(.01, .0, test_d);

	//cur_d = sdBox(text_uv * u_grid - vec2(8, u_line + .5), vec2(u_grid.x*.5, .1));
	//cur_fnl_a = smoothstep(.1, -.1, cur_d);

	ivec2 nuv = ivec2(text_uv * u_grid);
	int ind =  indecies[nuv.x + nuv.y * u_grid.x];

	//vec2 rand = mix(vec2(0.0), vec2(1.), random_vec(text_uv));

	float grid_d = sdBox(fract(text_uv*u_grid) - vec2(.5), vec2(.5, .5));
	float grid_a = smoothstep(.03, .0, abs(grid_d) - .01);
	//grid_a = smoothstep(.03, .0, grid_d);

	float char_d = construct_char(ind, text_uv * u_grid);

	float r = smoothstep(.05, -.05, char_d);
   	FragColor = vec4(0., r, cur_fnl_a, 1.);
}
