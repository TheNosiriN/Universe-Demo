#version 460


#include "common.inl.glsl"
#include "common_math.inl.glsl"
#include "universe_gen.inl.glsl"



layout(std140, binding = 0) uniform GlobalCamConstants {
	CameraConstants camera;
};



layout(location = 0) out vec4 FragColor;

layout(binding = 0) uniform sampler2D previous_frame;
layout(binding = 3) uniform sampler2D raymarch_tex;




#define ADDTEX(texname, posuv) tex = max(texture(texname, posuv), 0.0); color = mix(tex+color, color, color.a);

vec4 makeComposite(vec2 uv){
    vec4 color = vec4(0);
	vec4 tex = vec4(0);

	// if (camera.level >= E_VIEW_LAYER_GALAXY){
	// 	color += texture(nebula_tex, uv) * (1.0-color.a);
	// }

	// float fadeout = mix(1.0, 0.1, easeOutExpo(max(camera.level-E_VIEW_LAYER_UNIVERSE, 0.0)/E_VIEW_LAYERS_COUNT));
	// color += texture(galaxy_tex, uv) * (1.0-color.a);

    ADDTEX(raymarch_tex, uv);

	return clamp(color, 0.0, 1.0);
}




void main(){
	vec2 uv = gl_FragCoord.xy/camera.frame_size;


	vec4 center = makeComposite(uv);
    vec4 minColor = center;
    vec4 maxColor = center;
    for (int iy = -1; iy <= 1; ++iy){
        for (int ix = -1; ix <= 1; ++ix){
            if (ix == 0 && iy == 0)
                continue;

            vec2 offsetUV = ((gl_FragCoord.xy + vec2(ix, iy)) / camera.frame_size.xy);
            vec4 color = makeComposite(offsetUV);
            minColor = min(minColor, color);
            maxColor = max(maxColor, color);
        }
    }

    // get last frame's pixel and clamp it to the neighborhood of this frame
    vec4 old = texture(previous_frame, uv);
    old = max(minColor, old);
    old = min(maxColor, old);


    // interpolate from the clamped old color to the new color.
    // Reject all history when the mouse moves.
    vec4 color = mix(old, center, 0.1);


	FragColor = color;
}




	//
	// vec3 center = max(texture(galaxy_mask_tex, uv).rgb, 0.0);
    // vec3 minColor = center;
    // vec3 maxColor = center;
    // for (int iy = -1; iy <= 1; ++iy){
    //     for (int ix = -1; ix <= 1; ++ix){
    //         if (ix == 0 && iy == 0)
    //             continue;
	//
    //         vec2 offsetUV = ((gl_FragCoord.xy + vec2(ix, iy)) / res.xy);
    //         vec3 color = max(texture(galaxy_mask_tex, offsetUV).rgb, 0.0);
    //         minColor = min(minColor, color);
    //         maxColor = max(maxColor, color);
    //     }
    // }
	//
    // // get last frame's pixel and clamp it to the neighborhood of this frame
    // vec3 old = texture(previous_frame, uv).rgb;
    // old = max(minColor, old);
    // old = min(maxColor, old);
	//
    // // interpolate from the clamped old color to the new color.
    // // Reject all history when the mouse moves.
    // // float lerpAmount = (iMouse.z > 0.0 || uv.y > 0.5f) ? 1.0f : 0.1f;
    // vec3 color = mix(old, center, 0.4);
	//
