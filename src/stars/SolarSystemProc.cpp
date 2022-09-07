
#include <algorithm>




float __solve_kepler(float e, float M, const int N){
    float E = M;
    for(int i=0; i<N; ++i){
        E = M + e*sin(E);
    }
    return E;
}

inline double __calculate_density(const double& mass, const double& radius){
	return mass / ((4.0/3.0) * Mathgl::pi<double>() * pow(radius, 3.0));
}

inline double __calculate_mass_from_density(const double& density, const double& radius){
	return density * ((4.0/3.0) * Mathgl::pi<double>() * pow(radius, 3.0));
}

inline double __calculate_radius_from_density(const double& density, const double& mass){
	return pow( (mass / density) / ((4.0/3.0) * Mathgl::pi<double>()), 1.0/3.0 );
}

/// see http://www.jgiesen.de/astro/stars/roche.htm for derivation
inline double __calculate_roche_limit(const double star_radius, const double& star_density, const double& planet_density){
	return star_radius * pow((16.0*star_density) / planet_density, 1.0/3.0);
}

inline double __calculate_hill_sphere(const double& star_mass, const double& planet_mass, const double& planet_orbital_radius){
	return planet_orbital_radius * pow(planet_mass / (3.0 * star_mass), 1.0/3.0);
}
///




vec3 GetKeplerOrbitPosition(float time, float speed, const SolarSystemOrbit& orbit, float& angle){
	vec2 angle_offset = unpackHalf2x16(orbit.angle_offset);

	float t = time;
    float e = orbit.eccentrity;
    float a = orbit.semi_major_axis;
    float n = speed;

    float M = n * t;
    float E = __solve_kepler(e, M, 16);

    angle = 2.0f * atan(sqrt( (1.0f+e)/(1.0f-e) ) * tan(E/2.0f));
    float dist = (a*(1.0f-e*e))/(1.0f + e*cos(angle));

	vec3 pos = vec3(cos(angle), 0, sin(angle)) * dist;

	vec2 tpos;
	tpos = rotation2D(angle_offset.x) * vec2(pos.x,pos.z);
	pos.x = tpos.x; pos.z = tpos.y;
	tpos = rotation2D(angle_offset.y) * vec2(pos.x,pos.y);
	pos.x = tpos.x; pos.y = tpos.y;

	return pos;
}


