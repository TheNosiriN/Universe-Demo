


/// clearer implementation found here: https://playground.babylonjs.com/#ZU2FWP#97


namespace IcosahedronTerrainGenResources {

	const uint StartRes = 50;//50;
    const uint MaxLevel = _PLANETS_MAX_TERRAIN_LEVELS;
    const uint StartLevel = 10;

	float DistanceLevels[MaxLevel];


	const uint Idx[63] = {
	    0, 11, 5, 0, 5, 1, 0, 1, 7, 0, 7, 10, 0, 10, 11,
	    1, 5, 9, 5, 11, 4, 11, 10, 2, 10, 7, 6, 10, 7, 6, 7, 1, 8,
	    3, 9, 4, 3, 4, 2, 3, 2, 6, 3, 6, 8, 3, 8, 9,
	    4, 9, 5, 2, 4, 11, 6, 2, 10, 8, 6, 7, 9, 8, 1
	};

	const float Ico_t = (1.0f + sqrt(5.0f)) / 2.0f;
	const vec3 IcoPoints[12] = {
	    vec3( -1, Ico_t, 0 ), vec3( 1, Ico_t, 0 ),
	    vec3( -1, -Ico_t, 0 ), vec3( 1, -Ico_t, 0 ),
	    vec3( 0, -1, Ico_t ), vec3( 0, 1, Ico_t ),
	    vec3( 0, -1, -Ico_t ), vec3( 0, 1, -Ico_t ),
	    vec3( Ico_t, 0, -1 ), vec3( Ico_t, 0, 1 ),
	    vec3( -Ico_t, 0, -1 ), vec3( -Ico_t, 0, 1 )
	};

	constexpr uint RecurseIdx[12] = {0, 3, 5, 5, 3, 4, 3, 1, 4, 5, 4, 2};


	void AddTriangle(PlanetProc* proc, const uint level, const vec3& p1, const vec3& p2, const vec3& p3){
		float rad = float(proc->planet->radius / CameraViewpoint::GetScale(proc->scale_type));

		proc->terrain_vertices.push_back(vec4(p3*rad, level));
		proc->terrain_vertices.push_back(vec4(p2*rad, level));
		proc->terrain_vertices.push_back(vec4(p1*rad, level));
	};


	// void Recurse(PlanetProc* proc, const uni_vec3& observer, uni_vec3 p1, uni_vec3 p2, uni_vec3 p3, uint level){
	// 	/// culling
	// 	if (
	//         dot( p1, observer ) < 0.75 &&
	//         dot( p2, observer ) < 0.75 &&
	//         dot( p3, observer ) < 0.75
	//     ){ return; }
	//
	// 	uni_vec3 edges[3] = {
	//         (p1 + p2) / 2.0,
	//         (p2 + p3) / 2.0,
	//         (p3 + p1) / 2.0
	//     };
	//     uint8_t edgeDistMask;
	//
	// 	for (uint i=0; i<3; i++){
	//         edgeDistMask |= (distance(edges[i], observer) > DistanceLevels[level]) << i;
	//     }
	//
	// 	/// Add Triangle
	//     if ( (edgeDistMask == 7u) || level >= MaxLevel ){
	// 		if (level >= StartLevel){
	// 			// auto cullrad = distance(p1, (p1+p2+p3)/3.0)*2.0;
	//
	// 			bool cull = true;
	// 			for (uint i=0; i<4; ++i){
	// 				cull = cull && (dot(p1-observer, uni_vec3(proc->frustum->faces[i])) + proc->frustum->faces[i].w < 0);
	// 				cull = cull && (dot(p2-observer, uni_vec3(proc->frustum->faces[i])) + proc->frustum->faces[i].w < 0);
	// 				cull = cull && (dot(p3-observer, uni_vec3(proc->frustum->faces[i])) + proc->frustum->faces[i].w < 0);
	// 			}
	//
	// 			if (!cull){
	// 				AddTriangle(proc, level, p1, p2, p3);
	// 			}
	// 		}
	//         return;
	//     }
	//
	//
	// 	uni_vec3 p[6] = {
    //         p1, p2, p3,
    //         edges[0], edges[1], edges[2]
    //     };
    //     uint8_t valid = 15u; /// 1111 (4 flags true)
	//
    //     if (edgeDistMask & (1<<0)){ p[3] = p1; valid ^= (1<<0); } // skip triangle 0
    //     if (edgeDistMask & (1<<1)){ p[4] = p2; valid ^= (1<<2); } // skip triangle 2
    //     if (edgeDistMask & (1<<2)){ p[5] = p3; valid ^= (1<<3); } // skip triangle 3
	//
	// 	for (uint i=0; i<4; ++i){
    //         if (valid & (1<<i)){
	// 			Recurse(
	// 				proc, observer,
	// 				normalize( p[RecurseIdx[3 * i + 0]] ),
	// 				normalize( p[RecurseIdx[3 * i + 1]] ),
	// 				normalize( p[RecurseIdx[3 * i + 2]] ),
	// 				level+1u
	//             );
	// 		}
	// 	}
	// }


