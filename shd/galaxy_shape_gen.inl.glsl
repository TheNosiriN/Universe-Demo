
float ellipse( in vec2 p, in vec2 r ){
    float k1 = length(p/r);
    return length(p)*(1.0-1.0/k1);
}



vec2 galaxy_rad_temp;
SpiralGalaxyProps prop;
float cutoff = 1.0;

float density = 0.0;
float density_low = 0.0;
float density_high = 0.0;



vec2 SpiralTempShape(vec2 p, float y, float rot, out float density, float dpeye){
	float l = length(p);//ellipse(p, vec2(0.01, 0.0));
    float a = atan(p.x, p.y) + mod(rot, PI*4.);

    float v = log(l)/(
		mix(prop.spiral_factor.x, prop.spiral_factor.y, smoothstep(0.0, 0.8, length(p)))
		* log(1.618)
	) - a/(PI/(prop.arm_count/2.0));
	float v1 = log(l)/(4.0*log(1.618)) - a/(PI/(prop.arm_count/2.));

	vec3 samp = vec3(l, abs(fract(v1*2.0)-0.5), y-rot);
    density = smoothstep(1.0, prop.clouds_scale, fbm(whitenoise, samp * 200.));


#ifdef COMPILE_GALAXY_NEAR_SHD
	float density_near = 0.0;
	float lfalloff = smoothstep(galaxy_rad_temp.x*0.2, galaxy_rad_temp.x*0.05, dpeye);
	if (lfalloff > 0.1){
		density_near = fbm(whitenoise, vec3(p.x,y,p.y) * 2000.0);
		density_high = density_near;
		density_near = density_near *0.4-0.2;
		density_near *= lfalloff;
	}
	density -= density_near;
#endif


    float t1 = abs(fract(v-density)-0.5);
	float t2 = abs(fract(v)-0.5);
	// return smoothstep(0.0, 1.0, t);

	float d = smoothstep(0.3, 0.7, t1);
	//d *= smoothstep(0.9, 0.5, l);
    return vec2(d, t2);//min(d + smoothstep(0.0, 1.0, d+0.1), 1.0);
}



float SpiralGalaxyCloudsVolume(vec3 p, float shape, float density){
	float height = galaxy_rad_temp.y * mix(0.0, 0.1, length(p/galaxy_rad_temp.xyx));
	float disk = max(
		abs(p.y) - height,
		length(p.xz)-galaxy_rad_temp.x//*0.95
	);

	// return (abs(p.y-shape*height)) + mix(height, -height*2.0, shape);//(abs(p.y-(shape*2.-1.)*height*0.5) - height) + mix(height, 0.0, shape) + 0.4;
	return (disk) + mix(height, 0.0, shape) + cutoff*1.5;
}

vec3 SpiralGalaxyColor(vec3 p, float dpeye, float density){
	vec3 dust_col = vec3(0.620,0.235,0.055);
	vec3 col1 = mix(-prop.blackhole_color*prop.dust_thickness, dust_col, density);
	return col1;
}

