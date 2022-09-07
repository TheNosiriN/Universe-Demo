#version 460
// precision mediump float;



#include "common.inl.glsl"
#include "common_math.inl.glsl"
#include "universe_gen.inl.glsl"









layout(std140, binding = 0) uniform GlobalCamConstants {
	CameraConstants camera;
};

layout(std140, binding = 1) uniform CurrentSeedsCBuffer {
    CurrentSeedsConstants current_seeds;
};





layout(location = 0) c_attrib vec3 outpos;
layout(location = 1) c_attrib vec3 rasterpos;
layout(location = 2) c_attrib flat vec4 inv_rotquat;
layout(location = 3) c_attrib flat uint galaxy_seed;
layout(location = 4) c_attrib flat vec2 galaxy_radius;
layout(location = 5) c_attrib flat mat4 cur_galaxy_prop;







#ifdef HX_VERTEX_SHADER



layout(location = 0) in vec3 pos; /// unused

layout(std430, binding = 0) buffer GalaxyPositions {
    GalaxyParticle particles[];
};



void main(){
	GalaxyParticle galaxy = particles[ gl_InstanceIndex ];
	if (current_seeds.galaxy_seed == galaxy.seed){
		return;
	}

	galaxy_seed = galaxy.seed;
	galaxy_radius.x = galaxy.radius;
	galaxy_radius.y = galaxy_radius.x * 0.5;

	outpos = galaxy.renderpos;
	inv_rotquat = galaxy.rotationInv;


	SpiralGalaxyProps prop;
	GenerateSpiralGalaxyProps(galaxy_seed, prop);
	PackSpiralGalaxyProps(cur_galaxy_prop, prop);


	vec3 vertpos = triangle_strip_cube_vertices[
		triangle_strip_cube_indices[ gl_VertexIndex ]
	] * galaxy_radius.xxx;

	rasterpos = outpos + mulvq(galaxy.rotation, vertpos);

	gl_Position = camera.projection * camera.view * vec4(rasterpos, 1);
}

#endif












#ifdef HX_FRAGMENT_SHADER



layout(location = 0) out vec4 FragColor;

layout(binding = 0) uniform sampler2D whitenoise;
layout(binding = 1) uniform sampler2D bluenoise;




/// Global var
// SpiralGalaxyProps prop;

// float scale = 40.0/galaxy_radius.x;
// float inv_sqrt_scale = inversesqrt(scale);
// float inv_scale = 1.0/scale;
// float cutoff = 1.0;//galaxy_radius.y*0.1;

///////




#include "galaxy_shape_gen.inl.glsl"




// vec2 SpiralTempShape(vec2 p, float y, float rot, out float density, float dpeye){
// 	float l = length(p);//ellipse(p, vec2(0.15, 0.05));
//     float a = atan(p.x, p.y) + mod(rot, PI*4.);
//
//     float v = log(l)/(mix(prop.spiral_factor, prop.spiral_factor*100.0, saturate(l))*log(1.618)) - a/(PI/(prop.arm_count/2.));
// 	float v1 = log(l)/(4.0*log(1.618)) - a/PI;
//
// 	vec3 samp = vec3(l, abs(fract(v*2.0)-0.5), y-rot);
//     density = fbm(whitenoise, samp * 600.) * 0.5;
//
//     float t1 = abs(fract(v-density)-0.5);
// 	float t2 = abs(fract(v)-0.5);
// 	// return smoothstep(0.0, 1.0, t);
//
// 	float d = smoothstep(0.3, 0.7, t1);
// 	d *= smoothstep(0.9, 0.5, l);
//     return vec2(d, t2);//min(d + smoothstep(0.0, 1.0, d+0.1), 1.0);
// }
//
//
//
// float SpiralGalaxyCloudsVolume(vec3 p, float shape){
// 	float height = galaxy_radius.y*0.025;
// 	float disk = max(
// 		abs(p.y) - height,
// 		length(p.xz)-galaxy_radius.x*0.9
// 	);
//
// 	return disk + mix(height, 0.0, shape) + 0.4;//galaxy_radius.y*0.005;//cutoff*2.0;
// }

