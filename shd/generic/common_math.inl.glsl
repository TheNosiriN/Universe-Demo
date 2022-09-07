
uint ConstructBasicSeed(ivec4 p){
	return (uint(p.x)*1973u + uint(p.y)*9277u + uint(p.z)*10195u + uint(p.w)*26699u) | 1u;
}

uint wang_hash(inout uint seed){
    seed = (seed ^ 61u) ^ (seed >> 16u);
    seed *= 9u;
    seed = seed ^ (seed >> 4);
    seed *= 0x27d4eb2du;
    seed = seed ^ (seed >> 15);
    return seed;

	// seed = seed * 747796405u + 2891336453u;
	// seed = ((seed >> ((seed >> 28u) + 4u)) ^ seed) * 277803737u;
	// return (seed >> 22u) ^ seed;

	// uint seed = s;
	// seed ^= seed >> 17;
	// seed *= 0xed5ad4bbU;
	// seed ^= seed >> 11;
	// seed *= 0xac4c1b51U;
	// seed ^= seed >> 15;
	// seed *= 0x31848babU;
	// seed ^= seed >> 14;
	// return seed;
}
float frandom(inout uint state){
    return float(wang_hash(state)) / float(0xffffffffu);
}





float easeInExpo(float x) {
	return x==0.0f ? 0.0f : pow(2.0f, 10.0f*x - 10.0f);
}
float easeOutExpo(float x) {
	return x==1.0f ? 1.0f : 1.0f - pow(2.0f, -10.0f * x);
}
float easeInOutExpo(float x) {
    return x == 0.0
      ? 0.0
      : x == 1.0
      ? 1.0
      : x < 0.5 ? pow(2.0, 20.0 * x - 10.0) / 2.0
      : (2.0 - pow(2.0, -20.0 * x + 10.0)) / 2.0;
}
float easeOutInExpo(float t) {
    if( t < 0.5f ) return easeOutExpo(2.0f * t) / 2.0f;
    return easeInExpo(2.0f * t - 1.0f) / 2.0f + 0.5f;
}

float easeOutBounce(float x) {
    const float n1 = 7.5625;
    const float d1 = 2.75;

    if (x < 1.0 / d1) {
        return n1 * x * x;
    } else if (x < 2.0 / d1) {
        return n1 * (x -= 1.5 / d1) * x + 0.75;
    } else if (x < 2.5 / d1) {
        return n1 * (x -= 2.25 / d1) * x + 0.9375;
    } else {
        return n1 * (x -= 2.625 / d1) * x + 0.984375;
    }
}
float easeInBounce(float x) {
    return 1.0 - easeOutBounce(1.0 - x);
}
float easeInOutBounce(float x) {
    return x < 0.5
      ? (1.0 - easeOutBounce(1.0 - 2.0 * x)) / 2.0
      : (1.0 + easeOutBounce(2.0 * x - 1.0)) / 2.0;
}

float easeOutCirc(float x) {
    return sqrt(1.0 - pow(x - 1.0, 2.0));
}
float easeInCirc(float x) {
    return 1.0 - sqrt(1.0 - pow(x, 2.0));
}





mat3 rotationAxisAngle3D(vec3 v, float a){
    const float si = sin( a );
    const float co = cos( a );
    const float ic = 1.0f - co;

    return mat3x3( v.x*v.x*ic + co,       v.y*v.x*ic - si*v.z,    v.z*v.x*ic + si*v.y,
                   v.x*v.y*ic + si*v.z,   v.y*v.y*ic + co,        v.z*v.y*ic - si*v.x,
                   v.x*v.z*ic - si*v.y,   v.y*v.z*ic + si*v.x,    v.z*v.z*ic + co );
}

mat3 rotationAlign3D(vec3 d, vec3 z){
    const vec3  v = cross( z, d );
    const float c = dot( z, d );
    const float k = 1.0f/(1.0f+c);

    return mat3x3(
		v.x*v.x*k + c,     v.y*v.x*k - v.z,    v.z*v.x*k + v.y,
		v.x*v.y*k + v.z,   v.y*v.y*k + c,      v.z*v.y*k - v.x,
		v.x*v.z*k - v.y,   v.y*v.z*k + v.x,    v.z*v.z*k + c
	);
}

mat2 rotation2D(float angle){
    float s = sin(angle), c = cos(angle);
    return mat2( c, -s, s, c );
}


vec3 mulvq(vec4 q, vec3 v){		//rotates a 3D vector by a quaternion.
	vec3 result;
	float axx = q.x * 2.0;
	float ayy = q.y * 2.0;
	float azz = q.z * 2.0;
	float awxx = q.a * axx;
	float awyy = q.a * ayy;
	float awzz = q.a * azz;
	float axxx = q.x * axx;
	float axyy = q.x * ayy;
	float axzz = q.x * azz;
	float ayyy = q.y * ayy;
	float ayzz = q.y * azz;
	float azzz = q.z * azz;
	result.x = ((v.x * ((1.0 - ayyy) - azzz)) + (v.y * (axyy - awzz))) + (v.z * (axzz + awyy));
	result.y = ((v.x * (axyy + awzz)) + (v.y * ((1.0 - axxx) - azzz))) + (v.z * (ayzz - awxx));
	result.z = ((v.x * (axzz - awyy)) + (v.y * (ayzz + awxx))) + (v.z * ((1.0 - axxx) - ayyy));
	return result;
}

