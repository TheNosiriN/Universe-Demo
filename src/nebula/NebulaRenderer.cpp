void UniverseRenderer::InitNebulae(Application& app){
	nebulae.cmdbuff = app.hxg.CreateCommandBuffer(g_cmdalloc, HX_GRAPHICS_CMDBUFFER_GRAPHICS, HX_GRAPHICS_CMDBUFFER_ONCE);


	/// init peipeline
	HXGraphicsPipelineConfig g_pipconfig{};
	g_pipconfig.DepthOperation = HX_GRAPHICS_DEPTH_OP_GREATER_EQUAL; /// because reverse depth
	g_pipconfig.BlendConfig[0].Operation = HX_GRAPHICS_BLEND_OP_ADD;
	g_pipconfig.BlendConfig[0].SrcFactor = HX_GRAPHICS_BLEND_ONE;
	g_pipconfig.BlendConfig[0].DstFactor = HX_GRAPHICS_BLEND_ONE_MINUS_SRC_ALPHA;
	g_pipconfig.BlendCount = 1;
	g_pipconfig.CullOperation = HX_GRAPHICS_CULL_OP_BACK;

	char* vert_blob = LoadShaderBlob("cluster_mask_vertex");
	ShaderC::LoadShaderBinary(vert_blob, HX_SHADERC_TYPE_VERTEX, g_pipconfig.VertexShader, g_pipconfig.Metadata);

	char* frag_blob = LoadShaderBlob("cluster_mask_fragment");
	ShaderC::LoadShaderBinary(frag_blob, HX_SHADERC_TYPE_FRAGMENT, g_pipconfig.FragmentShader, g_pipconfig.Metadata);


	/// create other resources
	// CreateDefaultRendererBundle(app, nebulae.rfar, nebulae.scale, HX_R16_G16_B16_A16_FLOAT, g_pipconfig);
	CreateDefaultRendererBundle(app, nebulae.rnear, nebulae.scale, HX_R16_G16_B16_A16_FLOAT, g_pipconfig);


	delete vert_blob;
	delete frag_blob;

}


void UniverseRenderer::DrawFarNebulae(Application& app){
	HXSetGraphicsPipelineCmd setpip{};
	setpip.pipeline = app.hxg.GetData(nebulae.rfar.pipeline);

	HXSetRenderpassCmd setrp{};
	setrp.renderpass = app.hxg.GetData(nebulae.rfar.renderpass);
	setrp.viewport = uvec4(0,0, app.current_width/nebulae.scale, app.current_height/nebulae.scale);

	HXVertexDrawCall drawcall{};
	drawcall.Count = 14;
	drawcall.InstanceCount = app.universe.cluster_psys.GetCurrentSize();
	drawcall.VDesc = NULL;
	drawcall.Mode = HX_GRAPHICS_DRAW_TRIANGLE_STRIP;

	HXUpdateShaderUniformsCmd comp_uniforms{};
	comp_uniforms.Inputs = cluster_mask_inputs;
	comp_uniforms.Length = HX_LENGTH_C_ARRAY(cluster_mask_inputs);

	app.hxg.InsertCommands(nebulae.cmdbuff, setrp, setpip, comp_uniforms, drawcall);
	app.hxg.ExecuteCommands(nebulae.cmdbuff);
}

void UniverseRenderer::DrawNearNebulae(Application& app){
	// HXVertexDrawCall drawcall{};
	// drawcall.Count = 14;
	// drawcall.InstanceCount = app.universe.cluster_psys.GetCurrentSize();
	// drawcall.VDesc = NULL;
	// drawcall.Mode = HX_GRAPHICS_DRAW_TRIANGLE_STRIP;
	//
	// HXUpdateShaderUniformsCmd comp_uniforms{};
	// comp_uniforms.Inputs = cluster_mask_inputs;
	// comp_uniforms.Length = HX_LENGTH_C_ARRAY(cluster_mask_inputs);
	//
	// app.hxg.InsertCommands(nebulae.cmdbuff, comp_uniforms, drawcall);
	//
	// app.hxg.ExecuteGraphicsCommands(
	// 	nebulae.rnear.pipeline, nebulae.rnear.renderpass,
	// 	nebulae.cmdbuff,
	// 	uvec4(0,0,
	// 		app.current_width/nebulae.scale,
	// 		app.current_height/nebulae.scale
	// 	)
	// );
}


HXTexture& UniverseRenderer::GetNebulaeTex(Application& app){
	return nebulae.rfar.rendertex;
}
