
HXShaderUniform planets_far_renderpos_uniform;
HXShaderUniform planets_far_surfacetex_uniform;
HXShaderUniform planets_far_normalstex_uniform;
HXShaderUniform planets_texture_gen_input_uniforms[3];
Utils::TypedVector<PlanetFarProperties> planets_far_temp_variables;





void UniverseRenderer::InitPlanets(Application& app){
	planets.cmdbuff = app.hxg.CreateCommandBuffer(g_cmdalloc, HX_GRAPHICS_CMDBUFFER_GRAPHICS, HX_GRAPHICS_CMDBUFFER_ONCE);



	/// init surface texture
	HXTextureConfig texconfig{};
	texconfig.Type = HX_GRAPHICS_TEXTURE_2D;
	texconfig.Depth = 1;
	texconfig.Usage = HX_GRAPHICS_USAGE_DYNAMIC;
	texconfig.GenerateMips = false;

	for (uint i=0; i<_PLANET_TEXTURE_COUNT; ++i){
		texconfig.Width = size_t(_PLANET_TEXTURE_WIDTH / pow(4.0, float(i)));
		texconfig.Height = size_t(_PLANET_TEXTURE_WIDTH / pow(4.0, float(i)));

		texconfig.Format = HX_R8_G8_B8_A8_BYTE;
		planets.surfaceTex[i] = app.hxg.CreateTexture(texconfig, NULL);
		texconfig.Format = HX_R16_G16_B16_A16_FLOAT;
		planets.normalsTex[i] = app.hxg.CreateTexture(texconfig, NULL);
	}



	/// init planet tex indices buffer
	planets.planet_tex_indices_buffer = app.hxg.CreateStorageBuffer(
		HXSBufferConfig{ sizeof(PlanetTextureProps), HX_GRAPHICS_USAGE_DYNAMIC }, NULL,
		HX_GRAPHICS_UNIFORMBUFFER
	);



	/// init planet tex compute pipeline
	{
		HXComputePipelineConfig c_pipconfig{};
		char* comp_blob = LoadShaderBlob("planet_texture_gen_compute");
		ShaderC::LoadShaderBinary(comp_blob, HX_SHADERC_TYPE_COMPUTE, c_pipconfig.ComputeShader, c_pipconfig.Metadata);
		planets.planet_tex_pip = app.hxg.CreateComputePipeline(c_pipconfig);
		delete comp_blob;
	}



	/// init far pipeline
	{
		HXGraphicsPipelineConfig g_pipconfig{};
		g_pipconfig.DepthOperation = HX_GRAPHICS_DEPTH_OP_GREATER;
		g_pipconfig.CullOperation = HX_GRAPHICS_CULL_OP_BACK;

		char* vert_blob = LoadShaderBlob("planets_far_mask_vertex");
		char* frag_blob = LoadShaderBlob("planets_far_mask_fragment");

		ShaderC::LoadShaderBinary(vert_blob, HX_SHADERC_TYPE_VERTEX, g_pipconfig.VertexShader, g_pipconfig.Metadata);
		ShaderC::LoadShaderBinary(frag_blob, HX_SHADERC_TYPE_FRAGMENT, g_pipconfig.FragmentShader, g_pipconfig.Metadata);

		CreateDepthColorRendererBundle(app, planets.rfar, planets.scale, HX_R16_G16_B16_A16_FLOAT, g_pipconfig);

		delete vert_blob;
		delete frag_blob;
	}



	/// init near pipeline
	planets.terrain_vdesc = app.hxg.CreateVertexBufferDescriptor(
		HXVBufferDescPair{
			app.hxg.GetData(app.universe.planet_sys.terrain_vertbuffer), 0, sizeof(vec4),
			app.hxg.GetConfig(app.universe.planet_sys.terrain_vertbuffer).Size/sizeof(vec4), 0, 4
		},
		HXVBufferDescPair{
			app.hxg.GetData(app.universe.planet_sys.terrain_normalsbuffer), 0, sizeof(vec3),
			app.hxg.GetConfig(app.universe.planet_sys.terrain_normalsbuffer).Size/sizeof(vec3), 1, 3
		}
	);

	{
		HXGraphicsPipelineConfig g_pipconfig{};
		g_pipconfig.DepthOperation = HX_GRAPHICS_DEPTH_OP_GREATER;
		g_pipconfig.CullOperation = HX_GRAPHICS_CULL_OP_BACK;

		char* vert_blob = LoadShaderBlob("planets_near_mask_vertex");
		char* frag_blob = LoadShaderBlob("planets_near_mask_fragment");

		ShaderC::LoadShaderBinary(vert_blob, HX_SHADERC_TYPE_VERTEX, g_pipconfig.VertexShader, g_pipconfig.Metadata);
		ShaderC::LoadShaderBinary(frag_blob, HX_SHADERC_TYPE_FRAGMENT, g_pipconfig.FragmentShader, g_pipconfig.Metadata);

		CreateDepthColorRendererBundle(app, planets.rnear, planets.scale, HX_R16_G16_B16_A16_FLOAT, g_pipconfig);

		delete vert_blob;
		delete frag_blob;
	}



	/// get push constants
	planets_far_renderpos_uniform = app.hxg.GetUniform(planets.rfar.pipeline, "Properties");
	planets_far_surfacetex_uniform = app.hxg.GetUniform(planets.rfar.pipeline, "diffuse_tex");
	planets_far_normalstex_uniform = app.hxg.GetUniform(planets.rfar.pipeline, "normals_tex");

	// planets_texture_gen_input_uniforms[0] = app.hxg.GetUniform(planets.planet_tex_pip, "diffuse_tex[0]");
	// planets_texture_gen_input_uniforms[1] = app.hxg.GetUniform(planets.planet_tex_pip, "normals_tex[0]");
	// planets_texture_gen_input_uniforms[2] = app.hxg.GetUniform(planets.planet_tex_pip, "Props");

}




