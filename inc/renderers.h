

using namespace StaticUtils;

#define __CONVERT_SHADER_HEADER
#include "../shd/universe_gen.inl.glsl"




struct UniverseObjectComponent {
	// uni_ivec3 gridpos;
	// uni_vec3 additionalpos;
	uni_vec3 absolute_pos;

	// static void Update(UniverseObjectComponent&, const uni_vec3&);
};









struct Galaxy {
	uint32_t seed;
	CurrentGalaxyProps props;
};





struct Cluster {
	uint32_t seed;
	ClusterParentProps parent;
};

struct ClusterProps {

};





struct Star  {
	uint32_t seed = 0u;
	float temperature;
	double radius;
	double mass;
	double density;
	double radius_of_influence;

	SolarSystemProperties props;
};





struct Planet {
	uint32_t seed = 0u;
	double mass;
	double radius;
	double density;
	double hill_sphere;

	uint moonOrbitOffset;
	uint8_t orbitIndex;
	uint8_t orbitCount;
};





struct Moon {

};





struct AsteroidBelt {

};









template<typename Particle_t>
struct ParticleGrid {
	HXStorageBuffer gpu_buffer;
	size_t count;
	size_t closest_index;
	Particle_t closest_particle;

	constexpr inline size_t GetClosestIndex(){ return closest_index; }
	constexpr inline Particle_t& GetClosestParticle(){ return closest_particle; }
};


template<typename Particle_t>
struct GPUParticleGrid {
	HXComputePipeline pipeline;
	HXStorageBuffer gpu_buffer;
	HXCommandBuffer cmdbuff;
	HXGPUFence fence;
	uvec3 size;

	// HXComputePipeline closest_pip;
	// HXGPUFence closest_fence;

	struct _PipelinePushConst {
		vec4 parentRot;
		vec4 parentRotInv;
		ivec3 grid_cam_pos;
		float parentRadius;
		uint parentSeed;
		uint block_size;
	} push_constant;

	uni_ivec3 last_grid_pos;

	HXStorageBuffer closest_buff;
	Particle_t closest_particle;
	float closest_dist;

	constexpr inline HXStorageBuffer& GetBuffer(){ return gpu_buffer; }
	constexpr inline HXStorageBuffer& GetClosestBuffer(){ return closest_buff; }
	constexpr inline Particle_t& GetClosestParticle(){ return closest_particle; }
	constexpr inline float& GetClosestDistance(){ return closest_dist; }
};


template<typename Particle_t>
struct ParticleMovement {
	Hexo::Utils::TypedVector<UniverseObjectComponent> cpu_positions;

	size_t TranslateParticles(Application&, Particle_t*, const uni_vec3&, const vec3&, const vec4&, float);
	constexpr inline size_t GetCurrentSize(){ return cpu_positions.size(); }
};



struct GalaxyParticleGrid : ParticleGrid<GalaxyParticle>, ParticleMovement<GalaxyParticle> {
	uvec3 chunk_bounds_size;
	uni_ivec3 last_grid_pos;
	CurrentGalaxyProps current_props;
	vec3 closest_renderpos;

	void Init(Application&, uvec3);
	void Cleanup(Application&);
	void ReconstructGrid(Application&, const uni_vec3&);
};



struct ClusterParticleGrid : ParticleGrid<ClusterParticle>, ParticleMovement<ClusterParticle> {
	void Init(Application&, size_t);
	void Cleanup(Application&);
	void ReconstructGrid(Application&, uni_vec3, const vec4&, const vec4&, HXUINTP);
};



struct StarsParticleGrid {
	Galaxy* galaxy;

	vec3 closest_renderpos;
	size_t cellsize;
	size_t totalsize;
	HXStorageBuffer local_renderpos;
	GPUParticleGrid<StarParticle> grid;

	void Init(Application&);
	void Cleanup(Application&);
	void Update(Application&, uni_vec3);
	void ReconstructGrid(Application&, uni_vec3);

	inline void SetGalaxy(Galaxy& g){ galaxy = &g; }
	constexpr inline size_t GetCurrentSize(){ return totalsize; }
};



struct SolarSystemProc {
	Star* star;
	vec3 star_renderpos;

	uint planetCount;

	struct OrbitPosUpdate {
		uni_vec3 absolute_pos;
		vec3 renderpos;
		uint16_t parentIndex;
	};

	Utils::TypedVector<OrbitPosUpdate> temp_pos;
	Utils::TypedVector<Planet> planets;
	Utils::TypedVector<SolarSystemOrbit> orbits;
	uint8_t closest_planets[_SOLAR_SYSTEM_MAX_ORBITS];

