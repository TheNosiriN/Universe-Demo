#include "main.h"
#include "utils.h"
#include "structs.h"





/// global variables that don't need to be class members
HXShaderInput galaxy_mask_inputs[4];
HXShaderInput galaxy_mask_near_inputs[6];
HXShaderInput cluster_mask_inputs[6];
HXShaderInput stars_mask_inputs[6];
HXShaderInput solar_system_rings_mask_inputs[4];
HXShaderInput planets_far_mask_inputs[2];
HXShaderInput planets_near_mask_inputs[2];
HXShaderInput planets_texture_gen_inputs[1 + _PLANET_TEXTURE_COUNT*2];
HXShaderInput taa_pass_inputs[4];

const size_t c_galaxy_shape_map_size = 512;






void UniverseRenderer::SetupShaderInputs(Application& app){

	/// setup shader inputs
	galaxy_mask_inputs[0] = { app.hxg.GetUniform(galaxies.rfar.pipeline, "GlobalCamConstants"), app.hxg.GetData(constants_buff) };
	galaxy_mask_inputs[1] = { app.hxg.GetUniform(galaxies.rfar.pipeline, "GalaxyPositions"), app.hxg.GetData(app.universe.galaxy_psys.gpu_buffer) };
	galaxy_mask_inputs[2] = { app.hxg.GetUniform(galaxies.rfar.pipeline, "whitenoise"), app.hxg.GetData(app.whiteNoise2DTex), &app.linearSampler };
	galaxy_mask_inputs[3] = { app.hxg.GetUniform(galaxies.rfar.pipeline, "CurrentSeedsCBuffer"), app.hxg.GetData(cur_seeds_buff) };


	galaxy_mask_near_inputs[0] = { app.hxg.GetUniform(galaxies.rnear.pipeline, "RenderPos"), &(app.universe.galaxy_psys.GetClosestParticle().renderpos) };
	galaxy_mask_near_inputs[1] = { app.hxg.GetUniform(galaxies.rnear.pipeline, "GlobalCamConstants"), app.hxg.GetData(constants_buff) };
	galaxy_mask_near_inputs[2] = { app.hxg.GetUniform(galaxies.rnear.pipeline, "whitenoise"), app.hxg.GetData(app.whiteNoise2DTex), &app.linearSampler };
	galaxy_mask_near_inputs[3] = { app.hxg.GetUniform(galaxies.rnear.pipeline, "bluenoise"), app.hxg.GetData(app.blueNoise2DTex), &app.nearestSampler };
	galaxy_mask_near_inputs[4] = { app.hxg.GetUniform(galaxies.rnear.pipeline, "CurrentGalaxyPropsCBuffer"), app.hxg.GetData(galaxies.current_props) };
	galaxy_mask_near_inputs[5] = { app.hxg.GetUniform(galaxies.rnear.pipeline, "outTex"), app.hxg.GetData(galaxies.rnear.rendertex) };


	// cluster_mask_inputs[0] = { app.hxg.GetUniform(nebulae.rnear.pipeline, "GlobalCamConstants"), app.hxg.GetData(constants_buff) };
	// cluster_mask_inputs[1] = { app.hxg.GetUniform(nebulae.rnear.pipeline, "ClusterPositions"), app.hxg.GetData(app.universe.cluster_psys.gpu_buffer) };
	// cluster_mask_inputs[2] = { app.hxg.GetUniform(nebulae.rnear.pipeline, "whitenoise"), app.hxg.GetData(app.whiteNoise2DTex), &app.linearSampler };
	// cluster_mask_inputs[3] = { app.hxg.GetUniform(nebulae.rnear.pipeline, "bluenoise"), app.hxg.GetData(app.blueNoise2DTex) };


	stars_mask_inputs[0] = { app.hxg.GetUniform(stars.pipeline, "GlobalCamConstants"), app.hxg.GetData(constants_buff) };
	stars_mask_inputs[1] = { app.hxg.GetUniform(stars.pipeline, "CurrentSeedsCBuffer"), app.hxg.GetData(cur_seeds_buff) };
	stars_mask_inputs[2] = { app.hxg.GetUniform(stars.pipeline, "StarPositions"), app.hxg.GetData(app.universe.stars_psys.grid.GetBuffer()) };
	stars_mask_inputs[3] = { app.hxg.GetUniform(stars.pipeline, "StarBlocks"), app.hxg.GetData(app.universe.stars_psys.local_renderpos) };
	stars_mask_inputs[4] = { app.hxg.GetUniform(stars.pipeline, "StarHeader"), app.hxg.GetData(app.universe.stars_psys.grid.GetClosestBuffer()) };
	stars_mask_inputs[5] = { app.hxg.GetUniform(stars.pipeline, "StarRenderpos"), &(app.universe.stars_psys.closest_renderpos) };


	solar_system_rings_mask_inputs[0] = { app.hxg.GetUniform(solar_system.rings.pipeline, "GlobalCamConstants"), app.hxg.GetData(constants_buff) };
	solar_system_rings_mask_inputs[1] = { app.hxg.GetUniform(solar_system.rings.pipeline, "SolarSystemPropertiesCBuff"), app.hxg.GetData(solar_system.props) };
	solar_system_rings_mask_inputs[2] = { app.hxg.GetUniform(solar_system.rings.pipeline, "SolarSystemOrbitsCBuff"), app.hxg.GetData(solar_system.orbits) };
	solar_system_rings_mask_inputs[3] = { app.hxg.GetUniform(solar_system.rings.pipeline, "SolarSystemOrbitsUpdateCBuff"), app.hxg.GetData(solar_system.orbit_updates) };


	planets_far_mask_inputs[0] = { app.hxg.GetUniform(planets.rfar.pipeline, "GlobalCamConstants"), app.hxg.GetData(constants_buff) };
	planets_far_mask_inputs[1] = { app.hxg.GetUniform(planets.rfar.pipeline, "SolarSystemPropertiesCBuff"), app.hxg.GetData(solar_system.props) };
	// planets_far_mask_inputs[2] = { app.hxg.GetUniform(planets.rfar.pipeline, "PlanetSurfaces_tex"), app.hxg.GetData(planets.surfaceTex) };


	planets_texture_gen_inputs[0] = { app.hxg.GetUniform(planets.planet_tex_pip, "Props"), app.hxg.GetData(planets.planet_tex_indices_buffer) };
	for (uint i=0; i<_PLANET_TEXTURE_COUNT; ++i){
		planets_texture_gen_inputs[1 + (i*2+0)] = { app.hxg.GetUniform(planets.planet_tex_pip, "diffuse_tex"+std::to_string(i)), app.hxg.GetData(planets.surfaceTex[i]) };
		planets_texture_gen_inputs[1 + (i*2+1)] = { app.hxg.GetUniform(planets.planet_tex_pip, "normals_tex"+std::to_string(i)), app.hxg.GetData(planets.normalsTex[i]) };
	}


	planets_near_mask_inputs[0] = { app.hxg.GetUniform(planets.rnear.pipeline, "GlobalCamConstants"), app.hxg.GetData(constants_buff) };
	planets_near_mask_inputs[1] = { app.hxg.GetUniform(planets.rnear.pipeline, "Renderpos"), &(app.universe.planet_sys.planet_renderpos) };


	taa_pass_inputs[0] = { app.hxg.GetUniform(taa_pass.pipeline, "previous_frame"), NULL, &app.linearSampler };
	taa_pass_inputs[1] = { app.hxg.GetUniform(taa_pass.pipeline, "galaxy_tex"), app.hxg.GetData(galaxies.rnear.rendertex), &app.linearSampler };
	taa_pass_inputs[2] = { app.hxg.GetUniform(taa_pass.pipeline, "nebula_tex"), app.hxg.GetData(nebulae.rnear.rendertex), &app.linearSampler };
	taa_pass_inputs[3] = { app.hxg.GetUniform(taa_pass.pipeline, "GlobalCamConstants"), app.hxg.GetData(constants_buff) };

}







