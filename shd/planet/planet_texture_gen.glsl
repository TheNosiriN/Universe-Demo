#version 460


#include "common.inl.glsl"
#include "common_math.inl.glsl"
#include "universe_gen.inl.glsl"
#include "terrain/terrain_gen.inl.glsl"



layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;


layout(binding = 0, rgba8) uniform image2D diffuse_tex0;
layout(binding = 1, rgba8) uniform image2D diffuse_tex1;
layout(binding = 2, rgba8) uniform image2D diffuse_tex2;
layout(binding = 3, rgba8) uniform image2D diffuse_tex3;

layout(binding = 4, rgba16f) uniform image2D normals_tex0;
layout(binding = 5, rgba16f) uniform image2D normals_tex1;
layout(binding = 6, rgba16f) uniform image2D normals_tex2;
layout(binding = 7, rgba16f) uniform image2D normals_tex3;


layout(std140, binding = 0) uniform Props {
	PlanetTextureProps props;
};



// vec3 makesamp(vec3 p, uint octaves){
// 	return p - normalize(p) * planet_fbm_far(p, 8u) * 0.01;
// }

// vec3 makenormal(vec3 p, uint octaves){
//  	vec3 P = vec3(-4, 4, 0) * 0.0001;
//  	return normalize(
//         makesamp(p+P.xyy, octaves) * P.xyy +
// 		makesamp(p+P.yxy, octaves) * P.yxy +
// 		makesamp(p+P.yyx, octaves) * P.yyx +
// 		makesamp(p+P.xxx, octaves) * P.xxx
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

// vec3 makenormal(vec2 uv, uint octaves){
// 	vec3 v1 = warp_uvsphere_to_vec3(uv);
// 	vec3 v2 = warp_uvsphere_to_vec3(uv+vec2(0,1)*0.0001);
// 	vec3 v3 = warp_uvsphere_to_vec3(uv+vec2(1,0)*0.0001);
// 	v1 -= normalize(v1)*planet_fbm_far(v1, 8u)*0.01;
// 	v2 -= normalize(v2)*planet_fbm_far(v2, 8u)*0.01;
// 	v3 -= normalize(v3)*planet_fbm_far(v3, 8u)*0.01;
//
// 	return flatnormal(v1, v2, v3);
// }



void main(){
	uint index = props.currentTexIndices[gl_GlobalInvocationID.z];
	uint width = uint(float(_PLANET_TEXTURE_WIDTH) / pow(4.0, float(index)));

	ivec2 Ci = ivec2(gl_GlobalInvocationID.xy);
	if (Ci.x>=width && Ci.y>=width){
		return;
	}

	vec2 uv = vec2(Ci) / float(width);

	vec3 npos = warp_uvsphere_to_vec3(uv);
	// float height = planet_fbm_far(npos, 8u);

	float height = planet_fbm_far(npos, 8u);

	vec3 v1 = npos;
	vec3 v2 = warp_uvsphere_to_vec3(mod(uv+vec2(0,1)*0.00001, 2.0));
	vec3 v3 = warp_uvsphere_to_vec3(mod(uv+vec2(1,0)*0.00001, 2.0));
	v1 -= v1*height*0.01;
	v2 -= v2*planet_fbm_far(v2, 8u)*0.01;
	v3 -= v3*planet_fbm_far(v3, 8u)*0.01;

	vec3 normal = -flatnormal(v1, v2, v3);
	vec3 color = vec3(height);



	switch(index) {
		case 0u:
		imageStore(diffuse_tex0, Ci, vec4(color, 0));
		imageStore(normals_tex0, Ci, vec4(normal, 0));
		break;
		case 1u:
		imageStore(diffuse_tex1, Ci, vec4(color, 0));
		imageStore(normals_tex1, Ci, vec4(normal, 0));
		break;
		case 2u:
		imageStore(diffuse_tex2, Ci, vec4(color, 0));
		imageStore(normals_tex2, Ci, vec4(normal, 0));
		break;
		case 3u:
		imageStore(diffuse_tex3, Ci, vec4(color, 0));
		imageStore(normals_tex3, Ci, vec4(normal, 0));
		break;
	}
}
