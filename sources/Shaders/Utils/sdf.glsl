float sdSeg(vec2 _p, vec2 _a, vec2 _b)
{
	vec2 lv = _b-_a;
	vec2 pv = _p-_a;

	float dt = clamp(dot(lv, pv) / dot(lv, lv), 0., 1.);

	return length(_p-(_a + dt*(_b-_a)));
}

float dot2(vec2 _p)
{
	return dot(_p, _p);
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

float sd_circle(vec2 _p, float _r)
{
	return length(_p) - _r;
}