void UniverseRenderer::DrawPlanetsFar(Application& app){

	// HXWaitForFenceCmd wait{};
	// wait.fence = app.hxg.GetData(planets.rfar.fence);

	HXSetGraphicsPipelineCmd setpip{};
	setpip.pipeline = app.hxg.GetData(planets.rfar.pipeline);

	HXSetRenderpassCmd setrp{};
	setrp.renderpass = app.hxg.GetData(planets.rfar.renderpass);
	setrp.viewport = uvec4(0,0, app.current_width/planets.scale, app.current_height/planets.scale);

	HXUpdateShaderUniformsCmd comp_uniforms{};
	comp_uniforms.Inputs = planets_far_mask_inputs;
	comp_uniforms.Length = HX_LENGTH_C_ARRAY(planets_far_mask_inputs);

	app.hxg.InsertCommands(planets.cmdbuff, setpip, setrp, comp_uniforms);


	planets_far_temp_variables.clearFast();
	planets_far_temp_variables.reserve(app.universe.solar_system_sys.planets.size());

	int surface_tex_index_counter = 0;

	for (uint i=0; i<app.universe.solar_system_sys.planets.size(); ++i){
		PlanetFarProperties props{};
		const Planet& planet = app.universe.solar_system_sys.GetClosestPlanet(i);
		uint flags = app.universe.solar_system_sys.GetClosestPlanetOrbit(i).flags;

		/// check if this is the current planet or not, if so then don't draw the planet
		// if (app.universe.IsCurrentPlanet(app, planet)){
		// 	continue;
		// }

		if (flags & _SOLAR_SYSTEM_ORBIT_PLANET_FLAG){
			props.radius = float(planet.radius / CameraViewpoint::GetScale(E_VIEW_LAYER_PLANET_ORBIT));
		}else
		if (flags & _SOLAR_SYSTEM_ORBIT_MOON_FLAG){
			props.radius = float(planet.radius / CameraViewpoint::GetScale(E_VIEW_LAYER_MOON_ORBIT));
		}
		props.renderpos = app.universe.solar_system_sys.GetClosestPlanetOrbitTempPos(i).renderpos;
		props.lightDir = vec4(normalize(
			uni_vec3(app.universe.solar_system_sys.star_renderpos) - app.universe.solar_system_sys.GetClosestPlanetOrbitTempPos(i).absolute_pos
		), 0);

		/// check if we're close enough to draw at all
		if (length(props.renderpos) > props.radius*1000.0){
			if (i == 0){
				planets.closestPlanetInView = false;
				break;
			}
			continue;
		}

		/// set this to true if the closest planet is in view, false if not
		/// if it was not then theres no point in rendering anything else planet related,
		/// hence the point of this boolean
		/// NOTE_TO_SELF: please don't delete it in case you forget
		if (i == 0){ planets.closestPlanetInView = true; }


		/// configure surface texture index
		props.surfaceTextureIndex = surface_tex_index_counter;
		surface_tex_index_counter += 1;

		size_t index = planets_far_temp_variables.push_back(props);



		HXUpdateShaderUniformCmd renderpos_uniform{};
		renderpos_uniform.Uniform = planets_far_renderpos_uniform;
		renderpos_uniform.Data = planets_far_temp_variables+index;

		HXUpdateShaderUniformCmd surfacetex_uniform{};
		surfacetex_uniform.Uniform = planets_far_surfacetex_uniform;
		surfacetex_uniform.Data = app.hxg.GetData(planets.surfaceTex[min(3, props.surfaceTextureIndex)]);
		surfacetex_uniform.Extra = app.hxg.GetData(app.linearSampler);

		HXUpdateShaderUniformCmd normalstex_uniform{};
		normalstex_uniform.Uniform = planets_far_normalstex_uniform;
		normalstex_uniform.Data = app.hxg.GetData(planets.normalsTex[min(3, props.surfaceTextureIndex)]);
		normalstex_uniform.Extra = app.hxg.GetData(app.linearSampler);

		HXIndexDrawCall drawcall{};
		drawcall.StartIndex = 0;
		drawcall.Count = std::numeric_limits<uint32_t>::max();
		drawcall.InstanceCount = 1;
		drawcall.VDesc = app.hxg.GetData(resources.sphere_vdesc);
		drawcall.IDesc = app.hxg.GetData(resources.sphere_idesc);
		drawcall.Mode = HX_GRAPHICS_DRAW_TRIANGLE;

		app.hxg.InsertCommands(planets.cmdbuff, renderpos_uniform, surfacetex_uniform, normalstex_uniform, drawcall);
	}


	// HXInsertFenceCmd fnc{};
	// fnc.fence = app.hxg.GetData(planets.rfar.fence);
	// app.hxg.InsertCommands(planets.cmdbuff, fnc);

	app.hxg.ExecuteCommands(planets.cmdbuff);
}