// vec3 SpiralGalaxyColor(vec3 p, float dpeye, float density){
//
// 	vec3 dust_col = vec3(0.620,0.235,0.055);
// 	vec3 dust = mix(-prop.blackhole_color, dust_col, density);
//
// 	return dust;
// }
//
//
//
// float hash13(vec3 p3)
// {
// 	p3  = fract(p3 * .1031);
//     p3 += dot(p3, p3.zyx + 31.32);
//     return fract((p3.x + p3.y) * p3.z);
// }
//
//
// vec4 SpiralGalaxyClouds(vec3 eye, vec3 teye, vec3 dir, float steps, float jit){
// 	vec3 p = vec3(0);
// 	vec3 q = vec3(0);
// 	float nd = 0.0;
// 	float depth = 0.0;
// 	vec4 col = vec4(0);
// 	vec4 sum = vec4(0);
// 	float td = 0.0;
//
// 	float lcpxz = 0.0;
// 	float dpeye = 0.0;
//
// 	vec3 lightColor = vec3(0);
//
//     for (float i=0.0; i<steps; i++){
//         if (sum.a >= 0.90)break;
//
//         p = eye + dir * depth;
//
// 		q = abs(p) - galaxy_radius.xyx;
//         if (max(q.x, max(q.y,q.z)) > 0.001){ break; }
//
//
// 		lcpxz = length(p.xz);
// 		dpeye = length(eye-p);
//
// 		float falloff = 1.0-smoothstep(
// 			prop.spiral_falloff,
// 			prop.spiral_falloff*0.25,
// 			length(p.xz/galaxy_radius.x)
// 		);
//
// 		float density;
// 		vec2 shape = SpiralTempShape(p.xz/galaxy_radius.x, p.y/galaxy_radius.y - 1.0, prop.rot, density, dpeye);
// 		shape *= mix(0.2, 1.0, min(falloff*2.0, 1.0));
//
// 		p.y -= mix(0.0, shape.y*2.0-1.0, falloff) * galaxy_radius.y*0.05;
// 		float falloff_y = min(0.01/abs(p.y/galaxy_radius.y), 1.0);
//
// 		float nd = SpiralGalaxyCloudsVolume(p, shape.x);
// 		nd = max(nd, 0.08);
//
//         if (nd < cutoff){
// 			vec3 cloud_col = SpiralGalaxyColor(p, dpeye, td);
//
// 			col.a = (1.0-td) * (cutoff-(nd));
// 			col.a *= 0.185;
// 			col.xyz = cloud_col * col.a;
// 			sum += col * (1. - sum.a);
//         }
// 		td += 1.0/48.0;
//
//
//
// 		float lnp = max(length(p/galaxy_radius.xyx * vec3(1,1,1.5)), 0.001);
// 		float lDist = max(length(p/galaxy_radius.x * vec3(1,1,0.5)), 0.100);
// 		sum.rgb += 0.001 * prop.blackhole_color/length(p/galaxy_radius.x);
//
//
// 		sum.rgb += mix(
// 			(0.1*prop.blackhole_color)/lDist,
// 			(0.5*prop.halo_color)/max(lnp + 0.01, 0.1) * mix(0.5, density*shape.x*3.0, falloff_y),
// 			falloff
// 		) * (1.0/steps);// + 0.1*noise3d(whitenoise, p*10.0)*shape.x;//mix(1.0, 2.0, abs(dir.y));
//
// 		vec3 stars = (1.0-prop.blackhole_color) * 2.0;
// 		stars *= falloff_y * (1.0-shape.x) * smoothstep(1.0, 0.0, length(p/galaxy_radius.x));
// 		sum.rgb += stars/steps * (density+0.1);
//
//
// 		float overstep = mix(
// 			0.5, 1.5, smoothstep(0.0, galaxy_radius.x*0.25, length(teye-p))
// 		);
//
// 		nd += jit * density;//((abs(p.y)));//*10.0;
// 		depth += nd * overstep * mix(1.0, 0.5, abs(dir.y));
// 	}
//
//
// 	sum.a = sqrt(sum.a);
// 	sum.rgb = min(sum.rgb, 1.0);
// 	return sum;
// }