// void _GenerateOrbitParents(uint& seed, uint parentIndex, uint depth, uint& counter, SolarSystemProperties& prop, SolarSystemUpdates& upds){
// 	if (depth > 1){ return; }
//
// 	uint orbitCount = 0u;
//
// 	if (depth == 0u){
// 		parentIndex = 0xFFFFFFFF;
// 		orbitCount = uint(mix(1.0f, float(_SOLAR_SYSTEM_MAX_STAR_ORBITS), frandom(seed)));
// 		std::cout << "first orbit count: " << orbitCount << '\n';
// 	}else{
// 		orbitCount = uint(mix(
// 			1.5f, float(
// 				float(_SOLAR_SYSTEM_MAX_STAR_ORBITS)//std::max<uint>(_SOLAR_SYSTEM_MAX_ORBITS-prop.orbitCount, 0u)//std::min<uint>(_SOLAR_SYSTEM_MAX_STAR_ORBITS, std::max<uint>(_SOLAR_SYSTEM_MAX_ORBITS-prop.orbitCount, 0u))
// 			), (frandom(seed))
// 		));
// 	}
//
// 	prop.orbitCount += orbitCount;
// 	float radius = mix(0.01f, 1.0f, pow(frandom(seed), 2.0f));
//
// 	for (uint i=0; i<orbitCount; ++i){
// 		uint orbitIndex = counter;
// 		counter += 1u;
//
// 		upds.orbits[orbitIndex].parentIndex = parentIndex;
// 		prop.orbits[orbitIndex].flags |= (depth==0) ? _SOLAR_SYSTEM_ORBIT_DEPTH_PLANET : _SOLAR_SYSTEM_ORBIT_DEPTH_MOON;
//
// 		if (frandom(seed) < 0.8f){
// 			prop.orbits[orbitIndex].flags |= _SOLAR_SYSTEM_ORBIT_PLANET_FLAG;
// 			/// TODO: current problem: more planets then rings are spawning
// 			/// FIX: this part will add planets even at depth = 1, move it to outside the loop
// 			prop.planets[prop.planetCount].orbitIndex = orbitIndex;
// 			prop.planetCount += 1u;
// 			_GenerateOrbitParents(seed, orbitIndex, depth+1, counter, prop, upds);
// 		}else{
// 			prop.orbits[orbitIndex].flags |= _SOLAR_SYSTEM_ORBIT_ASTEROID_BELT_FLAG;
// 		}
//
// 		float rad = easeInExpo( float(i+1)/float(orbitCount) + (1.0f/orbitCount) * (frandom(seed)-0.5f) );
// 		rad = clamp(rad, 0.0f, 1.0f);
// 		rad *= radius;
//
//
//
// 		vec2 orbit_angle;
// 		orbit_angle.x = frandom(seed) * Mathgl::pi<float>() * 2.0f;
// 		orbit_angle.y = (pow(frandom(seed),2.0f)*10.0f-20.0f) * Mathgl::pi<float>()/180.0f;
//
// 		SolarSystemOrbit orbit{};
// 		orbit.angle_offset = Mathgl::packHalf2x16(orbit_angle);
// 		orbit.semi_major_axis = rad;
// 		orbit.eccentrity = 0.1f;
//
// 		prop.orbits[orbitIndex] = orbit;
//
// 	}
//
// }
//
//
// void _GenerateSolarSystemProperties(uint seed, SolarSystemProperties& prop, SolarSystemUpdates& upds){
// 	prop.orientation = frandom_quaternion<vec4>(seed);
// 	prop.planetCount = 0u;
// 	prop.orbitCount = 0u;
//
// 	uint counter = 0u;
// 	_GenerateOrbitParents(seed, 0, 0, counter, prop, upds);
// 	std::cout << " count: " << prop.orbitCount << '\n';
//
// 	// for (uint i=0; i<3; ++i){
// 	//
// 	// 	prop.orbitCount += orbitCount;
// 	//
// 	// 	for (uint j=0; j<orbitCount; ++j){
// 	// 		if (frandom(seed) < 0.8f){
// 	// 			prop.orbits[i].flags |= _SOLAR_SYSTEM_ORBIT_PLANET_FLAG;
// 	// 			// upds.orbits[i].parentIndex = i;
// 	// 			prop.planetCount += 1u;
// 	// 			_GenerateOrbitProps(seed, prop, upds);
// 	// 		}else{
// 	// 			prop.orbits[i].flags |= _SOLAR_SYSTEM_ORBIT_ASTEROID_BELT_FLAG;
// 	// 		}
// 	// 	}
// 	// }
//
// 	// for (uint i=0; i<prop.orbitCount; ++i){
// 	// 	// float rad = last_orbit_rad + mix(0.1f, float(i+1), frandom(seed));//float(i+1)/float(prop.orbitCount);
// 	// 	// last_orbit_rad = rad;
// 	//
// 	// 	float rad = easeInExpo( float(i+1)/float(prop.orbitCount) + (1.0f/prop.orbitCount) * (frandom(seed)-0.5f) );
// 	// 	rad = clamp(rad, 0.0f, 1.0f);
// 	// 	rad *= prop.radius;
// 	//
// 	// 	prop.orbits[i].semi_major_axis = rad;// + (1.0f/prop.orbitCount) * (frandom(seed)-0.5f);
// 	// 	prop.orbits[i].eccentrity = 0.1f;
// 	// 	prop.orbits[i].angle_offset.x = frandom(seed) * Mathgl::pi<float>() * 2.0f;
// 	// 	prop.orbits[i].angle_offset.y = (pow(frandom(seed),2.0f)*10.0f-20.0f) * Mathgl::pi<float>()/180.0f;//radians( mix(-25.0f, 25.0f, pow(frandom(seed),2.0f)) );
// 	//
// 	//
// 	// 	// Get number
// 	//
// 	//
// 	// 	if (frandom(seed) < 0.8f){
// 	// 		prop.orbits[i].flags |= _SOLAR_SYSTEM_ORBIT_PLANET_FLAG;
// 	// 		// upds.orbits[i].parentIndex = i;
// 	// 		prop.planetCount += 1u;
// 	// 	}else{
// 	// 		prop.orbits[i].flags |= _SOLAR_SYSTEM_ORBIT_ASTEROID_BELT_FLAG;
// 	// 		break;
// 	// 	}
// 	// }
// }







