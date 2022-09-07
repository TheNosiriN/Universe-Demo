#include "main.h"
#include "utils.h"
#include "structs.h"

#include "StaticParticleGrid.cpp"
#include "stars/SolarSystemProc.cpp"
#include "planets/PlanetProc.cpp"





void GenerateGalaxyProps(Application&, CurrentGalaxyProps& prop, uint seed){
	SpiralGalaxyProps spiral_prop{};
	GenerateSpiralGalaxyProps(seed, spiral_prop);
	PackSpiralGalaxyProps(prop.packed, spiral_prop);
}

void Universe::CheckCurrentGalaxy(Application& app){
	GalaxyParticle& galaxy = galaxy_psys.GetClosestParticle();

	/// calculate viewer's position in galaxy
	float dist = length(galaxy.renderpos);
	// app.camera.view.eye.set_rel_pos(E_VIEW_LAYER_GALAXY, galaxy.renderpos);

	/// check if we're in the radius of the galaxy
	if (dist > galaxy.radius*4.0f){
		if (app.camera.view.eye.level == E_VIEW_LAYER_GALAXY){
			app.camera.view.eye.set_level(E_VIEW_LAYER_UNIVERSE);
		}
		currentGalaxy.seed = 0;
		return;
	}


	if (currentGalaxy.seed != galaxy.seed){
		app.camera.view.eye.set_level(E_VIEW_LAYER_GALAXY);
		app.camera.view.eye.recalculate_position(E_VIEW_LAYER_UNIVERSE, E_VIEW_LAYER_GALAXY, galaxy.renderpos);

		/// set the galaxy
		currentGalaxy.seed = galaxy.seed;
		currentGalaxy.props.radius = galaxy.radius;
		currentGalaxy.props.rotation = galaxy.rotation;
		currentGalaxy.props.rotationInv = galaxy.rotationInv;
		GenerateGalaxyProps(app, currentGalaxy.props, currentGalaxy.seed);
		stars_psys.SetGalaxy(currentGalaxy);

		/// update props buffer
		app.universeRenderer.UpdateCurrentGalaxyBuffer(app, &currentGalaxy.props);

		std::cout << "current galaxy: " << currentGalaxy.seed << ", dist: " << dist << '\n';
	}

}





void Universe::CheckCurrentCluster(Application& app){

}







void Universe::CheckCurrentStar(Application& app){
	StarParticle& star = stars_psys.grid.GetClosestParticle();
	float dist = stars_psys.grid.GetClosestDistance();
	// app.camera.view.eye.set_rel_pos(E_VIEW_LAYER_STAR_ORBIT, star.renderpos);

	/// check if we're in the range of the star
	if (dist > 0.1f){
		if (app.camera.view.eye.level >= E_VIEW_LAYER_STAR_ORBIT){
			app.camera.view.eye.set_level(E_VIEW_LAYER_GALAXY);
		}
		currentStar.seed = 0;
		return;
	}


	if (currentStar.seed != star.seed){
		app.camera.view.eye.set_level(E_VIEW_LAYER_STAR_ORBIT);
		app.camera.view.eye.recalculate_position(E_VIEW_LAYER_GALAXY, E_VIEW_LAYER_STAR_ORBIT, star.renderpos);

		/// set the star
		currentStar.seed = star.seed;
		solar_system_sys.Reset(app, currentStar);

		std::cout << "current star: " << currentStar.seed << ", dist: " << dist << '\n';
	}

}









void Universe::CheckCurrentPlanet(Application& app){
	const Planet& planet = solar_system_sys.GetClosestPlanet(0);
	const vec3& renderpos = solar_system_sys.GetClosestPlanetOrbitTempPos(0).renderpos;
	const uint& orbitflags = solar_system_sys.GetClosestPlanetOrbit(0).flags;
	float dist = length(renderpos);

	auto scale_type = orbitflags & _SOLAR_SYSTEM_ORBIT_PLANET_FLAG ? E_VIEW_LAYER_PLANET_ORBIT : E_VIEW_LAYER_MOON_ORBIT;
	float radius = float(planet.radius / CameraViewpoint::GetScale(scale_type));

	if (dist > radius*2.0f){
		if (app.camera.view.eye.level >= E_VIEW_LAYER_PLANET_ORBIT){
			app.camera.view.eye.set_level(E_VIEW_LAYER_STAR_ORBIT);
		}
		currentPlanet.seed = 0;
		return;
	}


	if (currentPlanet.seed != planet.seed){
		app.camera.view.eye.set_level(scale_type);
		app.camera.view.eye.recalculate_position(scale_type-1, scale_type, renderpos);

		/// set the planet
		currentPlanet = planet;
		planet_sys.Reset(app, currentPlanet, solar_system_sys.GetClosestPlanetOrbitTempPos(0).absolute_pos, orbitflags);

		std::cout << "current planet: " << currentPlanet.seed << ", dist: " << dist << ", rad x 2: " << (radius*2.) << '\n';
	}

}