// vec3 SpiralGalaxyColor(vec3 p, float dpeye, float density){
// 	vec3 dust_col = vec3(0.620,0.235,0.055);
// 	return mix(-prop.blackhole_color, dust_col, density);
// }
//
// vec4 SpiralGalaxyClouds(vec3 eye, vec3 teye, vec3 dir, float steps, float jit){
// 	vec3 p = vec3(0);
// 	vec3 q = vec3(0);
// 	float nd = 0.0;
// 	float depth = 0.0;
// 	vec4 col = vec4(0);
// 	vec4 sum = vec4(0);
// 	float td = 0.0;
//
// 	float lcpxz = 0.0;
// 	float dpeye = 0.0;
//
// 	vec3 lightColor = vec3(0);
//
//     for (float i=0.0; i<steps; i++){
//         if (sum.a >= 0.90)break;
//
//         p = eye + dir * depth;
//
// 		q = abs(p) - galaxy_radius.xyx;
//         if (max(q.x, max(q.y,q.z)) > 0.001){ break; }
//
//
// 		lcpxz = length(p.xz);
// 		dpeye = length(eye-p);
//
// 		float falloff = 1.0-smoothstep(
// 			prop.spiral_falloff,
// 			prop.spiral_falloff*0.25,
// 			length(p.xz/galaxy_radius.x)
// 		);
//
// 		float density;
// 		vec2 shape = SpiralTempShape(p.xz/galaxy_radius.x, p.y/galaxy_radius.y - 1.0, prop.rot, density, dpeye);
// 		shape *= mix(0.2, 1.0, min(falloff*2.0, 1.0));
//
// 		p.y -= mix(0.0, shape.y*2.0-1.0, falloff) * galaxy_radius.y*0.05;
// 		float falloff_y = min(0.01/abs(p.y/galaxy_radius.y), 1.0);
//
// 		float nd = SpiralGalaxyCloudsVolume(p, shape.x);
// 		nd = max(nd, 0.08);
//
//         if (nd < cutoff){
// 			vec3 cloud_col = SpiralGalaxyColor(p, dpeye, td);
//
// 			col.a = (1.0-td) * (cutoff-(nd));
// 			col.a *= 0.185;
// 			col.xyz = cloud_col * col.a;
// 			sum += col * (1. - sum.a);
//         }
// 		td += 1.0/48.0;
// 		td = min(td, 1.0);
//
//
// 		// float overstep = smoothstep(0.0, galaxy_radius.x*0.25, length(teye-p));
// 		// float overstep2 = smoothstep(0.0, galaxy_radius.x*0.5, length(teye-p));
// 		//
// 		// float lnp = max(length(p/galaxy_radius.xyx * vec3(1,1,1.5)), 0.001);
// 		// float lDist = max(length(p/galaxy_radius.x * vec3(1,1,0.5)), 0.100);
// 		// sum.rgb += 0.001 * prop.blackhole_color/length(p/galaxy_radius.x) * overstep2;
//
// 		// lightColor = mix(
// 		// 	(0.1*prop.blackhole_color)/lDist,
// 		// 	(0.5*prop.halo_color)/max(lnp + 0.01, 0.1) * mix(0.5, density*shape.x*3.0, falloff_y),
// 		// 	falloff
// 		// ) * (1.0/steps) * overstep2;// + 0.1*noise3d(whitenoise, p*10.0)*shape.x;//mix(1.0, 2.0, abs(dir.y));
//
// 		vec3 stars = prop.halo_color;
// 		stars *= falloff_y * (1.0-shape.x) * smoothstep(1.0, 0.0, length(p/galaxy_radius.x));
// 		sum.rgb += stars/steps * 8.0 * (1.0-density) * (shape.x*0.8+0.2);
//
//
// 		depth += nd * 1.5 * mix(1.0, 0.5, abs(dir.y));
// 	}
//
//
//
// 	bool inside;
//     vec2 tfn;
//     bool hit;
// 	float density_halo = 0.0;
//     RaySphere(eye/galaxy_radius.x, dir, 1.0, inside, hit, tfn);
//
// 	if (hit){
// 		if (inside){
// 			density_halo = abs(tfn.x-tfn.y);
// 		}else{
// 			density_halo = tfn.y-tfn.x;
// 		}
// 		density_halo = 1.0-density_halo/2.0;
// 		vec3 hole_color = mix(prop.blackhole_color, vec3(1,0.5,0.25), saturate(density_halo));
// 		float hole_falloff = smoothstep(1.0, 0.5, density_halo);
//
// 	    density_halo = 0.06 / pow(max(density_halo, 0.00001), 1.0/3.0);
// 		sum.rgb += density_halo * hole_color * hole_falloff * (1.0-sum.a);
// 	}
//
//
//
// 	sum = min(sum, 1.0);
// 	sum.a = sqrt(sum.a);
// 	return sum;
// }










