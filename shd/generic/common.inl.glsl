#define PI 3.1415926535897932384626433832795
#define saturate(x) clamp(x, 0.0f, 1.0f)


#define POSITIVE_INFINITY uintBitsToFloat(0x7F800000)
#define NEGATIVE_INFINITY uintBitsToFloat(0xFF800000)




#ifdef HX_VERTEX_SHADER
	#define c_attrib out
#endif
#ifdef HX_FRAGMENT_SHADER
	#define c_attrib in
#endif



/// Cube: triangle strip version
const vec3 triangle_strip_cube_vertices[8] = vec3[](
	vec3(-1.0, -1.0,  1.0),
    vec3(1.0, -1.0,  1.0),
    vec3(-1.0,  1.0,  1.0),
    vec3(1.0,  1.0,  1.0),
    vec3(-1.0, -1.0, -1.0),
    vec3(1.0, -1.0, -1.0),
    vec3(-1.0,  1.0, -1.0),
    vec3(1.0,  1.0, -1.0)
);

const uint triangle_strip_cube_indices[14] = uint[](
	0, 1, 2, 3, 7, 1, 5, 4, 7, 6, 2, 4, 0, 1
);

/// Quad
const vec2 triangle_strip_quad_vertices[6] = vec2[](
	vec2(-1.0, -1.0),
	vec2(-1.0, 1.0),
	vec2(1.0, -1.0),
	vec2(1.0, -1.0),
	vec2(-1.0, 1.0),
	vec2(1.0, 1.0)
);
