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

layout(std140, binding = 2) uniform ParentProps {
	ClusterParentProps parent;
};





layout(location = 0) c_attrib vec3 outpos;
layout(location = 1) c_attrib vec3 rasterpos;
layout(location = 2) c_attrib flat vec3 centerdist;
layout(location = 3) c_attrib flat vec3 gridpos;



float temprad = 1.0*0.25;


#ifdef HX_VERTEX_SHADER



layout(location = 0) in vec3 pos; /// unused

layout(std430, binding = 0) buffer ClusterPositions {
    ClusterParticle particles[];
};






void main(){
	ClusterParticle cluster = particles[ gl_InstanceIndex ];

	outpos = vec3(cluster.renderpos) * 1.0;
	centerdist = cluster.distToCenter;
	gridpos = cluster.gridpos;

	vec3 vertpos = triangle_strip_cube_vertices[
		triangle_strip_cube_indices[ gl_VertexIndex ]
	] * temprad;

	rasterpos = outpos + mulvq(parent.rotation, vertpos);
	gl_Position = camera.projection * camera.view * vec4(rasterpos, 1);

}

#endif













#ifdef HX_FRAGMENT_SHADER

layout(location = 0) out vec4 FragColor;

layout(binding = 0) uniform sampler2D whitenoise;
layout(binding = 1) uniform sampler2D bluenoise;
layout(binding = 2) uniform sampler2D uniformMask;
layout(binding = 3) uniform sampler2D galaxyMap;



vec3 hash33(vec3 p3){
	p3 = fract(p3 * vec3(.1031,.11369,.13787));
    p3 += dot(p3, p3.yxz+19.19);
    return -1.0 + 2.0 * fract(vec3((p3.x + p3.y)*p3.z, (p3.x+p3.z)*p3.y, (p3.y+p3.z)*p3.x));
}
float perlin(vec3 p){
    vec3 pi = floor(p);
    vec3 pf = p - pi;

    vec3 w = pf * pf * (3.0 - 2.0 * pf);

    return 	mix(
        		mix(
                	mix(dot(pf - vec3(0, 0, 0), hash33(pi + vec3(0, 0, 0))),
                        dot(pf - vec3(1, 0, 0), hash33(pi + vec3(1, 0, 0))),
                       	w.x),
                	mix(dot(pf - vec3(0, 0, 1), hash33(pi + vec3(0, 0, 1))),
                        dot(pf - vec3(1, 0, 1), hash33(pi + vec3(1, 0, 1))),
                       	w.x),
                	w.z),
        		mix(
                    mix(dot(pf - vec3(0, 1, 0), hash33(pi + vec3(0, 1, 0))),
                        dot(pf - vec3(1, 1, 0), hash33(pi + vec3(1, 1, 0))),
                       	w.x),
                   	mix(dot(pf - vec3(0, 1, 1), hash33(pi + vec3(0, 1, 1))),
                        dot(pf - vec3(1, 1, 1), hash33(pi + vec3(1, 1, 1))),
                       	w.x),
                	w.z),
    			w.y);
}




const float nudge = 0.739513;	// size of perpendicular vector
float normalizer = 1.0 / sqrt(1.0 + nudge*nudge);	// pythagorean theorem on that perpendicular to maintain scale
float SpiralNoiseC(vec3 p)
{
    float n = 0.0;	// noise amount
    float iter = 1.0;
    for (int i = 0; i < 8; i++)
    {
        // add sin and cos scaled inverse with the frequency
        n += -abs(sin(p.y*iter) + cos(p.x*iter)) / iter;	// abs for a ridged look
        // rotate by adding perpendicular and scaling down
        p.xy += vec2(p.y, -p.x) * nudge;
        p.xy *= normalizer;
        // rotate on other axis
        p.xz += vec2(p.z, -p.x) * nudge;
        p.xz *= normalizer;
        // increase the frequency
        iter *= 1.733733;
    }
    return n;
}




float StarClusterVolume(vec3 p, float radius, out float density){
	float noise = fbm(whitenoise, p*20.0*0.01);
	// float noise = perlin(p*0.1)*0.5+0.5;
	// density = noise;
	// float d = length(p) - radius;// + noise*0.02;

	// density = max(0.0, density);
	return length(p) - max(radius - noise*radius*0.5, 0.0);
}


float StarClusterShape(vec3 p){
	return length(p) - 100.0;
}