bool Universe::IsCurrentGalaxy(Application& app, const Galaxy& obj){
	return obj.seed == currentGalaxy.seed;
}

bool Universe::IsCurrentCluster(Application& app, const Cluster& obj){
	return obj.seed == currentCluster.seed;
}

bool Universe::IsCurrentStar(Application& app, const Star& obj){
	return obj.seed == currentStar.seed;
}

bool Universe::IsCurrentPlanet(Application& app, const Planet& obj){
	return obj.seed == currentPlanet.seed;
}







void Universe::Init(Application& app){
	/// init galaxy particle system
	galaxy_psys.Init(app, uvec3(10));
	// cluster_psys.Init(app, 10);
	stars_psys.Init(app);
	solar_system_sys.Init(app);
	planet_sys.Init(app);


	app.camera.view.eye.set_level(E_VIEW_LAYER_UNIVERSE);
}


void Universe::Update(Application& app){
	// std::cout << size_t(app.camera.view.eye.level) << '\n';
	/// update galaxy particle system
	// ivec3 grid_cam_pos = Mathgl::floor(app.camera.view.eye / universe_cell_size);
	// vec3 fract_cam_pos = Mathgl::fract(app.camera.view.eye / universe_cell_size) * universe_cell_size;

	if (app.camera.view.eye.level >= E_VIEW_LAYER_UNIVERSE){
		// if (app.camera.view.eye.level == E_VIEW_LAYER_UNIVERSE){
		// 	app.camera.view.eye.set_rel_pos();
		// }
		galaxy_psys.ReconstructGrid(app, app.camera.view.eye.get(E_VIEW_LAYER_UNIVERSE));
		CheckCurrentGalaxy(app);
	}

	if (app.camera.view.eye.level >= E_VIEW_LAYER_GALAXY){
		if (app.camera.view.eye.level == E_VIEW_LAYER_GALAXY){
			app.camera.view.SetFocusPoint(galaxy_psys.GetClosestParticle().renderpos);
			app.camera.view.eye.set_rel_pos(galaxy_psys.GetClosestParticle().renderpos);
		}
		stars_psys.Update(app, app.camera.view.eye.get(E_VIEW_LAYER_GALAXY));
		CheckCurrentStar(app);
	}

	if (app.camera.view.eye.level >= E_VIEW_LAYER_STAR_ORBIT){
		if (app.camera.view.eye.level == E_VIEW_LAYER_STAR_ORBIT){
			app.camera.view.SetFocusPoint(stars_psys.grid.GetClosestParticle().renderpos);
			app.camera.view.eye.set_rel_pos(stars_psys.grid.GetClosestParticle().renderpos);
		}
		solar_system_sys.UpdateStarBodies(app, app.camera.view.eye.get(E_VIEW_LAYER_STAR_ORBIT));
		solar_system_sys.UpdatePlanetBodies(app);
		CheckCurrentPlanet(app);
	}

	if (app.camera.view.eye.level >= E_VIEW_LAYER_PLANET_ORBIT){
		if (app.camera.view.eye.level == planet_sys.scale_type){
			app.camera.view.SetFocusPoint(solar_system_sys.GetClosestPlanetOrbitTempPos(0).renderpos);
			app.camera.view.eye.set_rel_pos(solar_system_sys.GetClosestPlanetOrbitTempPos(0).renderpos);
		}
		planet_sys.Update(app, app.camera.view.eye.get(planet_sys.scale_type));
	}

}


void Universe::Cleanup(Application& app){
	galaxy_psys.Cleanup(app);
	cluster_psys.Cleanup(app);
	stars_psys.Cleanup(app);
	solar_system_sys.Cleanup(app);
	planet_sys.Cleanup(app);
}
