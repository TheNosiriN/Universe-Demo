


#include "NoiseFuncCPU.cpp"

// #define _USE_GPU_ICO_LOD


/// clearer implementation found here: https://playground.babylonjs.com/#ZU2FWP#97


namespace IcosahedronTerrainGenResources {


	struct TriangleNode {
		uni_vec3 p1;
		uni_vec3 p2;
		uni_vec3 p3;
		uint8_t level;
	};

	struct TempFixedArray {
		TriangleNode data[_PLANET_TERRAIN_CUSTOM_STACK_MAX_SIZE];
		uint8_t length;

		TempFixedArray() : length(0){}

		void push(const TriangleNode& node){
			if (this->length >= _PLANET_TERRAIN_CUSTOM_STACK_MAX_SIZE){
				std::cout << "EXCEEDED MAX LENGTH" << '\n';
				return;
			}

			this->data[this->length] = node;
			this->length += 1;
		}

		TriangleNode pop(){
			this->length -= 1;
			return this->data[this->length];
		}
	};



	const uint StartRes = 75;
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


	void AddTriangle(PlanetProc* proc, const TriangleNode& node, const double& rad){
		proc->terrain_vertices.push_back(vec4(vec3(node.p3*rad), node.level));
		proc->terrain_vertices.push_back(vec4(vec3(node.p2*rad), node.level));
		proc->terrain_vertices.push_back(vec4(vec3(node.p1*rad), node.level));
	};

	// uint maxdepth = 0;


	inline bool DotprodCull(const uni_vec3& p1, const uni_vec3& p2, const uni_vec3& p3, const uni_vec3& observer){
		return (
			dot( p1, observer ) < 0.75 &&
			dot( p2, observer ) < 0.75 &&
			dot( p3, observer ) < 0.75
		);
	}



	inline bool FrustumCull(const Frustum& frustum, const uni_vec3& p1, const uni_vec3& p2, const uni_vec3& p3, const uni_vec3& observer){
		// auto cullrad = distance(p1, (p1+p2+p3)/3.0)*2.0;
		bool cull = true;
		for (uint i=0; i<4; ++i){
			cull = cull && (dot((p1-observer) - double(frustum.faces[i].w), uni_vec3(frustum.faces[i])) < 0);
			cull = cull && (dot((p2-observer) - double(frustum.faces[i].w), uni_vec3(frustum.faces[i])) < 0);
			cull = cull && (dot((p3-observer) - double(frustum.faces[i].w), uni_vec3(frustum.faces[i])) < 0);
		}
		return cull;
	}



	void Recurse(PlanetProc* proc, const uni_vec3& observer, const double& rad, TempFixedArray& stack){
		while (stack.length > 0u) {
			TriangleNode node = stack.pop();

			// uni_vec3 ave_point = normalize((node.p1 + node.p2 + node.p3) / 3.0);
			// double samp = NoiseFuncs::planet_fbm_far(warp_uvsphere_to_vec3(warp_vec3_to_uvsphere(ave_point)), 8u);
			uni_vec3 tp1 = node.p1;// - normalize(node.p1)*samp*double(rad)*0.01;
			uni_vec3 tp2 = node.p2;// - normalize(node.p2)*samp*double(rad)*0.01;
			uni_vec3 tp3 = node.p3;// - normalize(node.p3)*samp*double(rad)*0.01;


			/// culling
			if (DotprodCull(tp1, tp2, tp3, observer)){ continue; }
			if (FrustumCull(*(proc->frustum), tp1, tp2, tp3, observer)){ continue; }


			uni_vec3 edges[3] = {
		        (tp1 + tp2) / 2.0,
		        (tp2 + tp3) / 2.0,
		        (tp3 + tp1) / 2.0
		    };
			uint8_t edgeDistMask = 0u;

			for (uint i=0; i<3; ++i){
		        edgeDistMask |= (distance(edges[i], observer) > DistanceLevels[node.level]) << i;
		    }

			/// Add Triangle
		    if ( (edgeDistMask >= 7u) || node.level >= MaxLevel ){
				if (node.level >= StartLevel){

					AddTriangle(proc, node, rad);
					// AddTriangle(proc, level, p1 - observer, p2 - observer, p3 - observer);
				}
		        continue;
		    }


			uni_vec3 p[6] = {
	            tp1, tp2, tp3,
	            edges[0], edges[1], edges[2]
	        };
	        uint8_t valid = std::numeric_limits<uint8_t>::max();

			if (edgeDistMask & (1 << 0)){ p[3] = tp1; valid ^= 1 << 0; } // skip triangle 0
	        if (edgeDistMask & (1 << 1)){ p[4] = tp2; valid ^= 1 << 2; } // skip triangle 2
	        if (edgeDistMask & (1 << 2)){ p[5] = tp3; valid ^= 1 << 3; } // skip triangle 3

			for (uint i=0; i<4; ++i){
	            if (!(valid & (1 << i))){ continue; }

				stack.push(TriangleNode{
					normalize( p[RecurseIdx[3 * i + 0]] ),
					normalize( p[RecurseIdx[3 * i + 1]] ),
					normalize( p[RecurseIdx[3 * i + 2]] ),
					node.level+1u
				});
			}

			// maxdepth = std::max<uint>(maxdepth, uint(stack.length));

		}
	}



