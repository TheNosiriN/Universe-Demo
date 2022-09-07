#version 460
precision highp float;


#include "common.inl.glsl"
#include "common_math.inl.glsl"
#include "universe_gen.inl.glsl"




layout(location = 0) c_attrib vec3 vpos;
layout(location = 1) c_attrib vec3 vnormal;
layout(location = 2) c_attrib vec3 rasterpos;
layout(location = 3) c_attrib flat vec3 vlightDir;
// layout(location = 2) c_attrib vec3 position;
// layout(location = 3) c_attrib flat int texID;




#ifdef HX_VERTEX_SHADER

layout(location = 0) in vec4 vertpos;
layout(location = 1) in vec3 normal;


layout(std140, binding = 0) uniform GlobalCamConstants {
	CameraConstants camera;
};

layout(push_constant) uniform Renderpos {
	vec4 renderpos;
	vec4 lightDir;
};



void main(){

	vpos = vertpos.xyz;
	vnormal = normal.xyz;
	vlightDir = lightDir.xyz;

	rasterpos = renderpos.xyz + vertpos.xyz;
	gl_Position = camera.projection * camera.view * vec4(rasterpos, 1);
}

#endif




#ifdef HX_FRAGMENT_SHADER

layout(location = 0) out vec4 FragColor;


float OrenNayar(in vec3 l, in vec3 n, in vec3 v, float r){
	float r2 = r*r;
    float a = 1.0 - 0.5*(r2/(r2+0.57));
    float b = 0.45*(r2/(r2+0.09));

    float nl = dot(n, l);
    float nv = dot(n, v);

    float ga = dot(v-n*nv,n-n*nl);

	return max(0.0,nl) * (a + b*max(0.0,ga) * sqrt((1.0-nv*nv)*(1.0-nl*nl)) / max(nl, nv));
}


void main(){

	vec3 dir = normalize(rasterpos);

	FragColor.xyz = vec3(1) * OrenNayar(vlightDir, vnormal, dir, 1.0);
	// FragColor.xyz = vec3(1) * max(0.0, dot(vnormal, vlightDir));
	FragColor.w = 1.0;
}

#endif