void CreateDefaultRendererBundle(
	Application& app, DefaultRendererBundle& bundle,
	size_t scale, InputFormat format,
	const HXGraphicsPipelineConfig& g_pipconfig
){
	HXTextureConfig texconfig{};
	texconfig.Type = HX_GRAPHICS_TEXTURE_2D;
	texconfig.Width = app.current_width/scale;
	texconfig.Height = app.current_height/scale;
	texconfig.Depth = 1;
	texconfig.Usage = HX_GRAPHICS_USAGE_DYNAMIC;
	texconfig.GenerateMips = false;
	texconfig.Format = format;
	bundle.rendertex = app.hxg.CreateTexture(texconfig, NULL);


	HXRenderPassConfig rpconfig{};
	rpconfig.Flags = HX_GRAPHICS_RENDERPASS_COLOUR;
	rpconfig.ClearColor = vec4(0);
	rpconfig.ClearFlags = rpconfig.Flags;
	rpconfig.Textures[0] = app.hxg.GetData(bundle.rendertex);
	rpconfig.TextureCount = 1;
	bundle.renderpass = app.hxg.CreateRenderPass(rpconfig);


	bundle.pipeline = app.hxg.CreateGraphicsPipeline(g_pipconfig);

}


void CreateDepthColorRendererBundle(
	Application& app, DepthColorRendererBundle& bundle,
	size_t scale, InputFormat format,
	const HXGraphicsPipelineConfig& g_pipconfig
){
	HXTextureConfig texconfig{};
	texconfig.Type = HX_GRAPHICS_TEXTURE_2D;
	texconfig.Width = app.current_width/scale;
	texconfig.Height = app.current_height/scale;
	texconfig.Depth = 1;
	texconfig.Usage = HX_GRAPHICS_USAGE_DYNAMIC;
	texconfig.GenerateMips = false;

	texconfig.Format = format;
	bundle.rendertex = app.hxg.CreateTexture(texconfig, NULL);

	texconfig.Format = HX_D32_FLOAT;
	bundle.depthtex = app.hxg.CreateTexture(texconfig, NULL);


	HXRenderPassConfig rpconfig{};
	rpconfig.Flags = HX_GRAPHICS_RENDERPASS_COLOUR | HX_GRAPHICS_RENDERPASS_DEPTH;
	rpconfig.ClearColor = vec4(0);
	rpconfig.ClearDepth = 0.0f;
	rpconfig.ClearFlags = rpconfig.Flags;
	rpconfig.Textures[0] = app.hxg.GetData(bundle.rendertex);
	rpconfig.DepthStencil = app.hxg.GetData(bundle.depthtex);
	rpconfig.TextureCount = 1;
	bundle.renderpass = app.hxg.CreateRenderPass(rpconfig);


	bundle.pipeline = app.hxg.CreateGraphicsPipeline(g_pipconfig);

}