	// Version without custom stack
	// void Recurse(PlanetProc* proc, const uni_vec3& observer, const float& rad, uni_vec3 p1, uni_vec3 p2, uni_vec3 p3, uint level){
	// 	uni_vec3 ave_point = normalize((p1 + p2 + p3) / 3.0);
	// 	double samp = NoiseFuncs::planet_fbm_far(warp_uvsphere_to_vec3(warp_vec3_to_uvsphere(ave_point)), 8u);
	// 	uni_vec3 tp1 = p1 - normalize(p1)*samp*double(rad)*0.01;
	// 	uni_vec3 tp2 = p2 - normalize(p2)*samp*double(rad)*0.01;
	// 	uni_vec3 tp3 = p3 - normalize(p3)*samp*double(rad)*0.01;
	//
	// 	/// culling
	// 	if (
	//         dot( tp1, observer ) < 0.75 &&
	//         dot( tp2, observer ) < 0.75 &&
	//         dot( tp3, observer ) < 0.75
	//     ){ return; }
	//
	//
	// 	uni_vec3 edges[3] = {
	//         (p1 + p2) / 2.0,
	//         (p2 + p3) / 2.0,
	//         (p3 + p1) / 2.0
	//     };
	//     bool edgeDistMask[3];
	//
	// 	for (uint i=0; i<3; ++i){
	//         edgeDistMask[i] = distance(edges[i], observer) > DistanceLevels[level];
	//     }
	//
	// 	/// Add Triangle
	//     if ( (edgeDistMask[0] && edgeDistMask[1] && edgeDistMask[2]) || level >= MaxLevel ){
	//         if (level >= StartLevel){
	// 			// auto cullrad = distance(p1, (p1+p2+p3)/3.0)*2.0;
	//
	// 			bool cull = true;
	// 			for (uint i=0; i<4; ++i){
	// 				cull = cull && (dot((tp1-observer) - double(proc->frustum->faces[i].w), uni_vec3(proc->frustum->faces[i])) < 0);
	// 				cull = cull && (dot((tp2-observer) - double(proc->frustum->faces[i].w), uni_vec3(proc->frustum->faces[i])) < 0);
	// 				cull = cull && (dot((tp3-observer) - double(proc->frustum->faces[i].w), uni_vec3(proc->frustum->faces[i])) < 0);
	// 			}
	//
	// 			if (!cull){
	// 				AddTriangle(proc, level, p1, p2, p3, rad);
	// 				// AddTriangle(proc, level, p1 - observer, p2 - observer, p3 - observer);
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
    //     bool valid[4] = { true, true, true, true };
	//
	// 	if (edgeDistMask[0]){ p[3] = p1; valid[0] = false; } // skip triangle 0
    //     if (edgeDistMask[1]){ p[4] = p2; valid[2] = false; } // skip triangle 2
    //     if (edgeDistMask[2]){ p[5] = p3; valid[3] = false; } // skip triangle 3
	//
	// 	for (uint i=0; i<4; ++i){
    //         if (!valid[i]){ continue; }
	// 		Recurse(
	// 			proc, observer, rad,
	// 			normalize( p[RecurseIdx[3 * i + 0]] ),
	// 			normalize( p[RecurseIdx[3 * i + 1]] ),
	// 			normalize( p[RecurseIdx[3 * i + 2]] ),
	// 			level+1u
	// 		);
	// 	}
	// }