vec4 qaxang(vec3 ax, float ang){	//creates a quaternion from axis and angle.
	float ha = ang * 0.5f;
	float sha = sin(ha);
	return vec4(ax.x * sha, ax.y * sha, ax.z * sha, cos(ha));
}


// Converts from pure Hue to linear RGB
vec3 hue_to_rgb(float hue){
    float R = abs(hue * 6.0f - 3.0f) - 1.0f;
    float G = 2.0f - abs(hue * 6.0f - 2.0f);
    float B = 2.0f - abs(hue * 6.0f - 4.0f);
    return clamp(vec3(R,G,B), 0.0, 1.0);
}

// Converts from HSV to linear RGB
vec3 hsv_to_rgb(vec3 hsv){
    vec3 rgb = hue_to_rgb(hsv.x);
    return ((rgb - 1.0f) * hsv.y + 1.0f) * hsv.z;
}





float noise3d(sampler2D tex, const vec3 x) {
    vec3 p = floor(x), f = fract(x);
	f *= f*(3.-f-f);
	vec2 uv = (p.xy+vec2(37.,17.)*p.z) + f.xy;
	vec2 rg = texture( tex, (uv+.5)/256.).yx;
	return mix(rg.x, rg.y, f.z);
}

float fbm(sampler2D tex, const vec3 p){
    return (
		noise3d(tex, p*0.06125)*0.5 +
		noise3d(tex, p*0.125)*0.25 +
		noise3d(tex, p*0.25)*0.125 +
		noise3d(tex, p*0.4)*0.2
	);
}


vec3 hash33(vec3 p3){
	p3 = fract(p3 * vec3(.1031,.11369,.13787));
    p3 += dot(p3, p3.yxz+19.19);
    return -1.0 + 2.0 * fract(vec3((p3.x + p3.y)*p3.z, (p3.x+p3.z)*p3.y, (p3.y+p3.z)*p3.x));
}

float perlin(vec3 p){
    vec3 pi = floor(p);
    vec3 pf = p - pi;

    vec3 w = pf * pf * (3.0 - 2.0 * pf);

    return 	mix(
        		mix(
                	mix(dot(pf - vec3(0, 0, 0), hash33(pi + vec3(0, 0, 0))),
                        dot(pf - vec3(1, 0, 0), hash33(pi + vec3(1, 0, 0))),
                       	w.x),
                	mix(dot(pf - vec3(0, 0, 1), hash33(pi + vec3(0, 0, 1))),
                        dot(pf - vec3(1, 0, 1), hash33(pi + vec3(1, 0, 1))),
                       	w.x),
                	w.z),
        		mix(
                    mix(dot(pf - vec3(0, 1, 0), hash33(pi + vec3(0, 1, 0))),
                        dot(pf - vec3(1, 1, 0), hash33(pi + vec3(1, 1, 0))),
                       	w.x),
                   	mix(dot(pf - vec3(0, 1, 1), hash33(pi + vec3(0, 1, 1))),
                        dot(pf - vec3(1, 1, 1), hash33(pi + vec3(1, 1, 1))),
                       	w.x),
                	w.z),
    			w.y);
}


vec4 permute(vec4 x) {
  return mod((34.0 * x + 1.0) * x, 289.0);
}
vec3 permute(vec3 x) {
  return mod((34.0 * x + 1.0) * x, 289.0);
}


#define K 0.142857142857
#define Ko 0.428571428571
#define K2 0.020408163265306
#define Kz 0.166666666667
#define Kzo 0.416666666667
#define jitter 0.8