vec4 SpiralGalaxyClouds(vec3 eye, vec3 teye, vec3 dir, float steps, float jit){
	float mulfac = 1000.0f/galaxy_radius.x;
	eye *= mulfac;
	teye *= mulfac;
	galaxy_rad_temp = galaxy_radius * mulfac;


	vec3 p = vec3(0);
	vec3 q = vec3(0);
	float nd = 0.0;
	float depth = galaxy_rad_temp.x*0.0025;
	vec4 col = vec4(0);
	vec4 sum = vec4(0);
	float td = 0.0;

	float lcpxz = 0.0;
	float dpeye = 0.0;

	vec3 lightColor = vec3(0);

    for (float i=0.0; i<steps; i++){
        if (sum.a >= 0.90)break;

        p = eye + dir * depth;

		q = abs(p) - galaxy_rad_temp.xxx;
        if (max(q.x, max(q.y,q.z)) > 0.001){ break; }
		// if (length(p)-galaxy_rad_temp.x > 0.001){ break; }

		// density_low = fbm(whitenoise, vec3(p.xz/galaxy_rad_temp.y, prop.rot) * 5.0);
		// p.xz += (density_low*2.-1.) * galaxy_rad_temp.y * 0.5 * (length(p)/galaxy_rad_temp.x);

		lcpxz = length(p.xz);
		dpeye = length(eye-p);

		#ifdef COMPILE_GALAXY_NEAR_SHD
			float overstep = smoothstep(0.0, galaxy_rad_temp.x*0.3, length(teye-p));
			float overstep_near = smoothstep(galaxy_rad_temp.x*0.0025, galaxy_rad_temp.x*0.05, length(teye-p));
		#else
			const float overstep = 1.0;
			const float overstep_near = 1.0;
		#endif

		float falloff = 1.0-smoothstep(
			prop.spiral_falloff,
			prop.spiral_falloff*0.25,
			length(p.xz/galaxy_rad_temp.x)
		);


		vec2 shape = SpiralTempShape(p.xz/galaxy_rad_temp.x, p.y/galaxy_rad_temp.x - 1.0, prop.rot, density, dpeye);
		shape *= mix(0.0, 1.0, min(falloff*2.0, 1.0));
		density *= mix(0.1, 1.0, easeInExpo(falloff));
		density = density * prop.softness;
		shape.x = shape.x * prop.softness;

		float falloff_y = min(0.01/abs(p.y/galaxy_rad_temp.y), 1.0);
		p.y -= mix(0.0, (shape.y*2.0-1.0) * density, falloff*falloff_y) * galaxy_rad_temp.y*0.125;

		float nd = SpiralGalaxyCloudsVolume(p, shape.x, density);
		nd = max(nd, 0.08);

        if (nd < cutoff){
			vec3 cloud_col = SpiralGalaxyColor(p, dpeye, td);

			col.a = (1.0-td) * (cutoff-abs(nd));
			col.a *= 0.185;
			col.a *= mix(0.1, 1.0, overstep_near);
			col.xyz = cloud_col * shape.x * col.a;// * overstep;
			sum += col * (1. - sum.a);
        }
		td += 1.0/48.0;
		td = min(td, 1.0);


		float lnp = max(length(p/galaxy_rad_temp.xyx * vec3(1,1,1.5)), 0.001);
		float lDist = max(length(p/galaxy_rad_temp.xyx * vec3(1,1,0.1)), 0.100);

		sum.rgb += mix(
			(0.1*prop.blackhole_color)/lDist,
			smoothstep(1.0, 0.5, length(p/galaxy_rad_temp.xyx)) * (0.2*prop.halo_color)/lnp * mix(0.0, density*(1.0-shape.x)*3.0, falloff_y),
			pow(falloff,1.0/3.0)
		) * (1.0/steps);

		vec3 stars = prop.halo_color*vec3(0.620,0.235,0.055);//mix(prop.halo_color, vec3(0.620,0.235,0.055), 0.5);
		stars *= falloff_y * (1.0-shape.x) * smoothstep(1.0, 0.5, length(p/galaxy_rad_temp.xyx));
		sum.rgb += stars/steps * (1.0-density) * mix(0.0, 1.75, overstep_near);


	#ifdef COMPILE_GALAXY_NEAR_SHD
		nd += jit*1.0 * (1.0-overstep);// * 100.0f;
		depth += nd * mix(1.0, 0.5, abs(dir.y)) * mix(0.5, 1.5, overstep);
	#else
		depth += nd * 1.5 * mix(1.0, 0.5, abs(dir.y));
	#endif
	}



	bool inside;
    vec2 tfn;
    bool hit;
	float density_halo = 0.0;
    RaySphere(eye/galaxy_rad_temp.x, dir, 1.0, inside, hit, tfn);

	if (tfn.y < 0.0){
		hit = false;
	}
	tfn.x = max(tfn.x, -1.0);

	if (hit){
		if (inside){
			density_halo = abs(tfn.x-tfn.y);
		}else{
			density_halo = tfn.y-tfn.x;
		}
		density_halo = 1.0-density_halo/2.0;
		vec3 hole_color = mix(prop.blackhole_color, vec3(1,0.5,0.25), 0.375);
		float hole_falloff = smoothstep(1.0, 0.0, density_halo);
		float fade = easeOutExpo(smoothstep(0.0, galaxy_rad_temp.x*0.5, length(teye)));

	    density_halo = 0.1 / pow(max(density_halo, mix(0.1, 0.000001, fade )), 1.0/4.0);
		density_halo *= density_halo * hole_falloff;
		sum += vec4(hole_color, 1) * density_halo * smoothstep(0.3, 0.0, sum.a) * fade;
	}



	sum = min(sum, 1.0);
	sum.a = sqrt(sum.a);
	return sum;
}