void CreateDefaultComputeBundle(
	Application& app, DefaultComputeBundle& bundle,
	size_t scale, InputFormat format,
	const HXComputePipelineConfig& c_pipconfig
){
	HXTextureConfig texconfig{};
	texconfig.Type = HX_GRAPHICS_TEXTURE_2D;
	texconfig.Width = app.current_width/scale;
	texconfig.Height = app.current_height/scale;
	texconfig.Depth = 1;
	texconfig.Usage = HX_GRAPHICS_USAGE_DYNAMIC;
	texconfig.GenerateMips = false;
	texconfig.Format = format;
	bundle.rendertex = app.hxg.CreateTexture(texconfig, NULL);

	bundle.pipeline = app.hxg.CreateComputePipeline(c_pipconfig);

}





#include "galaxy/GalaxyRenderer.cpp"
#include "nebula/NebulaRenderer.cpp"
#include "stars/StarsRenderer.cpp"
#include "stars/SolarSystemRenderer.cpp"
#include "planets/PlanetRenderer.cpp"







void UniverseRenderer::InitTAAPass(Application& app){
	HXTextureConfig texconfig{};
	texconfig.Type = HX_GRAPHICS_TEXTURE_2D;
	texconfig.Width = app.current_width/taa_pass.scale;
	texconfig.Height = app.current_height/taa_pass.scale;
	texconfig.Depth = 1;
	texconfig.Usage = HX_GRAPHICS_USAGE_DYNAMIC;
	texconfig.Format = HX_R16_G16_B16_A16_FLOAT;

	for (size_t i=0; i<TAAFilterPass::FrameCount; ++i){
		taa_pass.rendertex.objects[i] = app.hxg.CreateTexture(texconfig, NULL);
	}


	HXRenderPassConfig rpconfig{};
	rpconfig.ClearFlags = 0; /// dont waste time clearing
	rpconfig.Flags = HX_GRAPHICS_RENDERPASS_COLOUR;
	rpconfig.TextureCount = 1;

	for (size_t i=0; i<TAAFilterPass::FrameCount; ++i){
		rpconfig.Textures[0] = app.hxg.GetData(taa_pass.rendertex.objects[i]);
		taa_pass.renderpass.objects[i] = app.hxg.CreateRenderPass(rpconfig);
	}


	auto frag_blob = StaticUtils::LoadShaderBlob("universe_taa_pass_fragment");
	HXGraphicsPipelineConfig g_pipconfig{};
	ShaderC::LoadFullscreenVertexShaderBinary(g_pipconfig.VertexShader, g_pipconfig.Metadata);
	ShaderC::LoadShaderBinary(frag_blob, HX_SHADERC_TYPE_FRAGMENT, g_pipconfig.FragmentShader, g_pipconfig.Metadata);
	taa_pass.pipeline = app.hxg.CreateGraphicsPipeline(g_pipconfig);
	delete frag_blob;
}


