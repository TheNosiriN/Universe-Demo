#version 460

layout(local_size_x = 1, local_size_y = 1) in;
layout(binding = 0, r32f) uniform image2D colordepthTex;
layout(binding = 1, rgba32f) uniform image2D uvTex;


layout(push_constant) uniform Camera {
	vec2 frame_size;
	mat4 cam_projection;
	mat4 cam_view;
	vec3 cam_pos;
};


layout(std430, binding = 2) buffer Positions {
    vec3 pos_buffer[];
};


vec3 rayDirection(float fieldOfView, vec2 size, vec2 fragCoord) {
    vec2 xy = fragCoord - size / 2.0;
    float z = size.y / tan(radians(fieldOfView) / 2.0);
    return normalize(vec3(xy, -z));
}


vec2 rayBox(vec3 origin, vec3 direction, vec3 cMin, vec3 cMax) {
   vec3 tMin = (cMin - origin) / direction;
   vec3 tMax = (cMax - origin) / direction;
   vec3 t1 = min(tMin, tMax);
   vec3 t2 = max(tMin, tMax);
   float tNear = max(max(t1.x, t1.y), t1.z);
   float tFar = min(min(t2.x, t2.y), t2.z);
   return vec2(tNear, tFar);
}

float hash21(vec2 p)
{
    p  = 50.0*fract( p*0.3183099 + vec2(0.71,0.419));
    return fract( p.x*p.y*(p.x+p.y) );
}
const vec2 zOffset = vec2(37,17);
float tnoise( in vec3 x )
{
    vec3 i = floor(x);
	vec3 f = fract(x);
	f = f*f*f*(f*(f*6.0-15.0)+10.0);

	vec2 uv = i.xy + zOffset*i.z;
	vec2 rgA = vec2( hash21(uv+vec2(0,0)), hash21(uv+vec2(0,0)+zOffset) );
	vec2 rgB = vec2( hash21(uv+vec2(1,0)), hash21(uv+vec2(1,0)+zOffset) );
	vec2 rgC = vec2( hash21(uv+vec2(0,1)), hash21(uv+vec2(0,1)+zOffset) );
	vec2 rgD = vec2( hash21(uv+vec2(1,1)), hash21(uv+vec2(1,1)+zOffset) );
	vec2 rg = mix( mix( rgA, rgB, f.x ),
								 mix( rgC, rgD, f.x ), f.y );
	return mix( rg.x, rg.y, f.z );
}


float iBox( in vec3 ro, in vec3 rd, in vec2 distBound, inout vec3 normal, in vec3 boxSize ) {
    vec3 m = sign(rd)/max(abs(rd), 1e-8);
    vec3 n = m*ro;
    vec3 k = abs(m)*boxSize;

    vec3 t1 = -n - k;
    vec3 t2 = -n + k;

	float tN = max( max( t1.x, t1.y ), t1.z );
	float tF = min( min( t2.x, t2.y ), t2.z );

    if (tN > tF || tF <= 0.) {
        return -1;
    } else {
        if (tN >= distBound.x && tN <= distBound.y) {
        	normal = -sign(rd)*step(t1.yzx,t1.xyz)*step(t1.zxy,t1.xyz);
            return tN;
        } else if (tF >= distBound.x && tF <= distBound.y) {
        	normal = -sign(rd)*step(t1.yzx,t1.xyz)*step(t1.zxy,t1.xyz);
            return tF;
        } else {
            return -1;
        }
    }
}



mat4 viewMatrix(vec3 eye, vec3 center, vec3 up) {
    vec3 f = normalize(center - eye);
    vec3 s = normalize(cross(f, up));
    vec3 u = cross(s, f);
    return mat4(
        vec4(s, 0.0),
        vec4(u, 0.0),
        vec4(-f, 0.0),
        vec4(0.0, 0.0, 0.0, 1)
    );
}


#define R frame_size

void main() {
	ivec2 Ci = ivec2(gl_GlobalInvocationID.xy);
	vec2 C = vec2(Ci);
	vec2 uv = (C - R*0.5) / min(R.x, R.y);

    vec3 dir = normalize(vec3(uv, -10.0));
    dir = (cam_view * vec4(dir, 1.0)).xyz;


	vec3 normal;
    float dp = iBox(cam_pos, dir, vec2(.0001, 1000), normal, vec3(1.0));

	imageStore(colordepthTex, Ci, vec4(1.0));
	imageStore(uvTex, Ci, vec4( normal+dir, 0.0 ));
}