vec4 StarClusterRayMarch(vec3 eye, vec3 dir, uint steps){
	uint i;
	float nd;
	float depth = 0.0;

	vec4 sum = vec4(0);
	vec4 col = sum;

	for (i=0u; i<steps; ++i){
		// if (sum.a >= 0.90)break;

		vec3 p = eye + dir * depth;

		// vec3 q = abs(p) - radius-0.01;
        // if (max(q.x, max(q.y,q.z)) > 0.001){ break; }

		float density;
		nd = abs(StarClusterVolume(p, 75.0, density));
		// nd = max(nd, 0.05);

		if (nd < 0.1){
			// col.a = 0.1*depth - nd;//density*density*density * 6.0;
			// col.xyz = vec3(0.1,0.3,0.8) * col.a;
			// sum += col * (1. - sum.a);
			// sum += 0.5;
			// sum += 0.9;
			break;
		}

		depth += nd;// * 0.01;
	}

	// float fade = (float(i)/float(steps));
	// fade = 1.0-fade;
	// fade *= fade;
	// sum.rgb += vec3(0.1,0.3,0.8) * fade;

	return sum;
}



// const vec3 voxelSize = vec3(0.1);
//
// vec3 worldToVoxel(vec3 i){ return floor(i/voxelSize); }
// vec3 voxelToWorld(vec3 i){ return i*voxelSize; }
//
// vec3 RenderStarsVoxels(vec3 ro, vec3 rd){
//     const int maxSteps = 64;
//
//     vec3 voxel = worldToVoxel(ro);
//     vec3 dstep = sign(rd);
//
//     vec3 nearestVoxel = voxel + vec3(rd.x > 0.0, rd.y > 0.0, rd.z > 0.0);
//     vec3 tMax = (voxelToWorld(nearestVoxel) - ro) / rd;
//     vec3 tDelta = voxelSize / abs(rd);
//     //vec3 tDelta = (length(rd)*voxelSize) / abs(rd);
//
//     vec3 mask;
//     vec3 color;
//
//     bool hit = false;
//     float hitT = 0.0;
//     for(int i=0; i<maxSteps; i++) {
//         float d = StarClusterShape(voxelToWorld(voxel));
//         if (d <= 0.0) {
//             hit = true;
//
//             hitT = length(mask * tMax);
//             vec3 hitNormal = -dstep * mask;
//
//             // color += raytrace(
//             //     ro/voxelSize, rd,
//             //     (voxelToWorld(voxel)+voxelSize*0.5)/voxelSize,
//             //     voxel
//             // );
//             // color += (hitNormal*0.5+0.5);//*0.5;
//
//             // break;
//         }
//
//         //mask = vec3(lessThanEqual(tMax.xyz, min(tMax.yzx, tMax.zxy)));
//         mask = step(tMax.xyz, tMax.yzx) * step(tMax.xyz, tMax.zxy);
//
//         voxel += dstep * mask;
//         tMax += tDelta * mask;
//
//     }
//
//
//
//
//     //return voxelToWorld(voxel);
// 	//return ro + hitT*rd;
//     return color;
// }






void main(){


	vec3 eye = -outpos;
	vec3 dir = normalize(rasterpos);

	dir = mulvq(parent.rotationInv, dir);
	eye = mulvq(parent.rotationInv, eye);

	eye *= 0.1;
	temprad *= 0.1;

    vec3 normal;
    bool inside;
    vec2 tfn;
    bool hit;
    float dp = RayBox(eye, dir, vec2(0.0001, 100000.0), vec3(temprad), hit, inside, tfn);

    vec4 acc = vec4(0);
	uint steps = 32u;

    if (hit){
		// acc = vec4(1,0,0,1);

		vec3 sp = eye;
	    if (inside){
	        sp = eye;
			{ acc += (tfn.y)*1.0*vec4(1,0,0,1); }		/// for debugging
	    }else{
	        sp = eye + dir * dp;
			{ acc += abs(tfn.x-tfn.y)*1.0*vec4(1,0,0,1); }	/// for debugging
	    }

		/// INSERT VOLUME CODE HERE
		acc += StarClusterRayMarch(sp/temprad * 100.0, dir, steps);
		// acc.xyz += RenderStarsVoxels(sp/temprad, dir);
		// acc += vec4(abs(gridpos), 1);
		// acc += vec4(dir, 0.5);

    }else{
        discard;
    }

	// acc = saturate(acc);
	// acc = vec4(vec3(length(centerdist)), 1.0);
	acc *= smoothstep(2.1, 2.0, length(centerdist));
	FragColor = saturate(acc);//+0.1;

}

#endif