void UniverseRenderer::DrawTAAPass(Application& app){
	taa_pass_inputs[0].Data = app.hxg.GetData(taa_pass.rendertex.previous());

	HXSetGraphicsPipelineCmd setpip{};
	setpip.pipeline = app.hxg.GetData(taa_pass.pipeline);

	HXSetRenderpassCmd setrp{};
	setrp.renderpass = app.hxg.GetData(taa_pass.renderpass.current());
	setrp.viewport = uvec4(0,0, app.current_width/taa_pass.scale, app.current_height/taa_pass.scale);

	HXUpdateShaderUniformsCmd upd{};
	upd.Inputs = taa_pass_inputs;
	upd.Length = HX_LENGTH_C_ARRAY(taa_pass_inputs);

	HXWaitForFenceCmd wait{};
	wait.fence = app.hxg.GetData(galaxies.rnear.fence);

	HXFullscreenDraw draw{};
	app.hxg.InsertCommands(graphics_cmdbuff, setpip, setrp, upd, wait, draw);
	app.hxg.ExecuteCommands(graphics_cmdbuff);

	taa_pass.rendertex.advance();
	taa_pass.renderpass.advance();
}


HXTexture& UniverseRenderer::GetTAATex(Application& app){
	return taa_pass.rendertex.previous();
}













void UniverseRenderer::Init(Application& app){
	/// init global constants buffer
	constants_buff = app.hxg.CreateStorageBuffer(
		HXSBufferConfig{ sizeof(CameraConstants), HX_GRAPHICS_USAGE_DYNAMIC }, NULL,
		HX_GRAPHICS_UNIFORMBUFFER
	);


	/// init current seeds buffer
	cur_seeds_buff = app.hxg.CreateStorageBuffer(
		HXSBufferConfig{ sizeof(CurrentSeedsConstants), HX_GRAPHICS_USAGE_DYNAMIC }, NULL,
		HX_GRAPHICS_UNIFORMBUFFER
	);



	/// init global command buffers
	g_cmdalloc = app.hxg.CreateCommandAllocator(HX_GRAPHICS_CMDBUFFER_GRAPHICS);
	c_cmdalloc = app.hxg.CreateCommandAllocator(HX_GRAPHICS_CMDBUFFER_COMPUTE);
	graphics_cmdbuff = app.hxg.CreateCommandBuffer(g_cmdalloc, HX_GRAPHICS_CMDBUFFER_GRAPHICS, HX_GRAPHICS_CMDBUFFER_ONCE);
	compute_cmdbuff = app.hxg.CreateCommandBuffer(c_cmdalloc, HX_GRAPHICS_CMDBUFFER_COMPUTE, HX_GRAPHICS_CMDBUFFER_ONCE);



	/// setup scales
	galaxies.scale = 2;
	galaxies.near_scale = 2;
	nebulae.scale = 1;
	stars.scale = 1;
	solar_system.scale = 1;
	planets.scale = 1;
	taa_pass.scale = 1;



	/// init resources
	InitResources(app);



	/// init galaxy mask
	InitGalaxies(app);
	// InitNebulae(app);
	InitStars(app);
	InitSolarSystem(app);
	InitPlanets(app);


	/// init TAA
	InitTAAPass(app);


	/// setup inputs after initialization
	SetupShaderInputs(app);
}






