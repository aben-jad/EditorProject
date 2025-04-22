#version 460 core

#include "../Utils/sdf.glsl"

uniform float u_aspect;
uniform vec2 u_exp_pos;
uniform vec2 u_exp_dim;

uniform vec2 u_exp_start;
uniform vec2 u_exp_end;

in vec2 uv;

out vec4 FragColor;

vec3 draw_ide()
{
	float ide_d = sdBox(uv-vec2(u_aspect, 1.) * .5, vec2(u_aspect, 1.) *.5);
	float ide_a = smoothstep(.001, .0, abs(ide_d) - .001);

	//u_exp_start = vec2(.0, .0);
        //u_exp_end   = vec2(1., 1.);
	float exp_d = sdSeg(uv, u_exp_start*vec2(u_aspect, 1.), u_exp_end*vec2(u_aspect, 1.));
	float exp_a = smoothstep(.01, .0, exp_d);

	float final_a = max(ide_a, exp_a);
	vec3 ide_col = vec3(0., final_a*.3, 0.);

	return ide_col;
}

void main()
{
	vec3 final_col = draw_ide();

   	FragColor = vec4(final_col, 1.);
}
