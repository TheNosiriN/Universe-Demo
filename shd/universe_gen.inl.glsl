
#ifndef __CONVERT_SHADER_HEADER
	#define c_out_param(x) out x
	#define alignas(x)
	#define static
#else
	#define c_out_param(x) x&
#endif




/// math constants
#define C_GRAVITY_CONSTANT (6.6743e-11)




/// define view layers
#define E_VIEW_LAYER_MULTIVERSE 0
#define E_VIEW_LAYER_UNIVERSE 1
#define E_VIEW_LAYER_GALAXY 2
#define E_VIEW_LAYER_STAR_ORBIT 3
#define E_VIEW_LAYER_PLANET_ORBIT 4
#define E_VIEW_LAYER_MOON_ORBIT 5
#define E_VIEW_LAYER_ASTEROID 6
#define E_VIEW_LAYER_GROUND 7
#define E_VIEW_LAYERS_COUNT 8



/// camera constants
struct CameraConstants {
	mat4 projection;
	mat4 view;
	mat4 viewProjInv;
	alignas(16) vec2 frame_size;
	alignas(16) vec3 cam_pos;
	uint FrameIndex;
	float Time;
	int level;
};

/// camera settings
struct CameraSettings {
	float exposure;
};

/// current seeds
struct CurrentSeedsConstants {
	uint galaxy_seed;
	uint cluster_seed;
	uint star_seed;
	uint planet_seed;
};







static float StaticParticleGridFadeOut(vec3 p, float radius){
	return smoothstep(1.0f, 0.5f, length(p/radius)-1.0f);
}





#define _GALAXY_SPACING_FACTOR 10
#define _GALAXY_PACK_SIZE 12

struct alignas(16) GalaxyParticle {
	vec3 renderpos;
	uint seed;

	vec4 rotation;
	vec4 rotationInv;
	float radius;
};

struct SpiralGalaxyProps {
	float arm_count;
	vec2 spiral_factor;
	float spiral_falloff;
	vec2 spiral_radius;
	float dust_thickness;
	vec3 blackhole_color;
	vec3 halo_color;
	float clouds_scale;
	float rot;
	float softness;
};

struct CurrentGalaxyProps {
	mat4 packed;
	vec4 rotation;
	vec4 rotationInv;
	float radius;
	uint seed;
};



#if defined(HX_VERTEX_SHADER) || defined(__CONVERT_SHADER_HEADER)

#ifdef __CONVERT_SHADER_HEADER
static void PackSpiralGalaxyProps(mat4& cur_galaxy_prop, const SpiralGalaxyProps& prop){
#else
void PackSpiralGalaxyProps(out mat4 cur_galaxy_prop, SpiralGalaxyProps prop){
#endif
	cur_galaxy_prop[0][0]		= prop.arm_count;
	cur_galaxy_prop[0][1]		= prop.spiral_factor.x;
	cur_galaxy_prop[0][2]		= prop.spiral_factor.y;
	cur_galaxy_prop[0][3]		= prop.spiral_falloff;

	cur_galaxy_prop[1][0]		= prop.blackhole_color.x;
	cur_galaxy_prop[1][1]		= prop.blackhole_color.y;
	cur_galaxy_prop[1][2]		= prop.blackhole_color.z;
	cur_galaxy_prop[1][3]		= prop.dust_thickness;

	cur_galaxy_prop[2][0]		= prop.halo_color.x;
	cur_galaxy_prop[2][1]		= prop.halo_color.y;
	cur_galaxy_prop[2][2]		= prop.halo_color.z;
	cur_galaxy_prop[2][3]		= prop.clouds_scale;

	cur_galaxy_prop[3][0]		= prop.rot;
	cur_galaxy_prop[3][1]		= prop.softness;
}
#endif


/// because of how different C++ and GLSL compilers are, and the nature of frandom
/// I have to explicitly define the order at which frandom is called
/// because C++'s optimizer will change the invoke order if called in place
static void GenerateSpiralGalaxyProps(uint seed, c_out_param(SpiralGalaxyProps) prop){
	prop.arm_count = floor( mix(1.0f, 3.0f, frandom(seed)) );
	prop.rot = frandom(seed) * 3.14159f * 2.0f;
	prop.spiral_factor.x = mix(1.5f, 4.0f, frandom(seed));
	prop.spiral_factor.y = prop.spiral_factor.x - mix(0.1f, 0.5f, frandom(seed));
	prop.spiral_falloff = mix(0.1f, 0.25f, frandom(seed));

	vec3 col;
	col.x = frandom(seed);
	col.y = frandom(seed)*0.5f;
	col.z = frandom(seed)*0.75f+0.25f;
	prop.blackhole_color = hsv_to_rgb(col);

	col.x = frandom(seed);
	col.y = frandom(seed)*0.4f+0.05f;
	col.z = frandom(seed)*0.25f+0.25f;
	prop.halo_color = hsv_to_rgb(col);

	prop.clouds_scale = mix(-1.0f, 0.5f, frandom(seed));
	prop.dust_thickness = mix(0.5f, 0.75f, frandom(seed));
	prop.softness = mix(0.1f, 0.75f, frandom(seed));
}







struct alignas(16) ClusterParticle {
	vec3 renderpos;
	uint seed;

	vec3 distToCenter;
	vec3 gridpos;
};

struct ClusterParentProps {
	vec4 rotation;
	vec4 rotationInv;
	vec2 bounds;
	vec3 blackhole_color;
	vec3 halo_color;
};






#define _STARS_SPACING_FACTOR 10
#define _STARS_BLOCK_STAR_COUNT 32
#define _STARS_MAX_SURFACE_TEMPERATURE 40000.0f

struct StarParticle {
	vec3 renderpos;
	uint seed;
};

struct StarParticleBufferHeader {
	StarParticle closest_particle;
	float minDist;
	uint minDistInt;
};

struct alignas(16) StarBlock {
	vec3 renderpos;
	float spawnFactor;
	vec3 gravityPoint;
	float temperature;
	uint seed;
};

struct alignas(16) StarFarProperties {
	vec3 renderpos;
	float radius;
	uint seed;
};






#define _SOLAR_SYSTEM_MAX_ORBITS 128
#define _SOLAR_SYSTEM_MAX_PLANETS 12
#define _SOLAR_SYSTEM_MAX_ASTEROID_BELTS 10

#define _SOLAR_SYSTEM_ORBIT_PLANET_FLAG (1<<1)
#define _SOLAR_SYSTEM_ORBIT_MOON_FLAG (1<<2)
#define _SOLAR_SYSTEM_ORBIT_ASTEROID_BELT_FLAG (1<<3)
#define _SOLAR_SYSTEM_ORBIT_DUST_BELT_FLAG (1<<4)

struct SolarSystemOrbit {
	float eccentrity;
	float semi_major_axis;
	uint angle_offset;
	uint flags;
};

struct SolarSystemOrbitUpdate {
	float angle;
};

struct alignas(16) SolarSystemProperties {
	vec4 orientation;
	float radius;
};






#define _PLANETS_MAX_TERRAIN_LEVELS 64
#define _TERRAIN_HEIGHT_TEXTURE_WIDTH 4096
#define _PLANET_TEXTURE_COUNT 4
#define _PLANET_TEXTURE_WIDTH 2048

struct alignas(16) PlanetFarProperties {
	vec4 lightDir;
	vec3 renderpos;
	float radius;
	uint seed;
	int surfaceTextureIndex;
};

struct PlanetTextureProps {
	uvec4 currentTexIndices;
};

struct PlanetIcoLODUniforms {
	int level;
};
