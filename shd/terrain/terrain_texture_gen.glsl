#version 460


#include "common.inl.glsl"
#include "common_math.inl.glsl"
#include "universe_gen.inl.glsl"
#include "terrain/terrain_gen.inl.glsl"



layout(local_size_x = 32, local_size_y = 32) in;


layout(binding = 0, r32f) uniform writeonly image2D heightTex;



void main(){
	ivec2 Ci = ivec2(gl_GlobalInvocationID.xy);


	vec3 npos = warp_uvsphere_to_vec3(vec2(Ci)/_TERRAIN_HEIGHT_TEXTURE_WIDTH);
	float height = planet_fbm_far(npos, 8u);


	imageStore(heightTex, Ci, vec4(height, 0,0,0));
}
