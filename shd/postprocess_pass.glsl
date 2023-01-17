#version 460



#include "common.inl.glsl"
#include "common_math.inl.glsl"
#include "universe_gen.inl.glsl"



layout(location = 0) out vec4 FragColor;

layout(binding = 0) uniform sampler2D worldui_tex;
layout(binding = 1) uniform sampler2D depthless_tex;
layout(binding = 2) uniform sampler2D primitives_tex;
layout(binding = 3) uniform sampler2D taa_tex;

layout(std140, binding = 0) uniform GlobalCamConstants {
	CameraConstants camera;
};

layout(push_constant) uniform CameraSettingsCBuff {
	CameraSettings camera_settings;
};




#define ADDTEX(texname, posuv) tex = max(texture(texname, posuv),0.0); color = mix(tex+color, color, color.a);


vec4 makeComposite(vec2 uv){
	vec4 color = vec4(0);
	vec4 tex = vec4(0);

	// if (camera.level >= E_VIEW_LAYER_PLANET_ORBIT){
	// 	ADDTEX(planets_near_tex, uv);
	// }
    //
	// if (camera.level >= E_VIEW_LAYER_STAR_ORBIT){
	// 	// color += max(texture(planets_far_tex, uv), 0.0) * (1.0-color.a);
	// 	// color += max(texture(solar_system_rings_tex, uv), 0.0) * (1.0-color.a);
	// 	ADDTEX(planets_far_tex, uv);
	// 	ADDTEX(solar_system_rings_tex, uv);
	// }
    //
	// if (camera.level >= E_VIEW_LAYER_GALAXY){
	// 	// color += max(texture(stars_tex, uv), 0.0) * (1.0-color.a);
	// 	// color += max(texture(taa_tex, uv), 0.0) * (1.0-color.a);
	// 	ADDTEX(stars_tex, uv);
	// 	ADDTEX(taa_tex, uv);
	// }
    //
	// // float fadeout = mix(1.0, 0.1, easeOutExpo(max(camera.level-E_VIEW_LAYER_UNIVERSE, 0.0)/E_VIEW_LAYERS_COUNT));
	// // color += max(texture(galaxy_far_tex, uv), 0.0) * (1.0-color.a);
	// ADDTEX(galaxy_far_tex, uv);
	// color += texture(nebula_far_tex, uv) * (1.0-color.a);

    ADDTEX(primitives_tex, uv);
    ADDTEX(depthless_tex, uv);
    ADDTEX(taa_tex, uv);
    ADDTEX(worldui_tex, uv);
    // ADDTEX(raymarch_tex, uv);
    // color += max(texture(raymarch_tex, uv), 0.0) * (1.0-color.a);

	return clamp(color, 0.0, 1.0);
}





float linear_to_srgb(float channel) {
    if(channel <= 0.0031308){
        return 12.92 * channel;
    } else {
        return (1.0 + 0.055) * pow(channel, 1.0/2.4) - 0.055;
	}
}

vec3 linear_to_srgb(vec3 c){
    return vec3(linear_to_srgb(c.r), linear_to_srgb(c.g), linear_to_srgb(c.b));
}



vec3 ACESToneMap(vec3 x)
{
    float a = 2.51;
    float b = 0.03; float c = 2.43;
    float d = 0.59; float e = 0.14;
    return (x*(a*x+b))/(x*(c*x+d)+e);
}


vec3 texSample(int x,int y, vec2 uv)
{
	vec2 res = textureSize(taa_tex, 0);
	uv *= res.xy;
	uv = (uv + vec2(x, y)) / res.xy;
	return makeComposite(uv).xyz;
}
vec3 sharpen(vec3 col, vec2 uv, float strength){
	vec3 f = (
		texSample(-1,-1, uv) *  -1. +
		texSample( 1,-1, uv) *  -1. +
		texSample( 0, 0, uv) *   5. +
		texSample(-1, 1, uv) *  -1. +
		texSample( 1, 1, uv) *  -1.
	);
	return mix(col, f, strength);
}


void main(){
	vec2 uv = gl_FragCoord.xy/camera.frame_size;


	vec3 color = makeComposite(uv).xyz;

	// color = sharpen(color, uv, 0.1);

	color = 1.0 - exp(-camera_settings.exposure * color);
	color = ACESToneMap(color);
	color *= 0.3 + 0.8*pow(32.0*uv.x*uv.y*(1.0-uv.x)*(1.0-uv.y),0.2);
    // color = linear_to_srgb(color);


	FragColor += vec4(color, 1);
}