HXTexture& UniverseRenderer::GetPlanetsFarTex(Application& app){
	return planets.rfar.rendertex;
}




void UniverseRenderer::DrawPlanetsNear(Application& app){
	if (!planets.closestPlanetInView){ return; }

	// HXWaitForFenceCmd wait{};
	// wait.fence = app.hxg.GetData(galaxies.rnear.fence);

	HXSetGraphicsPipelineCmd setpip{};
	setpip.pipeline = app.hxg.GetData(planets.rnear.pipeline);

	HXSetRenderpassCmd setrp{};
	setrp.renderpass = app.hxg.GetData(planets.rnear.renderpass);
	setrp.viewport = uvec4(0,0, app.current_width/planets.scale, app.current_height/planets.scale);

	HXUpdateShaderUniformsCmd comp_uniforms{};
	comp_uniforms.Inputs = planets_near_mask_inputs;
	comp_uniforms.Length = HX_LENGTH_C_ARRAY(planets_near_mask_inputs);

	HXUpdateShaderUniformCmd lightDir_uniform{};
	lightDir_uniform.Uniform = planets_near_mask_inputs[1].Uniform;
	lightDir_uniform.Uniform.size = sizeof(vec4);
	lightDir_uniform.Data = &(planets_far_temp_variables[0].lightDir);
	lightDir_uniform.Extra = sizeof(vec4);

	HXWaitForFenceCmd waitforgen{};
	waitforgen.fence = app.hxg.GetData(app.universe.planet_sys.gen_fence);

	HXVertexDrawCall drawcall{};
	drawcall.StartVertex = 0;
	drawcall.Count = app.universe.planet_sys.GetCurrentVertexCount();
	drawcall.InstanceCount = 1;
	drawcall.VDesc = app.hxg.GetData(planets.terrain_vdesc);
	drawcall.Mode = app.settings.wireframe ? HX_GRAPHICS_DRAW_LINE_STRIP : HX_GRAPHICS_DRAW_TRIANGLE;

	app.hxg.InsertCommands(planets.cmdbuff, setpip, setrp, comp_uniforms, lightDir_uniform, waitforgen, drawcall);


	// HXInsertFenceCmd fnc{};
	// fnc.fence = app.hxg.GetData(planets.rnear.fence);
	// app.hxg.InsertCommands(planets.cmdbuff, fnc);

	app.hxg.ExecuteCommands(planets.cmdbuff);
}


HXTexture& UniverseRenderer::GetPlanetsNearTex(Application& app){
	return planets.rnear.rendertex;
}




