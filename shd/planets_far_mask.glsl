#version 460
precision highp float;


#include "common.inl.glsl"
#include "common_math.inl.glsl"
#include "universe_gen.inl.glsl"




layout(location = 0) c_attrib vec2 outUV;
layout(location = 1) c_attrib vec3 vnormal;
layout(location = 2) c_attrib vec3 vposition;
layout(location = 3) c_attrib vec3 rasterpos;
layout(location = 4) c_attrib flat vec3 lightDir;
layout(location = 5) c_attrib flat int texID;




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
	PlanetFarProperties props;
};


void main(){
	outUV = uvs;
	vnormal = normalize(vertpos);
	vposition = vertpos;
	lightDir = props.lightDir.xyz;
	texID = props.surfaceTextureIndex;

	rasterpos = props.renderpos + vertpos*props.radius;//mulvq(star_props.orientation, vertpos*props.radius);
	gl_Position = camera.projection * camera.view * vec4(rasterpos, 1);
}

#endif




#ifdef HX_FRAGMENT_SHADER

layout(location = 0) out vec4 FragColor;
layout(binding = 0) uniform sampler2D diffuse_tex;
layout(binding = 1) uniform sampler2D normals_tex;

vec2 cart_to_polar_no_r(vec3 p){
	return vec2( atan(p.z,p.x), atan(length(p.xz), p.y) );
}
vec3 polar_to_cart_no_r(vec2 uv){
	return vec3(
		sin(uv.x) * cos(uv.y),
		sin(uv.x) * sin(uv.y),
		cos(uv.x)
	);
}


vec3 blendNormal(vec3 N, vec3 B){
	B = (B-N*dot(B,N));
    return normalize(N+B);
}

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
	// if (texID == 0){
	// 	FragColor = vec4(outUV, 0.0, 1.0);
	// 	return;
	// }
	// FragColor = vec4(vec3(perlin(warp_uvsphere_to_vec3(outUV)*10.0)), 1);

	vec3 dir = normalize(rasterpos);
	vec3 normal = texture(normals_tex, warp_vec3_to_uvsphere(vposition)).xyz;

	FragColor.xyz = vec3(1) * OrenNayar(lightDir, normal, dir, 1.0);
	// FragColor.xyz = vec3(1) * max(0.0, dot(vnormal, lightDir));
	FragColor.a = 1.0;
}

#endif
