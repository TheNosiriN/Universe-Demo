

namespace NoiseFuncs {
	constexpr static float K = 0.142857142857f;
	constexpr static float Ko = 0.428571428571f;
	constexpr static float K2 = 0.020408163265306f;
	constexpr static float Kz = 0.166666666667f;
	constexpr static float Kzo = 0.416666666667f;
	constexpr static float jitter = 0.8f;


	vec4 permute(vec4 x) {
	  return mod((34.0f * x + 1.0f) * x, 289.0f);
	}
	vec3 permute(vec3 x) {
	  return mod((34.0f * x + 1.0f) * x, 289.0f);
	}


	vec2 cellular(vec3 P) {
		vec3 Pi = mod(floor(P), 289.0f);
	 	vec3 Pf = fract(P);
		vec4 Pfx = Pf.x + vec4(0.0, -1.0, 0.0, -1.0);
		vec4 Pfy = Pf.y + vec4(0.0, 0.0, -1.0, -1.0);
		vec4 p = permute(Pi.x + vec4(0.0, 1.0, 0.0, 1.0));
		p = permute(p + Pi.y + vec4(0.0, 0.0, 1.0, 1.0));
		vec4 p1 = permute(p + Pi.z);
		vec4 p2 = permute(p + Pi.z + vec4(1.0));
		vec4 ox1 = fract(p1*K) - Ko;
		vec4 oy1 = mod(floor(p1*K), 7.0f)*K - Ko;
		vec4 oz1 = floor(p1*K2)*Kz - Kzo;
		vec4 ox2 = fract(p2*K) - Ko;
		vec4 oy2 = mod(floor(p2*K), 7.0f)*K - Ko;
		vec4 oz2 = floor(p2*K2)*Kz - Kzo;
		vec4 dx1 = Pfx + jitter*ox1;
		vec4 dy1 = Pfy + jitter*oy1;
		vec4 dz1 = Pf.z + jitter*oz1;
		vec4 dx2 = Pfx + jitter*ox2;
		vec4 dy2 = Pfy + jitter*oy2;
		vec4 dz2 = Pf.z - 1.0f + jitter*oz2;
		vec4 d1 = dx1 * dx1 + dy1 * dy1 + dz1 * dz1;
		vec4 d2 = dx2 * dx2 + dy2 * dy2 + dz2 * dz2;

		/* Sort out the two smallest distances (F1, F2) */
		// Cheat and sort out only F1
		d1 = min(d1, d2);
		vec2 td1 = min(vec2(d1.x, d1.y), vec2(d1.w, d1.z));
		d1.x = td1.x;
		d1.y = td1.y;
		d1.x = min(d1.x, d1.y);
		return sqrt(vec2(d1.x, d1.x));
	}



	constexpr static mat3 M3 = mat3(0.00,0.80,0.60,-0.80,0.36,-0.48,-0.60,-0.48,0.64);
	float planet_fbm_far(vec3 p, uint octaves){
		p *= 2.0f;
		float v = 0.0f;
	    float a = 0.5f;

	    for (uint i=0u; i<octaves; ++i){
	        //float n = perlin(p);
	        float n = cellular(p).x*2.0f-1.0f;
	        //n = (n);
	        v += a * n;
	        v = 1.0f-abs(v);
	        //v = v * v * v;
	        // v = easeInCirc(v);
	        p = M3 * p * 2.0f + n*0.25f+0.25f;
	        a *= 0.5f;
	    }

	    return clamp(easeOutExpo(v), 0.0f,1.0f);
	}


};
