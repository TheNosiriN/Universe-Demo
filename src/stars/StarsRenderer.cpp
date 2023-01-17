void UniverseRenderer::InitStars(Application& app){
	/// init peipeline
	HXGraphicsPipelineConfig g_pipconfig{};
	g_pipconfig.DepthOperation = HX_GRAPHICS_DEPTH_OP_GREATER_EQUAL; /// because reverse depth
	g_pipconfig.BlendConfig[0].Operation = HX_GRAPHICS_BLEND_OP_ADD;
	g_pipconfig.BlendConfig[0].SrcFactor = HX_GRAPHICS_BLEND_ONE;
	g_pipconfig.BlendConfig[0].DstFactor = HX_GRAPHICS_BLEND_ONE;
	g_pipconfig.BlendCount = 1;
	g_pipconfig.CullOperation = HX_GRAPHICS_CULL_OP_NONE;

	char* vert_blob = LoadShaderBlob("stars_mask_vertex");
	// char* geom_blob = LoadShaderBlob("stars_mask_geometry");
	char* frag_blob = LoadShaderBlob("stars_mask_fragment");

	ShaderC::LoadShaderBinary(vert_blob, HX_SHADERC_TYPE_VERTEX, g_pipconfig.VertexShader, g_pipconfig.Metadata);
	// ShaderC::LoadShaderBinary(geom_blob, HX_SHADERC_TYPE_GEOMETRY, g_pipconfig.GeometryShader, g_pipconfig.Metadata);
	ShaderC::LoadShaderBinary(frag_blob, HX_SHADERC_TYPE_FRAGMENT, g_pipconfig.FragmentShader, g_pipconfig.Metadata);


	/// create other resources
    stars.pipeline = app.hxg.CreateGraphicsPipeline(g_pipconfig);


	delete vert_blob;
	delete frag_blob;

}


void UniverseRenderer::DrawStars(Application& app){
    HXSetGraphicsPipelineCmd setpip{};
	setpip.pipeline = app.hxg.GetData(stars.pipeline);

	HXVertexDrawCall drawcall{};
	drawcall.Count = 6;//app.universe.stars_psys.GetCurrentSize();
	drawcall.InstanceCount = app.universe.stars_psys.GetCurrentSize();
	drawcall.VDesc = NULL;
	drawcall.Mode = HX_GRAPHICS_DRAW_TRIANGLE_STRIP;

	HXUpdateShaderUniformsCmd comp_uniforms{};
	comp_uniforms.Inputs = stars_mask_inputs;
	comp_uniforms.Length = HX_LENGTH_C_ARRAY(stars_mask_inputs);

	app.hxg.InsertCommands(resources.depthlessPass.cmdbuff, setpip, comp_uniforms, drawcall);
}
