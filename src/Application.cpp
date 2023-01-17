#include "main.h"
#include "utils.h"
#include "structs.h"

#include "stb_image.h"

uint8_t* LoadPNGTexture(const std::string& name, int width, int height, int channels){
	const char* cname = ("assets/textures/"+name+".png").c_str();
	return stbi_load(cname, &width, &height, &channels, 0);
}




void Application::Init(){
	auto whitenoise_img = LoadPNGTexture("whitenoise_rgba_2D", 256, 256, 4);
	auto bluenoise_img = LoadPNGTexture("bluenoise_rgba_2D", 512, 512, 4);
	auto pebbles_img = LoadPNGTexture("pebbles_r_2D", 512, 512, 1);


	g_cmdalloc = hxg.CreateCommandAllocator(HX_GRAPHICS_CMDBUFFER_GRAPHICS);
	t_cmdalloc = hxg.CreateCommandAllocator(HX_GRAPHICS_CMDBUFFER_TRANSFER);
	c_cmdalloc = hxg.CreateCommandAllocator(HX_GRAPHICS_CMDBUFFER_COMPUTE);


	/// init utils textures
	HXTextureConfig texconfig{};
	texconfig.Type = HX_GRAPHICS_TEXTURE_2D;
	texconfig.Depth = 1;
	texconfig.Usage = HX_GRAPHICS_USAGE_STATIC;
	texconfig.GenerateMips = false;
	texconfig.Format = HX_R8_G8_B8_A8_BYTE;

	texconfig.Width = 256;
	texconfig.Height = 256;
	whiteNoise2DTex = hxg.CreateTexture(texconfig, whitenoise_img);

	texconfig.Width = 1024;
	texconfig.Height = 1024;
	blueNoise2DTex = hxg.CreateTexture(texconfig, bluenoise_img);

	texconfig.Width = 512;
	texconfig.Height = 512;
	texconfig.Format = HX_R8_BYTE;
	uniformMask2DTex = hxg.CreateTexture(texconfig, pebbles_img);

	stbi_image_free(whitenoise_img);
	stbi_image_free(bluenoise_img);
	stbi_image_free(pebbles_img);


	/// init texture samplers
	HXSamplerConfig sampconfig{};
	sampconfig.WrapX = HX_GRAPHICS_WRAP_REPEAT;
	sampconfig.WrapY = sampconfig.WrapZ = sampconfig.WrapX;
	sampconfig.MinLOD = 0.0f;
	sampconfig.MaxLOD = 0.0f;

	sampconfig.MinFilter = HX_GRAPHICS_SAMPLE_LINEAR;
	sampconfig.MagFilter = sampconfig.MinFilter;
	linearSampler = hxg.CreateSampler(sampconfig);

	sampconfig.MinFilter = HX_GRAPHICS_SAMPLE_NEAREST;
	sampconfig.MagFilter = sampconfig.MinFilter;
	nearestSampler = hxg.CreateSampler(sampconfig);


	universe.Init(*this);
	universeRenderer.Init(*this);
	uiRenderer.Init(*this);
	camera.Init(*this);
}




void Application::Update(){
	FrameIndex += 1;

	/// pause time
	if (hxi.GetKey(keyboard, HX_INPUT_VK_CHAR_P, HX_INPUT_KEYSTATE_PRESSED)){
		settings.timePaused = !settings.timePaused;
		std::cout << "paused pressed" << '\n';
	}

	if (!settings.timePaused){
		Time += uint(deltaTime*1000);
	}


	/// lock mouse
	if (hxi.GetKey(keyboard, HX_INPUT_VK_CONTROL, HX_INPUT_KEYSTATE_PRESSED)){
		settings.mouseLocked = !settings.mouseLocked;

		if (settings.mouseLocked){
			hxi.HideAndLockCursorToWindow(windowBinding);
		}else{
			hxi.UnhideAndUnlockCursor();
		}
	}


	/// hide orbits
	if (hxi.GetKey(keyboard, HX_INPUT_VK_CHAR_O, HX_INPUT_KEYSTATE_PRESSED)){
		settings.renderStellarOrbits = !settings.renderStellarOrbits;
	}


	/// switch to wireframe
	if (hxi.GetKey(keyboard, HX_INPUT_VK_CHAR_M, HX_INPUT_KEYSTATE_PRESSED)){
		settings.wireframe = !settings.wireframe;
	}



	camera.Update(*this);
	universe.Update(*this);
	universeRenderer.Update(*this, camera);
	hxg.PresentTexture(camera.renderer.postprocess_tex, window);

}



void Application::Cleanup(){
	camera.Cleanup(*this);
	universe.Cleanup(*this);
	universeRenderer.Cleanup(*this);
}
