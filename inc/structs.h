#ifndef UNIVERSE_STRUCTS_H
#define UNIVERSE_STRUCTS_H


#include "renderers.h"
#include "ui.h"



struct CameraViewpoint {
	// struct Pos {
	// 	ivec3 gridpos;
	// 	vec3 fractpos;
	// } positions[N];

	static constexpr size_t LayersCount = E_VIEW_LAYERS_COUNT;
	static constexpr double Scales[LayersCount] = {
		9.46073e+26,	/*E_VIEW_LAYER_MULTIVERSE*/			/// 100 billion light years in meters
		9.46073e+22,	/*E_VIEW_LAYER_UNIVERSE*/			/// 10^7 lightyears in meters
		9.46073e+15,	/*E_VIEW_LAYER_GALAXY*/				/// 1 lightyear in meters
		1.4958e+12,		/*E_VIEW_LAYER_STAR_ORBIT*/			/// 2150 solar radii (Stephenson 2-18)
		4.82386e+8,		/*E_VIEW_LAYER_PLANET_ORBIT*/		/// 75.6311 earth radii
		1.60934e+7,		/*E_VIEW_LAYER_MOON_ORBIT*/			/// 100000 miles in meters
		2.62700e+5,		/*E_VIEW_LAYER_ASTEROID*/			/// 262.7 kilometers (4 vesta)
		10.0000000,		/*E_VIEW_LAYER_GROUND*/				/// 1 meter
	};

	ViewLayer level;
	vec3 current_relative_pos;
	uni_vec3 relative_pos[LayersCount];
	uni_vec3 positions[LayersCount];

	CameraViewpoint(){
		level = 0;
		reset(0);
	}

	void translate(const uni_vec3& delta){
		// positions[0] += delta / Scales[0];
		//
		// for (int i=1; i<=level; ++i){
		// 	double dist = length(relative_pos[i-1]);
		// 	uni_vec3 dir = normalize(relative_pos[i-1]);
		//
		// 	double newdist = dist * Scales[i-1];
		// 	uni_vec3 newpos = -dir * (newdist/Scales[i]);
		// 	positions[i] = newpos;
		// }

		//
		// positions[level] += delta / Scales[level];
		//
		// for (int i=level-1; i>=0; --i){
		// 	// positions[i] = CalculateLayeredPos(
		// 	// 	level, i,
		// 	// 	-current_relative_pos
		// 	// );
		//
		// 	positions[i] = positions[level] / GetScaleDiff(i, level);
		// }

		for (int i=0; i<=LayersCount; ++i){
			positions[i] += delta / GetScale(i);
		}

		// for (int i=level-1; i>=0; --i){
		// 	positions[i] = (positions[i+1] * Scales[i+1]) / Scales[i];
		// }

	}

	inline void set_level(ViewLayer n){
		level = n;
	}

	inline void recalculate_position(ViewLayer from, ViewLayer to, const uni_vec3& rp){
		positions[to] = CalculateLayeredPos(from, to, rp);
	}

	inline void set_pos(ViewLayer n, const uni_vec3& p){
		positions[n] = p;
	}

	inline void set_rel_pos(const vec3& p){
		current_relative_pos = p;
	}

	inline void reset(ViewLayer n){
		for (auto i=n; i<LayersCount; ++i){
			positions[i] = uni_vec3(0);
		}
	}

	inline uni_vec3 get(ViewLayer i) {
		return positions[i];
	}

	static uni_vec3 CalculateLayeredPos(ViewLayer from, ViewLayer to, const uni_vec3& rp){
		uni_float dist = length(rp);
		uni_vec3 dir = normalize(rp);

		uni_float newdist = dist * GetScale(from);
		uni_vec3 newpos = -dir * (newdist/GetScale(to));
		return newpos;
	}

	static constexpr uni_float GetScale(ViewLayer n){
		return uni_float(Scales[n]);
	}

