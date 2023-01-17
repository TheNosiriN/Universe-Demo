#version 460
precision highp float;


#include "common.inl.glsl"
#include "common_math.inl.glsl"
#include "universe_gen.inl.glsl"




layout(location = 0) c_attrib vec2 outUV;
layout(location = 1) c_attrib vec3 normal;
layout(location = 2) c_attrib vec3 position;




#ifdef HX_VERTEX_SHADER

layout(location = 0) in vec3 vertpos;
layout(location = 1) in vec2 uvs;


layout(std140, binding = 0) uniform GlobalCamConstants {
	CameraConstants camera;
};

layout(std140, binding = 1) uniform SolarSystemPropertiesCBuff {
	SolarSystemProperties star_props;
};


layout(push_constant) uniform Properties {
	StarFarProperties props;
};


void main(){
	outUV = uvs;
	normal = normalize(vertpos);
	position = vertpos;

	vec3 rasterpos = props.renderpos + mulvq(star_props.orientation, vertpos*props.radius);
	gl_Position = camera.projection * camera.view * vec4(rasterpos, 1);
}

#endif




#ifdef HX_FRAGMENT_SHADER

layout(location = 0) out vec4 FragColor;
layout(binding = 0) uniform sampler2D StarSurface_tex;


vec2 cart_to_polar_no_r(vec3 p){
	return vec2( atan(p.z,p.x), atan(length(p.xz), p.y) );
}

vec3 warp_uvsphere_to_vec3(vec2 uv){
	vec2 thetaphi = (uv*2.0-1.0) * vec2(PI, PI/2.0);
    return vec3(cos(thetaphi.y) * cos(thetaphi.x), sin(thetaphi.y), cos(thetaphi.y) * sin(thetaphi.x));
}
vec2 warp_vec3_to_uvsphere(vec3 p){
	p = normalize(p);
	return vec2((atan(p.z, p.x) / (PI*2.0)) + 0.5, acos(p.y) / PI);
}


void main(){
	FragColor = vec4(outUV, 0.0, 1.0);
}

#endif
