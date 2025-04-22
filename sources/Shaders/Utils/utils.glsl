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


vec2 random_vec(vec2 _uv)
{
	vec2 rs = cos(_uv*vec2(1047.24, 54786.26) + vec2(54.41, 124.5)) * vec2(2154.23, 1256.4);

	return fract(rs);
}