vec2 cellular(vec3 P) {
	vec3 Pi = mod(floor(P), 289.0);
 	vec3 Pf = fract(P);
	vec4 Pfx = Pf.x + vec4(0.0, -1.0, 0.0, -1.0);
	vec4 Pfy = Pf.y + vec4(0.0, 0.0, -1.0, -1.0);
	vec4 p = permute(Pi.x + vec4(0.0, 1.0, 0.0, 1.0));
	p = permute(p + Pi.y + vec4(0.0, 0.0, 1.0, 1.0));
	vec4 p1 = permute(p + Pi.z);
	vec4 p2 = permute(p + Pi.z + vec4(1.0));
	vec4 ox1 = fract(p1*K) - Ko;
	vec4 oy1 = mod(floor(p1*K), 7.0)*K - Ko;
	vec4 oz1 = floor(p1*K2)*Kz - Kzo;
	vec4 ox2 = fract(p2*K) - Ko;
	vec4 oy2 = mod(floor(p2*K), 7.0)*K - Ko;
	vec4 oz2 = floor(p2*K2)*Kz - Kzo;
	vec4 dx1 = Pfx + jitter*ox1;
	vec4 dy1 = Pfy + jitter*oy1;
	vec4 dz1 = Pf.z + jitter*oz1;
	vec4 dx2 = Pfx + jitter*ox2;
	vec4 dy2 = Pfy + jitter*oy2;
	vec4 dz2 = Pf.z - 1.0 + jitter*oz2;
	vec4 d1 = dx1 * dx1 + dy1 * dy1 + dz1 * dz1;
	vec4 d2 = dx2 * dx2 + dy2 * dy2 + dz2 * dz2;

	/* Sort out the two smallest distances (F1, F2) */
#if 1
	// Cheat and sort out only F1
	d1 = min(d1, d2);
	d1.xy = min(d1.xy, d1.wz);
	d1.x = min(d1.x, d1.y);
	return sqrt(d1.xx);
#else
	// Do it right and sort out both F1 and F2
	vec4 d = min(d1,d2); // F1 is now in d
	d2 = max(d1,d2); // Make sure we keep all candidates for F2
	d.xy = (d.x < d.y) ? d.xy : d.yx; // Swap smallest to d.x
	d.xz = (d.x < d.z) ? d.xz : d.zx;
	d.xw = (d.x < d.w) ? d.xw : d.wx; // F1 is now in d.x
	d.yzw = min(d.yzw, d2.yzw); // F2 now not in d2.yzw
	d.y = min(d.y, d.z); // nor in d.z
	d.y = min(d.y, d.w); // nor in d.w
	d.y = min(d.y, d2.x); // F2 is now in d.y
	return sqrt(d.xy); // F1 and F2
#endif
}






int ind_3Dto1D(ivec3 p, ivec3 bmax) {
    return (p.z * bmax.x * bmax.y) + (p.y * bmax.x) + p.x;
}

ivec3 ind_1Dto3D(int idx, ivec3 bmax) {
    int z = idx / (bmax.x * bmax.y);
    idx -= (z * bmax.x * bmax.y);
    int y = idx / bmax.x;
    int x = idx % bmax.x;
    return ivec3(x, y, z);
}





float RayBox( in vec3 ro, in vec3 rd, in vec2 distBound, in vec3 boxSize, out bool hit, out bool inside, out vec2 tfn) {
    vec3 m = sign(rd)/max(abs(rd), 1e-8);
    vec3 n = m*ro;
    vec3 k = abs(m)*boxSize;

    vec3 t1 = -n - k;
    vec3 t2 = -n + k;

	float tN = max( max( t1.x, t1.y ), t1.z );
	float tF = min( min( t2.x, t2.y ), t2.z );
    tfn = vec2(tN, tF);

    if (tN > tF || tF <= 0.) {
		hit = false;
        return 0;
    } else {
        if (tN >= distBound.x && tN <= distBound.y) {
			hit = true;
			inside = false;
			// normal = -sign(rd)*step(t1.yzx,t1.xyz)*step(t1.zxy,t1.xyz);
            return tN;
        } else if (tF >= distBound.x && tF <= distBound.y) {
			hit = true;
			inside = true;
			// normal = -sign(rd)*step(t1.yzx,t1.xyz)*step(t1.zxy,t1.xyz);
            return tF; // this means inside
        } else {
			hit = false;
            return 0;
        }
    }
}

float RaySphere(vec3 eye, vec3 dir, float rad, out bool inside, out bool hit, out vec2 tfn){
    float b = dot(eye, dir);
    float c = dot(eye, eye) - rad*rad;
    float d = b*b - c;

    if (d < 0.0){
        hit = false;
    }else{
        hit = true;
    }

	d = sqrt(d);
	tfn.x = -b-d;
	tfn.y = -b+d;

    if (tfn.x < 0.){
        inside = true;
        return tfn.y;
    }else{
        inside = false;
        return tfn.x;
    }
}



// vec3 warp_uvsphere_to_vec3(vec2 uv){
// 	vec2 thetaphi = (uv*2.0-1.0) * vec2(PI, PI/2.0);
//     return vec3(cos(thetaphi.y) * cos(thetaphi.x), sin(thetaphi.y), cos(thetaphi.y) * sin(thetaphi.x));
// }
// vec2 warp_vec3_to_uvsphere(vec3 p){
// 	p = normalize(p);
// 	return vec2((atan(p.z, p.x) / (PI*2.0)) + 0.5, acos(p.y) / PI);
// }

/// from: http://www.vias.org/comp_geometry/math_coord_convert_3d.htm
vec3 warp_uvsphere_to_vec3(vec2 uv){
	vec2 s = (uv-vec2(0,0.5)) * vec2(PI,PI*2.0);
    return vec3(
        sin(s.x) * cos(s.y),
        sin(s.x) * sin(s.y),
        cos(s.x)
    );
}
vec2 warp_vec3_to_uvsphere(vec3 p){
	p = normalize(p);
	vec2 uv = vec2(
        acos(p.z)/PI,
        atan(p.y,p.x)/(PI*2.0) + 0.5
    );
    return uv;
}
