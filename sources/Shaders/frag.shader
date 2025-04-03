#version 430 core

layout(std430, binding=0) buffer CellBuffer {
	uint cellData[]; 
};

layout(std430, binding=1) buffer bufferIndecies {
	int indecies[];
};


out vec4 FragColor;
in vec2 uv;

struct bez_pts {
	vec2 a, c, b;
};

uniform ivec2 u_grid;
uniform ivec2 u_cursor;
uniform ivec2 u_cel_size;
uniform vec2 u_upm;
uniform int u_cpc;
uniform int u_index;
uniform float u_edge;
uniform float u_timer;


float dot2(vec2 _p)
{
	return dot(_p, _p);
}

float sdCircle(vec2 _p, float _r) {
	 return length(_p) - _r;
}

float sdBezier( in vec2 pos, in vec2 A, in vec2 B, in vec2 C )
{    
	vec2 a = B - A;
	vec2 b = A - 2.0*B + C;
	vec2 c = a * 2.0;
	vec2 d = A - pos;
	float kk = 1.0/dot(b,b);
	float kx = kk * dot(a,b);
	float ky = kk * (2.0*dot(a,a)+dot(d,b)) / 3.0;
	float kz = kk * dot(d,a);      
	float res = 0.0;
	float p = ky - kx*kx;
	float p3 = p*p*p;
	float q = kx*(2.0*kx*kx-3.0*ky) + kz;
	float h = q*q + 4.0*p3;
	if( h >= 0.0) 
	{ 
		h = sqrt(h);
		vec2 x = (vec2(h,-h)-q)/2.0;
		vec2 uv = sign(x)*pow(abs(x), vec2(1.0/3.0));
		float t = clamp( uv.x+uv.y-kx, 0.0, 1.0 );
		res = dot2(d + (c + b*t)*t);
	}
	else
	{
		float z = sqrt(-p);
		float v = acos( q/(p*z*2.0) ) / 3.0;
		float m = cos(v);
		float n = sin(v)*1.732050808;
		vec3  t = clamp(vec3(m+m,-n-m,n-m)*z-kx,0.0,1.0);
		res = min( dot2(d+(c+b*t.x)*t.x),
	               dot2(d+(c+b*t.y)*t.y) );
	}
	return sqrt( res );
}

float sdBox(vec2 _p, vec2 _r) {
	 _p = abs(_p) - _r;
    vec2 n_p = max(_p, vec2(0.));
    float d = length(n_p) + min(0, max(_p.x, _p.y));
	 return d;
}

float sdSeg(vec2 _p, vec2 _a, vec2 _b) {
   	vec2 lv = _b-_a;
   	vec2 pv = _p-_a;
   	float dt = clamp(dot(lv, pv) / dot(lv, lv), 0., 1.);
   	return length(_p-(_a + dt*(_b-_a)));
}

vec3 draw_grid()
{
	float asp_rat= float(float(u_cel_size.y) / float(u_cel_size.x));
	vec2 scaled_uv = fract(uv * u_grid) * vec2(1., asp_rat);

	float d = sdBox(scaled_uv - vec2(.5, .5 * asp_rat), vec2(.5, .5 * asp_rat));
	float a = smoothstep(.001, .0, abs(d) -.001);

	return mix(vec3(0., .1, 0.), vec3(0., .3, 0.), a);
}

vec4 draw_cursor()
{
	float asp_rat= float(float(u_cel_size.y) / float(u_cel_size.x));
	vec2 scaled_uv = uv * u_grid * vec2(1., asp_rat);

	vec2 p = scaled_uv - vec2(u_cursor) *  vec2(1., asp_rat) - vec2(.5, .5*asp_rat);

	float box_size = .48;
	float d = sdBox(p, box_size*vec2(1, asp_rat));
	float a = smoothstep(0.01, 0., d);
	float flash = (fract(u_timer) < .85) ? 1. : 0.3;
	return vec4(vec3(a * .3), a * flash);
}

vec3 solve_quadratic_equation(float _a, float _b, float _c)
{
	float delta = _b * _b - 4 * _a * _c;

	float sqrtd = sqrt(max(delta, 0));

	return (delta >= 0) ? vec3(delta, (-_b-sqrtd)/ (2*_a), (-_b+sqrtd)/ (2*_a)) : vec3 (delta, 0, 0);
}

bool pt_itr_seg(vec2 _p, vec2 _a, vec2 _b)
{
	bool inter1 = _p.y < _b.y && _p.y > _a.y;
	bool dt1 = determinant(mat2(_p-_a, _b-_a)) < 0.;

	bool inter2 = _p.y > _b.y && _p.y < _a.y;
	bool dt2 = determinant(mat2(_p-_a, _b-_a)) > 0.;

	return (inter1 && dt1) || (inter2 && dt2);
}