	void Init(Application&);
	void Cleanup(Application&);
	void Reset(Application&, Star&);
	void SortPlanets(Application&);
	void UpdateStarBodies(Application&, const uni_vec3&);
	void UpdatePlanetBodies(Application&);

	inline void SetStar(Star& st){ star = &st; }
	inline constexpr size_t GetPlanetCount(){ return planetCount; }
	inline uint8_t* GetClosestPlanets(){ return closest_planets; }

	inline const Planet& GetPlanet(uint i){ return planets[i]; }
	inline const Planet& GetClosestPlanet(uint i){ return planets[closest_planets[i]]; }

	inline const SolarSystemOrbit& GetPlanetOrbit(uint i){ return orbits[planets[i].orbitIndex]; }
	inline const SolarSystemOrbit& GetClosestPlanetOrbit(uint i){ return GetPlanetOrbit(closest_planets[i]); }

	inline const OrbitPosUpdate& GetPlanetOrbitTempPos(uint i){ return temp_pos[planets[i].orbitIndex]; }
	inline const OrbitPosUpdate& GetClosestPlanetOrbitTempPos(uint i){ return GetPlanetOrbitTempPos(closest_planets[i]); }


};



struct PlanetProc {
	enum TerrainGenState : uint8_t {
		_IDLE,

		_SETUP_TERRAIN_GEN,
		_REBUILD_TERRAIN_GEN,
		_EXIT_TERRAIN_GEN,

		_REBUILDING_TERRAIN,
		_REBUILD_FINISHED,

	};

	const Planet* planet;
	const uni_vec3* absolute_pos;
	vec3 planet_renderpos;
	Frustum* frustum;
	ViewLayer scale_type;

	uni_vec3 observer;
	size_t CurrentVertexCount;

	uint8_t terrainGenState;
	uint8_t feedbackState;
	std::thread terrain_thread;
	std::mutex terrain_thread_lock;

	HXComputePipeline normals_gen_pip;
	HXComputePipeline terrain_tex_pip;
	HXCommandBuffer cmdbuff;
	HXTexture terrain_height_tex;
	HXGPUFence gen_fence;

	Utils::TypedVector<vec4> terrain_vertices;
	HXStorageBuffer terrain_vertbuffer;
	HXStorageBuffer terrain_normalsbuffer;
	HXStorageBuffer readback_ubuffer;

	vec3 levels_renderpos[_PLANETS_MAX_TERRAIN_LEVELS];

	void Init(Application&);
	void Update(Application&, const uni_vec3&);
	void Cleanup(Application&);
	void Reset(Application&, const Planet&, const uni_vec3&, uint);

	inline size_t GetCurrentVertexCount(){ return CurrentVertexCount; }
};











/// this is the root, the universe, so it has no space bounds
struct Universe {
	Universe() :
		galaxy_psys(), cluster_psys(),
		currentGalaxy(), currentCluster(), currentStar(), currentPlanet()
	{}

	GalaxyParticleGrid galaxy_psys;
	ClusterParticleGrid cluster_psys;
	StarsParticleGrid stars_psys;
	SolarSystemProc solar_system_sys;
	PlanetProc planet_sys;

	Galaxy currentGalaxy;
	Cluster currentCluster;
	Star currentStar;
	Planet currentPlanet;


	void CheckCurrentGalaxy(Application&);
	void CheckCurrentCluster(Application&);
	void CheckCurrentStar(Application&);
	void CheckCurrentPlanet(Application&);

	bool IsCurrentGalaxy(Application&, const Galaxy&);
	bool IsCurrentCluster(Application&, const Cluster&);
	bool IsCurrentStar(Application&, const Star&);
	bool IsCurrentPlanet(Application&, const Planet&);

	void Init(Application&);
	void Update(Application&);
	void Cleanup(Application&);
};









struct DefaultRendererBundle {
	HXTexture rendertex;
	HXRenderPass renderpass;
	HXGraphicsPipeline pipeline;
	HXGPUFence fence;
};

struct DepthColorRendererBundle {
	HXTexture rendertex;
	HXTexture depthtex;
	HXRenderPass renderpass;
	HXGraphicsPipeline pipeline;
	HXGPUFence fence;
};

struct DefaultComputeBundle {
	HXTexture rendertex;
	HXComputePipeline pipeline;
	HXGPUFence fence;
};




struct GlobalRendererResources {
	uint zero_memory = 0u; /// for pointing to 32 bit zero memory

	HXStorageBuffer sphere_vert;
	HXStorageBuffer sphere_uvs;
	HXStorageBuffer sphere_indices;
	HXVertexBufferDescriptor sphere_vdesc;
	HXIndexBufferDescriptor sphere_idesc;
};




