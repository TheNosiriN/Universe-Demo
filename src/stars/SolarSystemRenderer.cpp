
HXShaderUniform orbits_renderpos_uniform;
HXShaderUniform star_renderpos_uniform;


void UniverseRenderer::InitSolarSystem(Application& app){
	solar_system.cmdbuff = app.hxg.CreateCommandBuffer(g_cmdalloc, HX_GRAPHICS_CMDBUFFER_GRAPHICS, HX_GRAPHICS_CMDBUFFER_ONCE);


	/// init star properties buffer
	solar_system.props = app.hxg.CreateStorageBuffer(
		HXSBufferConfig{ sizeof(SolarSystemProperties), HX_GRAPHICS_USAGE_DYNAMIC }, NULL,
		HX_GRAPHICS_UNIFORMBUFFER
	);


	/// init orbits buffer
	solar_system.orbits = app.hxg.CreateStorageBuffer(
		HXSBufferConfig{ sizeof(SolarSystemOrbit) * _SOLAR_SYSTEM_MAX_ORBITS, HX_GRAPHICS_USAGE_DYNAMIC }, NULL,
		HX_GRAPHICS_SHADERBUFFER
	);


	/// init orbit updates buffer
	solar_system.orbit_updates = app.hxg.CreateStorageBuffer(
		HXSBufferConfig{ sizeof(SolarSystemOrbitUpdate) * _SOLAR_SYSTEM_MAX_ORBITS, HX_GRAPHICS_USAGE_DYNAMIC }, NULL,
		HX_GRAPHICS_SHADERBUFFER
	);



	/// init rings pipeline
	HXGraphicsPipelineConfig g_pipconfig{};
	g_pipconfig.DepthOperation = HX_GRAPHICS_DEPTH_OP_GREATER_EQUAL;
	g_pipconfig.BlendConfig[0].Operation = HX_GRAPHICS_BLEND_OP_NONE;
	g_pipconfig.CullOperation = HX_GRAPHICS_CULL_OP_BACK;

	char* vert_blob = LoadShaderBlob("rings_mask_vertex");
	char* frag_blob = LoadShaderBlob("rings_mask_fragment");
	ShaderC::LoadShaderBinary(vert_blob, HX_SHADERC_TYPE_VERTEX, g_pipconfig.VertexShader, g_pipconfig.Metadata);
	ShaderC::LoadShaderBinary(frag_blob, HX_SHADERC_TYPE_FRAGMENT, g_pipconfig.FragmentShader, g_pipconfig.Metadata);

	CreateDefaultRendererBundle(app, solar_system.rings, solar_system.scale, HX_R16_G16_B16_A16_FLOAT, g_pipconfig);



	/// init star pipeline



	/// get push constant uniforms
	orbits_renderpos_uniform = app.hxg.GetUniform(solar_system.rings.pipeline, "RenderPos");

}

void UniverseRenderer::DrawPlanetOrbits(Application& app){
	HXVertexDrawCall drawcall{};
	drawcall.Count = 128;
	drawcall.InstanceCount = app.universe.solar_system_sys.GetPlanetCount();
	drawcall.VDesc = NULL;
	drawcall.Mode = HX_GRAPHICS_DRAW_LINE_LOOP;

	HXUpdateShaderUniformsCmd comp_uniforms{};
	comp_uniforms.Inputs = solar_system_rings_mask_inputs;
	comp_uniforms.Length = HX_LENGTH_C_ARRAY(solar_system_rings_mask_inputs);

	HXUpdateShaderUniformCmd renderpos_uniform{};
	renderpos_uniform.Uniform = orbits_renderpos_uniform;
	renderpos_uniform.Data = &app.universe.solar_system_sys.star_renderpos;

	HXUpdateShaderUniformCmd offset_uniform{};
	offset_uniform.Uniform = orbits_renderpos_uniform;
	offset_uniform.Uniform.size = sizeof(uint);
	offset_uniform.Extra = sizeof(vec3);
	offset_uniform.Data = &resources.zero_memory;

	app.hxg.InsertCommands(solar_system.cmdbuff, renderpos_uniform, offset_uniform, drawcall);
}