bool pt_itr_bez(vec2 _p, vec2 _a, vec2 _c, vec2 _b)
{

	float A = _a.y -2 * _c.y + _b.y;
	float B =  -2 * _a.y +2* _c.y;
	float C = _a.y - _p.y;
	vec3 s = solve_quadratic_equation(A, B, C);


	float X1 = (_a.x -2 * _c.x + _b.x) *s.y*s.y + ( -2 * _a.x + 2 * _c.x) * s.y + (_a.x - _p.x);
	float X2 = (_a.x -2 * _c.x + _b.x) *s.z*s.z + ( -2 * _a.x + 2 * _c.x) * s.z + (_a.x - _p.x);

	return (s.x >= 0) && ((X1 >= 0 && s.y >= 0 && s.y <= 1) || (X2 >= 0 && s.z >= 0 && s.z <= 1));
}

float construct_sdf(int _index) {

	if (_index == -1)	return 10;

	vec2 asp_rat = vec2(1., (float(u_cel_size.y) / float(u_cel_size.x)));
	vec2 scaled_uv = fract(uv * u_grid) * asp_rat;

	float d = 1;
	float px0 = float(bitfieldExtract(cellData[_index + 1], 0, 15));
	float py0 = float(bitfieldExtract(cellData[_index + 1], 15, 15));
	vec2 p0 = vec2(px0 / u_upm.x, 1.- py0 / u_upm.y) ;
	int con1 = int(bitfieldExtract(cellData[_index], 0, 6)) + 1;
	int con_size = int(step(1, int(bitfieldExtract(cellData[_index], 0, 6))) + step(1, int(bitfieldExtract(cellData[_index], 6, 6))) 
			+ step(1, int(bitfieldExtract(cellData[_index], 12, 6))) + step(1, int(bitfieldExtract(cellData[_index], 18, 6)))
			+ step(1, int(bitfieldExtract(cellData[_index], 24, 6))));
	int start_index = 0;
	int end_index = int(bitfieldExtract(cellData[_index], 0, 6)) + 1;
	float prev_x = float(bitfieldExtract(cellData[_index + 1], 0, 15));
	float prev_y = float(bitfieldExtract(cellData[_index + 1], 15, 15));
	vec2 prev_p = vec2(prev_x / u_upm.x, 1.- prev_y / u_upm.y) ;
	float sign = 1.0;
	for (int coni = 0; coni < con_size; coni++)
	{
		for (int pi = 1; pi <= end_index; pi++)
		{
			float px = float(bitfieldExtract(cellData[_index + 1 +start_index + (pi % end_index)], 0, 15));
			float py = float(bitfieldExtract(cellData[_index + 1 +start_index + (pi % end_index)], 15, 15));
			uint r_flag = bitfieldExtract(cellData[_index + 1 +start_index + (pi % end_index)], 30, 2);
			vec2 p = vec2(px / u_upm.x, 1.- py / u_upm.y) ;

                	if (r_flag == 0)
			{
				vec2 pc = p;
				pi++;
				float p1x = float(bitfieldExtract(cellData[_index + 1 +start_index + (pi % end_index)], 0, 15));
				float p1y = float(bitfieldExtract(cellData[_index + 1 +start_index + (pi % end_index)], 15, 15));
				uint r_flag1 = bitfieldExtract(cellData[_index + 1 + start_index + (pi % end_index)], 30, 2);
				p = vec2(p1x / u_upm.x, 1.- p1y / u_upm.y);
				if (r_flag1 == 0)
				{
					vec2 tmp = p;
					p = (tmp+pc)*.5;
					pi--;
				}
				d = min(d, sdBezier(scaled_uv, prev_p, pc, p));
				sign *= pt_itr_bez(scaled_uv, prev_p, pc, p) ? -1 : 1;
			}
			else
			{
				d = min(d, sdSeg(scaled_uv, p, prev_p));
				sign *= pt_itr_seg(scaled_uv, p, prev_p) ? -1 : 1;
			}
			prev_p = p;
		}
		start_index += end_index;
		prev_x = float(bitfieldExtract(cellData[_index + 1 + start_index], 0, 15));
		prev_y = float(bitfieldExtract(cellData[_index + 1 + start_index], 15, 15));
		prev_p = vec2(prev_x / u_upm.x, 1.- prev_y / u_upm.y) ;
		end_index = int(bitfieldExtract(cellData[_index], (coni + 1) * 6, 6)) - start_index  + 1;
	}

	return d * sign;
}

void main() {
	ivec2 nuv = ivec2(uv * u_grid);
	int ind =  indecies[nuv.x + nuv.y * u_grid.x];
	//ind =  indecies[4];
	float char_d = construct_sdf(ind);

	//vec2 asp_rat = vec2(1., (float(u_cel_size.y) / float(u_cel_size.x)));
	//vec2 scaled_uv = fract(uv * u_grid) * asp_rat;
	//char_d = sdBox(scaled_uv - vec2(.1), vec2(.1));
	//float sd_bez_a = smoothstep(u_edge, -u_edge, char_d);
	float sd_bez_a = smoothstep(0.05, -0.03, char_d);

	vec3 grid_col = draw_grid();
	vec4 cur_col = draw_cursor();
    	vec3 text_col = vec3(sd_bez_a, 0., 0.);
	vec3 final_col = mix(cur_col.xyz, grid_col.xyz, 1-cur_col.a);
	final_col = mix(final_col, text_col.xyz, sd_bez_a);
    	FragColor = vec4(final_col, 1.);
    	//FragColor = vec4(text_col, 1.);
}