// void _GenerateMoonOrbitProperties(Star& star, uint seed, uint planetIndex){
//
// }
//
//
// void _GeneratePlanetProperties(Satr& star, uint seed, uint orbitIndex){
//
// }
//
//
// void _GeneratePlanetOrbitProperties(Star& star, uint seed){
// 	star.props.orientation = frandom_quaternion<vec4>(seed);
// 	// star.prop.planetCount = uint(frandom(seed) * _SOLAR_SYSTEM_MAX_PLANETS);
// 	// star.prop.beltCount = uint(easeInExpo(frandom(seed)) * _SOLAR_SYSTEM_MAX_ASTEROID_BELTS);
//
// 	/// this constant allows me to get 3.9505 (radius of solar system) from 1/2150 units (radius of sun)
// 	/// using (1/2150)^x * 50.0 = 3.9505 (these units are for the star orbit level, see CameraViewpoint struct for unit to real scale conversion)
// 	float radius = pow(star.props.radius, 0.3307841f) * 50.0f;
// 	star.props.orbitCount = uint(pow(frandom(seed), max(star.props.radius*50.0f, 1.0f)) * _SOLAR_SYSTEM_MAX_PLANETS);
//
// 	for (uint i=0; i<star.props.orbitCount; ++i){
// 		float rad = easeInExpo(
// 			float(i+1)/float(star.props.orbitCount) + (1.0f/star.props.orbitCount) * mix(0.0f, 0.5f, frandom(seed))
// 		);
// 		rad = clamp(rad, 0.0f, 1.0f);
//
//
// 		SolarSystemOrbit orbit{};
//
// 		vec2 angle_offset;
// 		angle_offset.x = frandom(seed) * Mathgl::pi<float>() * 2.0f;
// 		angle_offset.y = (pow(frandom(seed),2.0f)*10.0f-20.0f) * Mathgl::pi<float>()/180.0f;
//
// 		orbit.angle_offset = Mathgl::packHalf2x16(angle_offset);
// 		orbit.semi_major_axis = mix(star.props.radius*1.5f, radius, rad);
//
// 		if (frandom(seed) < 0.5f*mix(1.0, 0.0, (1.0-rad) * star.temperature)){
// 			orbit.flags |= _SOLAR_SYSTEM_ORBIT_PLANET_FLAG;
// 			star.props.planets[star.props.planetCount].orbitIndex = i;
// 			orbit.eccentrity = 0.1f;
//
// 			// _GenerateMoonOrbitProperties(star, seed, star.props.planetCount);
// 			_GeneratePlanetProperties(star, seed, i);
// 			star.props.planetCount += 1u;
//
// 		}else{
// 			orbit.flags |= _SOLAR_SYSTEM_ORBIT_ASTEROID_BELT_FLAG;
// 			orbit.eccentrity = 0.0f;
// 		}
//
// 		star.props.orbits[i] = orbit;
// 	}
// }







void _GenerateStarProperties(SolarSystemProc& proc, Star& star, uint seed){
	star.props.orientation = frandom_quaternion<vec4>(seed);

	vec2 unpack = Mathgl::unpackHalf2x16(star.seed);
	float radius_fac = frandom(seed);
	float closeness_fac = unpack.y;

	star.props.radius = mix(0.0001f, 1.0f, pow(radius_fac, mix(8.0f, 4.0f, star.temperature))); /// 1.0 = 2150 solar radii (1.0/2150 = radius of sun)
	star.radius = star.props.radius * CameraViewpoint::GetScale(E_VIEW_LAYER_STAR_ORBIT);	/// in meters

	star.temperature = unpack.x;
	// star.mass = mix(0.1f, 2150.0f, star.temperature + frandom(seed)*0.1) * (1.989e+30); 	/// in kg, 16.0 = 16 times larger than the sun

	constexpr double largest_density = 1.0;		/// kg/m^3
	constexpr double smallest_density = 5000.0;	/// kg/m^3
	star.density = mix(smallest_density, largest_density, easeOutExpo(frandom(seed)));

	star.mass = __calculate_mass_from_density(star.density, star.radius);

	// proc.planetCount = uint(pow(frandom(seed), mix(1.0f,20.0f,closeness_fac)) * _SOLAR_SYSTEM_MAX_PLANETS + 0.5f);
	proc.planetCount = uint(weight_sum<float, 3>(
		2 * (1.0-closeness_fac),
		1 * pow(frandom(seed), 2.0f)
	) * _SOLAR_SYSTEM_MAX_PLANETS);

	star.radius_of_influence = mix(25.0f, 200.0f, frandom(seed)*radius_fac*(1.0-closeness_fac)) * CameraViewpoint::GetScale(E_VIEW_LAYER_STAR_ORBIT);
	std::cout << "generated influence fac of: " << radius_fac*(1.0-closeness_fac) << '\n';
	std::cout << "generated radius of: " << star.radius_of_influence/CameraViewpoint::GetScale(E_VIEW_LAYER_STAR_ORBIT) << '\n';
}