	static constexpr uni_float GetScaleDiff(ViewLayer n1, ViewLayer n2){
		return GetScale(n1) / GetScale(n2);
	}
};



struct RenderCamera {
	CameraSettings settings;

	HXTexture postprocess_tex;
	HXRenderPass postprocess_pass;
	HXGraphicsPipeline postprocess;
	HXCommandBuffer g_cmdbuff;

	void Init(Application&);
	void Update(Application&);
	void Resize(Application&);
	void Cleanup(Application&);
};

struct PerspectiveProjection {
	mat4 matrix;
	float rfov;
	float fov;

	void Init(Application&);
	void Update(Application&);
	void Resize(Application&);
	void Cleanup(Application&);
};

struct FlyCamera {
	enum CameraType : uint8_t {
		_FLY_CAM_TYPE = 0,
		_ORBIT_CAM_TYPE = 1,
		_CAM_TYPE_COUNT,
	};

	uint8_t type;
	CameraViewpoint eye;

	mat4 matrix;

	uni_vec3 last_coord;
	uni_vec3 focus_point;

	vec2 polar_coord;
	float roll_val;
	quat last_orientation;
	quat rorientation;
	quat orientation;
	double zoom_fac;
	double rzoom_fac;
	float lerpspeed;

	uint8_t speedgear;
	double speedgearbox;
	double acceleration;
	double velocity;

	void Init(Application&);
	void Update(Application&);
	void Cleanup(Application&);

	void UpdateFlyCam(Application&);
	void UpdateOrbitCam(Application&);
	inline void SetFocusPoint(const uni_vec3& f){
		if (type == _FLY_CAM_TYPE){ focus_point = f; }
	}
};

// struct OrbitCamera {
// 	CameraViewpoint eye;
//
// 	vec3 dir;
// 	mat4 matrix;
//
// 	float lerpspeed;
//
// 	uni_vec3 last_coord;
// 	vec3 rpolar_coord;
// 	vec3 polar_coord;
//
// 	void Init(Application&);
// 	void Update(Application&);
// 	void Cleanup(Application&);
// };

struct Frustum {
	vec4 faces[4];
};


struct Camera {
	FlyCamera view;
	RenderCamera renderer;
	PerspectiveProjection projection;
	Frustum frustum;


	void ComputeFrustum();

	inline void Init(Application& app){
		view.Init(app);
		projection.Init(app);
		renderer.Init(app);
	}
	inline void Update(Application& app){
		view.Update(app);
		projection.Update(app);
		renderer.Update(app);
		ComputeFrustum();
	}
	inline void Resize(Application& app){
		projection.Resize(app);
		renderer.Resize(app);
	}
	inline void Cleanup(Application& app){
		view.Cleanup(app);
		projection.Cleanup(app);
		renderer.Cleanup(app);
	}

};







struct Application {
	struct Settings {
		uint8_t mask_scale;
		bool mouseLocked = false;
		bool timePaused = false;
		bool renderStellarOrbits = true;
		bool wireframe = false;
	} settings;


	uint32_t FrameIndex;
	uint64_t Time;
	float deltaTime;


	/// graphics objects
	GraphicsEngine hxg;
	HXWindow window;
	size_t windowBinding;

	HXCommandAllocator g_cmdalloc;
	HXCommandAllocator c_cmdalloc;
	HXCommandAllocator t_cmdalloc;

	size_t current_width;
	size_t current_height;


	/// input objects
	InputEngine hxi;
	HXMouse mouse;
	HXKeyboard keyboard;


	/// utils storage objects
	HXSampler linearSampler;
	HXSampler nearestSampler;
	HXTexture whiteNoise2DTex;
	HXTexture blueNoise2DTex;
	HXTexture uniformMask2DTex;


	/// main objects
	Camera camera;

	Universe universe;
	UniverseRenderer universeRenderer;

	UIRenderer uiRenderer;


	void Init();
	void Update();
	void Cleanup();
};



#endif /* end of include guard: UNIVERSE_STRUCTS_H */
