#version 460


#include "common.inl.glsl"
#include "common_math.inl.glsl"
#include "universe_gen.inl.glsl"





#define LUMINANCE_PRESERVATION 0.75
#define EPSILON 1e-10

vec3 ColorTemperatureToRGB(float temperatureInKelvins){
	vec3 retColor = vec3(0);
    temperatureInKelvins = clamp(temperatureInKelvins, 1000.0, 40000.0) / 100.0;

    if (temperatureInKelvins <= 66.0){
        retColor.r = 1.0;
        retColor.g = saturate(0.39008157876901960784 * log(temperatureInKelvins) - 0.63184144378862745098);
    }else{
    	float t = temperatureInKelvins - 60.0;
        retColor.r = saturate(1.29293618606274509804 * pow(t, -0.1332047592));
        retColor.g = saturate(1.12989086089529411765 * pow(t, -0.0755148492));
    }

    if (temperatureInKelvins >= 66.0){
        retColor.b = 1.0;
    }else if(temperatureInKelvins <= 19.0){
        retColor.b = 0.0;
    }else{
        retColor.b = saturate(0.54320678911019607843 * log(temperatureInKelvins - 10.0) - 1.19625408914);
	}

    return retColor;
}






layout(location = 0) c_attrib vec2 vertpos;
layout(location = 1) c_attrib vec4 color;
layout(location = 2) c_attrib flat highp float luminousity;



layout(std140, binding = 0) uniform GlobalCamConstants {
	CameraConstants camera;
};

layout(std140, binding = 1) uniform CurrentSeedsCBuffer {
    CurrentSeedsConstants current_seeds;
};



#ifdef HX_VERTEX_SHADER

layout(location = 0) in vec3 pos; /// unused


layout(push_constant) uniform StarRenderpos {
	vec3 closest_renderpos;
};


layout(std140, binding = 2) uniform StarBlocks {
	StarBlock blocks[27];
};

layout(std430, binding = 3) buffer StarHeader {
	StarParticleBufferHeader header;
};

layout(std430, binding = 4) buffer StarPositions {
	StarParticle particles[];
};



float log10(float x){
	return log(x)/log(10);
}

void main(){
	StarParticle p = particles[ gl_InstanceIndex ];

	if (p.seed == 0u){
		return;
	}



	int block_index = int(gl_InstanceIndex) / (_STARS_BLOCK_STAR_COUNT*_STARS_BLOCK_STAR_COUNT*_STARS_BLOCK_STAR_COUNT);

	vec3 pos;
	if (current_seeds.star_seed == p.seed){
		pos = closest_renderpos;
	}else{
		pos = blocks[ block_index ].renderpos + p.renderpos;
	}

	p.renderpos = pos;	/// set renderpos to the appropriate one to be used on the CPU side
	float dist = length(pos);

	if (current_seeds.star_seed == p.seed){
		header.minDist = dist;
		header.closest_particle = p;
		// return;
	}else{
		if (camera.level == E_VIEW_LAYER_GALAXY && gl_VertexIndex == 0 && dist < 0.1){
			uint tdist = uint(dist * 0xFFFFFFFF);
			atomicMin(header.minDistInt, tdist);

			if (header.minDistInt == tdist){
				header.minDist = dist;
				header.closest_particle = p;
			}
		}
	}


	float scale = mix(0.5, 1.0, smoothstep(24.0*_STARS_SPACING_FACTOR*0.5, 24.0*_STARS_SPACING_FACTOR, dist));
	scale *= max(easeOutExpo(smoothstep(0.0, 0.1, dist)), 0.000001);


	color.a = unpackHalf2x16(p.seed).x;
	float temperature = color.a * _STARS_MAX_SURFACE_TEMPERATURE;
	color.xyz = ColorTemperatureToRGB(temperature);

	// luminousity = (pow(temperature/5778, 4.0)) * 3.828e+26;
	// highp float M = -2.5 * log10(luminousity/3.0128e28);
	// luminousity = exp( -((M/9.46073e+15) - 5 + 5*log10(dist)) *0.1);

	// luminousity = pow(temperature/5778, 4.0) * (1.0/dist);//exp(10.0/(dist+1.0));
	luminousity = 1.0;

	mat4 view_proj = camera.projection * camera.view;
	vec3 camUp = vec3(
		camera.view[0][1], camera.view[1][1], camera.view[2][1]
	);
	vec3 camRight = vec3(
		camera.view[0][0], camera.view[1][0], camera.view[2][0]
	);

	vertpos = triangle_strip_quad_vertices[ gl_VertexIndex ];
	vec3 rasterpos = pos + camRight*vertpos.x*scale + camUp*vertpos.y*scale;
	gl_Position = view_proj * vec4(rasterpos, 1);

}

#endif






// #ifdef HX_GEOMETRY_SHADER
//
// layout(points) in;
// layout(triangle_strip, max_vertices = 6) out;
//
// layout(location = 0) out vec2 vertpos;
//
//
// void main(){
// 	vec3 renderpos = gl_in[0].gl_Position.xyz;
// 	float seed = gl_in[0].gl_Position.w;
//
// 	mat4 view_proj = camera.projection * camera.view;
// 	vec3 camUp = vec3(
// 		camera.view[0][1], camera.view[1][1], camera.view[2][1]
// 	);
// 	vec3 camRight = vec3(
// 		camera.view[0][0], camera.view[1][0], camera.view[2][0]
// 	);
//
// 	float scale = 0.1;
//
// 	for (int i=0; i<6; ++i){
// 		vertpos = triangle_strip_quad_vertices[i];
// 		vec3 rasterpos = renderpos + camRight*vertpos.x*scale + camUp*vertpos.y*scale;
//
// 		gl_Position = view_proj * vec4(rasterpos, 1);
// 		EmitVertex();
// 	}
//
//     EndPrimitive();
// }
//
// #endif






#ifdef HX_FRAGMENT_SHADER

layout(location = 0) out vec4 FragColor;

void main(){
	float d = length(vertpos);
	d = 0.1 * inversesqrt(d) * smoothstep(1.0, 0.0, d);
	d *= d;

	FragColor = vec4(color.xyz*luminousity, max(luminousity,1.0)) * d;
}

#endif
