
void UniverseRenderer::InitGalaxies(Application& app){

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
    galaxies.rnear_pipeline = app.hxg.CreateComputePipeline(c_pipconfig);
    // CreateDefaultComputeBundle(app, galaxies.rnear, galaxies.near_scale, HX_R16_G16_B16_A16_FLOAT, c_pipconfig);


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

	galaxies.rfar_pipeline = app.hxg.CreateGraphicsPipeline(g_pipconfig);


	delete comp_blob;
	delete vert_blob;
	delete frag_blob;
}


void UniverseRenderer::DrawFarGalaxies(Application& app){
	HXSetGraphicsPipelineCmd setpip{};
	setpip.pipeline = app.hxg.GetData(galaxies.rfar_pipeline);

	HXUpdateShaderUniformsCmd comp_uniforms{};
	comp_uniforms.Inputs = galaxy_mask_inputs;

	comp_uniforms.Length = HX_LENGTH_C_ARRAY(galaxy_mask_inputs);
	HXVertexDrawCall drawcall{};
	drawcall.Count = 14;
	drawcall.InstanceCount = app.universe.galaxy_psys.GetCurrentSize();
	drawcall.VDesc = NULL;
	drawcall.Mode = HX_GRAPHICS_DRAW_TRIANGLE_STRIP;

	app.hxg.InsertCommands(resources.raymarchPass.cmdbuff, setpip, comp_uniforms, drawcall);

}


void UniverseRenderer::DrawNearGalaxies(Application& app){
	HXSetComputePipelineCmd setpip{};
	setpip.pipeline = app.hxg.GetData(galaxies.rnear_pipeline);

	HXUpdateShaderUniformsCmd comp_uniforms{};
	comp_uniforms.Inputs = galaxy_mask_near_inputs;
	comp_uniforms.Length = HX_LENGTH_C_ARRAY(galaxy_mask_near_inputs);

	HXDispatchComputeCmd discmd{};
	discmd.x = size_t(ceil(float(app.current_width/resources.raymarchPass.scale) / 32));
	discmd.y = size_t(ceil(float(app.current_height/resources.raymarchPass.scale) / 32));
	discmd.z = 1;

	app.hxg.InsertCommands(resources.raymarchPass.cmdbuff, setpip, comp_uniforms, discmd);
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