// star: 10.4655888
// planet: 323.2742878

void _GenerateMoonOrbits(SolarSystemProc& proc, const Star& star, const Planet& planet){
	// std::cout << "using count: " << planet.orbitCount << '\n';

	uint seed = ConstructBasicSeed(ivec4(star.seed, planet.seed, 0, 0));
	seed = wang_hash(seed);

	proc.planets.reserve(planet.orbitCount);
	proc.orbits.reserve(planet.orbitCount);
	proc.temp_pos.reserve(planet.orbitCount);


	double largest_density = (16.0*planet.density)/3.0;
	double smallest_density = (16.0 * planet.density * pow(planet.radius,3.0)) / pow(planet.hill_sphere,3.0);

	double largest_rad = max(planet.radius*0.25, 1000.0);
	double smallest_rad = 1000.0;

	/// multiple of the planet's mass
	double largest_mass = 2.0e-4;
	double smallest_mass = 2.0e-21;

	for (uint i=0u; i<planet.orbitCount; ++i){
		Planet moon{};
		SolarSystemOrbit orbit{};

		moon.seed = seed;

		orbit.flags |= _SOLAR_SYSTEM_ORBIT_MOON_FLAG;

		float density_fac = frandom(seed);


		moon.density = mix(smallest_density, largest_density, density_fac);
		moon.mass = mix(smallest_mass, largest_mass, frandom(seed)) * planet.mass;

		moon.radius = __calculate_radius_from_density(moon.density, moon.mass);

		// std::cout << "r" << moon.radius << " -- ";


		double roche_limit = __calculate_roche_limit(planet.radius, planet.density, moon.density);
		double orbit_radius = mix(roche_limit, planet.hill_sphere, easeOutExpo(frandom(seed)));

		orbit.semi_major_axis = float(orbit_radius / CameraViewpoint::GetScale(E_VIEW_LAYER_PLANET_ORBIT));
		orbit.eccentrity = min(weight_sum<float, 2>(
			1 * orbit_radius/star.radius_of_influence,
			1 * pow(frandom(seed), 3.0)
		), 0.9f);

		vec2 angle_offset;
		angle_offset.x = frandom(seed) * Mathgl::pi<float>() * 2.0f;
		angle_offset.y = mix(-25.0f, 25.0f, easeOutInExpo(frandom(seed))) * Mathgl::pi<float>()/180.0f;
		orbit.angle_offset = Mathgl::packHalf2x16(angle_offset);

		moon.orbitIndex = (uint8_t)proc.orbits.push_back(orbit);
		proc.closest_planets[proc.planets.size()] = (uint8_t)proc.planets.push_back(moon);
		proc.temp_pos.emplace_back(uni_vec3(0), vec3(0), planet.orbitIndex);

	}
	// std::cout << '\n';
}




