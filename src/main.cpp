
#include "main.h"
#include "utils.h"
#include "structs.h"


// #include "Application.cpp"
// #include "Camera.cpp"
// #include "renderers.cpp"
// #include "Universe.cpp"
// #include "StaticParticleGrid.cpp"


Application app{};






void InitErrorCallbacks(){
	/// init the error app
    Graphics::SetErrorCallback([](HXRC_STATE state){
		if (state.Code != HXRC_INFO){
			std::cout << "Graphics Engine: " << state.ErrorString << '\n';
		}
        if (state.Code == HXRC_FATAL){
			exit(0);
		}
    });

	Input::SetErrorCallback([](HXRC_STATE state){
        std::cout << "Input Engine: " << state.ErrorString << '\n';
        if (state.Code == HXRC_FATAL)exit(0);
    });

    /// test the error app
    HX_GRAPHICS_ERROR_PRINT("Error App is Working");
	HX_INPUT_ERROR_PRINT("Error App is Working");
}




void InitAppEssentials(){

	/// init engines
	Graphics::InitGraphics();
	app.hxg.InitInThread();
	app.hxi.InitInThread();



	/// setup input devices
	app.keyboard = app.hxi.CreateKeyboardDevice();
	app.mouse = app.hxi.CreateMouseDevice();
	app.hxi.UseFocus(true);



	/// create window and setup input for it
	HXWindowConfig winconfig{};
	winconfig.Name = "The Universe";
	winconfig.Width = 1280;
	winconfig.Height = 720;
	winconfig.Fullscreen = false;
	winconfig.RefreshRate = 60;
	winconfig.VSync = false;
	winconfig.Transparency = false;

	winconfig.OnResize = [&](GraphicsEngine* hxg, HXUINTP, HXSIZE, HXSIZE, HXSIZE w, HXSIZE h) -> HXRC {
		app.current_width = w;
		app.current_height = h;
		app.camera.Resize(app);
		app.universeRenderer.Resize(app);
		return HXRC_OK;
	};

	app.window = app.hxg.CreateNewWindow(winconfig);
	app.windowBinding = app.hxi.AddWindowBinding(app.hxg.GetNativeWindowHandle(app.window));


	app.current_width = app.hxg.GetConfig(app.window).Width;
	app.current_height = app.hxg.GetConfig(app.window).Height;
	app.FrameIndex = 0;
	app.Time = 0;

}





int main(){

	InitErrorCallbacks();
    InitAppEssentials();
	app.Init();


	while (app.hxg.GetNumberOfWindows()){
		auto start = std::chrono::high_resolution_clock::now();

		if (app.hxi.GetKey(app.keyboard, HX_INPUT_VK_ESCAPE, HX_INPUT_KEYSTATE_PRESSED)){
			break;
		}

		app.hxi.Update();

		app.hxg.OnWindowUpdate([](GraphicsEngine* hxg, const HXWindow& window){
			return HXRC_OK;
		});

		app.Update();

		app.deltaTime = std::chrono::duration<float, std::milli>(
			std::chrono::high_resolution_clock::now() - start
		).count();
		// std::cout << "time: " << elapsedTime << '\n';
	}


	app.Cleanup();
	std::cout << "\nExited Successfully\n";


	return 0;
}
