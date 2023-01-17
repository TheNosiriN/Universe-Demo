#ifndef UNIVERSE_UTILS_H
#define UNIVERSE_UTILS_H



// #ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC

// #endif


#define saturate(x) clamp(x, 0.0f, 1.0f)


namespace StaticUtils {

    template<typename T, int Total>
    inline static T weight_sum(const std::initializer_list<T> list) noexcept {
        T result = 0;
        for (const T& val : list){ result += val; }
        return result / Total;
    }


	template<typename T>
	struct LerpValue {
		T current;
		T previous;
	};
	template<typename T>
	static void UpdateLerpVal(LerpValue<T>& val, T lerpspeed){
		val = Mathgl::mix(val.previous, val.current, lerpspeed);
	}



	static char* LoadShaderBlob(const std::string& name){
		std::ifstream file("assets/shaders/"+name+".hxasset", std::ios::binary);
		if (file.fail()){
			std::cerr << "Failed to open file: " << name << '\n';
		}

		file.seekg(0, std::ios::end);
		size_t length = file.tellg();
		file.seekg(0, std::ios::beg);

		// file.ignore( std::numeric_limits<std::streamsize>::max() );
		// std::streamsize length = file.gcount();
		// file.clear();   //  Since ignore will have set eof.
		// file.seekg( 0, std::ios_base::beg );

		char* data = new char[length];
		file.read(data, length);

		return data;
	}




	static uint ConstructBasicSeed(ivec4 p){
		return (uint(p.x)*1973u + uint(p.y)*9277u + uint(p.z)*10195u + uint(p.w)*26699u) | 1u;
	}

	static uint32_t wang_hash(uint& seed){
		seed = (seed ^ 61u) ^ (seed >> 16u);
	    seed *= 9u;
	    seed = seed ^ (seed >> 4);
	    seed *= 0x27d4eb2du;
	    seed = seed ^ (seed >> 15);
	    return seed;

		// seed = seed * 747796405u + 2891336453u;
		// seed = ((seed >> ((seed >> 28u) + 4u)) ^ seed) * 277803737u;
		// return (seed >> 22u) ^ seed;

		// uint seed = s;
		// seed ^= seed >> 17;
	    // seed *= uint(0xed5ad4bbU);
	    // seed ^= seed >> 11;
	    // seed *= uint(0xac4c1b51U);
	    // seed ^= seed >> 15;
	    // seed *= uint(0x31848babU);
	    // seed ^= seed >> 14;
	    // return seed;
	}

	static float frandom(uint& state){
	    return float(wang_hash(state)) / float(0xffffffffu);
	}

	static vec3 frandom_sphere(uint32_t& seed){
		vec3 x = vec3(
			frandom(seed), frandom(seed), frandom(seed)
		) *2.0f - 1.0f;
		return normalize(x);
	}

	template<typename Quat_t = quat>
	static Quat_t frandom_quaternion(uint32_t& seed){
		float u = frandom(seed);
		float v = frandom(seed);
		float w = frandom(seed);
		float two_pi = Mathgl::pi<float>() * 2.0f;
		return Quat_t(
			sqrt(1-u)*sin(two_pi*v),
			sqrt(1-u)*cos(two_pi*v),
			sqrt(u)*sin(two_pi*w),
			sqrt(u)*cos(two_pi*w)
		);
	}



	static int ind_3Dto1D(ivec3 p, ivec3 bmax) {
	    return (p.z * bmax.x * bmax.y) + (p.y * bmax.x) + p.x;
	}

	static ivec3 ind_1Dto3D(int idx, ivec3 bmax) {
	    int z = idx / (bmax.x * bmax.y);
	    idx -= (z * bmax.x * bmax.y);
	    int y = idx / bmax.x;
	    int x = idx % bmax.x;
	    return ivec3(x, y, z);
	}



