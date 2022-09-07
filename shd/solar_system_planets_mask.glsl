#version 460
precision highp float;


#include "common.inl.glsl"
#include "common_math.inl.glsl"
#include "universe_gen.inl.glsl"




#ifdef HX_VERTEX_SHADER

layout(location = 0) in vec3 pos; /// unused


layout(std140, binding = 0) uniform GlobalCamConstants {
	CameraConstants camera;
};

layout(std140, binding = 1) uniform SolarSystemPropertiesCBuff {
	SolarSystemProperties prop;
};


layout(push_constant) uniform RenderPos {
	vec3 renderpos;
};



// vec3 makeRingVertex(float vertex, uint index){
// 	float angle = vertex/64.0 * PI * 2.0;
// 	vec3 pos = vec3(cos(angle) * prop.orbits[index].radius.x, 0.0, sin(angle) * prop.orbits[index].radius.y);
//
// 	pos.xz = rotation2D(prop.orbits[index].angle_offset.x) * pos.xz;
// 	// pos.xy = rotation2D(prop.orbits[index].angle_offset.y) * pos.xy;
//
// 	return pos;
// }



void main(){

	// vec3 vertpos = makeRingVertex( gl_VertexIndex, gl_InstanceIndex );

	vec3 rasterpos = renderpos + mulvq(prop.orientation, pos);
	gl_Position = camera.projection * camera.view * vec4(rasterpos, 1);
}

#endif




#ifdef HX_FRAGMENT_SHADER

layout(location = 0) out vec4 FragColor;

void main(){
	FragColor = vec4(0,1,0,1);
}

#endif
