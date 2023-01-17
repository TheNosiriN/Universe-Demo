#version 460


#include "common.inl.glsl"
#include "common_math.inl.glsl"
#include "universe_gen.inl.glsl"



layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;


layout(binding = 0, rgba16f) uniform image2D outTex;
layout(binding = 1) uniform sampler2D whitenoise;
layout(binding = 2) uniform sampler2D bluenoise;


layout(std140, binding = 0) uniform GlobalCamConstants {
	CameraConstants camera;
};

layout(std140, binding = 1) uniform CurrentGalaxyPropsCBuffer {
    CurrentGalaxyProps galaxy;
};


layout(push_constant) uniform RenderPos {
	vec3 renderpos;
};






vec2 galaxy_radius;


#define COMPILE_GALAXY_NEAR_SHD
#include "galaxy_shape_gen.inl.glsl"






void main(){
	ivec2 Ci = ivec2(gl_GlobalInvocationID.xy);
	vec2 C = vec2(Ci);
	vec2 R = imageSize(outTex);
	vec2 uv = (C/R)*2.0-1.0;

    if (Ci.x>=R.x && Ci.y>=R.y){
		return;
	}

	vec3 eye = vec3(0) - renderpos;
	vec3 rasterpos = vec3(uv,1);
	vec3 tn = (camera.viewProjInv * vec4(rasterpos,1)).xyz;
	vec3 dir = normalize(tn);

	dir = mulvq(galaxy.rotationInv, dir);
	eye = mulvq(galaxy.rotationInv, eye);


	galaxy_radius.x = galaxy.radius;
	galaxy_radius.y = galaxy_radius.x * 0.5;


    bool inside;
    vec2 tfn;
    bool hit;
    // float dp = RayBox(eye, dir, vec2(0.0001, POSITIVE_INFINITY), galaxy_radius.xxx, hit, inside, tfn);
	float dp = RaySphere(eye, dir, galaxy_radius.x, inside, hit, tfn);



	if (hit){
        vec3 sp = eye;
	    if (inside){
	        sp = eye;
			// { acc += (tfn.y)*vec4(dir,1)*0.001; }		/// for debugging
	    }else{
	        sp = eye + dir * dp;
			// { acc += abs(tfn.x-tfn.y)*vec4(dir,1)*0.001; }	/// for debugging
	    }


		UnpackSpiralGalaxyProps(prop);


		/// blue noise jittering
	    vec2 noiseSize = vec2(1024);
	    ivec2 repCoord = ivec2(fract(C/noiseSize) * noiseSize);
	    float tang = mod(float(camera.FrameIndex), PI*2.);
	    vec2 repOffset = vec2(cos(tang), sin(tang)) * 10.;

		// float jit1 = texelFetch(bluenoise, repCoord, 0).x;
	    // vec4 jit4 = texelFetch(bluenoise,
	    //     ivec2(fract( (vec2(repCoord)+repOffset) /noiseSize) * noiseSize),
	    // 0).xyzw;
		//
	    // float jit = (jit1.x+jit4.x)/2.0;

		const float c_goldenRatioConjugate = 0.61803398875f;
	    float blueNoise = texelFetch(bluenoise, repCoord, 0).x;
	    float jit = fract(blueNoise + float(camera.FrameIndex % 64) * c_goldenRatioConjugate);
		//
		float steps = floor( mix(24.0, 1.0, min(length(renderpos)/(_GALAXY_SPACING_FACTOR*4.0), 1.0)) );

		vec4 clouds = SpiralGalaxyClouds(sp, eye, dir, steps, jit);


        if (length(clouds) > 0.01){
            vec4 acc = imageLoad(outTex, Ci);
            acc += clouds;
            imageStore(outTex, Ci, acc);
        }
	}

}