/// WARNING: Allocates memory
void _GenerateBaseUVSphere(uint longitudes, uint latitudes, Hexo::Utils::TypedVector<vec3>& vertices, Hexo::Utils::TypedVector<vec2>& uvs, Hexo::Utils::TypedVector<uint32_t>& indices){
	float deltaLatitude = Mathgl::pi<float>()/latitudes;
    float deltaLongitude = 2.0f * Mathgl::pi<float>()/longitudes;
    float latitudeAngle;
    float longitudeAngle;

    for (uint i=0; i<=latitudes; ++i){
        latitudeAngle = Mathgl::pi<float>()/2.0f - i*deltaLatitude; /* Starting -pi/2 to pi/2 */
        float xy = cosf(latitudeAngle);    /* r * cos(phi) */
        float z = sinf(latitudeAngle);     /* r * sin(phi )*/

        /*
         * We add (latitudes + 1) vertices per longitude because of equator,
         * the North pole and South pole are not counted here, as they overlap.
         * The first and last vertices have same position and normal, but
         * different tex coords.
         */
        for (uint j=0; j<=longitudes; ++j){
            longitudeAngle = j * deltaLongitude;

            vec3 vertex;
            vertex.x = xy * cosf(longitudeAngle);       /* x = r * cos(phi) * cos(theta)  */
            vertex.y = xy * sinf(longitudeAngle);       /* y = r * cos(phi) * sin(theta) */
            vertex.z = z;                               /* z = r * sin(phi) */
			vertices.push_back(vertex);

			vec2 uv;
			uv.x = (float)j/longitudes;             /* s */
            uv.y = (float)i/latitudes;              /* t */
            uvs.push_back(uv);
        }
    }



	uint k1, k2;
    for(uint i=0; i<latitudes; ++i){
        k1 = i * (longitudes + 1);
        k2 = k1 + longitudes + 1;
        // 2 Triangles per latitude block excluding the first and last longitudes blocks
        for(uint j=0; j<longitudes; ++j, ++k1, ++k2){
            if (i != 0){
				indices.push_back(k1 + 1);
                indices.push_back(k2);
				indices.push_back(k1);
            }

            if (i != (latitudes - 1)){
				indices.push_back(k2 + 1);
                indices.push_back(k2);
				indices.push_back(k1 + 1);
            }
        }
    }
}


void UniverseRenderer::InitResources(Application& app){
	/// init zero memory
	resources.zero_memory = 0u;

	/// init sphere geometry
	Hexo::Utils::TypedVector<vec3> vertices;
	Hexo::Utils::TypedVector<vec2> uvs;
	Hexo::Utils::TypedVector<uint32_t> indices;
	_GenerateBaseUVSphere(32, 32, vertices, uvs, indices);

	resources.sphere_vert = app.hxg.CreateStorageBuffer(
		HXSBufferConfig{vertices.size() * sizeof(vec3), HX_GRAPHICS_USAGE_STATIC},
		vertices.data(), HX_GRAPHICS_VERTEXBUFFER
	);
	resources.sphere_uvs = app.hxg.CreateStorageBuffer(
		HXSBufferConfig{uvs.size() * sizeof(vec2), HX_GRAPHICS_USAGE_STATIC},
		uvs.data(), HX_GRAPHICS_VERTEXBUFFER
	);
	resources.sphere_indices = app.hxg.CreateStorageBuffer(
		HXSBufferConfig{indices.size() * sizeof(uint32_t), HX_GRAPHICS_USAGE_STATIC},
		indices.data(), HX_GRAPHICS_INDEXBUFFER
	);

	resources.sphere_vdesc = app.hxg.CreateVertexBufferDescriptor(
		HXVBufferDescPair{ app.hxg.GetData(resources.sphere_vert), 0, sizeof(vec3), vertices.size(), 0, 3 },
		HXVBufferDescPair{ app.hxg.GetData(resources.sphere_uvs), 0, sizeof(vec2), uvs.size(), 1, 2 }
	);
	resources.sphere_idesc = app.hxg.CreateIndexBufferDescriptor(resources.sphere_indices, 0, HX_R32_UINT, indices.size());
}





