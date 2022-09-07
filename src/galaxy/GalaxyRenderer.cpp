
void UniverseRenderer::InitGalaxies(Application& app){
	galaxies.cmdbuff = app.hxg.CreateCommandBuffer(g_cmdalloc, HX_GRAPHICS_CMDBUFFER_GRAPHICS, HX_GRAPHICS_CMDBUFFER_ONCE);
	galaxies.cmdbuff_compute = app.hxg.CreateCommandBuffer(c_cmdalloc, HX_GRAPHICS_CMDBUFFER_COMPUTE, HX_GRAPHICS_CMDBUFFER_ONCE);


	/// init near storage buffer
	galaxies.current_props = app.hxg.CreateStorageBuffer(
		HXSBufferConfig{ sizeof(CurrentGalaxyProps), HX_GRAPHICS_USAGE_DYNAMIC }, NULL,
		HX_GRAPHICS_UNIFORMBUFFER
	);


	/// init peipelines
	/// near
	HXComputePipelineConfig c_pipconfig{};
	char* comp_blob = LoadShaderBlob("galaxy_mask_near_compute");
	ShaderC::LoadShaderBinary(comp_blob, HX_SHADERC_TYPE_COMPUTE, c_pipconfig.ComputeShader, c_pipconfig.Metadata);
	CreateDefaultComputeBundle(app, galaxies.rnear, galaxies.near_scale, HX_R16_G16_B16_A16_FLOAT, c_pipconfig);


	/// far
	HXGraphicsPipelineConfig g_pipconfig{};
	g_pipconfig.DepthOperation = HX_GRAPHICS_DEPTH_OP_GREATER_EQUAL; /// because reverse depth
	g_pipconfig.BlendConfig[0].Operation = HX_GRAPHICS_BLEND_OP_ADD;
	g_pipconfig.BlendConfig[0].SrcFactor = HX_GRAPHICS_BLEND_ONE;
	g_pipconfig.BlendConfig[0].DstFactor = HX_GRAPHICS_BLEND_ONE;
	g_pipconfig.BlendCount = 1;
	g_pipconfig.CullOperation = HX_GRAPHICS_CULL_OP_BACK;

	char* vert_blob = LoadShaderBlob("galaxy_mask_vertex");
	ShaderC::LoadShaderBinary(vert_blob, HX_SHADERC_TYPE_VERTEX, g_pipconfig.VertexShader, g_pipconfig.Metadata);
	char* frag_blob = LoadShaderBlob("galaxy_mask_fragment");
	ShaderC::LoadShaderBinary(frag_blob, HX_SHADERC_TYPE_FRAGMENT, g_pipconfig.FragmentShader, g_pipconfig.Metadata);

	CreateDefaultRendererBundle(app, galaxies.rfar, galaxies.scale, HX_R16_G16_B16_A16_FLOAT, g_pipconfig);


	delete comp_blob;
	delete vert_blob;
	delete frag_blob;
}


void UniverseRenderer::DrawFarGalaxies(Application& app){

	HXWaitForFenceCmd wait{};
	wait.fence = app.hxg.GetData(galaxies.rfar.fence);

	HXSetGraphicsPipelineCmd setpip{};
	setpip.pipeline = app.hxg.GetData(galaxies.rfar.pipeline);

	HXSetRenderpassCmd setrp{};
	setrp.renderpass = app.hxg.GetData(galaxies.rfar.renderpass);
	setrp.viewport = uvec4(0,0, app.current_width/galaxies.scale, app.current_height/galaxies.scale);

	HXUpdateShaderUniformsCmd comp_uniforms{};
	comp_uniforms.Inputs = galaxy_mask_inputs;

	comp_uniforms.Length = HX_LENGTH_C_ARRAY(galaxy_mask_inputs);
	HXVertexDrawCall drawcall{};
	drawcall.Count = 14;
	drawcall.InstanceCount = app.universe.galaxy_psys.GetCurrentSize();
	drawcall.VDesc = NULL;
	drawcall.Mode = HX_GRAPHICS_DRAW_TRIANGLE_STRIP;

	HXInsertFenceCmd fnc{};
	fnc.fence = app.hxg.GetData(galaxies.rfar.fence);

	app.hxg.InsertCommands(galaxies.cmdbuff, setpip, setrp, comp_uniforms, wait, drawcall, fnc);
	app.hxg.ExecuteCommands(galaxies.cmdbuff);
}


void UniverseRenderer::DrawNearGalaxies(Application& app){

	HXWaitForFenceCmd wait{};
	wait.fence = app.hxg.GetData(galaxies.rnear.fence);

	HXSetComputePipelineCmd setpip{};
	setpip.pipeline = app.hxg.GetData(galaxies.rnear.pipeline);

	HXUpdateShaderUniformsCmd comp_uniforms{};
	comp_uniforms.Inputs = galaxy_mask_near_inputs;
	comp_uniforms.Length = HX_LENGTH_C_ARRAY(galaxy_mask_near_inputs);

	HXDispatchComputeCmd discmd{};
	discmd.x = size_t(ceil(float(app.current_width/galaxies.near_scale) / 32));
	discmd.y = size_t(ceil(float(app.current_height/galaxies.near_scale) / 32));
	discmd.z = 1;

	HXInsertFenceCmd fnc{};
	fnc.fence = app.hxg.GetData(galaxies.rnear.fence);

	// GLint vec[3];
	// glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, vec+0);
	// glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, vec+1);
	// glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, vec+2);
	// std::cout << vec[0] << " -- " << vec[1] << " -- " << vec[2] << '\n';

	app.hxg.InsertCommands(galaxies.cmdbuff_compute, setpip, comp_uniforms, wait, discmd, fnc);
	app.hxg.ExecuteCommands(galaxies.cmdbuff_compute);
}



void UniverseRenderer::UpdateCurrentGalaxyBuffer(Application& app, CurrentGalaxyProps* props){
	HXCopyStorageBufferCmd copybuffcmd{};
	copybuffcmd.source = props;
	copybuffcmd.destination = app.hxg.GetData(galaxies.current_props);
	copybuffcmd.sourceOffset = 0;
	copybuffcmd.destinationOffset = 0;
	copybuffcmd.size = sizeof(CurrentGalaxyProps);
	app.hxg.CopyStorageBuffer(copybuffcmd, HX_GRAPHICS_COPY_CPU_GPU);
}



HXTexture& UniverseRenderer::GetGalaxiesTex(Application& app){
	return galaxies.rfar.rendertex;
}
