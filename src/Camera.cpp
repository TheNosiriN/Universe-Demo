#include "main.h"
#include "utils.h"
#include "structs.h"




HXShaderInput shaderInputsPostProcess[9];


void RenderCamera::Init(Application& app){
	settings.exposure = 1.0f;



	/// create post process texture
	HXTextureConfig texconfig{};
	texconfig.Type = HX_GRAPHICS_TEXTURE_2D;
	texconfig.Width = app.current_width;
	texconfig.Height = app.current_height;
	texconfig.Depth = 1;
	texconfig.Usage = HX_GRAPHICS_USAGE_DYNAMIC;
	texconfig.Format = HX_R16_G16_B16_FLOAT;
	postprocess_tex = app.hxg.CreateTexture(texconfig, NULL);



	/// create post process render pass
	HXRenderPassConfig rpconfig{};
	rpconfig.ClearFlags = 0; /// dont waste time clearing
	rpconfig.Flags = HX_GRAPHICS_RENDERPASS_COLOUR;
	rpconfig.TextureCount = 1;
	rpconfig.Textures[0] = app.hxg.GetData(postprocess_tex);
	postprocess_pass = app.hxg.CreateRenderPass(rpconfig);



	/// create graphics pipeline
	auto frag_blob = StaticUtils::LoadShaderBlob("postprocess_pass_fragment");
	HXGraphicsPipelineConfig g_pipconfig{};
	ShaderC::LoadFullscreenVertexShaderBinary(g_pipconfig.VertexShader, g_pipconfig.Metadata);
	ShaderC::LoadShaderBinary(frag_blob, HX_SHADERC_TYPE_FRAGMENT, g_pipconfig.FragmentShader, g_pipconfig.Metadata);
	postprocess = app.hxg.CreateGraphicsPipeline(g_pipconfig);
	delete frag_blob;


	g_cmdbuff = app.hxg.CreateCommandBuffer(app.g_cmdalloc, HX_GRAPHICS_CMDBUFFER_GRAPHICS, HX_GRAPHICS_CMDBUFFER_ONCE);


	/// make shader inputs
	shaderInputsPostProcess[0] = {
		app.hxg.GetUniform(postprocess, "galaxy_far_tex"),
		app.hxg.GetData(app.universeRenderer.GetGalaxiesTex(app)),
		&app.linearSampler
	};
	shaderInputsPostProcess[1] = {
		app.hxg.GetUniform(postprocess, "nebula_far_tex"),
		app.hxg.GetData(app.universeRenderer.GetNebulaeTex(app)),
		&app.linearSampler
	};
	shaderInputsPostProcess[2] = {
		app.hxg.GetUniform(postprocess, "stars_tex"),
		app.hxg.GetData(app.universeRenderer.GetStarsTex(app)),
		&app.linearSampler
	};
	shaderInputsPostProcess[3] = {
		app.hxg.GetUniform(postprocess, "solar_system_rings_tex"),
		app.hxg.GetData(app.universeRenderer.GetSolarSystemRingsTex(app)),
		&app.linearSampler
	};
	shaderInputsPostProcess[4] = {
		app.hxg.GetUniform(postprocess, "planets_far_tex"),
		app.hxg.GetData(app.universeRenderer.GetPlanetsFarTex(app)),
		&app.linearSampler
	};
	shaderInputsPostProcess[5] = {
		app.hxg.GetUniform(postprocess, "planets_near_tex"),
		app.hxg.GetData(app.universeRenderer.GetPlanetsNearTex(app)),
		&app.linearSampler
	};
	shaderInputsPostProcess[6] = {
		app.hxg.GetUniform(postprocess, "taa_tex"),
		NULL,
		&app.linearSampler
	};
	shaderInputsPostProcess[7] = {
		app.hxg.GetUniform(postprocess, "GlobalCamConstants"),
		app.hxg.GetData(app.universeRenderer.constants_buff)
	};
	shaderInputsPostProcess[8] = {
		app.hxg.GetUniform(postprocess, "CameraSettingsCBuff"),
		&settings
	};

}


