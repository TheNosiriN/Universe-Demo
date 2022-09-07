#version 460

#define R frame_size
#define saturate(x) clamp(x, 0.0, 1.0)
#define PI 3.14159265359




struct BasicParticle {
	vec3 renderpos;
	uint seed;

	vec4 rotation_quaternion;
	ivec3 gridpos;
	vec3 additionalpos;
};






layout(local_size_x = 1, local_size_y = 1) in;
layout(binding = 0, rg16ui) uniform uimage2D maskTex;
layout(binding = 1, rgba16f) uniform image2D colorTex;


layout(push_constant) uniform GalaxyConstants {
	float time;
	mat4 view_projection;
	mat4 inv_view_projection;
	vec2 frame_size;
	vec3 cam_pos;
};


layout(std430, binding = 0) buffer GalaxyPositions {
    BasicParticle particles[];
};



void MakeRay(vec2 uv, mat4 invModelViewProjMatrix, vec3 eye, out vec3 dir){
	vec4 projectedPosition = invModelViewProjMatrix * vec4(uv, 1, 1);
	projectedPosition /= projectedPosition.w;
	dir = normalize(projectedPosition.xyz - eye);
}


float rayBox( in vec3 ro, in vec3 rd, in vec2 distBound, in vec3 boxSize, out bool hit, out vec3 normal, out bool inside, out vec2 tfn) {
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
			normal = -sign(rd)*step(t1.yzx,t1.xyz)*step(t1.zxy,t1.xyz);
            return tN;
        } else if (tF >= distBound.x && tF <= distBound.y) {
			hit = true;
			inside = true;
			normal = -sign(rd)*step(t1.yzx,t1.xyz)*step(t1.zxy,t1.xyz);
            return tF; // this means inside
        } else {
			hit = false;
            return 0;
        }
    }
}

float sphere(vec3 eye, vec3 dir, float rad, out bool inside, out bool hit, out vec2 tfn){
    float a = dot(dir, dir);
    float b = 2.*dot(eye,dir);
    float c = dot(eye,eye)-rad*rad;
    float d = b*b-4.*a*c;

    if (d < 0.0){
        hit = false;
    }else{
        hit = true;
    }

    tfn.x = (-b-sqrt(d))/(2.*a);
    tfn.y = (-b+sqrt(d))/(2.*a);

    if (tfn.x < 0.){
        inside = true;
        return tfn.y;
    }else{
        inside = false;
        return tfn.x;
    }
}



uint wang_hash(inout uint seed){
    seed = (seed ^ 61u) ^ (seed >> 16u);
    seed *= 9u;
    seed = seed ^ (seed >> 4);
    seed *= 0x27d4eb2du;
    seed = seed ^ (seed >> 15);
    return seed;
}
float frandom(inout uint state){
    return float(wang_hash(state)) / float(0xffffffffU);
}

mat3 rotationAxisAngle(vec3 v, float a){
    const float si = sin( a );
    const float co = cos( a );
    const float ic = 1.0f - co;

    return mat3x3( v.x*v.x*ic + co,       v.y*v.x*ic - si*v.z,    v.z*v.x*ic + si*v.y,
                   v.x*v.y*ic + si*v.z,   v.y*v.y*ic + co,        v.z*v.y*ic - si*v.x,
                   v.x*v.z*ic - si*v.y,   v.y*v.z*ic + si*v.x,    v.z*v.z*ic + co );
}





float map(vec3 p){
    // float ds = mapVolume(p);// /2.;

    float rad = 0.5;//pow(p.y, 2.);
    return max(abs(p.y)-1.0, max(length(p.xz)-rad, -length(p.xz)+(rad-0.1) ));//length(q)-6.;//ds;//max(ds, length(p)-6.);
}
float rayVol(vec3 eye, vec3 dir, vec3 radius, float steps, float jit){
    float depth = 0.0;
    float i = 0.0;

    for (float n = 0.; n<steps && depth < 100.; n++){
        if (i >= 0.9)break;

        vec3 cp = eye + dir * depth;

        vec3 q = abs(cp) - radius;
        if (max(q.x, max(q.y,q.z)) > 0.001){
            break;
        }

        float nd = map(cp);

        //float dstep = mix(1., 1., saturate(length(eye-cp)/length(radius)) );
        if (nd < 0.5){
            i += 0.01 + saturate(length(eye-cp)/length(radius))*0.005;
        }

        nd = abs(nd) + jit;
        depth += nd * 1.0;

    }

    return i;
}





void main(){
	ivec2 Ci = ivec2(gl_GlobalInvocationID.xy);

	uvec4 mask = imageLoad(maskTex, Ci);
	if (mask.x == 0u){
		imageStore(colorTex, Ci, vec4(0));
		return;
	}

	mask -= 1u;
	BasicParticle galaxy = particles[int(mask.x)];

	uint seed = galaxy.seed;
	vec4 rnd = vec4(frandom(seed), frandom(seed), frandom(seed), frandom(seed));
	vec3 rndir = normalize(rnd.xyz*2.-1.);
	mat3 rotmat = rotationAxisAngle(rndir, rnd.w * PI * 2.0);
	mat3 inv_rotmat = inverse(rotmat);


	vec2 C = vec2(Ci);
	vec2 uv = (C/R)*2.-1.;

	vec3 eye = cam_pos;
	vec3 dir;
	MakeRay(uv, inv_view_projection, eye, dir);

	vec3 galaxy_pos = eye-galaxy.renderpos;
	eye = galaxy_pos;
	// galaxy_pos = rotmat * galaxy_pos;
	// dir = (inv_rotmat * dir);
	// galaxy_pos = (inv_rotmat * galaxy_pos);

	// vec3 normal;
	// bool hit;
	// bool inside;
	// vec2 tfn;
    // float dp = rayBox(galaxy_pos, dir, vec2(.0001, 10000), vec3(1.0), hit, normal, inside, tfn);
	// rayBox(eye, dir, vec2(0.0001, 10000), vec3(particle_size, 0.5, particle_size), );
	// float dp = sphere(-galaxy.renderpos, dir, 10.0, inside, hit, tfn);

	// float val = abs(tfn.x-tfn.y)*0.1;
	//
	// vec3 col;
	// // col += galaxy.renderpos*0.1;//vec3(mask.xyz)/600.;
	// if (hit){
	// 	col += val;
	// }else{
	// 	// col = dir;
	// }


	vec3 radius = vec3(1.);
    float steps = 48.0;

    vec3 normal;
    bool inside;
    vec2 tfn;
    bool hit;
    float dp = rayBox(eye, dir, vec2(.0001, 10000), vec3(1.0), hit, normal, inside, tfn);
    //float dp = sphere(eye, dir, radius.x, inside, hit, tfn);

    float acc = 0.;

    if (hit){
        acc = abs(tfn.x-tfn.y)*0.5;

		vec3 sp = eye;
	    if (inside){
	        sp = eye;
	    }else{
	        sp = eye + dir * dp;
	    }

	    // float stp = rayVol(sp, dir, radius, steps, 0.);///64.;
	    // acc += stp;

    }else{
        // O = vec4(0);
    }



	imageStore(colorTex, Ci, vec4(vec3(acc),1));
}
