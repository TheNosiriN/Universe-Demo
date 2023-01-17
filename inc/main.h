#ifndef UNIVERSE_MAIN_H
#define UNIVERSE_MAIN_H


#define HEXO_PLATFORM_OS_WINDOWS

#define HEXO_GRAPHICS_RHI_GLES3
#define HEXO_GRAPHICS_WINDOW_GLFW
#include "HexoGraphics.h"

#define HEXO_INPUT_IHI_WIN32_RAWINPUT
#include "HexoInput.h"


using namespace Hexo;
using namespace Hexo::Mathgl;
using namespace Hexo::ShaderC;



/// define precisions
typedef uint8_t ViewLayer;

/// single precision
// typedef float uni_float;
// typedef int uni_int;
// typedef vec3 uni_vec3;
// typedef ivec3 uni_ivec3;
// typedef vec2 uni_vec2;
// typedef ivec2 uni_ivec2;

/// double precision
typedef double uni_float;
typedef int64_t uni_int;
typedef f64vec3 uni_vec3;
typedef i64vec3 uni_ivec3;
typedef f64vec2 uni_vec2;
typedef i64vec2 uni_ivec2;



struct Application;

struct RenderCamera;
struct PerspectiveProjection;
struct FlyCamera;
struct OrbitCamera;
struct Frustum;
struct Camera;

struct Galaxy;
struct Universe;

struct UniverseRenderer;



#endif /* end of include guard: UNIVERSE_MAIN_H */
