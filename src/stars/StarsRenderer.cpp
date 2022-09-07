void UniverseRenderer::InitStars(Application& app){
	stars.cmdbuff = app.hxg.CreateCommandBuffer(g_cmdalloc, HX_GRAPHICS_CMDBUFFER_GRAPHICS, HX_GRAPHICS_CMDBUFFER_ONCE);


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
	CreateDefaultRendererBundle(app, stars, stars.scale, HX_R16_G16_B16_A16_FLOAT, g_pipconfig);


	delete vert_blob;
	delete frag_blob;

}


void UniverseRenderer::DrawStars(Application& app){
	HXWaitForFenceCmd wait{};
	wait.fence = app.hxg.GetData(stars.fence);

	HXSetGraphicsPipelineCmd setpip{};
	setpip.pipeline = app.hxg.GetData(stars.pipeline);

	HXSetRenderpassCmd setrp{};
	setrp.renderpass = app.hxg.GetData(stars.renderpass);
	setrp.viewport = uvec4(0,0, app.current_width/stars.scale, app.current_height/stars.scale);

	HXVertexDrawCall drawcall{};
	drawcall.Count = 6;//app.universe.stars_psys.GetCurrentSize();
	drawcall.InstanceCount = app.universe.stars_psys.GetCurrentSize();
	drawcall.VDesc = NULL;
	drawcall.Mode = HX_GRAPHICS_DRAW_TRIANGLE_STRIP;

	HXUpdateShaderUniformsCmd comp_uniforms{};
	comp_uniforms.Inputs = stars_mask_inputs;
	comp_uniforms.Length = HX_LENGTH_C_ARRAY(stars_mask_inputs);

	HXInsertFenceCmd fnc{};
	fnc.fence = app.hxg.GetData(stars.fence);

	app.hxg.InsertCommands(stars.cmdbuff, setpip, setrp, comp_uniforms, wait, drawcall, fnc);
	app.hxg.ExecuteCommands(stars.cmdbuff);
}


HXTexture& UniverseRenderer::GetStarsTex(Application& app){
	return stars.rendertex;
}