void UniverseRenderer::DrawMoonOrbits(Application& app){
	for (uint i=0; i<app.universe.solar_system_sys.planetCount; ++i){
		const Planet& planet = app.universe.solar_system_sys.GetClosestPlanet(i);
		const vec3& renderpos = app.universe.solar_system_sys.GetClosestPlanetOrbitTempPos(i).renderpos;

		/// check if we're close enough to draw at all
		if (length(renderpos) > float(planet.hill_sphere / CameraViewpoint::GetScale(E_VIEW_LAYER_PLANET_ORBIT)) * 500.0f){
			continue;
		}

		HXVertexDrawCall drawcall{};
		drawcall.Count = 128;
		drawcall.InstanceCount = planet.orbitCount;
		drawcall.VDesc = NULL;
		drawcall.Mode = HX_GRAPHICS_DRAW_LINE_LOOP;

		HXUpdateShaderUniformCmd renderpos_uniform{};
		renderpos_uniform.Uniform = orbits_renderpos_uniform;
		renderpos_uniform.Data = &renderpos;

		HXUpdateShaderUniformCmd offset_uniform{};
		offset_uniform.Uniform = orbits_renderpos_uniform;
		offset_uniform.Uniform.size = sizeof(uint);
		offset_uniform.Extra = sizeof(vec3);	/// this is the offset when dealing with push constants
		offset_uniform.Data = &planet.moonOrbitOffset;

		app.hxg.InsertCommands(solar_system.cmdbuff, renderpos_uniform, offset_uniform, drawcall);
	}
}


void UniverseRenderer::DrawSolarSystemRings(Application& app){
	HXSetGraphicsPipelineCmd setpip{};
	setpip.pipeline = app.hxg.GetData(solar_system.rings.pipeline);

	HXSetRenderpassCmd setrp{};
	setrp.renderpass = app.hxg.GetData(solar_system.rings.renderpass);
	setrp.viewport = uvec4(0,0, app.current_width/solar_system.scale, app.current_height/solar_system.scale);

	app.hxg.InsertCommands(solar_system.cmdbuff, setpip, setrp);


	if (app.universe.solar_system_sys.GetPlanetCount() && app.settings.renderStellarOrbits){
		HXUpdateShaderUniformsCmd comp_uniforms{};
		comp_uniforms.Inputs = solar_system_rings_mask_inputs;
		comp_uniforms.Length = HX_LENGTH_C_ARRAY(solar_system_rings_mask_inputs);
		app.hxg.InsertCommands(solar_system.cmdbuff, comp_uniforms);

		DrawPlanetOrbits(app);
		DrawMoonOrbits(app);
	}


	// HXInsertFenceCmd fnc{};
	// fnc.fence = &solar_system.rings.fence;
	// app.hxg.InsertCommands(solar_system.cmdbuff, fnc);

	app.hxg.ExecuteCommands(solar_system.cmdbuff);
}


void UniverseRenderer::DrawSolarSystemStar(Application& app){

}


void UniverseRenderer::UpdateSolarSystemPropsBuffer(Application& app, SolarSystemProperties* upd){
	HXCopyStorageBufferCmd copybuffcmd{};
	copybuffcmd.source = upd;
	copybuffcmd.destination = app.hxg.GetData(solar_system.props);
	copybuffcmd.sourceOffset = 0;
	copybuffcmd.destinationOffset = 0;
	copybuffcmd.size = sizeof(SolarSystemProperties);
	app.hxg.CopyStorageBuffer(copybuffcmd, HX_GRAPHICS_COPY_CPU_GPU);
}


void UniverseRenderer::UpdateSolarSystemOrbitsBuffer(Application& app, SolarSystemOrbit* upd, size_t count){
	HXCopyStorageBufferCmd copybuffcmd{};
	copybuffcmd.source = upd;
	copybuffcmd.destination = app.hxg.GetData(solar_system.orbits);
	copybuffcmd.sourceOffset = 0;
	copybuffcmd.destinationOffset = 0;
	copybuffcmd.size = sizeof(SolarSystemOrbit) * count;
	app.hxg.CopyStorageBuffer(copybuffcmd, HX_GRAPHICS_COPY_CPU_GPU);
}


HXTexture& UniverseRenderer::GetSolarSystemRingsTex(Application& app){
	return solar_system.rings.rendertex;
}