void UniverseRenderer::Update(Application& app){

	/// update copy of global constants
	constants.frame_size = vec2(app.current_width, app.current_height);
	constants.projection = app.camera.projection.matrix;
	constants.view = app.camera.view.matrix;
	constants.viewProjInv = inverse(constants.projection * constants.view);
	constants.cam_pos = vec3(0);//app.camera.view.eye;
	constants.Time = app.Time / 100.0f;
	constants.FrameIndex = app.FrameIndex;
	constants.level = app.camera.view.eye.level;

	HXCopyStorageBufferCmd constcmd{};
	constcmd.source = &constants;
	constcmd.destination = app.hxg.GetData(constants_buff);
	constcmd.sourceOffset = 0;
	constcmd.destinationOffset = 0;
	constcmd.size = sizeof(CameraConstants);
	app.hxg.CopyStorageBuffer(constcmd, HX_GRAPHICS_COPY_CPU_GPU);



	/// update copy of current seeds
	current_seeds.galaxy_seed = app.universe.currentGalaxy.seed;
	current_seeds.star_seed = app.universe.currentStar.seed;
	// std::cout << current_seeds.cur_galaxy_seed << '\n';
	// cur_star_seed = app.universe.curr

	HXCopyStorageBufferCmd curseedcmd{};
	curseedcmd.source = &current_seeds;
	curseedcmd.destination = app.hxg.GetData(cur_seeds_buff);
	curseedcmd.sourceOffset = 0;
	curseedcmd.destinationOffset = 0;
	curseedcmd.size = sizeof(CurrentSeedsConstants);
	app.hxg.CopyStorageBuffer(curseedcmd, HX_GRAPHICS_COPY_CPU_GPU);


	/// draw masks
	if (app.camera.view.eye.level >= E_VIEW_LAYER_PLANET_ORBIT){
		DrawPlanetsNear(app);
	}

	if (app.camera.view.eye.level >= E_VIEW_LAYER_STAR_ORBIT){
		DrawPlanetsFar(app);
		RegeneratePlanetTextures(app);
		DrawSolarSystemRings(app);
	}

	if (app.camera.view.eye.level >= E_VIEW_LAYER_GALAXY){
		DrawStars(app);
		// DrawClusterMask(app);
		DrawNearGalaxies(app);
	}

	DrawFarGalaxies(app);
	DrawTAAPass(app);

}





void UniverseRenderer::Resize(Application& app){

	app.hxg.ResizeTexture(
		planets.rnear.rendertex,
		app.current_width/planets.scale,
		app.current_height/planets.scale,
		1, NULL
	);
	app.hxg.ResizeTexture(
		planets.rnear.depthtex,
		app.current_width/planets.scale,
		app.current_height/planets.scale,
		1, NULL
	);


	app.hxg.ResizeTexture(
		planets.rfar.rendertex,
		app.current_width/planets.scale,
		app.current_height/planets.scale,
		1, NULL
	);
	app.hxg.ResizeTexture(
		planets.rfar.depthtex,
		app.current_width/planets.scale,
		app.current_height/planets.scale,
		1, NULL
	);


	app.hxg.ResizeTexture(
		solar_system.rings.rendertex,
		app.current_width/solar_system.scale,
		app.current_height/solar_system.scale,
		1, NULL
	);


	app.hxg.ResizeTexture(
		stars.rendertex,
		app.current_width/stars.scale,
		app.current_height/stars.scale,
		1, NULL
	);


	// app.hxg.ResizeTexture(
	// 	nebulae.rfar.rendertex,
	// 	app.current_width/nebulae.scale,
	// 	app.current_height/nebulae.scale,
	// 	1, NULL
	// );
	// app.hxg.ResizeTexture(
	// 	nebulae.rnear.rendertex,
	// 	app.current_width/nebulae.scale,
	// 	app.current_height/nebulae.scale,
	// 	1, NULL
	// );


	app.hxg.ResizeTexture(
		galaxies.rfar.rendertex,
		app.current_width/galaxies.scale,
		app.current_height/galaxies.scale,
		1, NULL
	);
	app.hxg.ResizeTexture(
		galaxies.rnear.rendertex,
		app.current_width/galaxies.near_scale,
		app.current_height/galaxies.near_scale,
		1, NULL
	);


	for (size_t i=0; i<TAAFilterPass::FrameCount; ++i){
		app.hxg.ResizeTexture(
			taa_pass.rendertex.objects[i],
			app.current_width/taa_pass.scale,
			app.current_height/taa_pass.scale,
			1, NULL
		);
	}

}




void UniverseRenderer::Cleanup(Application& app){

}
