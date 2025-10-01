#version 460 core

out vec4 FragColor;
in vec2 uv;

#include "./sdf.glsl"

#include "./utils.glsl"

//construct_char depend on alphabet_data , u_upm!!
//#include "./font.glsl"

float char_a()
{
	vec2 p0 = vec2(.3, .7);
	return sd_circle(uv - p0, .001);
}

void main()
{
	float d = char_a();
	float a = smoothstep(0.001, 0., d);

	FragColor = vec4(.0, a, 0.0, 1.0);
}

