

template<typename Particle_t>
size_t ParticleMovement<Particle_t>::TranslateParticles(
	Application& app, Particle_t* array,
	const uni_vec3& absolute_cam_pos, const vec3& closest_renderpos,
	const vec4& parentRot, float space_factor
){
	float minDist = std::numeric_limits<float>::max();
	size_t closest_index = 0;
	for (size_t i=0; i<cpu_positions.size(); ++i){
		Particle_t& pt = array[i];
		UniverseObjectComponent& pos = cpu_positions[i];

		if (pt.seed == app.universe.currentGalaxy.seed){
			pt.renderpos = closest_renderpos;
		}else{
			pt.renderpos = vec3(pos.absolute_pos - absolute_cam_pos);
		}
		pt.renderpos = mulvq(parentRot, pt.renderpos);
		pt.renderpos *= space_factor;
		// pt.renderpos = pt.additionalpos + vec3(pt.gridpos*cell_size + cell_size/2);
		// pt.renderpos = pos.additionalpos + vec3((pos.gridpos-grid_cam_pos)*int(tcellsize) + int(tcellsize)/2) - fract_cam_pos;

		float tlp = length(pt.renderpos);
		if (tlp < minDist){
			closest_index = i;
			minDist = tlp;
		}

	}

	return closest_index;
}










void GalaxyParticleGrid::Init(Application& app, uvec3 size){
	last_grid_pos = uni_ivec3(1);
	chunk_bounds_size = size;
	size_t real_size = size.x * size.y * size.z;

	cpu_positions.reserve(real_size);
	for (size_t i=0; i<real_size; ++i){
		cpu_positions.emplace_back();
	}

	/// make gpu buffer
	gpu_buffer = app.hxg.CreateStorageBuffer(
		HXSBufferConfig{ real_size*sizeof(GalaxyParticle), HX_GRAPHICS_USAGE_DYNAMIC }, NULL
	);

	/// warmup the gpu buffer
	app.hxg.MapStorageBuffer(gpu_buffer, 0, app.hxg.GetConfig(gpu_buffer).Size, HX_GRAPHICS_MEM_ACCESS_W);
	app.hxg.UnmapStorageBuffer(gpu_buffer);
}

void GalaxyParticleGrid::Cleanup(Application& app){
	app.hxg.DestroyStorageBuffer(gpu_buffer);
}


void Galaxy_GenerateMinimalProps(GalaxyParticle& pt, uni_vec3& random_pos, uint32_t seed){

	// pt.radius = mix(0.04f*0.1f, 0.04f*10.0f, frandom(seed)) * 1000.0f;
	pt.radius = mix(0.001f, 1.0f, frandom(seed));

	// quat trot = qaxang(
	// 	normalize(vec3(
	// 		frandom(seed),
	// 		frandom(seed),
	// 		frandom(seed)
	// 	)*2.0f-1.0f),
	// 	frandom(seed) * Mathgl::pi<float>() * 2.0f
	// );
	quat trot = frandom_quaternion(seed);
	quat troti = Mathgl::inverse(trot);

	pt.rotation = vec4(trot.x, trot.y, trot.z, trot.w);
	pt.rotationInv = vec4(troti.x, troti.y, troti.z, troti.w);


	random_pos = (uni_vec3(
		mix(pt.radius/2.0f, _GALAXY_SPACING_FACTOR - pt.radius/2.0f, frandom(seed)),
		mix(pt.radius/2.0f, _GALAXY_SPACING_FACTOR - pt.radius/2.0f, frandom(seed)),
		mix(pt.radius/2.0f, _GALAXY_SPACING_FACTOR - pt.radius/2.0f, frandom(seed))
	));
}


