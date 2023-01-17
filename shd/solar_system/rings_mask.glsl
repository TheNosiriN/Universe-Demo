#version 460
precision highp float;


#include "common.inl.glsl"
#include "common_math.inl.glsl"
#include "universe_gen.inl.glsl"




float solveKepler(float e, float M, const int N){
    float E = M;
    for(int i=0; i<N; ++i){
        E = M + e*sin(E);
    }
    return E;
}


vec3 makeKeplerOrbit(float vertex, SolarSystemOrbit orbit, out float angle){
	vec2 angle_offset = unpackHalf2x16(orbit.angle_offset);

	float t = vertex/128.0;
    float e = orbit.eccentrity;
    float a = orbit.semi_major_axis;
    float n = PI*2.0;

    float M = n * t;
    float E = solveKepler(e, M, 8);

    angle = 2.0 * atan(sqrt( (1.0+e)/(1.0-e) ) * tan(E/2.0));
    float dist = (a*(1.0-e*e))/(1.0 + e*cos(angle));

	vec3 pos = vec3(cos(angle), 0, sin(angle)) * dist;
	pos.xz = rotation2D(angle_offset.x) * pos.xz;
	pos.xy = rotation2D(angle_offset.y) * pos.xy;

	return pos;
}





layout(std140, binding = 0) uniform GlobalCamConstants {
	CameraConstants camera;
};

layout(std140, binding = 1) uniform SolarSystemPropertiesCBuff {
	SolarSystemProperties prop;
};


layout(std430, binding = 2) buffer SolarSystemOrbitsCBuff {
	SolarSystemOrbit orbits[];
};
layout(std430, binding = 3) buffer SolarSystemOrbitsUpdateCBuff {
	SolarSystemOrbitUpdate orbit_updates[];
};


layout(push_constant) uniform RenderPos {
	vec3 renderpos;
	uint index_offset;
};




#ifdef HX_VERTEX_SHADER

layout(location = 0) in vec3 pos; /// unused

layout(location = 0) out flat uint flags;
layout(location = 1) out flat uint vertIndex;



void main(){
	uint index = gl_InstanceIndex + index_offset;
	SolarSystemOrbit orbit = orbits[ index ];
	flags = orbit.flags;

    float e = orbit.eccentrity;
    float a = orbit.semi_major_axis;
	const float tau = 6.283185307179586;

	vertIndex = gl_VertexIndex==0u ? 128u : gl_VertexIndex;
	float intensity = float(gl_VertexIndex)/128.0;

	float phi = orbit_updates[ index ].angle + tau*intensity;
	float dist = (a*(1.0-e*e))/(1.0 + e*cos(phi));

	vec3 pos = vec3(cos(phi), 0, sin(phi)) * dist;

	vec2 angle_offset = unpackHalf2x16(orbit.angle_offset);
	pos.xz = rotation2D(angle_offset.x) * pos.xz;
	pos.xy = rotation2D(angle_offset.y) * pos.xy;

	vec3 rasterpos = renderpos.xyz + mulvq(prop.orientation, pos);
	gl_Position = camera.projection * camera.view * vec4(rasterpos, 1.0);

}

#endif





// #ifdef HX_GEOMETRY_SHADER
//
// layout(points) in;
// layout(line_strip, max_vertices = 64) out;
//
// layout(location = 0) in flat uint index[];
// layout(location = 0) out flat uint flags;
//
//
// void main(){
// 	vec3 outpos = gl_in[0].gl_Position.xyz;
// 	// float seed = gl_in[0].gl_Position.w;
//
//
// 	SolarSystemOrbit orbit = prop.orbits[index[0]];
// 	mat4 viewproj = camera.projection * camera.view;
//
// 	for (int i=0; i<64; ++i){
// 		vec3 vertpos = makeKeplerOrbit(i, orbit);
// 		vec3 rasterpos = outpos + mulvq(prop.orientation, vertpos);
// 		gl_Position = viewproj * vec4(rasterpos, 1);
// 		EmitVertex();
// 	}
//
//     EndPrimitive();
// }
//
// #endif






#ifdef HX_FRAGMENT_SHADER

layout(location = 0) out vec4 FragColor;

layout(location = 0) in flat uint flags;
layout(location = 1) in flat uint vertIndex;

void main(){
	vec3 color = vec3(1,0,0);

	if (
		bool(flags & _SOLAR_SYSTEM_ORBIT_PLANET_FLAG) || 
		bool(flags & _SOLAR_SYSTEM_ORBIT_MOON_FLAG)
	){
		color = vec3(0.431,0.502,0.643);
	}else
	// if (bool(flags & _SOLAR_SYSTEM_ORBIT_MOON_FLAG)){
	// 	color = vec3(0.431,0.502,0.643) * 0.5;
	// }else
	if (bool(flags & _SOLAR_SYSTEM_ORBIT_ASTEROID_BELT_FLAG)){
		color = vec3(0.353,0.286,0.329);
	}

	FragColor = vec4(color, 1) * (float(vertIndex)/128.0);
}

#endif