struct PlanetRenderer {
	uint8_t scale = 1;
	DepthColorRendererBundle rfar;
	DepthColorRendererBundle rnear;

	HXVertexBufferDescriptor terrain_vdesc;

	bool closestPlanetInView;

	HXComputePipeline planet_tex_pip;
	HXStorageBuffer planet_tex_indices_buffer;
	uint currentTexIndices[_PLANET_TEXTURE_COUNT];
	uint previousClosests[_PLANET_TEXTURE_COUNT];
	HXTexture surfaceTex[_PLANET_TEXTURE_COUNT];
	HXTexture normalsTex[_PLANET_TEXTURE_COUNT];

	HXStorageBuffer props;
	HXCommandBuffer cmdbuff;
};




struct SolarSystemRenderer {
	uint8_t scale = 1;
	DefaultRendererBundle rings;
	DefaultRendererBundle planets;

	HXStorageBuffer props;
	HXStorageBuffer orbits;
	HXStorageBuffer orbit_updates;
	HXCommandBuffer cmdbuff;
};




struct StarsRenderer : DefaultRendererBundle {
	uint8_t scale = 3;
	HXCommandBuffer cmdbuff;
};




struct NebulaRenderer {
	uint8_t scale = 1;

	DefaultRendererBundle rfar;
	DefaultRendererBundle rnear;

	HXCommandBuffer cmdbuff;
	HXStorageBuffer parent_props_buff;
};




struct GalaxyRenderer {
	uint8_t scale = 1;
	uint8_t near_scale = 1;

	DefaultRendererBundle rfar;
	DefaultComputeBundle rnear;

	HXStorageBuffer current_props;
	HXCommandBuffer cmdbuff;
	HXCommandBuffer cmdbuff_compute;
};




struct TAAFilterPass {
	uint8_t scale = 1;

	constexpr static int FrameCount = 2;
	SwapHelper<HXTexture, FrameCount> rendertex;
	SwapHelper<HXRenderPass, FrameCount> renderpass;
	HXGraphicsPipeline pipeline;
};




struct FontRendererSDF {
	HXTexture atlasTex;
	HXStorageBuffer buffer;
};




struct UniverseRenderer {
	CameraConstants constants;
	HXStorageBuffer constants_buff;

	CurrentSeedsConstants current_seeds;
	HXStorageBuffer cur_seeds_buff;

	GlobalRendererResources resources;

	GalaxyRenderer galaxies;
	StarsRenderer stars;
	NebulaRenderer nebulae;
	SolarSystemRenderer solar_system;
	PlanetRenderer planets;

	TAAFilterPass taa_pass;


	HXCommandAllocator g_cmdalloc;
	HXCommandAllocator c_cmdalloc;
	HXCommandBuffer graphics_cmdbuff;
	HXCommandBuffer compute_cmdbuff;


	void SetupShaderInputs(Application&);


	void InitGalaxies(Application&);
	void DrawFarGalaxies(Application&);
	void DrawNearGalaxies(Application&);
	void UpdateCurrentGalaxyBuffer(Application&, CurrentGalaxyProps*);
	HXTexture& GetGalaxiesTex(Application&);



	void InitNebulae(Application&);
	void DrawFarNebulae(Application&);
	void DrawNearNebulae(Application&);
	HXTexture& GetNebulaeTex(Application&);



	void InitStars(Application&);
	void DrawStars(Application&);
	HXTexture& GetStarsTex(Application&);



	void InitSolarSystem(Application&);
	void DrawPlanetOrbits(Application&);
	void DrawMoonOrbits(Application&);
	void DrawSolarSystemRings(Application&);
	void DrawSolarSystemStar(Application&);
	void UpdateSolarSystemPropsBuffer(Application&, SolarSystemProperties*);
	void UpdateSolarSystemOrbitsBuffer(Application&, SolarSystemOrbit*, size_t);
	HXTexture& GetSolarSystemRingsTex(Application&);



	void InitPlanets(Application&);
	void DrawPlanetsFar(Application&);
	HXTexture& GetPlanetsFarTex(Application&);
	void DrawPlanetsNear(Application&);
	HXTexture& GetPlanetsNearTex(Application&);
	void RegeneratePlanetTextures(Application&);



	void InitTAAPass(Application&);
	void DrawTAAPass(Application&);
	HXTexture& GetTAATex(Application&);



	void InitResources(Application&);
	void Init(Application&);
	void Update(Application&);
	void Resize(Application&);
	void Cleanup(Application&);

};