// void UnpackSpiralGalaxyProps(out SpiralGalaxyProps prop){
// 	prop.arm_count 				= cur_galaxy_prop[0][0];
// 	prop.spiral_factor 			= cur_galaxy_prop[0][1];
// 	prop.spiral_falloff 		= cur_galaxy_prop[0][2];
// 	prop.clouds_scale 			= cur_galaxy_prop[0][3];
//
// 	prop.blackhole_color.x 		= cur_galaxy_prop[1][0];
// 	prop.blackhole_color.y 		= cur_galaxy_prop[1][1];
// 	prop.blackhole_color.z 		= cur_galaxy_prop[1][2];
// 	prop.blackhole_size 		= cur_galaxy_prop[1][3];
//
// 	prop.halo_color.x 			= cur_galaxy_prop[2][0];
// 	prop.halo_color.y 			= cur_galaxy_prop[2][1];
// 	prop.halo_color.z 			= cur_galaxy_prop[2][2];
// 	prop.rot 					= cur_galaxy_prop[2][3];
// }





void main(){
	if (current_seeds.galaxy_seed == galaxy_seed){
		return;
	}

	float fadeout = 1.0;//smoothstep(_GALAXY_SPACING_FACTOR*4.5, _GALAXY_SPACING_FACTOR*3.0, length(outpos));
	if (fadeout < 0.1){
		discard;
	}

	float steps = floor( mix(24.0, 1.0, min(length(outpos)/(_GALAXY_SPACING_FACTOR*4.0), 1.0)) );



	vec3 eye = vec3(0) - outpos;
	vec3 tn = rasterpos;
	vec3 dir = normalize(tn);

	dir = mulvq(inv_rotquat, dir);
	eye = mulvq(inv_rotquat, eye);




    bool inside;
    vec2 tfn;
    bool hit;
    // float dp = RayBox(eye, dir, vec2(0.0001, POSITIVE_INFINITY), galaxy_radius.xxx, hit, inside, tfn);
	float dp = RaySphere(eye, dir, galaxy_radius.x, inside, hit, tfn);

    vec4 acc = vec4(0);


    if (hit){
		vec3 sp = eye;
	    if (inside){
	        sp = eye;
			// { acc += (tfn.y)*vec4(dir,0)*0.001; }		/// for debugging
	    }else{
	        sp = eye + dir * dp;
			// { acc += abs(tfn.x-tfn.y)*vec4(dir,0)*0.001; }	/// for debugging
	    }

		UnpackSpiralGalaxyProps(prop);


		vec4 clouds = SpiralGalaxyClouds(sp, eye, dir, steps, 0.0);
		acc += clouds;

    }else{
        discard;
    }


	acc *= fadeout;
	FragColor = acc;
}

#endif