void RenderCamera::Update(Application& app){

	int exp_inc = app.hxi.GetKey(app.keyboard, HX_INPUT_VK_EQUALS, HX_INPUT_KEYSTATE_DOWN | HX_INPUT_KEYSTATE_PRESSED);
	int exp_dec = app.hxi.GetKey(app.keyboard, HX_INPUT_VK_SUBTRACT, HX_INPUT_KEYSTATE_DOWN | HX_INPUT_KEYSTATE_PRESSED);
	settings.exposure += (exp_inc-exp_dec) * 0.01f;
	settings.exposure = clamp(settings.exposure, 0.0f,1.0f);


	shaderInputsPostProcess[6].Data = app.hxg.GetData(app.universeRenderer.GetTAATex(app));

	HXSetGraphicsPipelineCmd setpip{};
	setpip.pipeline = app.hxg.GetData(postprocess);

	HXSetRenderpassCmd setrp{};
	setrp.renderpass = app.hxg.GetData(postprocess_pass);
	setrp.viewport = uvec4(0,0, app.current_width, app.current_height);

	HXUpdateShaderUniformsCmd upd{};
	upd.Inputs = shaderInputsPostProcess;
	upd.Length = HX_LENGTH_C_ARRAY(shaderInputsPostProcess);

	HXFullscreenDraw draw{};
	app.hxg.InsertCommands(g_cmdbuff, setpip, setrp, upd, draw);
	app.hxg.ExecuteCommands(g_cmdbuff);
}

void RenderCamera::Resize(Application& app){
	app.hxg.ResizeTexture(postprocess_tex, app.current_width, app.current_height, 1, NULL);
}

void RenderCamera::Cleanup(Application& app){
	app.hxg.DestroyCommandBuffer(g_cmdbuff);
	app.hxg.DestroyGraphicsPipeline(postprocess);
	app.hxg.DestroyRenderPass(postprocess_pass);
	app.hxg.DestroyTexture(postprocess_tex);
}






// to get a perspective matrix with reversed z, simply swap the near and far plane
mat4 perspectiveFovReverseZLH_ZO(float fov, float width, float height, float zNear, float zFar) {
    return perspectiveFovLH_ZO(fov, width, height, zFar, zNear);
};

// now let zFar go towards infinity
mat4 infinitePerspectiveFovReverseZLH_ZO(float fov, float width, float height, float zNear) {
    const float h = glm::cot(0.5f * fov);
    const float w = h * height / width;
    mat4 result = Mathgl::zero<glm::mat4>();
    result[0][0] = w;
    result[1][1] = h;
    result[2][2] = 0.0f;
    result[2][3] = 1.0f;
    result[3][2] = zNear;
    return result;
};

void PerspectiveProjection::Init(Application& app){
	rfov = 60.0f;
	fov = rfov;
	matrix = infinitePerspectiveFovReverseZLH_ZO(radians(fov), (float)app.current_width, (float)app.current_height, 10e-9f);
}

void PerspectiveProjection::Update(Application& app){
	if (app.hxi.GetKey(app.keyboard, HX_INPUT_VK_SHIFT, HX_INPUT_KEYSTATE_DOWN)){
		rfov = 60.0f;
		fov = mix(fov, rfov, 0.025);
	}else{
		rfov = 45.0f;
		fov = mix(fov, rfov, 0.5);
	}

	matrix = infinitePerspectiveFovReverseZLH_ZO(radians(fov), (float)app.current_width, (float)app.current_height, 10e-9f);
}

void PerspectiveProjection::Resize(Application& app){
	// matrix = infinitePerspectiveFovReverseZLH_ZO(radians(60.0f), (float)app.current_width, (float)app.current_height, 0.01f);
}

void PerspectiveProjection::Cleanup(Application& app){

}





vec3 _ConstructDirFromPolarCoord(const vec2& polar_coord){
	vec2 r = vec2(
		polar_coord.x * pi<float>()*2.0f,
		polar_coord.y * pi<float>()
	);

	return vec3(cos(r.x)*sin(r.y), cos(r.y), sin(r.x)*sin(r.y));
}


void FlyCamera::Init(Application& app){
	type = _FLY_CAM_TYPE;

	rpolar_coord = vec3(0,0.5,0);
	polar_coord = rpolar_coord;

	vec3 target = _ConstructDirFromPolarCoord(polar_coord);
	dir = normalize(-target);
	last_dir = dir;
	last_coord = uni_vec3(0);


	velocity = 0.0;
	lerpspeed = 0.995f;
	rzoom_fac = 0.0;
	zoom_fac = 0.0;
}


quat orientation;
quat rorientation;