void GalaxyParticleGrid::ReconstructGrid(Application& app, const uni_vec3& absolute_cam_pos){
	GalaxyParticle* mapped = (GalaxyParticle*)app.hxg.MapStorageBuffer(gpu_buffer, 0, app.hxg.GetConfig(gpu_buffer).Size, HX_GRAPHICS_MEM_ACCESS_W);

	uni_float cellsize = uni_float(_GALAXY_SPACING_FACTOR);

	// uni_vec3 fract_cam_pos = absolute_cam_pos / universe_cell_size

	uni_ivec3 grid_cam_pos = floor(absolute_cam_pos/cellsize + uni_float(0.5));

	/// is the grid cell has changed we have to regenerate everything
	if (last_grid_pos != grid_cam_pos){

		for (uint32_t x=0, c=0; x<chunk_bounds_size.x; ++x){
			for (uint32_t y=0; y<chunk_bounds_size.y; ++y){
				for (uint32_t z=0; z<chunk_bounds_size.z; ++z, ++c){

					uni_ivec3 tp = uni_ivec3(x, y, z) + grid_cam_pos;
					uint32_t seed = uint32_t(tp.x*1973u + tp.y*9277u + tp.z*10195u) | 1u;

					/// determine the type of galaxy out of 4 types
					uint64_t type = uint64_t(frandom(seed)*3);

					/// use the left most 2 bits to store that type
					seed |= uint32_t(type << 30);


					GalaxyParticle pt{};
					UniverseObjectComponent pos{};

					uni_vec3 gridpos = uni_vec3(tp) - uni_vec3(chunk_bounds_size)/uni_float(2.0);
					gridpos -= 0.5;

					/// generate minimal particle properties
					uni_vec3 additionalpos;
					Galaxy_GenerateMinimalProps(pt, additionalpos, seed);

					pos.absolute_pos = gridpos*cellsize + additionalpos;
					pt.seed = seed;


					/// add to buffer
					mapped[c] = pt;
					cpu_positions[c] = pos;

				}
			}
		}

	}


	{
		uni_vec3 dpos = app.camera.view.eye.get(E_VIEW_LAYER_GALAXY);
		closest_renderpos = vec3(uni_vec3(0) - dpos) / float(CameraViewpoint::GetScaleDiff(E_VIEW_LAYER_UNIVERSE, E_VIEW_LAYER_GALAXY));
	}


	vec4 identity_quat = vec4(0,0,0,1);
	closest_index = TranslateParticles(app, mapped, absolute_cam_pos, closest_renderpos, identity_quat, 1);
	closest_particle = mapped[closest_index];


	app.hxg.UnmapStorageBuffer(gpu_buffer);
	last_grid_pos = grid_cam_pos;
}











void ClusterParticleGrid::Init(Application& app, size_t count){
	this->count = count;

	/// reserve random access space in positions buffer
	cpu_positions.reserve(count);
	for (size_t i=0; i<count; ++i){
		cpu_positions.emplace_back();
	}

	/// make gpu buffer
	gpu_buffer = app.hxg.CreateStorageBuffer(
		HXSBufferConfig{ count*sizeof(ClusterParticle), HX_GRAPHICS_USAGE_DYNAMIC }, NULL
	);

	/// warmup the gpu buffer
	app.hxg.MapStorageBuffer(gpu_buffer, 0, app.hxg.GetConfig(gpu_buffer).Size, HX_GRAPHICS_MEM_ACCESS_W);
	app.hxg.UnmapStorageBuffer(gpu_buffer);
}

void ClusterParticleGrid::Cleanup(Application& app){
	app.hxg.DestroyStorageBuffer(gpu_buffer);
}


void ClusterParticleGrid::ReconstructGrid(Application& app, uni_vec3 absolute_cam_pos, const vec4& parentRot, const vec4& parentRotInv, HXUINTP parent){

}









HXShaderInput stars_particles_inputs[3];