	void Rebuild(PlanetProc* proc, const uni_vec3 observer){
		double rad = proc->planet->radius / CameraViewpoint::GetScale(proc->scale_type);

		TempFixedArray stack{};

		for (uint i=0; i < 63/3; ++i){
			vec3 p1 = normalize( IcoPoints[Idx[i * 3 + 0]] ); // triangle point 1
			vec3 p2 = normalize( IcoPoints[Idx[i * 3 + 1]] ); // triangle point 2
			vec3 p3 = normalize( IcoPoints[Idx[i * 3 + 2]] ); // triangle point 3
			stack.push(TriangleNode{p1, p2, p3, 0});
		}

		Recurse(proc, observer, rad, stack);
		// std::cout << "max depth: " << maxdepth << '\n';
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




HXShaderInput terrain_heights_compute_inputs[4];
HXShaderInput terrain_compute_inputs[3];
HXShaderUniform terrain_tex_compute_tex_uniform;
const size_t maxStartCount = 5000000;


void PlanetProc::Init(Application& app){
	frustum = &(app.camera.frustum);
	CurrentVertexCount = 0u;

	terrain_vertices.reserve(maxStartCount);


	/// init resources
	cmdbuff = app.hxg.CreateCommandBuffer(
		app.universeRenderer.c_cmdalloc, HX_GRAPHICS_CMDBUFFER_COMPUTE, HX_GRAPHICS_CMDBUFFER_ONCE
	);

	terrain_vertbuffer = app.hxg.CreateStorageBuffer(
		HXSBufferConfig{maxStartCount*sizeof(vec4), HX_GRAPHICS_USAGE_DYNAMIC},
		NULL, 10
	);
	terrain_normalsbuffer = app.hxg.CreateStorageBuffer(
		HXSBufferConfig{maxStartCount*sizeof(vec3), HX_GRAPHICS_USAGE_DYNAMIC},
		NULL, HX_GRAPHICS_VERTEXBUFFER
	);
	terrain_props_ubuff = app.hxg.CreateStorageBuffer(
		HXSBufferConfig{sizeof(TerrainComputeProps), HX_GRAPHICS_USAGE_DYNAMIC},
		NULL, HX_GRAPHICS_UNIFORMBUFFER
	);
	atomicCounterBuffer = app.hxg.CreateStorageBuffer(
		HXSBufferConfig{sizeof(uint), HX_GRAPHICS_USAGE_DYNAMIC},
		&app.universeRenderer.resources.zero_memory, HX_GRAPHICS_SHADERBUFFER
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



	/// init normals gen compute pipeline
	{
		HXComputePipelineConfig c_pipconfig{};
		char* comp_blob = LoadShaderBlob("icosahedron_lod_compute");
		ShaderC::LoadShaderBinary(comp_blob, HX_SHADERC_TYPE_COMPUTE, c_pipconfig.ComputeShader, c_pipconfig.Metadata);
		normals_gen_pip = app.hxg.CreateComputePipeline(c_pipconfig);
		delete comp_blob;

		terrain_heights_compute_inputs[0] = { app.hxg.GetUniform(normals_gen_pip, "VertexBuffer"), app.hxg.GetData(terrain_vertbuffer) };
		terrain_heights_compute_inputs[1] = { app.hxg.GetUniform(normals_gen_pip, "NormalsBuffer"), app.hxg.GetData(terrain_normalsbuffer) };
		terrain_heights_compute_inputs[2] = { app.hxg.GetUniform(normals_gen_pip, "heightTex"), app.hxg.GetData(terrain_height_tex), app.hxg.GetData(app.linearSampler) };
		terrain_heights_compute_inputs[3] = { app.hxg.GetUniform(normals_gen_pip, "Props"), &CurrentVertexCount };

	}; {
		HXComputePipelineConfig c_pipconfig{};
		char* comp_blob = LoadShaderBlob("terrain_texture_gen_compute");
		ShaderC::LoadShaderBinary(comp_blob, HX_SHADERC_TYPE_COMPUTE, c_pipconfig.ComputeShader, c_pipconfig.Metadata);
		terrain_tex_pip = app.hxg.CreateComputePipeline(c_pipconfig);
		delete comp_blob;

		terrain_tex_compute_tex_uniform = app.hxg.GetUniform(terrain_tex_pip, "heightTex");
	}



#ifdef _USE_GPU_ICO_LOD
	/// init icosahedron compute pipeline
	{
		HXComputePipelineConfig c_pipconfig{};
		char* comp_blob = LoadShaderBlob("ico_terrain_gen_compute");
		ShaderC::LoadShaderBinary(comp_blob, HX_SHADERC_TYPE_COMPUTE, c_pipconfig.ComputeShader, c_pipconfig.Metadata);
		terrain_gen_pip = app.hxg.CreateComputePipeline(c_pipconfig);
		delete comp_blob;

		terrain_compute_inputs[0] = { app.hxg.GetUniform(terrain_gen_pip, "Props"), &terrain_compute_props };
		terrain_compute_inputs[1] = { app.hxg.GetUniform(terrain_gen_pip, "VertBuffer"), app.hxg.GetData(terrain_vertbuffer) };
		terrain_compute_inputs[2] = { app.hxg.GetUniform(terrain_gen_pip, "AtomicCounter"), app.hxg.GetData(atomicCounterBuffer) };
	};

#else
	/// init icosahedron LOD thread
	terrain_thread = std::thread(IcosahedronTerrainGenHandler, &app, this);
#endif


}




bool updatecam = true;
void PlanetProc::Update(Application& app, const uni_vec3& absolute_cam_pos){
	uni_vec3 drenderpos = (*absolute_pos);

	if (app.hxi.GetKey(app.keyboard, HX_INPUT_VK_CHAR_J, HX_INPUT_KEYSTATE_PRESSED)){
		std::cout << "stopped updating cam" << '\n';
		updatecam = !updatecam;
	}

	if (updatecam){
		observer = drenderpos / uni_float(planet->radius / CameraViewpoint::GetScale(scale_type));
		// std::cout << observer.x << " -- " << observer.y <<  " -- " << observer.z << '\n';

	}
	planet_renderpos = vec3(-drenderpos);
	// std::cout << (*renderpos).x << " -- " << (*renderpos).y << " -- " << (*renderpos).z << '\n';


#ifdef _USE_GPU_ICO_LOD
	/// update push_constant props
	terrain_compute_props.observer = vec3(observer);
	terrain_compute_props.radius = float(planet->radius / CameraViewpoint::GetScale(scale_type));

	IcosahedronTerrainComputePipeline(app);
	app.hxg.ExecuteCommands(cmdbuff);
	// app.hxg.WaitForFenceCPU(gen_fence);

	uint* mapped = (uint*)app.hxg.MapStorageBuffer(atomicCounterBuffer, 0, sizeof(uint), HX_GRAPHICS_MEM_ACCESS_RW);
	CurrentVertexCount = *mapped;
	*mapped = 0u;
	app.hxg.UnmapStorageBuffer(atomicCounterBuffer);
	// std::cout << "vertcount: " << CurrentVertexCount << '\n';

	/// run normals gen compute shader
	GenerateTerrainHeightsAndNormals(app, CurrentVertexCount);
	app.hxg.ExecuteCommands(cmdbuff);

#else
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
		GenerateTerrainHeightsAndNormals(app, CurrentVertexCount);

		terrain_thread_lock.unlock();

	}else{
		terrainGenState = _IDLE;
	}

	app.hxg.ExecuteCommands(cmdbuff);

#endif





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
	// app.hxg.ExecuteCommands(cmdbuff);
	// std::cout << "dispatched: " << discmd.x << " -- " << discmd.y << '\n';

	terrainGenState = _REBUILD_TERRAIN_GEN;
}




void PlanetProc::IcosahedronTerrainComputePipeline(Application& app){
	HXWaitForFenceCmd wait{};
	wait.fence = app.hxg.GetData(gen_fence);

	HXSetComputePipelineCmd comp_pip{};
	comp_pip.pipeline = app.hxg.GetData(terrain_gen_pip);

	HXUpdateShaderUniformsCmd comp_uniforms{};
	comp_uniforms.Inputs = terrain_compute_inputs;
	comp_uniforms.Length = HX_LENGTH_C_ARRAY(terrain_compute_inputs);

	HXDispatchComputeCmd discmd{};
	discmd.x = 63/3;
	discmd.y = 1;
	discmd.z = 1;

	HXInsertFenceCmd fnc{};
	fnc.fence = app.hxg.GetData(gen_fence);

	app.hxg.InsertCommands(cmdbuff, comp_pip, comp_uniforms, wait, discmd, fnc);
}




void PlanetProc::GenerateTerrainHeightsAndNormals(Application& app, size_t vertcount){
	// HXWaitForFenceCmd wait{};
	// wait.fence = app.hxg.GetData(gen_fence);

	HXSetComputePipelineCmd comp_pip{};
	comp_pip.pipeline = app.hxg.GetData(normals_gen_pip);

	HXUpdateShaderUniformsCmd comp_uniforms{};
	comp_uniforms.Inputs = terrain_heights_compute_inputs;
	comp_uniforms.Length = HX_LENGTH_C_ARRAY(terrain_heights_compute_inputs);

	HXDispatchComputeCmd discmd{};
	discmd.x = size_t(ceil(double(vertcount / 3) / 1024));
	discmd.y = 1;
	discmd.z = 1;

	HXInsertFenceCmd fnc{};
	fnc.fence = app.hxg.GetData(gen_fence);

	app.hxg.InsertCommands(cmdbuff, comp_pip, comp_uniforms, discmd, fnc);
}