	void Recurse(PlanetProc* proc, const uni_vec3& observer, uni_vec3 p1, uni_vec3 p2, uni_vec3 p3, uint level){
		/// culling
		if (
	        dot( p1, observer ) < 0.75 &&
	        dot( p2, observer ) < 0.75 &&
	        dot( p3, observer ) < 0.75
	    ){ return; }


		uni_vec3 edges[3] = {
	        (p1 + p2) / 2.0,
	        (p2 + p3) / 2.0,
	        (p3 + p1) / 2.0
	    };
	    bool edgeDistMask[3];

		for (uint i=0; i<3; ++i){
	        edgeDistMask[i] = distance(edges[i], observer) > DistanceLevels[level];
	    }

		/// Add Triangle
	    if ( (edgeDistMask[0] && edgeDistMask[1] && edgeDistMask[2]) || level >= MaxLevel ){
	        if (level >= StartLevel){
				// auto cullrad = distance(p1, (p1+p2+p3)/3.0)*2.0;

				bool cull = true;
				for (uint i=0; i<4; ++i){
					cull = cull && (dot((p1-observer) - double(proc->frustum->faces[i].w), uni_vec3(proc->frustum->faces[i])) < 0);
					cull = cull && (dot((p2-observer) - double(proc->frustum->faces[i].w), uni_vec3(proc->frustum->faces[i])) < 0);
					cull = cull && (dot((p3-observer) - double(proc->frustum->faces[i].w), uni_vec3(proc->frustum->faces[i])) < 0);
				}

				if (!cull){
					AddTriangle(proc, level, p1, p2, p3);
					// AddTriangle(proc, level, p1 - observer, p2 - observer, p3 - observer);
				}
			}
	        return;
	    }


		uni_vec3 p[6] = {
            p1, p2, p3,
            edges[0], edges[1], edges[2]
        };
        bool valid[4] = { true, true, true, true };

		if (edgeDistMask[0]){ p[3] = p1; valid[0] = false; } // skip triangle 0
        if (edgeDistMask[1]){ p[4] = p2; valid[2] = false; } // skip triangle 2
        if (edgeDistMask[2]){ p[5] = p3; valid[3] = false; } // skip triangle 3

		for (uint i=0; i<4; ++i){
            if (!valid[i]){ continue; }
			Recurse(
				proc, observer,
				normalize( p[RecurseIdx[3 * i + 0]] ),
				normalize( p[RecurseIdx[3 * i + 1]] ),
				normalize( p[RecurseIdx[3 * i + 2]] ),
				level+1u
			);
		}
	}


	void Rebuild(PlanetProc* proc, const uni_vec3 observer){
		for (uint i=0; i < 63/3; ++i){
			vec3 p1 = normalize( IcoPoints[Idx[i * 3 + 0]] ); // triangle point 1
			vec3 p2 = normalize( IcoPoints[Idx[i * 3 + 1]] ); // triangle point 2
			vec3 p3 = normalize( IcoPoints[Idx[i * 3 + 2]] ); // triangle point 3

            Recurse(proc, observer, p1, p2, p3, 0);
		}
	}
};