void StarsParticleGrid::Init(Application& app){
	totalsize = _STARS_BLOCK_STAR_COUNT*_STARS_BLOCK_STAR_COUNT*_STARS_BLOCK_STAR_COUNT * _STARS_BLOCK_COUNT;

	grid.last_grid_pos = uni_ivec3(0);


	/// create compute pipeline
	HXComputePipelineConfig c_pipconfig{};
	char* comp_blob = LoadShaderBlob("stars_particles_compute");
	ShaderC::LoadShaderBinary(comp_blob, HX_SHADERC_TYPE_COMPUTE, c_pipconfig.ComputeShader, c_pipconfig.Metadata);
	grid.pipeline = app.hxg.CreateComputePipeline(c_pipconfig);


	/// make gpu buffer
	grid.gpu_buffer = app.hxg.CreateStorageBuffer(
		HXSBufferConfig{ totalsize*sizeof(StarParticle), HX_GRAPHICS_USAGE_DYNAMIC }, NULL
	);


	grid.closest_buff = app.hxg.CreateStorageBuffer(
		HXSBufferConfig{ sizeof(StarParticleBufferHeader), HX_GRAPHICS_USAGE_DYNAMIC }, NULL
	);
	StarParticleBufferHeader* mapped = (StarParticleBufferHeader*)(app.hxg.MapStorageBuffer(grid.closest_buff, 0, sizeof(StarParticleBufferHeader), HX_GRAPHICS_MEM_ACCESS_W));
	mapped->minDistInt = uint(0xFFFFFFFF);
	app.hxg.UnmapStorageBuffer(grid.closest_buff);


	local_renderpos = app.hxg.CreateStorageBuffer(
		HXSBufferConfig{ _STARS_BLOCK_COUNT*sizeof(StarBlock), HX_GRAPHICS_USAGE_DYNAMIC }, NULL
	);
	app.hxg.MapStorageBuffer(local_renderpos, 0, app.hxg.GetConfig(local_renderpos).Size, HX_GRAPHICS_MEM_ACCESS_W);
	app.hxg.UnmapStorageBuffer(local_renderpos);


	/// make command buffer
	grid.cmdbuff = app.hxg.CreateCommandBuffer(app.universeRenderer.c_cmdalloc, HX_GRAPHICS_CMDBUFFER_COMPUTE, HX_GRAPHICS_CMDBUFFER_REUSE);



	/// pre record commands
	stars_particles_inputs[0] = { app.hxg.GetUniform(grid.pipeline, "Constants"), &grid.push_constant };
	stars_particles_inputs[1] = { app.hxg.GetUniform(grid.pipeline, "StarPositions"), app.hxg.GetData(grid.gpu_buffer) };
	stars_particles_inputs[2] = { app.hxg.GetUniform(grid.pipeline, "StarBlocks"), app.hxg.GetData(local_renderpos) };

	HXWaitForFenceCmd wait{};
	wait.fence = app.hxg.GetData(grid.fence);

	HXSetComputePipelineCmd comp_pip{};
	comp_pip.pipeline = app.hxg.GetData(grid.pipeline);

	HXUpdateShaderUniformsCmd comp_uniforms{};
	comp_uniforms.Inputs = stars_particles_inputs;
	comp_uniforms.Length = HX_LENGTH_C_ARRAY(stars_particles_inputs);

	HXDispatchComputeCmd discmd{};
	discmd.x = totalsize / 1024;
	discmd.y = 1;
	discmd.z = 1;

	HXInsertFenceCmd fnc{};
	fnc.fence = app.hxg.GetData(grid.fence);

	app.hxg.InsertCommands(grid.cmdbuff, wait, comp_pip, comp_uniforms, discmd, fnc);

}


void StarsParticleGrid::Cleanup(Application& app){
	app.hxg.DestroyStorageBuffer(local_renderpos);
	app.hxg.DestroyStorageBuffer(grid.gpu_buffer);
}