void FlyCamera::UpdateFlyCam(Application& app){
	int up = app.hxi.GetKey(app.keyboard, HX_INPUT_VK_CHAR_W, HX_INPUT_KEYSTATE_DOWN);
	int down = app.hxi.GetKey(app.keyboard, HX_INPUT_VK_CHAR_S, HX_INPUT_KEYSTATE_DOWN);
	int left = app.hxi.GetKey(app.keyboard, HX_INPUT_VK_CHAR_A, HX_INPUT_KEYSTATE_DOWN);
	int right = app.hxi.GetKey(app.keyboard, HX_INPUT_VK_CHAR_D, HX_INPUT_KEYSTATE_DOWN);
	int rollleft = app.hxi.GetKey(app.keyboard, HX_INPUT_VK_CHAR_Q, HX_INPUT_KEYSTATE_DOWN);
	int rollright = app.hxi.GetKey(app.keyboard, HX_INPUT_VK_CHAR_E, HX_INPUT_KEYSTATE_DOWN);
	int space = app.hxi.GetKey(app.keyboard, HX_INPUT_VK_SPACE, HX_INPUT_KEYSTATE_DOWN);
	// int left = app.hxi.GetKey(app.keyboard, HX_INPUT_VK_CHAR_W, HX_INPUT_KEYSTATE_DOWN);
	// int right = app.hxi.GetKey(app.keyboard, HX_INPUT_VK_CHAR_W, HX_INPUT_KEYSTATE_DOWN);

	// int acc = app.hxi.GetKey(app.keyboard, HX_INPUT_VK_CHAR_X, HX_INPUT_KEYSTATE_DOWN);
	// int dec = app.hxi.GetKey(app.keyboard, HX_INPUT_VK_CHAR_C, HX_INPUT_KEYSTATE_DOWN);


	acceleration = pow(1.01088e+20, 1.0/(double(speedgear)*0.3 + 10*speedgearbox + 1.0));
	acceleration += acceleration * 2.0 * app.hxi.GetKey(app.keyboard, HX_INPUT_VK_SHIFT, HX_INPUT_KEYSTATE_DOWN);

	velocity += (up - down) * acceleration * app.deltaTime * 0.1;
	if (space){
		velocity *= pow(0.995, app.deltaTime);
	}

	// if (up || down){
	// 	std::cout << velocity << '\n';
	// }


	quat qYaw = angleAxis(mod(rpolar_coord.x,pi<float>()*2.0f), 	vec3(0,1,0));
	quat qPitch = angleAxis(mod(rpolar_coord.y,pi<float>()*2.0f), 	vec3(1,0,0));
	quat qRoll = angleAxis(0.0f, vec3(0,0,1));

	rorientation = normalize(qYaw * qPitch * qRoll);
	orientation = glm::slerp(orientation, rorientation, float(1.0-pow(lerpspeed, app.deltaTime))); /// this is slerp
	mat4 rotation = mat4_cast(orientation);


	uni_vec3 delta = uni_vec3(-last_dir) * uni_float(velocity * double(app.deltaTime) * 0.1);
	eye.translate(delta);
	last_coord = uni_vec3(0);

	// vec3 target = _ConstructDirFromPolarCoord(polar_coord);
	// dir = normalize(-target);

	dir = normalize(orientation * vec3(0,0,-1));
	// std::cout << dir.x << " -- " << dir.y << " -- " << dir.z << '\n';

	matrix = transpose(rotation);//lookAtLH(vec3(0), -dir, vec3(0,1,0));

	if (app.settings.mouseLocked){
		last_dir = dir;
	}

}



void FlyCamera::UpdateOrbitCam(Application& app){

	rzoom_fac -= app.hxi.GetMouseWheelDelta(app.mouse).y * pow(1.01088e+20, 1.0/(double(speedgear)*0.3 + 10*speedgearbox + 1.0));
	rzoom_fac = max(rzoom_fac, 0.0);
	zoom_fac = mix(zoom_fac, rzoom_fac, 1.0-pow(lerpspeed, app.deltaTime));


	vec3 target = normalize(_ConstructDirFromPolarCoord(vec2{ polar_coord.x, polar_coord.y })); //
	uni_vec3 coord = focus_point - uni_vec3(target) * zoom_fac;
	uni_vec3 delta = coord - last_coord;
	last_coord = coord;

	eye.translate(delta);

	dir = -target;
	matrix = lookAtLH(vec3(0), target, vec3(0,1,0));
}



