//float construct_char(int _index, ivec2 _grid, ivec2 _cel_size, vec2 u_upm)//, inout uint alphabet_data[])
float construct_char(int _index, vec2 _uv)//, inout uint alphabet_data[])
{
	if (_index == -1)	return 10.;
	//else return -1.;

	//vec2 uv = text_uv;

	//vec2 asp_rat = vec2((float(_cel_size.y) / float(_cel_size.x)), 1.);
	//vec2 asp_rat = vec2((float(_grid.x) / float(_grid.y)), 1.);
	//vec2 scaled_uv = (fract(uv * _grid) - vec2(.5)) * asp_rat;
	vec2 scaled_uv = fract(_uv);
	scaled_uv.y = 1 - scaled_uv.y;
	//vec2 scaled_uv = (text_uv-vec2(.5));


	float d = 1;
	float px0 = float(bitfieldExtract(alphabet_data[_index + 1], 0, 15));
	float py0 = float(bitfieldExtract(alphabet_data[_index + 1], 15, 15));
	vec2 p0 = vec2(px0 / u_upm.x, py0 / u_upm.y) ;
	int con1 = int(bitfieldExtract(alphabet_data[_index], 0, 6)) + 1;
	int con_size = int(step(1, int(bitfieldExtract(alphabet_data[_index], 0, 6))) + step(1, int(bitfieldExtract(alphabet_data[_index], 6, 6))) 
			+ step(1, int(bitfieldExtract(alphabet_data[_index], 12, 6))) + step(1, int(bitfieldExtract(alphabet_data[_index], 18, 6)))
			+ step(1, int(bitfieldExtract(alphabet_data[_index], 24, 6))));
	int start_index = 0;
	int end_index = int(bitfieldExtract(alphabet_data[_index], 0, 6)) + 1;
	float prev_x = float(bitfieldExtract(alphabet_data[_index + 1], 0, 15));
	float prev_y = float(bitfieldExtract(alphabet_data[_index + 1], 15, 15));
	vec2 prev_p = vec2(prev_x / u_upm.x, prev_y / u_upm.y) ;
	float sign = 1.0;

	for (int coni = 0; coni < con_size; coni++)
	{
		for (int pi = 1; pi <= end_index; pi++)
		{
			float px = float(bitfieldExtract(alphabet_data[_index + 1 +start_index + (pi % end_index)], 0, 15));
			float py = float(bitfieldExtract(alphabet_data[_index + 1 +start_index + (pi % end_index)], 15, 15));
			uint r_flag = bitfieldExtract(alphabet_data[_index + 1 +start_index + (pi % end_index)], 30, 2);
			vec2 p = vec2(px / u_upm.x, py / u_upm.y) ;


                	if (r_flag == 0)
			{
				vec2 pc = p;
				pi++;
				float p1x = float(bitfieldExtract(alphabet_data[_index + 1 +start_index + (pi % end_index)], 0, 15));
				float p1y = float(bitfieldExtract(alphabet_data[_index + 1 +start_index + (pi % end_index)], 15, 15));
				uint r_flag1 = bitfieldExtract(alphabet_data[_index + 1 + start_index + (pi % end_index)], 30, 2);
				p = vec2(p1x / u_upm.x, p1y / u_upm.y);
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
		prev_x = float(bitfieldExtract(alphabet_data[_index + 1 + start_index], 0, 15));
		prev_y = float(bitfieldExtract(alphabet_data[_index + 1 + start_index], 15, 15));
		prev_p = vec2(prev_x / u_upm.x, prev_y / u_upm.y) ;
		end_index = int(bitfieldExtract(alphabet_data[_index], (coni + 1) * 6, 6)) - start_index  + 1;
	}

	return d * sign;
}