void _GeneratePlanetOrbits(SolarSystemProc& proc, const Star& star){
	if (proc.planetCount == 0u)return;

	uint seed = ConstructBasicSeed(ivec4(star.seed));
	seed = wang_hash(seed);


	proc.planets.reserve(proc.planetCount);
	proc.orbits.reserve(proc.planetCount);
	proc.temp_pos.reserve(proc.planetCount);


	constexpr double smallest_rad = 2255334.0; /// meters (0.354 earth rad)
	constexpr double largest_rad = 482385900; /// meters (6.9 jupiter rad)
	// constexpr double smallest_mass = 5.0e+22; /// kilograms (0.01 earth mass)
	// constexpr double largest_mass = 1.52e+29; /// kilograms (80 jupter mass)

	/// calculate the largest possible density so the ratio of (16*star)/planet > 3.0
	double largest_density = (16.0*star.density)/3.0;
	/// calculate the smallest density possible for the object to be stable within the radius_of_influence
	double smallest_density = (16.0 * star.density * pow(star.radius,3.0)) / pow(star.radius_of_influence,3.0);

	// std::cout << "star mass: " << star.mass << " -- radius: " << star.radius << " -- density: " << star.density << '\n';
	// std::cout << "ring count: " << proc.planetCount << '\n';
	// std::cout << "largest density: " << largest_density << '\n';
	// std::cout << "hill sphere: " << star.radius_of_influence << '\n';


	uint curnumorbits = proc.planetCount;


	for (uint i=0u; i<proc.planetCount; ++i){
		Planet planet{};
		SolarSystemOrbit orbit{};

		planet.seed = seed;

		float density_fac = frandom(seed);
		float radius_fac = frandom(seed);
		float orbit_rad_sample_fac = frandom(seed);

		orbit.flags |= _SOLAR_SYSTEM_ORBIT_PLANET_FLAG;

		planet.radius = mix(
			smallest_rad, largest_rad,
			pow(radius_fac, mix( 10.0f,1.0f,float(i)/proc.planetCount ))
		);

		planet.density = mix(smallest_density, largest_density, density_fac);

		planet.mass = __calculate_mass_from_density(planet.density, planet.radius);

		/// calulate roche limit to star
		double roche_limit = __calculate_roche_limit(star.radius, star.density, planet.density);

		/// generate the orbit radius (semi-major axis)
		float trad = easeInExpo(
			float(i+1)/float(proc.planetCount) + mix(0.0f, 0.25f/proc.planetCount, orbit_rad_sample_fac)
		);
		trad = clamp(trad, 0.0f, 1.0f);
		double orbit_radius = mix(roche_limit, star.radius_of_influence, trad);

		/// calulate the planet's hill sphere
		planet.hill_sphere = min(
			200.0 * (orbit_radius/star.radius_of_influence) * CameraViewpoint::GetScale(E_VIEW_LAYER_PLANET_ORBIT),
			__calculate_hill_sphere(star.mass, planet.mass, orbit_radius)
		);

		/// determine the semi-major axis relative to current scale
		orbit.semi_major_axis = float(orbit_radius / CameraViewpoint::GetScale(E_VIEW_LAYER_STAR_ORBIT));
		orbit.eccentrity = min(weight_sum<float, 2>(
			1 * easeInExpo(float(orbit_radius/star.radius_of_influence)),
			1 * pow(frandom(seed), 3.0f)
		), 0.9f);

		vec2 angle_offset;
		angle_offset.x = frandom(seed) * Mathgl::pi<float>() * 2.0f;
		angle_offset.y = mix(-25.0f, 25.0f, easeOutInExpo(frandom(seed))) * Mathgl::pi<float>()/180.0f;
		orbit.angle_offset = Mathgl::packHalf2x16(angle_offset);


		/// generate moon count
		uint num_orbits = (_SOLAR_SYSTEM_MAX_ORBITS-curnumorbits) / (proc.planetCount-i);
		planet.orbitCount = uint8_t(
			weight_sum<float, 12>(
				9 * pow(orbit_radius/star.radius_of_influence, 10.0f),
				2 * frandom(seed),
				1 * density_fac
			) * num_orbits
		);
		curnumorbits += planet.orbitCount;


		// std::cout << "for " << i << ":" << '\n';
		// std::cout << "	mass: " << planet.mass << '\n';
		// std::cout << "	radius: " << planet.radius << '\n';
		// std::cout << "	density: " << planet.density << '\n';
		// std::cout << "	roche limit: " << roche_limit << '\n';
		// std::cout << "	hill sphere: " << planet.hill_sphere << '\n';
		// std::cout << "	orbital radius: " << orbit_radius << '\n';
		// std::cout << "	semi-major axis: " << orbit.semi_major_axis << '\n';
		// std::cout << "	number of moons: " << size_t(planet.orbitCount) << " -- orbs: " << num_orbits << '\n';

		planet.orbitIndex = (uint8_t)proc.orbits.push_back(orbit);
		proc.closest_planets[proc.planets.size()] = (uint8_t)proc.planets.push_back(planet);
		proc.temp_pos.emplace_back();


	}


	/// generate moons
	for (uint i=0u; i<proc.planetCount; ++i){
		Planet& planet = proc.planets[i];
		if (planet.orbitCount == 0u){ continue; }
		planet.moonOrbitOffset = uint(proc.orbits.size());
		_GenerateMoonOrbits(proc, star, planet);
	}


	std::cout << "total orbits generated: " << proc.orbits.size() << '\n';
}








void SolarSystemProc::Init(Application& app){

}