void FlyCamera::Update(Application& app){
	if (app.hxi.GetKey(app.keyboard, HX_INPUT_VK_CHAR_C, HX_INPUT_KEYSTATE_PRESSED)){
		type = (type+1) % _CAM_TYPE_COUNT;

		if (type == _FLY_CAM_TYPE){
			eye.translate(focus_point-last_coord);
		}

		std::cout << "cam type: " << size_t(type) << '\n';
	}
// std::cout << eye.positions[0].x << " -- " << eye.positions[0].y << " -- " << eye.positions[0].z << '\n';


	for (int i=0; i<10; ++i){
		auto number = app.hxi.GetKey(app.keyboard, HX_INPUT_VK_CHAR_0 + i, HX_INPUT_KEYSTATE_PRESSED);
		auto gkey = app.hxi.GetKey(app.keyboard, HX_INPUT_VK_CHAR_G, HX_INPUT_KEYSTATE_PRESSED | HX_INPUT_KEYSTATE_DOWN);

		if (!number){ continue; }
		speedgear = i;

		if (gkey){
			speedgearbox = i;
			std::cout << "gear box: " << speedgearbox << '\n';
			continue;
		}

		std::cout << "gear box: " << size_t(speedgearbox) << ", speed gear " << size_t(speedgear) << '\n';

	}


	Math::Vector2 mp;
	if (app.settings.mouseLocked || app.hxi.GetMouseKey(app.mouse, HX_INPUT_VK_LMBUTTON, HX_INPUT_KEYSTATE_DOWN)){
		mp = app.hxi.GetMousePosition(app.mouse, HX_INPUT_PAXIS_RELATIVE) * 0.00025f * app.deltaTime;
		rpolar_coord += vec2{ mp.x, mp.y };
	}
	// rpolar_coord.y = clamp(rpolar_coord.y, 0.001f, 1.0f);


	switch (type){
		case _FLY_CAM_TYPE:
			UpdateFlyCam(app);
		break;
		case _ORBIT_CAM_TYPE:
			UpdateOrbitCam(app);
		break;
	}

	polar_coord = mix(polar_coord, rpolar_coord, 1.0-pow(lerpspeed, app.deltaTime));
}






void FlyCamera::Cleanup(Application& app){

}






void OrbitCamera::Init(Application& app){
	rpolar_coord = vec3(0.75,0.25,0);
	polar_coord = rpolar_coord;
	lerpspeed = 0.25;
}

void OrbitCamera::Update(Application& app){

	Math::Vector2 mp;
	if (app.settings.mouseLocked || app.hxi.GetMouseKey(app.mouse, HX_INPUT_VK_LMBUTTON, HX_INPUT_KEYSTATE_DOWN)){
		mp = app.hxi.GetMousePosition(app.mouse, HX_INPUT_PAXIS_RELATIVE) * 0.002f;
		rpolar_coord += vec3{ -mp.x, -mp.y, 0.0f };
	}
	rpolar_coord.z -= app.hxi.GetMouseWheelDelta(app.mouse).y * 0.005f;


	rpolar_coord.y = clamp(rpolar_coord.y, 0.001f, 1.0f);
	polar_coord = mix(polar_coord, rpolar_coord, 1.0-pow(lerpspeed, app.deltaTime));

	vec3 target = normalize(_ConstructDirFromPolarCoord(polar_coord));
	uni_vec3 coord = uni_vec3(target) * 10.0e+23;
	uni_vec3 delta = coord - last_coord;
	last_coord = coord;

	eye.translate(delta);

	dir = -target;

	matrix = lookAtLH(vec3(0), dir, vec3(0,1,0));

}

void OrbitCamera::Cleanup(Application& app){

}






void Camera::ComputeFrustum(){
	mat4 matrix = projection.matrix * view.matrix;

	for (uint8_t i=0; i<4; ++i){ frustum.faces[0][i]   = matrix[i][3] + matrix[i][0]; }  /// leftFace
    for (uint8_t i=0; i<4; ++i){ frustum.faces[1][i]  = matrix[i][3] - matrix[i][0]; }  /// rightFace
    for (uint8_t i=0; i<4; ++i){ frustum.faces[2][i] = matrix[i][3] + matrix[i][1]; }  /// bottomFace
    for (uint8_t i=0; i<4; ++i){ frustum.faces[3][i]    = matrix[i][3] - matrix[i][1]; }  /// topFace
    // for (uint8_t i=0; i<4; ++i){ frustum.faces[4][i]   = matrix[i][3] + matrix[i][2]; }  /// nearFace
    // for (int i = 4; i--;){ frustum.far[i]    = mat[i][3] - mat[i][2]; }

	// frustum.left = matrix[i][3] + mat[i][0];
}
