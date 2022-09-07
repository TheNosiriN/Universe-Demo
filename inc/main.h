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

/// double precision
typedef double uni_float;
typedef int64_t uni_int;
typedef f64vec3 uni_vec3;
typedef i64vec3 uni_ivec3;



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
