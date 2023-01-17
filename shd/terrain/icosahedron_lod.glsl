#version 460



#include "common.inl.glsl"
#include "common_math.inl.glsl"
#include "terrain_gen.inl.glsl"



layout(local_size_x = 1024) in;


layout(std430, binding = 0) buffer VertexBuffer {
	vec4 vert_buffer[];
};
layout(std430, binding = 1) buffer NormalsBuffer {
	float normals_buffer[];
};
layout(std430, binding = 2) buffer IndexBuffer {
	uint index_buffer[];
};

layout(push_constant) uniform Props {
	uint vertcount;
};


layout(binding = 0) uniform sampler2D heightTex;








// vec3 normal(vec3 p, vec3 np, vec3 rad)
// {
//  	vec3 P = vec3(-4, 4, 0) * 0.0001;
//  	return normalize(
//         (np - fbm(p+P.xyy)*rad) * P.xyy +
// 		(np - fbm(p+P.yxy)*rad) * P.yxy +
// 		(np - fbm(p+P.yyx)*rad) * P.yyx +
// 		(np - fbm(p+P.xxx)*rad) * P.xxx
//     );
// }

vec3 flatnormal(vec3 v1, vec3 v2, vec3 v3){
	vec3 u = v2 - v1;
    vec3 v = v3 - v1;

	return normalize(vec3(
		(u.y*v.z) - (u.z*v.y),
		(u.z*v.x) - (u.x*v.z),
		(u.x*v.y) - (u.y*v.x)
	));
}

vec3 makenormal(vec2 uv, float samp, uint octaves){
	vec3 v1 = warp_uvsphere_to_vec3(uv);
	vec3 v2 = warp_uvsphere_to_vec3(mod(uv+vec2(0,1)*0.00001, 2.0));
	vec3 v3 = warp_uvsphere_to_vec3(mod(uv+vec2(1,0)*0.00001, 2.0));
	v1 -= v1*samp*0.01;
	v2 -= v2*planet_fbm_far(v2, octaves).x*0.01;
	v3 -= v3*planet_fbm_far(v3, octaves).x*0.01;

	return -flatnormal(v1, v2, v3);
}

void main(){
	uint id = gl_GlobalInvocationID.x;
	if (id >= vertcount)return;

	uint rid = id * 3u;

	vec4 pos[3];
	pos[0] = vert_buffer[rid+0];
	pos[1] = vert_buffer[rid+1];
	pos[2] = vert_buffer[rid+2];

	vec3 norms[3];

	/// generate heights
	for (uint i=0; i<3; ++i){
		float rad = length(pos[i].xyz);
		vec3 npos = normalize(pos[i].xyz);

		float samp = planet_fbm_far(npos, 8u);
		// float samp = texture(heightTex, warp_vec3_to_uvsphere(npos)).x;
		pos[i].xyz -= samp*npos*rad*0.01;
		norms[i] = makenormal(warp_vec3_to_uvsphere(npos), samp, 8u);
	}


	/// generate normals
	// vec3 norm = -flatnormal(pos[0].xyz, pos[1].xyz, pos[2].xyz);


	for (uint i=0; i<3; ++i){
		vert_buffer[rid+i].xyz = pos[i].xyz;
		normals_buffer[(rid+i) * 3 + 0] = norms[i].x;
		normals_buffer[(rid+i) * 3 + 1] = norms[i].y;
		normals_buffer[(rid+i) * 3 + 2] = norms[i].z;
	}
	//normal(npos*10000.0, pos, npos*rad*0.0001);

}
