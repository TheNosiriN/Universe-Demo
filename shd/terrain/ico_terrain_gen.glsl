#version 460



#include "common.inl.glsl"
#include "common_math.inl.glsl"
#include "universe_gen.inl.glsl"
#include "terrain_gen.inl.glsl"



layout(local_size_x = 1) in;

layout(push_constant) uniform Props {
	TerrainComputeProps props;
};

layout(std430, binding = 0) buffer writeonly VertBuffer {
	vec4 OutputVertexBuffer[];
};

/// Vulkan doesn't support atomic counter buffers so I have to use an SSBO and atomicAdd
layout(std430, binding = 1) buffer AtomicCounter {
	uint CurrentVertexCounter;
};




const uint StartRes = 25;//50;
const uint MaxLevel = _PLANETS_MAX_TERRAIN_LEVELS;
const uint StartLevel = 0;//10;



const uint Idx[63] = uint[](
	0, 11, 5, 0, 5, 1, 0, 1, 7, 0, 7, 10, 0, 10, 11,
	1, 5, 9, 5, 11, 4, 11, 10, 2, 10, 7, 6, 10, 7, 6, 7, 1, 8,
	3, 9, 4, 3, 4, 2, 3, 2, 6, 3, 6, 8, 3, 8, 9,
	4, 9, 5, 2, 4, 11, 6, 2, 10, 8, 6, 7, 9, 8, 1
);

const float Ico_t = (1.0f + sqrt(5.0f)) / 2.0f;
const vec3 IcoPoints[12] = vec3[](
	vec3( -1, Ico_t, 0 ), vec3( 1, Ico_t, 0 ),
	vec3( -1, -Ico_t, 0 ), vec3( 1, -Ico_t, 0 ),
	vec3( 0, -1, Ico_t ), vec3( 0, 1, Ico_t ),
	vec3( 0, -1, -Ico_t ), vec3( 0, 1, -Ico_t ),
	vec3( Ico_t, 0, -1 ), vec3( Ico_t, 0, 1 ),
	vec3( -Ico_t, 0, -1 ), vec3( -Ico_t, 0, 1 )
);

const uint RecurseIdx[12] = uint[](0, 3, 5, 5, 3, 4, 3, 1, 4, 5, 4, 2);



struct TriangleNode {
	vec3 p1;
	vec3 p2;
	vec3 p3;
	uint level;
};



void AddTriangle(TriangleNode node){
	atomicAdd(CurrentVertexCounter, 3);
	uint vertcount = CurrentVertexCounter-3;
	OutputVertexBuffer[vertcount+0] = vec4(node.p1*props.radius, node.level);
	OutputVertexBuffer[vertcount+1] = vec4(node.p2*props.radius, node.level);
	OutputVertexBuffer[vertcount+2] = vec4(node.p3*props.radius, node.level);
}



bool DotprodCull(vec3 p1, vec3 p2, vec3 p3, vec3 observer){
	return (
		dot( p1, observer ) < 0.75 &&
		dot( p2, observer ) < 0.75 &&
		dot( p3, observer ) < 0.75
	);
}




struct TempFixedArray {
	TriangleNode data[96];
	uint size;
} global_stack;


void TFA_push(TriangleNode node){
	global_stack.data[global_stack.size] = node;
	global_stack.size += 1u;
}
TriangleNode TFA_pop(){
	global_stack.size -= 1u;
	return global_stack.data[global_stack.size];
}
uint TFA_length(){
	return global_stack.size;
}



float DistanceLevels[MaxLevel];

void PrecomputeDistanceLevels(){
	for (uint i=0u; i<MaxLevel; ++i){
		DistanceLevels[i] = StartRes/pow(2.0, float(i));
	}
}




void Recurse(vec3 observer){
	while (TFA_length() > 0u) {
		TriangleNode node = TFA_pop();

		// vec3 ave_point = normalize((node.p1 + node.p2 + node.p3) / 3.0);
		// float samp = planet_fbm_far(warp_uvsphere_to_vec3(warp_vec3_to_uvsphere(ave_point)), 8u);
		vec3 tp1 = node.p1;// - normalize(node.p1)*samp*props.radius*0.01;
		vec3 tp2 = node.p2;// - normalize(node.p2)*samp*props.radius*0.01;
		vec3 tp3 = node.p3;// - normalize(node.p3)*samp*props.radius*0.01;


		/// culling
		if (DotprodCull(tp1, tp2, tp3, observer)){ continue; }
		// if (FrustumCull(*(proc->frustum), tp1, tp2, tp3, observer)){ continue; }


		vec3 edges[3] = vec3[](
			(tp1 + tp2) / 2.0,
			(tp2 + tp3) / 2.0,
			(tp3 + tp1) / 2.0
		);
		uint edgeDistMask = 0u;

		for (uint i=0u; i<3u; ++i){
			edgeDistMask |= uint(distance(edges[i], observer) > DistanceLevels[node.level]) << i;
		}

		/// Add Triangle
		if ( (edgeDistMask >= 7u) || node.level >= MaxLevel ){
			if (node.level >= StartLevel){
				AddTriangle(node);
			}
			continue;
		}


		vec3 p[6] = {
			tp1, tp2, tp3,
			edges[0], edges[1], edges[2]
		};
		uint valid = 255u;

		if (bool(edgeDistMask & (1 << 0))){ p[3] = tp1; valid ^= 1 << 0; } // skip triangle 0
		if (bool(edgeDistMask & (1 << 1))){ p[4] = tp2; valid ^= 1 << 2; } // skip triangle 2
		if (bool(edgeDistMask & (1 << 2))){ p[5] = tp3; valid ^= 1 << 3; } // skip triangle 3

		for (uint i=0; i<4; ++i){
			if (!bool(valid & (1 << i))){ continue; }

			TriangleNode new_node;
			new_node.p1 = normalize( p[RecurseIdx[3 * i + 0]] );
			new_node.p2 = normalize( p[RecurseIdx[3 * i + 1]] );
			new_node.p3 = normalize( p[RecurseIdx[3 * i + 2]] );
			new_node.level = node.level+1u;
			TFA_push(new_node);
		}

	}
}



void main(){
	uint id = gl_GlobalInvocationID.x;

	PrecomputeDistanceLevels();

	TriangleNode first_node;
	first_node.p1 = normalize( IcoPoints[Idx[id * 3 + 0]] );
	first_node.p2 = normalize( IcoPoints[Idx[id * 3 + 1]] );
	first_node.p3 = normalize( IcoPoints[Idx[id * 3 + 2]] );
	first_node.level = 0u;
	TFA_push(first_node);

	Recurse(props.observer);
}