	static inline float easeInExpo(float x) {
		return x==0.0f ? 0.0f : pow(2.0f, 10.0f*x - 10.0f);
	}
	static inline float easeOutExpo(float x) {
		return x==1.0f ? 1.0f : 1.0f - pow(2.0f, -10.0f * x);
	}
	static inline float easeOutInExpo( float t ){
		if( t < 0.5f ) return easeOutExpo(2.0f * t) / 2.0f;
		return easeInExpo(2.0f * t - 1.0f) / 2.0f + 0.5f;
	}



	static uni_vec3 mulvq(vec4 q, uni_vec3 v){		//rotates a 3D vector by a quaternion.
		uni_vec3 result;
		uni_float axx = q.x * 2.0f;
		uni_float ayy = q.y * 2.0f;
		uni_float azz = q.z * 2.0f;
		uni_float awxx = q.a * axx;
		uni_float awyy = q.a * ayy;
		uni_float awzz = q.a * azz;
		uni_float axxx = q.x * axx;
		uni_float axyy = q.x * ayy;
		uni_float axzz = q.x * azz;
		uni_float ayyy = q.y * ayy;
		uni_float ayzz = q.y * azz;
		uni_float azzz = q.z * azz;
		result.x = ((v.x * ((1.0f - ayyy) - azzz)) + (v.y * (axyy - awzz))) + (v.z * (axzz + awyy));
		result.y = ((v.x * (axyy + awzz)) + (v.y * ((1.0f - axxx) - azzz))) + (v.z * (ayzz - awxx));
		result.z = ((v.x * (axzz - awyy)) + (v.y * (ayzz + awxx))) + (v.z * ((1.0f - axxx) - ayyy));
		return result;
	}

	static quat qaxang(vec3 ax, float ang){	//creates a quaternion from axis and angle.
		float ha = ang * 0.5f;
		float sha = sin(ha);
		return quat(ax.x * sha, ax.y * sha, ax.z * sha, cos(ha));
	}

	static mat2 rotation2D(float angle){
	    float s = sin(angle), c = cos(angle);
	    return mat2( c, -s, s, c );
	}



	// Converts from pure Hue to linear RGB
	static vec3 hue_to_rgb(float hue){
	    float R = abs(hue * 6.0f - 3.0f) - 1.0f;
	    float G = 2.0f - abs(hue * 6.0f - 2.0f);
	    float B = 2.0f - abs(hue * 6.0f - 4.0f);
	    return clamp(vec3(R,G,B), 0.0f, 1.0f);
	}

	// Converts from HSV to linear RGB
	static vec3 hsv_to_rgb(vec3 hsv){
	    vec3 rgb = hue_to_rgb(hsv.x);
	    return ((rgb - 1.0f) * hsv.y + 1.0f) * hsv.z;
	}



	static uni_vec3 warp_uvsphere_to_vec3(uni_vec2 uv){
		uni_vec2 s = (uv-uni_vec2(0,0.5)) * uni_vec2(pi<double>(),pi<double>()*2.0);
	    return uni_vec3(
	        sin(s.x) * cos(s.y),
	        sin(s.x) * sin(s.y),
	        cos(s.x)
	    );
	}
	static uni_vec2 warp_vec3_to_uvsphere(uni_vec3 p){
		p = normalize(p);
		uni_vec2 uv = uni_vec2(
	        acos(p.z)/pi<double>(),
	        atan(p.y,p.x)/(pi<double>()*2.0) + 0.5
	    );
	    return uv;
	}
}






template<typename T, size_t C>
struct SwapHelper {
public:
	static constexpr size_t Count = C;

	T objects[Count];
	size_t current_index = 0;

	constexpr14 inline void advance(){
		current_index = (current_index+1) % Count;
	}

	constexpr14 inline T& current(){
		return objects[current_index];
	}

	constexpr14 inline T& next(){
		return objects[(current_index + 1) % Count];
	}

	constexpr14 inline T& previous(){
		return objects[(current_index - 1) % Count];
	}
};



#endif /* end of include guard: UNIVERSE_UTILS_H */