void UniverseRenderer::RegeneratePlanetTextures(Application& app){
	if (!planets.closestPlanetInView){ return; }

	size_t dispatchCount = 0;
	uint8_t* cl = app.universe.solar_system_sys.GetClosestPlanets();

	// for (uint i=0; i<_PLANET_TEXTURE_COUNT; ++i){
	// 	if(planets.previousClosests[i] == cl[i]){
	// 		continue;
	// 	}
	//
	// 	if (dispatchCount == 0){
	// 		HXSetComputePipelineCmd setpip{};
	// 		setpip.pipeline = app.hxg.GetData(planets.planet_tex_pip);
	// 		app.hxg.InsertCommands(planets.cmdbuff, setpip);
	// 	}
	//
	// 	uint width = uint(_PLANET_TEXTURE_WIDTH / pow(4.0, float(i)));
	// 	planets.push_constant_width[dispatchCount] = width;
	//
	//
	// 	HXUpdateShaderUniformCmd difftexcmd{};
	// 	difftexcmd.Uniform = planets_texture_gen_input_uniforms[0];
	// 	difftexcmd.Data = app.hxg.GetData(planets.surfaceTex[dispatchCount]);
	//
	// 	HXUpdateShaderUniformCmd normtexcmd{};
	// 	normtexcmd.Uniform = planets_texture_gen_input_uniforms[1];
	// 	normtexcmd.Data = app.hxg.GetData(planets.normalsTex[dispatchCount]);
	//
	// 	HXUpdateShaderUniformCmd pushconstcmd{};
	// 	pushconstcmd.Uniform = planets_texture_gen_input_uniforms[2];
	// 	pushconstcmd.Data = planets.push_constant_width+dispatchCount;
	//
	//
	// 	HXDispatchComputeCmd discmd{};
	// 	discmd.x = width / 32;
	// 	discmd.y = width / 32;
	// 	discmd.z = 1;
	//
	// 	app.hxg.InsertCommands(planets.cmdbuff, difftexcmd, normtexcmd, pushconstcmd, discmd);
	//
	//
	// 	planets.previousClosests[i] = cl[i];
	// 	dispatchCount += 1;
	// }


	// HXSetComputePipelineCmd setpip{};
	// setpip.pipeline = app.hxg.GetData(galaxies.rnear.pipeline);
	//
	// HXUpdateShaderUniformsCmd comp_uniforms{};
	// comp_uniforms.Inputs = galaxy_mask_near_inputs;
	// comp_uniforms.Length = HX_LENGTH_C_ARRAY(galaxy_mask_near_inputs);
	//
	// HXDispatchComputeCmd discmd{};
	// discmd.x = size_t(ceil(float( width ) / 32));
	// discmd.y = size_t(ceil(float( width ) / 32));
	// discmd.z = 1;
	//
	// HXInsertFenceCmd fnc{};
	// fnc.fence = app.hxg.GetData(galaxies.rnear.fence);
	//
	// // GLint vec[3];
	// // glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, vec+0);
	// // glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, vec+1);
	// // glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, vec+2);
	// // std::cout << vec[0] << " -- " << vec[1] << " -- " << vec[2] << '\n';
	//
	// app.hxg.InsertCommands(galaxies.cmdbuff_compute, setpip, comp_uniforms, discmd, fnc);
	// app.hxg.ExecuteCommands(galaxies.cmdbuff_compute);



	for (uint i=0; i<_PLANET_TEXTURE_COUNT; ++i){
		if(planets.previousClosests[i] == cl[i]){
			continue;
		}

		planets.currentTexIndices[dispatchCount] = i;
		planets.previousClosests[i] = cl[i];
		dispatchCount += 1;
	}
	if (!dispatchCount){ return; }

	HXCopyStorageBufferCmd constcmd{};
	constcmd.source = &planets.currentTexIndices;
	constcmd.destination = app.hxg.GetData(planets.planet_tex_indices_buffer);
	constcmd.sourceOffset = 0;
	constcmd.destinationOffset = 0;
	constcmd.size = sizeof(PlanetTextureProps);
	app.hxg.CopyStorageBuffer(constcmd, HX_GRAPHICS_COPY_CPU_GPU);


	size_t maxwidth = size_t(_PLANET_TEXTURE_WIDTH / pow(4.0, float( planets.currentTexIndices[0] )));


	HXSetComputePipelineCmd setpip{};
	setpip.pipeline = app.hxg.GetData(planets.planet_tex_pip);

	HXUpdateShaderUniformsCmd comp_uniforms{};
	comp_uniforms.Inputs = planets_texture_gen_inputs;
	comp_uniforms.Length = HX_LENGTH_C_ARRAY(planets_texture_gen_inputs);

	HXDispatchComputeCmd discmd{};
	discmd.x = maxwidth / 32;
	discmd.y = maxwidth / 32;
	discmd.z = dispatchCount;
	std::cout << "dis count: " << dispatchCount;
	for (uint i=0; i<dispatchCount; ++i){ std::cout << " --" << (_PLANET_TEXTURE_WIDTH / pow(4.0, float( planets.currentTexIndices[i] ))); }
	std::cout << '\n';

	app.hxg.InsertCommands(planets.cmdbuff, setpip, comp_uniforms, discmd);
	app.hxg.ExecuteCommands(planets.cmdbuff);
}