void IcosahedronTerrainGenHandler(Application* app, PlanetProc* proc){
	using namespace IcosahedronTerrainGenResources;

	for (uint i=0; i<MaxLevel; ++i){
        DistanceLevels[i] = StartRes/pow(2.0f, float(i));
    }

	bool closegen = false;

	while(!closegen){
		switch (proc->terrainGenState){

			case PlanetProc::_SETUP_TERRAIN_GEN:

			break;

			case PlanetProc::_REBUILD_TERRAIN_GEN:
				proc->feedbackState = PlanetProc::_REBUILDING_TERRAIN;

				proc->terrain_thread_lock.lock();
				proc->terrain_vertices.clearFast();

				app->hxg.WaitForFenceCPU(proc->gen_fence);
				Rebuild(proc, proc->observer);

				// std::cout << "vert size: " << proc->terrain_vertices.size() << '\n';
				proc->terrain_thread_lock.unlock();

				proc->feedbackState = PlanetProc::_REBUILD_FINISHED;

			break;

			case PlanetProc::_EXIT_TERRAIN_GEN:
				closegen = true;
			break;
		}
	}
}




HXShaderInput terrain_compute_inputs[3];
HXShaderUniform terrain_tex_compute_tex_uniform;
const size_t maxStartCount = 3000000;


void PlanetProc::Init(Application& app){
	frustum = &(app.camera.frustum);

	terrain_vertices.reserve(maxStartCount);


	/// init resources
	terrain_vertbuffer = app.hxg.CreateStorageBuffer(
		HXSBufferConfig{maxStartCount*sizeof(vec4), HX_GRAPHICS_USAGE_DYNAMIC},
		NULL, 10
	);
	terrain_normalsbuffer = app.hxg.CreateStorageBuffer(
		HXSBufferConfig{maxStartCount*sizeof(vec3), HX_GRAPHICS_USAGE_DYNAMIC},
		NULL, HX_GRAPHICS_VERTEXBUFFER
	);

	HXTextureConfig texconfig{};
	texconfig.Type = HX_GRAPHICS_TEXTURE_2D;
	texconfig.Depth = 1;
	texconfig.Usage = HX_GRAPHICS_USAGE_DYNAMIC;
	texconfig.GenerateMips = false;
	texconfig.Format = HX_R32_FLOAT;
	texconfig.Width = _TERRAIN_HEIGHT_TEXTURE_WIDTH;
	texconfig.Height = _TERRAIN_HEIGHT_TEXTURE_WIDTH;
	terrain_height_tex = app.hxg.CreateTexture(texconfig, NULL);


	/// init compute pipeline
	{
		HXComputePipelineConfig c_pipconfig{};
		char* comp_blob = LoadShaderBlob("icosahedron_lod_compute");
		ShaderC::LoadShaderBinary(comp_blob, HX_SHADERC_TYPE_COMPUTE, c_pipconfig.ComputeShader, c_pipconfig.Metadata);
		normals_gen_pip = app.hxg.CreateComputePipeline(c_pipconfig);
		delete comp_blob;
	}; {
		HXComputePipelineConfig c_pipconfig{};
		char* comp_blob = LoadShaderBlob("terrain_texture_gen_compute");
		ShaderC::LoadShaderBinary(comp_blob, HX_SHADERC_TYPE_COMPUTE, c_pipconfig.ComputeShader, c_pipconfig.Metadata);
		terrain_tex_pip = app.hxg.CreateComputePipeline(c_pipconfig);
		delete comp_blob;
	}

	cmdbuff = app.hxg.CreateCommandBuffer(app.universeRenderer.c_cmdalloc, HX_GRAPHICS_CMDBUFFER_COMPUTE, HX_GRAPHICS_CMDBUFFER_ONCE);

	terrain_compute_inputs[0] = { app.hxg.GetUniform(normals_gen_pip, "VertexBuffer"), app.hxg.GetData(terrain_vertbuffer) };
	terrain_compute_inputs[1] = { app.hxg.GetUniform(normals_gen_pip, "NormalsBuffer"), app.hxg.GetData(terrain_normalsbuffer) };
	terrain_compute_inputs[2] = { app.hxg.GetUniform(normals_gen_pip, "heightTex"), app.hxg.GetData(terrain_height_tex), app.hxg.GetData(app.linearSampler) };
	// terrain_compute_inputs[3] = { app.hxg.GetUniform(normals_gen_pip, "NormalsBuffer"), app.hxg.GetData(terrain_normalsbuffer) };
	terrain_tex_compute_tex_uniform = app.hxg.GetUniform(terrain_tex_pip, "heightTex");



	// readback_ubuffer = app.hxg.CreateStorageBuffer(
	// 	HXSBufferConfig{sizeof(PlanetIcoLODUniforms), HX_GRAPHICS_USAGE_DYNAMIC},
	// 	NULL, HX_GRAPHICS_UNIFORMBUFFER
	// );

	terrain_thread = std::thread(IcosahedronTerrainGenHandler, &app, this);
}