#ifdef COMPILE_GALAXY_NEAR_SHD
	#define _galaxy_packed_props_buffer galaxy.packed
#else
	#define _galaxy_packed_props_buffer cur_galaxy_prop
#endif
void UnpackSpiralGalaxyProps(out SpiralGalaxyProps prop){
	prop.arm_count 				= _galaxy_packed_props_buffer[0][0];
	prop.spiral_factor.x 		= _galaxy_packed_props_buffer[0][1];
	prop.spiral_factor.y 		= _galaxy_packed_props_buffer[0][2];
	prop.spiral_falloff 		= _galaxy_packed_props_buffer[0][3];

	prop.blackhole_color.x 		= _galaxy_packed_props_buffer[1][0];
	prop.blackhole_color.y 		= _galaxy_packed_props_buffer[1][1];
	prop.blackhole_color.z 		= _galaxy_packed_props_buffer[1][2];
	prop.dust_thickness 		= _galaxy_packed_props_buffer[1][3];

	prop.halo_color.x 			= _galaxy_packed_props_buffer[2][0];
	prop.halo_color.y 			= _galaxy_packed_props_buffer[2][1];
	prop.halo_color.z 			= _galaxy_packed_props_buffer[2][2];
	prop.clouds_scale 			= _galaxy_packed_props_buffer[2][3];

	prop.rot 					= _galaxy_packed_props_buffer[3][0];
	prop.softness 				= _galaxy_packed_props_buffer[3][1];
}