void StarsParticleGrid::Update(Application& app, uni_vec3 absolute_cam_pos){
	absolute_cam_pos = mulvq(galaxy->props.rotationInv, absolute_cam_pos);
	uni_ivec3 grid_cam_pos = floor(absolute_cam_pos/uni_float(_STARS_BLOCK_STAR_COUNT * _STARS_SPACING_FACTOR) + uni_float(0.5));
	bool updateGrid = grid_cam_pos != grid.last_grid_pos;

	auto diff = CameraViewpoint::GetScaleDiff(E_VIEW_LAYER_UNIVERSE, E_VIEW_LAYER_GALAXY) * galaxy->props.radius;


	StarBlock* mapped = (StarBlock*)(app.hxg.MapStorageBuffer(local_renderpos, 0, app.hxg.GetConfig(local_renderpos).Size, HX_GRAPHICS_MEM_ACCESS_W));

	if (updateGrid){
		grid.last_grid_pos = grid_cam_pos;

		for (int i=0; i<_STARS_BLOCK_COUNT; ++i){
			uni_ivec3 block_cell = uni_ivec3(ind_1Dto3D(i, ivec3(_STARS_BLOCK_WIDTH)) - ivec3(_STARS_BLOCK_WIDTH/2)) + grid.last_grid_pos;
			uni_vec3 tp = uni_vec3(block_cell * uni_int(_STARS_BLOCK_STAR_COUNT * _STARS_SPACING_FACTOR));

			uint seed = ConstructBasicSeed(ivec4(block_cell, galaxy->seed));

			uni_float cellfactor = smoothstep(uni_float(0.1), uni_float(0.0), (uni_float)length(tp) - diff);
			cellfactor = min(cellfactor, smoothstep(uni_float(0.05), uni_float(0.01), abs(uni_vec3(tp/diff).y) ));

			StarBlock& stbl = mapped[i];
			stbl.seed = seed;
			stbl.spawnFactor = float(cellfactor);
			stbl.gravityPoint = vec3(frandom(seed), frandom(seed), frandom(seed)) *0.5f - 0.25f;
			stbl.temperature = easeInExpo(frandom(seed));
		}
	}

	for (int i=0; i<_STARS_BLOCK_COUNT; ++i){
		uni_ivec3 block_cell = uni_ivec3(ind_1Dto3D(i, ivec3(_STARS_BLOCK_WIDTH)) - ivec3(_STARS_BLOCK_WIDTH/2)) + grid.last_grid_pos;
		uni_vec3 tp = uni_vec3(block_cell * uni_int(_STARS_BLOCK_STAR_COUNT * _STARS_SPACING_FACTOR));

		mapped[i].renderpos = mulvq(galaxy->props.rotation, tp - absolute_cam_pos);
	}

	app.hxg.UnmapStorageBuffer(local_renderpos);



	if (updateGrid){
		grid.push_constant.parentRot = galaxy->props.rotation;
		grid.push_constant.parentRotInv = galaxy->props.rotationInv;
		grid.push_constant.grid_cam_pos = ivec3(grid_cam_pos);
		grid.push_constant.parentSeed = galaxy->props.seed;
		grid.push_constant.block_size = uint(_STARS_BLOCK_STAR_COUNT);

		app.hxg.ExecuteCommands(grid.cmdbuff);
	}



	{
		StarParticleBufferHeader* mapped = (StarParticleBufferHeader*)(app.hxg.MapStorageBuffer(grid.closest_buff, 0, sizeof(StarParticleBufferHeader), HX_GRAPHICS_MEM_ACCESS_R));

		grid.closest_dist = mapped->minDist;
		grid.closest_particle = mapped->closest_particle;

		app.hxg.UnmapStorageBuffer(grid.closest_buff);
	}



	{
		uni_vec3 dpos = app.camera.view.eye.get(E_VIEW_LAYER_STAR_ORBIT);
		closest_renderpos = vec3(uni_vec3(0) - dpos) / float(CameraViewpoint::GetScaleDiff(E_VIEW_LAYER_GALAXY, E_VIEW_LAYER_STAR_ORBIT));
	}
}