void PlanetProc::Update(Application& app, const uni_vec3& absolute_cam_pos){
	uni_vec3 drenderpos = (*absolute_pos);
	observer = drenderpos / uni_float(planet->radius / CameraViewpoint::GetScale(scale_type));
	planet_renderpos = vec3(-drenderpos);
	// std::cout << (*renderpos).x << " -- " << (*renderpos).y << " -- " << (*renderpos).z << '\n';


	if (feedbackState == _REBUILD_FINISHED){
		terrain_thread_lock.lock();
		CurrentVertexCount = terrain_vertices.size();

		HXCopyStorageBufferCmd constcmd{};
		constcmd.source = terrain_vertices.data();
		constcmd.destination = app.hxg.GetData(terrain_vertbuffer);
		constcmd.sourceOffset = 0;
		constcmd.destinationOffset = 0;
		constcmd.size = std::min<size_t>(terrain_vertices.size(), maxStartCount) * sizeof(vec4);
		app.hxg.CopyStorageBuffer(constcmd, HX_GRAPHICS_COPY_CPU_GPU);


		feedbackState = _IDLE;
		terrainGenState = _REBUILD_TERRAIN_GEN;


		/// run normals gen compute shader
		HXWaitForFenceCmd wait{};
		wait.fence = app.hxg.GetData(gen_fence);

		HXSetComputePipelineCmd comp_pip{};
		comp_pip.pipeline = app.hxg.GetData(normals_gen_pip);

		HXUpdateShaderUniformsCmd comp_uniforms{};
		comp_uniforms.Inputs = terrain_compute_inputs;
		comp_uniforms.Length = HX_LENGTH_C_ARRAY(terrain_compute_inputs);

		HXDispatchComputeCmd discmd{};
		discmd.x = size_t(ceil(double(CurrentVertexCount / 3) / 1024));
		discmd.y = 1;
		discmd.z = 1;

		HXInsertFenceCmd fnc{};
		fnc.fence = app.hxg.GetData(gen_fence);

		app.hxg.InsertCommands(cmdbuff, comp_pip, comp_uniforms, wait, discmd, fnc);

		terrain_thread_lock.unlock();
		app.hxg.ExecuteCommands(cmdbuff);

	}else{
		terrainGenState = _IDLE;
	}

}


void PlanetProc::Cleanup(Application& app){
	terrainGenState = _EXIT_TERRAIN_GEN;
	terrain_thread.join();
}


void PlanetProc::Reset(Application& app, const Planet& planet, const uni_vec3& absolute_pos, uint orbit_type){
	this->planet = &planet;
	this->absolute_pos = &absolute_pos;
	this->scale_type = orbit_type == _SOLAR_SYSTEM_ORBIT_PLANET_FLAG ? E_VIEW_LAYER_PLANET_ORBIT : E_VIEW_LAYER_MOON_ORBIT;


	/// generate height texture
	app.hxg.WaitForFenceCPU(gen_fence);

	HXSetComputePipelineCmd comp_pip{};
	comp_pip.pipeline = app.hxg.GetData(terrain_tex_pip);

	HXDispatchComputeCmd discmd{};
	discmd.x = _TERRAIN_HEIGHT_TEXTURE_WIDTH / 32;
	discmd.y = _TERRAIN_HEIGHT_TEXTURE_WIDTH / 32;
	discmd.z = 1;

	HXUpdateShaderUniformCmd tex_uniform{};
	tex_uniform.Uniform = terrain_tex_compute_tex_uniform;
	tex_uniform.Data = app.hxg.GetData(terrain_height_tex);

	HXInsertFenceCmd fnc{};
	fnc.fence = app.hxg.GetData(gen_fence);

	app.hxg.InsertCommands(cmdbuff, comp_pip, tex_uniform, discmd);
	app.hxg.ExecuteCommands(cmdbuff);
	// std::cout << "dispatched: " << discmd.x << " -- " << discmd.y << '\n';

	terrainGenState = _REBUILD_TERRAIN_GEN;
}