// float ellipse( in vec2 p, in vec2 r ){
//     float k1 = length(p/r);
//     return length(p)*(1.0-1.0/k1);
// }
//
//
//
// vec2 galaxy_rad_temp;
// SpiralGalaxyProps prop;
// float cutoff = 0.1;
//
//
//
// vec2 SpiralTempShape(vec2 p, float y, float rot, out float density, float dpeye){
// 	float l = length(p);//ellipse(p, vec2(0.01, 0.0));
//     float a = atan(p.x, p.y) + mod(rot, PI*4.);
//
//     float v = log(l)/(
// 		mix(prop.spiral_factor.x, prop.spiral_factor.y, smoothstep(0.0, 0.8, length(vec3(p,y+1.0)))) * log(1.618)
// 	) - a/(PI/(prop.arm_count/2.0));
// 	float v1 = log(l)/(4.0*log(1.618)) - a/(PI/(prop.arm_count/2.));
//
// 	vec3 samp = vec3(l, abs(fract(v1*2.0)-0.5), y-rot);
//     density = smoothstep(1.0, prop.clouds_scale, fbm(whitenoise, samp * 200.));
//
//
// #ifdef COMPILE_GALAXY_NEAR_SHD
// 	float density_near = 0.0;
// 	float lfalloff = smoothstep(galaxy_rad_temp.x*0.2, galaxy_rad_temp.x*0.05, dpeye);
// 	if (lfalloff > 0.1){
// 		density_near = fbm(whitenoise, vec3(p.x,y,p.y) * 2000.0)*0.4-0.2;
// 		density_near *= lfalloff;
// 		density -= density_near;
// 	}
// #endif
//
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
// //Draw a spiral with twisting and superquadric section shape
// //c componenets must be integers and non simultenaousely equal to 0
// //c.x is the number of branches
// //c.y is the "bending" of the spiral (Actually it's c.y/c.x)
// float spiral(vec2 p, float y, vec2 c, float th, float rot){
// 	float r=length(p.xy);
// 	vec2 f=vec2(log(r),atan(p.y,p.x)-rot)*0.5/PI;//Log-polar coordinates
// 	float d=f.y*c.x-f.x*c.y;//apply rotation and scaling.
// 	d=fract(d);//"fold" d to [0,1] interval
// 	d=(d-0.5)*2.*PI*r/length(c);//(0.5-abs(d-0.5))*2.*PI*r/length(c);
//
// 	vec2 pp=vec2(d,y);
// 	float a=20.*1.*f.x;//twisting angle
// 	mat2 m=mat2(vec2(cos(a),-sin(a)), vec2(sin(a),cos(a)));
// 	pp=m*pp;//apply twist
// 	pp=abs(pp);
// 	float e=6.5+5.*1.;//superquadric param
// 	return 0.9*(pow(pow(pp.x,e)+pow(pp.y,e),1./e)-th*r);//distance have to be scaled down because this is just an approximation.
// }
//
//
//
// float SpiralGalaxyCloudsVolume(vec3 p, float shape, float density){
// 	float interp = smoothstep(0.2, 0.8, length(p.xz)/galaxy_rad_temp.x);
// 	float spiralness = mix(0.1, 0.5, interp);
//
// 	// spiralness = mod(camera.Time*0.1, 1.0) * 10.0;//fract(spiralness);
//
// 	float d = spiral(p.xz, p.y, vec2(prop.arm_count, spiralness), mix(0.1, 0.01, interp), 0.0);
//
// 	float noise = fbm(whitenoise, p*1.0);
// 	float befnoise = noise;
// 	noise -= 0.37;
// 	noise *= 0.7;
// 	// d = mix(d + noise*25.0, cutoff*2.0, noise);
// 	d = max(d+(1.0-befnoise)*25.0, noise);
//
// 	return d*0.8;
// }
//
// vec3 SpiralGalaxyColor(vec3 p, float dpeye, float density){
// 	vec3 dust_col = vec3(0.620,0.235,0.055);
// 	return mix(-prop.blackhole_color*prop.dust_thickness*10.0, dust_col, density) * 2.0;
// }
//
// vec4 SpiralGalaxyClouds(vec3 eye, vec3 teye, vec3 dir, float steps, float jit){
// 	float mulfac = 1000.0f/galaxy_radius.x;
// 	eye *= mulfac;
// 	teye *= mulfac;
// 	galaxy_rad_temp = galaxy_radius * mulfac;
//
//
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
// 	float density = 0.0;
// 	float density_low = 0.0;
//
// 	vec3 lightColor = vec3(0);
//
//     for (float i=0.0; i<steps; i++){
//         if (sum.a >= 0.90)break;
//
//         p = eye + dir * depth;
//
// 		q = abs(p) - galaxy_rad_temp.xxx;
//         if (max(q.x, max(q.y,q.z)) > 0.001){ break; }
// 		// if (length(p)-galaxy_rad_temp.x > 0.001){ break; }
//
// 		density_low = fbm(whitenoise, vec3(p.xz/galaxy_rad_temp.y, prop.rot) * 5.0);
// 		p.xz += (density_low *2.-1.) * galaxy_rad_temp.y * 0.5 * (length(p)/galaxy_rad_temp.x);
//
// 		lcpxz = length(p.xz);
// 		dpeye = length(eye-p);
//
// 		#ifdef COMPILE_GALAXY_NEAR_SHD
// 			float overstep = smoothstep(0.0, galaxy_rad_temp.x*0.3, length(teye-p));
// 			float overstep_near = smoothstep(galaxy_rad_temp.x*0.01, galaxy_rad_temp.x*0.10, length(teye-p));
// 		#else
// 			const float overstep = 1.0;
// 			const float overstep_near = 1.0;
// 		#endif
//
// 		float falloff = 1.0-smoothstep(
// 			prop.spiral_falloff,
// 			prop.spiral_falloff*0.25,
// 			length(p.xz/galaxy_rad_temp.x)
// 		);
//
//
// 		// vec2 shape = SpiralTempShape(p.xz/galaxy_rad_temp.x, p.y/galaxy_rad_temp.x - 1.0, prop.rot, density, dpeye);
// 		// shape *= mix(0.0, 1.0, min(falloff*2.0, 1.0));
// 		// density = density * prop.softness;
// 		// shape.x = shape.x * prop.softness;
// 		//
// 		// p.y -= mix(0.0, shape.y*2.0-1.0, falloff) * (density*0.5+0.5) * galaxy_rad_temp.y*0.05;
// 		float falloff_y = min(0.1/abs(p.y/galaxy_rad_temp.y), 1.0);
//
// 		float nd = SpiralGalaxyCloudsVolume(p, 0.0, 0.0);
// 		nd = max(nd, 0.08);
//
//         if (nd < cutoff){
// 			vec3 cloud_col = SpiralGalaxyColor(p, dpeye, td);
//
// 			col.a = (1.0-td) * (cutoff-(nd));
// 			col.a *= 0.185;
// 			col.xyz = cloud_col * col.a;// * shape.x * overstep;
// 			// col.a *= mix(0.1, 1.0, overstep_near);
// 			// col.xyz += td*0.01;
// 			sum += col * (1. - sum.a);
//         }
// 		td += 1.0/48.0;
// 		td = min(td, 1.0);
//
//
// 		vec3 stars = mix(prop.halo_color, vec3(0.620,0.235,0.055), 0.5);//mix(vec3(0.620,0.235,0.055), prop.halo_color, easeOutExpo(density_low)*density);
// 		stars *= falloff_y * smoothstep(1.0, 0.0, length(p/galaxy_rad_temp.x)) * 2.0;
// 		sum.rgb += stars/steps * jit;// * (1.0-density*density_low) * (shape.x*0.8+0.2) * mix(1.0, 5.0, overstep_near);
//
//
// 	#ifdef COMPILE_GALAXY_NEAR_SHD
// 		nd += jit * (1.0-overstep);
// 		depth += nd;// * mix(1.0, 0.5, abs(dir.y));// * mix(0.5, 1.5, overstep);
// 	#else
// 		depth += nd;// * mix(1.0, 0.5, abs(dir.y));// * 1.5;
// 	#endif
// 	}
//
//
//
// 	bool inside;
//     vec2 tfn;
//     bool hit;
// 	float density_halo = 0.0;
//     RaySphere(eye/galaxy_rad_temp.x, dir, 1.0, inside, hit, tfn);
//
// 	if (tfn.y < 0.0){
// 		hit = false;
// 	}
// 	tfn.x = max(tfn.x, -1.0);
//
// 	if (hit){
// 		if (inside){
// 			density_halo = abs(tfn.x-tfn.y);
// 		}else{
// 			density_halo = tfn.y-tfn.x;
// 		}
// 		density_halo = 1.0-density_halo/2.0;
// 		vec3 hole_color = mix(prop.blackhole_color, vec3(1,0.5,0.25), 0.375);
// 		float hole_falloff = smoothstep(1.0, 0.0, density_halo);
//
// 	    density_halo = 0.1 / pow(max(density_halo, 0.000001), 1.0/4.0);
// 		density_halo *= density_halo * hole_falloff;
// 		sum += vec4(hole_color, 1) * density_halo * (smoothstep(0.2, 0.0, sum.a));
// 	}
//
//
//
// 	sum = min(sum, 1.0);
// 	sum.a = sqrt(sum.a);
// 	return sum;
// }
//
//
//
//
// #ifdef COMPILE_GALAXY_NEAR_SHD
// 	#define _galaxy_packed_props_buffer galaxy.packed
// #else
// 	#define _galaxy_packed_props_buffer cur_galaxy_prop
// #endif
// void UnpackSpiralGalaxyProps(out SpiralGalaxyProps prop){
// 	prop.arm_count 				= _galaxy_packed_props_buffer[0][0];
// 	prop.spiral_factor.x 		= _galaxy_packed_props_buffer[0][1];
// 	prop.spiral_factor.y 		= _galaxy_packed_props_buffer[0][2];
// 	prop.spiral_falloff 		= _galaxy_packed_props_buffer[0][3];
//
// 	prop.blackhole_color.x 		= _galaxy_packed_props_buffer[1][0];
// 	prop.blackhole_color.y 		= _galaxy_packed_props_buffer[1][1];
// 	prop.blackhole_color.z 		= _galaxy_packed_props_buffer[1][2];
// 	prop.dust_thickness 		= _galaxy_packed_props_buffer[1][3];
//
// 	prop.halo_color.x 			= _galaxy_packed_props_buffer[2][0];
// 	prop.halo_color.y 			= _galaxy_packed_props_buffer[2][1];
// 	prop.halo_color.z 			= _galaxy_packed_props_buffer[2][2];
// 	prop.clouds_scale 			= _galaxy_packed_props_buffer[2][3];
//
// 	prop.rot 					= _galaxy_packed_props_buffer[3][0];
// 	prop.softness 				= _galaxy_packed_props_buffer[3][1];
// }
