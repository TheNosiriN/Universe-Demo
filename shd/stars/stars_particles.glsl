#version 460



#include "common.inl.glsl"
#include "common_math.inl.glsl"
#include "universe_gen.inl.glsl"



layout(local_size_x = 1024) in;


layout(push_constant) uniform Constants {
	vec4 parentRot;
	vec4 parentRotInv;
	ivec3 grid_cam_pos;
	float parentRadius;
	uint parentSeed;
	uint block_size;	/// unused
};


layout(std140, binding = 0) uniform StarBlocks {
	StarBlock blocks[_STARS_BLOCK_COUNT];
};

layout(std430, binding = 1) buffer StarPositions {
	StarParticle particles[];
};



void main(){
	uint id = gl_GlobalInvocationID.x;


	StarParticle pt = particles[id];
	uint rblocksize = _STARS_BLOCK_STAR_COUNT*_STARS_BLOCK_STAR_COUNT*_STARS_BLOCK_STAR_COUNT;
	float multfac = _STARS_BLOCK_STAR_COUNT * float(_STARS_SPACING_FACTOR);


	/// find the star block we're in
	int block_index = int(id / rblocksize);
	int star_index = int(id % rblocksize);
	ivec3 block_cell = (ind_1Dto3D(block_index, ivec3(_STARS_BLOCK_WIDTH)) - ivec3(_STARS_BLOCK_WIDTH/2)) + grid_cam_pos;

	StarBlock block = blocks[block_index];


	/// set the current star position using seed
	uint seed = ConstructBasicSeed(ivec4(star_index, block.seed, block_cell));


	vec3 randp = vec3(frandom(seed), frandom(seed), frandom(seed)) - 0.5;

	float closeness_to_center = frandom(seed);
	float closeness_to_sub_cluster = pow(mix(2.0,5.0,frandom(seed)), 10.0f*frandom(seed) - 10.0f);

	randp = normalize(randp) * sqrt(closeness_to_center);
	randp = mix(randp, block.gravityPoint, mix(0.0, 1.0 - 2.0/multfac, closeness_to_sub_cluster));


	pt.renderpos = randp * multfac;
	pt.renderpos = mulvq(parentRot, pt.renderpos);

	float factor = block.spawnFactor;
	if (frandom(seed) >= 1.0*factor){
		pt.seed = 0u;
	}else{
		/// generate temperature
		float temp = mix(block.temperature, easeInExpo(frandom(seed)), easeOutExpo(min(distance(randp, block.gravityPoint), 1.0)));
		pt.seed = packHalf2x16(vec2(temp, closeness_to_sub_cluster));
	}



	particles[id] = pt;
}