void SolarSystemProc::Cleanup(Application& app){

}


void SolarSystemProc::Reset(Application& app, Star& star){
	/// clear all previous data
	temp_pos.clearFast();
	planets.clearFast();
	orbits.clearFast();

	/// set star
	SetStar(star);

	/// generate new data
	_GenerateStarProperties(*this, star, star.seed);
	_GeneratePlanetOrbits(*this, star);

	/// update buffers
	app.universeRenderer.UpdateSolarSystemPropsBuffer(app, &star.props);
	app.universeRenderer.UpdateSolarSystemOrbitsBuffer(app, orbits.data(), orbits.size());
}


void SolarSystemProc::UpdateStarBodies(Application& app, const uni_vec3& absolute_cam_pos){
	/// update renderpos
	star_renderpos = vec3(uni_vec3(0) - absolute_cam_pos);


	SolarSystemOrbitUpdate* mapped = (SolarSystemOrbitUpdate*)(
		app.hxg.MapStorageBuffer(
			app.universeRenderer.solar_system.orbit_updates, 0,
			app.hxg.GetConfig(app.universeRenderer.solar_system.orbit_updates).Size,
			HX_GRAPHICS_MEM_ACCESS_W
		)
	);

	for (uint i=0; i<orbits.size(); ++i){
		const SolarSystemOrbit& orbit = orbits[i];

		if (orbit.flags & _SOLAR_SYSTEM_ORBIT_ASTEROID_BELT_FLAG){
			continue;
		}

		float outangle;
		uni_vec3 absolute_pos = uni_vec3(GetKeplerOrbitPosition(float(app.Time*0.001), Mathgl::pi<float>(), orbit, outangle));
		absolute_pos = mulvq(star->props.orientation, absolute_pos);


		if (orbit.flags & _SOLAR_SYSTEM_ORBIT_PLANET_FLAG){
			uni_vec3 temp_renderpos = absolute_pos - absolute_cam_pos;
			temp_pos[i].absolute_pos = CameraViewpoint::CalculateLayeredPos(E_VIEW_LAYER_STAR_ORBIT, E_VIEW_LAYER_PLANET_ORBIT, temp_renderpos);
			temp_pos[i].renderpos = vec3(uni_vec3(0) - temp_pos[i].absolute_pos);
		}else
		if (orbit.flags & _SOLAR_SYSTEM_ORBIT_MOON_FLAG){
			uni_vec3 temp_renderpos = absolute_pos - temp_pos[temp_pos[i].parentIndex].absolute_pos;
			temp_pos[i].absolute_pos = CameraViewpoint::CalculateLayeredPos(E_VIEW_LAYER_PLANET_ORBIT, E_VIEW_LAYER_MOON_ORBIT, temp_renderpos);
			temp_pos[i].renderpos = vec3(uni_vec3(0) - temp_pos[i].absolute_pos);
		}

		mapped[i].angle = outangle;

	}

	app.hxg.UnmapStorageBuffer(app.universeRenderer.solar_system.orbit_updates);


}


void SolarSystemProc::UpdatePlanetBodies(Application& app){
	SortPlanets(app);

}



void SolarSystemProc::SortPlanets(Application& app){
	auto predicate = [&](const uint8_t& a, const uint8_t& b) -> bool {

		float ra = length(GetPlanetOrbitTempPos(a).renderpos) - float(GetPlanet(a).radius / CameraViewpoint::GetScale(
			GetPlanetOrbit(a).flags & _SOLAR_SYSTEM_ORBIT_PLANET_FLAG ? E_VIEW_LAYER_PLANET_ORBIT : E_VIEW_LAYER_MOON_ORBIT)
		);
		float rb = length(GetPlanetOrbitTempPos(b).renderpos) - float(GetPlanet(b).radius / CameraViewpoint::GetScale(
			GetPlanetOrbit(b).flags & _SOLAR_SYSTEM_ORBIT_PLANET_FLAG ? E_VIEW_LAYER_PLANET_ORBIT : E_VIEW_LAYER_MOON_ORBIT)
		);

		return ra < rb;
	};
	std::sort(closest_planets, closest_planets+planets.size(), predicate);


	// std::cout << '\n';
	// for (int i=0; i<planetCount; ++i){
	// 	std::cout << length(temp_pos[planets[ closest_planets[i] ].orbitIndex].renderpos) << " -- ";
	// }
	// std::cout << '\n';
}
