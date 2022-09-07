

const mat3 M3 = mat3(0.00,0.80,0.60,-0.80,0.36,-0.48,-0.60,-0.48,0.64);

float planet_fbm_far(vec3 p, uint octaves){
	p *= 2.0;
	float v = 0.0;
    float a = 0.5;

    for (uint i=0u; i<octaves; ++i){
        //float n = perlin(p);
        float n = cellular(p).x*2.-1.;
        //n = (n);
        v += a * n;
        v = 1.0-abs(v);
        //v = v * v * v;
        // v = easeInCirc(v);
        p = M3 * p * 2.0 + n*0.25+0.25;
        a *= 0.5;
    }




    return clamp(easeOutExpo(v), 0.,1.);
}
