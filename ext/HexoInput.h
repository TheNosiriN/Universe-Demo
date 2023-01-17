

#ifndef HEXO_INPUT_H
#define HEXO_INPUT_H







#ifndef HEXO_DEFINITIONS_H
#define HEXO_DEFINITIONS_H



#if !defined(NOMINMAX)
	#define NOMINMAX
#endif
#define _CRT_SECURE_NO_WARNINGS





#include <cstdio>      // vsnprintf, sscanf, printf
#include <cmath>
#include <cstdlib>
#include <cstring>


#if !defined(alloca)
	#if defined(__GLIBC__) || defined(__sun) || defined(__CYGWIN__)
		#include <alloca.h>     // alloca
	#elif defined(_WIN32)
		#include <malloc.h>     // alloca
		#if !defined(alloca)
			#define alloca _alloca  // for clang with MS Codegen
		#endif
	#else
		#include <stdlib.h>     // alloca
	#endif
#endif


#ifdef _MSC_VER
	#define __HXCPP_VER _MSVC_LANG
#else
	#define __HXCPP_VER __cplusplus
#endif





#include <string>
#include <chrono>
#include <utility>
#include <limits>

#include <atomic>
#include <thread>
#include <functional>

#include <mutex>
#include <condition_variable>
#include <vector>	/// this is used in GraphicsEngine and AssetExports as ShaderIntermediateBinary
#include <fstream>
#include <iostream>




#ifndef _MSC_VER
	#include <cxxabi.h>
#endif

#ifndef HEXO_RUNTIME_NO_EXCEPTIONS
	#include <stdexcept>
#endif

#ifndef HEXO_BUILD_STATIC
	#include <type_traits>
#endif


#define hxmalloc malloc
#define hxmemset memset
#define hxmemcpy memcpy
#define hxmemmove memmove

#if __HXCPP_VER >= 201703L
	#define constexpr17 constexpr
#else
	#define constexpr17
#endif

#if __HXCPP_VER >= 201402L
	#define constexpr14 constexpr
#else
	#define constexpr14
#endif

#if __HXCPP_VER >= 201103L
	#define constexpr11 constexpr
#else
	#define constexpr11
#endif


#ifdef _MSC_VER
	#define hx_noinline __declspec(noinline)
#else
	#define hx_noinline __attribute__((noinline))
#endif


#ifdef _MSC_VER
	#define hx_forceinline __forceinline
#else
	#define hx_forceinline __attribute__((always_inline))
#endif







#ifdef UNICODE
	#define HX_FS_WIDESTRING
#endif



#ifdef HEXO_BUILD_STATIC
	#undef DEBUG
	#undef _DEBUG
	#define HEXO_GRAPHICS_NO_TRACKING
	#define HEXO_THREADING_NO_TRACKING
#endif



#if defined(DEBUG) || defined(_DEBUG) || defined(HEXO_BUILD_DEBUG)
	#define HEXO_ENGINE_ENABLE_ERRORS
    #define HEXO_THREADING_ENABLE_ERRORS
    #define HEXO_GRAPHICS_ENABLE_ERRORS
    #define HEXO_INPUT_ENABLE_ERRORS
    #define HEXO_AUDIO_ENABLE_ERRORS
#else
	#undef HEXO_ENGINE_ENABLE_ERRORS
	#undef HEXO_THREADING_ENABLE_ERRORS
	#undef HEXO_GRAPHICS_ENABLE_ERRORS
	#undef HEXO_INPUT_ENABLE_ERRORS
	#undef HEXO_AUDIO_ENABLE_ERRORS
#endif




#ifdef HEXO_PLATFORM_OS_WINDOWS
	#include <windows.h>
	#include <windowsx.h>
#endif











namespace Hexo {


	// By: TheNosiriN
	// HXUINTP can only be used with integral types
	template<typename Size, bool = (std::is_integral<Size>::value==true)>
	struct HXSIZEP_t;

	template<typename Size>
	struct HXSIZEP_t<Size, true> {
	    Size _value_;


	    // converters
	    template<
	        typename T,
	        typename = typename std::enable_if<
	            std::is_trivially_constructible<Size, typename std::remove_cv<T>::type>::value == false
	            && (sizeof(T) >= sizeof(Size))
	        >::type
	    >
	    constexpr inline operator T() const noexcept {
	        return (T)(_value_);
	    }

	    constexpr inline operator Size() const noexcept {
	        return _value_;
	    }


	    // constructors
	    constexpr inline HXSIZEP_t() noexcept : _value_(0) {}

	    template<typename T, typename = typename std::enable_if<sizeof(T) <= sizeof(Size)>::type>
	    constexpr inline HXSIZEP_t(T&& v) noexcept : _value_((Size)(v)) {}


		// custom arithmetic
		template<typename Int, typename = typename std::enable_if<std::is_integral<Int>::value == true>::type>
		constexpr inline HXSIZEP_t operator + (const Int& b) const noexcept {
			return HXSIZEP_t(_value_ + b);
		}

		template<typename Int, typename = typename std::enable_if<std::is_integral<Int>::value == true>::type>
		constexpr inline HXSIZEP_t operator * (const Int& b) const noexcept {
			return HXSIZEP_t(_value_ * b);
		}

		template<typename Int, typename = typename std::enable_if<std::is_integral<Int>::value == true>::type>
		constexpr inline HXSIZEP_t operator - (const Int& b) const noexcept {
			return HXSIZEP_t(_value_ - b);
		}


		// stream ops
		template<typename Stream_t>
	    constexpr inline friend Stream_t& operator<<(Stream_t& os, const HXSIZEP_t& p){ return os << p._value_; }

	    template<typename Stream_t>
	    constexpr inline friend Stream_t& operator>>(Stream_t& is, HXSIZEP_t& p){ return is >> p._value_; }

	};


	using HXUINTP = HXSIZEP_t<size_t>;		// specialized for size type
	using HXUINTP32 = HXSIZEP_t<uint32_t>;	// specialized for uint32_t
	////






    typedef size_t HXSIZE;
    typedef int HXSIZE_I;
	// typedef size_t HXUINTP;

	struct HXPTRPAIR {
		HXUINTP ptr;
		HXSIZE size;
	};


	#ifdef HX_FS_WIDESTRING
		typedef std::wstring HXFileString;
		typedef wchar_t HXFileChar;
		#define HX_MKFSTR(str) L ## str
	#else
		typedef std::string HXFileString;
		typedef char HXFileChar;
		#define HX_MKFSTR(str) str
	#endif

}











#ifndef HEXO_UTILS_H
#define HEXO_UTILS_H







#if (defined(HEXO_PLATFORM_OS_WINDOWS_11) || defined(HEXO_PLATFORM_OS_WINDOWS_10) || defined(HEXO_PLATFORM_OS_WINDOWS))
	#define HXENTRY_API extern "C" __declspec(dllexport)
#else
	#define HXENTRY_API
#endif



#ifdef _MSC_VER
#    if (_MSC_VER >= 1800)
#        define __alignas_is_defined 1
#    endif
#    if (_MSC_VER >= 1900)
#        define __alignof_is_defined 1
#    endif
#else
#    include <cstdalign>   // __alignas/of_is_defined directly from the implementation
#endif

#ifdef __alignas_is_defined
#    define ALIGN(X) alignas(X)
#else
#    ifdef __GNUG__
#        define ALIGN(X) __attribute__ ((aligned(X)))
#    elif defined(_MSC_VER)
#        define ALIGN(X) __declspec(align(X))
#    else
#        error Unknown compiler, unknown alignment attribute!
#    endif
#endif

#ifdef __alignof_is_defined
#    define ALIGNOF(X) alignof(x)
#else
#    ifdef __GNUG__
#        define ALIGNOF(X) __alignof__ (X)
#    elif defined(_MSC_VER)
#        define ALIGNOF(X) __alignof(X)
#    else
#        error Unknown compiler, unknown alignment attribute!
#    endif
#endif


#define HX_PLACEMENT_NEW(address, type) new(address) type


#define HX_MAKE_NON_COPYABLE_TYPE(type) type(const type&)=delete; \
type& operator=(const type&)=delete;


#define HX_CHECK_FLAG(p, flag, action) if (p & flag){ action; }


#define MKSTR(x) #x
#define __RAW_STRING__(x) R ## #x
#define MKRWSTR(x) __RAW_STRING__((x))


#define HX_PACK_STRUCT(str) _Pragma("pack(push, 1)") str _Pragma("pack(pop)")

#define HX_LENGTH_C_ARRAY(arr) (sizeof(arr)/sizeof(arr[0]))









namespace Hexo {

	namespace Utils {


		//////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////
		//////
		////// Null pointer function tester
		//////
		/////////////////////////////////////////////////////////////////////////////////////////////////////

		#define HX_TEST_FUNCTION(F, ...) (F==nullptr ? Hexo::Utils::__ReturnDefaultTypeForFunction(F, __VA_ARGS__) : F(__VA_ARGS__))

		/// If its less than C++17 standard
		#if __HXCPP_VER >= 201703L
			template<typename F, typename... P>
			using safe_result_of = std::invoke_result<F, P...>;
		#else
			template<typename F, typename... P>
			using safe_result_of = std::result_of<F(P...)>;
		#endif

		template<typename F, typename... P>
		constexpr inline typename safe_result_of<F, P...>::type __ReturnDefaultTypeForFunction(F&& a, P&&... v) {
			return typename safe_result_of<F, P...>::type{};
		}

		/////////////////////////////////////////////////////////////////////////////////////////////////////










		//////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////
		//////
		////// some string utils
		//////
		/////////////////////////////////////////////////////////////////////////////////////////////////////

		// inline std::string loadfile(const HXFileString& f){
		// 	std::ifstream file(f, std::ios::in | std::ios::binary);
	    //     return std::string(
	    //         std::istreambuf_iterator<char>( file ),
	    //         std::istreambuf_iterator<char>()
	    //     );
		// }

		inline std::string wide2str(const std::wstring& wide) noexcept {
			std::string str(wide.length(), 0);
			for (size_t i=0; i<wide.length(); (str[i] = char(wide[i])), ++i);
			return str;
		}

		inline std::wstring str2wide(const std::string& str) noexcept {
			std::wstring wide(str.length(), 0);
			for (size_t i=0; i<str.length(); (wide[i] = wchar_t(str[i])), ++i);
			return wide;
		}

		/////////////////////////////////////////////////////////////////////////////////////////////////////










		//////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        //////
        ////// Default Placeholder Allocator
        //////
        /////////////////////////////////////////////////////////////////////////////////////////////////////

	    template <typename T>
	    struct __DefaultAllocator{

	        constexpr14 inline T* allocate(const size_t& n) {
	            return static_cast<T*>(__allocate(n, sizeof(T)));
	        }

	        constexpr14 hx_noinline void deallocate(T* p, const size_t& n) noexcept {
	            std::free(p);
	        }

	        constexpr14 hx_noinline void* __allocate(const size_t& size, const size_t& stride) noexcept {
	            if (size > std::numeric_limits<size_t>::max() / stride)return nullptr; /// issue a HXRC_FATAL message

				void* p = hxmalloc(size*stride);
				if (!p){} /// issue a HXRC_FATAL message

				return p;
	        }

	    };

	    template <class T, class U>
	    inline bool operator==(const __DefaultAllocator<T>&, const __DefaultAllocator<U>&) { return true; }
	    template <class T, class U>
	    inline bool operator!=(const __DefaultAllocator<T>&, const __DefaultAllocator<U>&) { return false; }

		/////////////////////////////////////////////////////////////////////////////////////////////////////










		//////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        ////// Author: Fabio Alemagna <personal@fabioalemagna.net>
        ////// Source: https://github.com/falemagn/fameta-counter
        ////// Inspired to Filip Roséen's work. See https://stackoverflow.com/questions/60082260/c-compile-time-counters-revisited
        /////////////////////////////////////////////////////////////////////////////////////////////////////

		namespace fameta {
		    struct default_counter_tag{};

		    template <int StartN, int StartValue = 0, int Step = 1, typename Tag = default_counter_tag>
		    class counter {
		    #if defined(__INTEL_COMPILER) || defined(_MSC_VER) || !defined(__cpp_decltype_auto)
		        template <int N>
		        struct slot {
		            #if defined(__INTEL_COMPILER)
		            #   pragma warning push
		            #   pragma warning disable 1624
		            #elif defined(__GNUC__) && !defined(__clang__)
		            #   pragma GCC diagnostic push
		            #   pragma GCC diagnostic ignored "-Wnon-template-friend"
		            #endif
		            friend constexpr int slot_value(slot<N>);
		            #if defined(__INTEL_COMPILER)
		            #   pragma warning pop
		            #elif defined(__GNUC__) && !defined(__clang__)
		            #   pragma GCC diagnostic pop
		            #endif
		        };

		        template <int N, int I>
		        struct writer {
		            friend constexpr int slot_value(slot<N>) {
		                return I;
		            }

		            static constexpr int value = I;
		        };

		        template <int N, int R = slot_value(slot<N>())>
		        static constexpr int reader(int, slot<N>) {
		            return R;
		        };
		    #else
		        template <int N>
		        struct slot {
		            #if defined(__GNUC__) && !defined(__clang__)
		            #   pragma GCC diagnostic push
		            #   pragma GCC diagnostic ignored "-Wnon-template-friend"
		            #endif
		            friend constexpr auto slot_value(slot<N>);
		            #if defined(__GNUC__) && !defined(__clang__)
		            #   pragma GCC diagnostic pop
		            #endif
		        };

		        template <int N, int I>
		        struct writer {
		            friend constexpr auto slot_value(slot<N>) {
		                return I;
		            }

		            static constexpr int value = I;
		        };

		        template <int N, typename = decltype(slot_value(slot<N>()))>
		        static constexpr int reader(int, slot<N>, int R = slot_value(slot<N>())) {
		            return R;
		        }
		    #endif

		        static_assert(sizeof(writer<StartN, StartValue-Step>), "Base case");

		        template <int N>
		        static constexpr int reader(float, slot<N>, int R = reader(0, slot<N-1>())) {
		            return R;
		        }

		    public:

		    #if !defined(__clang_major__) || __clang_major__ > 7
		        template <int N>
		        static constexpr int next(int R = writer<N, reader(0, slot<N-1>())+Step>::value) {
		            return R;
		        }

		        template <int N>
		        static constexpr int current(int R = reader(0, slot<N-1>())) {
		            return R;
		        }
		    #else
		        template <int N>
		        static constexpr int next(int R = writer<N, reader(0, slot<N>())+Step>::value) {
		            return R;
		        }

		        template <int N>
		        static constexpr int current(int R = reader(0, slot<N>())) {
		            return R;
		        }
		    #endif
		    };
		};

		/////////////////////////////////////////////////////////////////////////////////////////////////////










		//////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        //////
        ////// Typename string demangler
        ////// NOTE: The result of this function is compiler implementation defined
        /////////////////////////////////////////////////////////////////////////////////////////////////////

		template<typename T>
    	inline std::string type_to_string() {
            using TR = typename std::remove_reference<T>::type;

		#ifndef _MSC_VER
			std::string name = abi::__cxa_demangle(typeid(TR).name(), NULL, NULL, NULL);
		#else
			std::string name = typeid(TR).name();
		#endif

            if (std::is_const<TR>::value) name += " const";
            if (std::is_volatile<TR>::value) name += " volatile";
            if (std::is_lvalue_reference<T>::value) name += "&";
            else if (std::is_rvalue_reference<T>::value) name += "&&";
            return name;
    	}

		/////////////////////////////////////////////////////////////////////////////////////////////////////



	}
}











#ifndef HEXO_UTILS_TYPED_ARRAY_H
#define HEXO_UTILS_TYPED_ARRAY_H



namespace Hexo {
	namespace Utils {


		template<typename T, size_t N>
		struct TypedArray {
			using value_type = T;
			using size_type = size_t;

		private:
			value_type m_data[N];

		public:
			constexpr inline value_type& operator[](const size_type& i) const {
				return m_data[i];
			}
			constexpr inline value_type* operator+(const size_type& i) const {
				return m_data+i;
			}

			constexpr inline value_type* at(const size_type& i) const {
				if (i >= N)return nullptr;
				return m_data+i;
			}


			constexpr inline value_type* data() const { return m_data; }
			constexpr inline size_type size() const { return N; }
			constexpr inline size_type stride() const { return sizeof(value_type); }


			constexpr inline void clear() {
				for (size_type i=0; i<N; ++i){
					m_data[i].~value_type();
				}
			}

		};


	}
}




#endif /* end of include guard: HEXO_UTILS_TYPED_ARRAY_H */




#ifndef HEXO_UTILS_TYPED_VECTOR_H
#define HEXO_UTILS_TYPED_VECTOR_H


#include <type_traits>
#include <typeindex>


namespace Hexo {
	namespace Utils {



		//////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////
		////// MiniTypedVector
		////// This vector is used when there is no need for speed on push_back and erase but still need for locality
		////// e.g. Resource management. This vector trusts that the allocator will handle its memory when speed is needed
		/////////////////////////////////////////////////////////////////////////////////////////////////////

		template<typename T, typename U = __DefaultAllocator<uint8_t>>
		struct mini_typed_vector {
			using _Uint = size_t;
			using size_type = _Uint;
			using alloc_type = U;
			using value_type = T;

		private:
			value_type* m_data = nullptr;
			_Uint m_size = 0;

			alloc_type alloc;

		public:

			constexpr inline value_type& operator[](const _Uint& i) const {
				return m_data[i];
			}
			constexpr inline value_type* operator+(const _Uint& i) const {
				return m_data+i;
			}

			constexpr inline value_type* at(const _Uint& i) const {
				if (!m_data || i >= m_size)return nullptr;
				return m_data + i;
			}


			constexpr14 inline void transfer(value_type** out) noexcept { (*out) = m_data; m_data=nullptr, m_size=0; }
			constexpr inline value_type* data() const { return this->m_data; }
			constexpr inline _Uint size() const { return this->m_size; }
			constexpr inline _Uint stride() const { return sizeof(value_type); }
			constexpr inline _Uint last_index() const { return (this->m_size>0) ? (this->m_size-1) : 0; }
			constexpr inline bool empty() const { return this->m_size == 0; }


			/// constructors
			constexpr14 mini_typed_vector() : m_size(0), m_data(nullptr), alloc() {}
			constexpr14 mini_typed_vector(const alloc_type& a) : m_size(0), m_data(nullptr), alloc(a) {}
			~mini_typed_vector(){ clear(); }

			/// copy semantics
			constexpr14 mini_typed_vector(const mini_typed_vector& other) : m_size(other.m_size), alloc(other.alloc){
				m_data = (value_type*)alloc.allocate(other.m_size*sizeof(value_type));
				__SafeCopy(m_data, other.m_data, other.m_size);
			}
			constexpr14 mini_typed_vector& operator=(const mini_typed_vector& other){
				if (m_data){
					alloc.deallocate((uint8_t*)(m_data), m_size*sizeof(value_type));
				}

				m_size = other.m_size;
				alloc = other.alloc;
				m_data = (value_type*)alloc.allocate(other.m_size*sizeof(value_type));
				__SafeCopy(m_data, other.m_data, other.m_size);

				return *this;
			}

			/// move semantics
			constexpr14 mini_typed_vector(mini_typed_vector&& other) : m_size(other.m_size), alloc(other.alloc){
				other.transfer(&this->m_data);
			}
			constexpr14 mini_typed_vector& operator=(mini_typed_vector&& other){
				if (m_data){
					alloc.deallocate((uint8_t*)(m_data), m_size*sizeof(value_type));
				}

				m_size = other.m_size;
				alloc = other.alloc;
				other.transfer(&this->m_data);
				return *this;
			}


			constexpr14 void reset(){
				if (m_data){
					alloc.deallocate((uint8_t*)(m_data), m_size*sizeof(value_type));
				}
				m_data = nullptr;
				m_size = 0;
			}


			template<typename NT = value_type>
			constexpr inline typename std::enable_if<(std::is_trivially_copyable<NT>::value || !std::is_copy_constructible<NT>::value) == true, void>::type
			__SafeCopy(NT* dst, const NT* src, const size_type& count){
				hxmemcpy(dst, src, count*sizeof(NT));
			}
			template<typename NT = value_type>
			constexpr inline typename std::enable_if<(std::is_trivially_copyable<NT>::value || !std::is_copy_constructible<NT>::value) == false, void>::type
			__SafeCopy(NT* dst, const NT* src, const size_type& count){
				for (size_t i=0; i<count; ++i){ HX_PLACEMENT_NEW(dst+i, NT{src[i]}); }
			}


			template<typename NT = value_type>
			constexpr inline typename std::enable_if<(std::is_trivially_move_assignable<NT>::value || !std::is_move_constructible<NT>::value) == true, void>::type
			__SafeMove(NT* dst, const NT* src, const size_type& count){
				hxmemmove(dst, src, count*sizeof(NT));
			}
			template<typename NT = value_type>
			constexpr inline typename std::enable_if<(std::is_trivially_move_assignable<NT>::value || !std::is_move_constructible<NT>::value) == false, void>::type
			__SafeMove(NT* dst, const NT* src, const size_type& count){
				for (size_t i=0; i<count; ++i){ HX_PLACEMENT_NEW(dst+i, NT{ std::move(src[i]) }); }
			}


			template<typename NT, typename... Args>
			constexpr inline typename std::enable_if<std::is_constructible<NT, Args...>::value == true, void>::type
			__ConstructInPlace(_Uint ptr, Args&&... a) {
				HX_PLACEMENT_NEW(m_data+ptr, NT(std::forward<NT>(a)...));
			}

			template<typename NT, typename... Args>
			constexpr inline typename std::enable_if<std::is_constructible<NT, Args...>::value == false, void>::type
			__ConstructInPlace(_Uint ptr, Args&&... a) {
				if (std::is_default_constructible<NT>::value){
					HX_PLACEMENT_NEW(m_data+ptr, NT{a...});
				}else{
					/// at this point, the object can never be constructed with the params the user gave it
					/// so just run its trivial default constructor
					HX_PLACEMENT_NEW(m_data+ptr, NT{});
				}
			}


			constexpr14 bool __Reallocate(const _Uint& count){
				value_type* ndata = (value_type*)(alloc.allocate((m_size+count)*sizeof(value_type)));
				if (!ndata)return false;

				if (m_data && ndata!=m_data){
					__SafeMove(ndata, m_data, m_size);
				}

				alloc.deallocate((uint8_t*)(m_data), m_size*sizeof(value_type));
				m_data = ndata;
				return true;
			}


			template<typename... Args>
			constexpr14 _Uint emplace_back(Args&&... a){
				if (!__Reallocate(1))return 0;

				__ConstructInPlace<T>(m_size, a...);
				m_size += 1;
				return m_size-1;
			}

			constexpr14 _Uint push_back(const value_type& obj){
				if (!__Reallocate(1))return 0;

				__SafeCopy<value_type>(m_data+m_size, &obj, 1);
				m_size += 1;
				return m_size-1;
			}


			/// TODO: FIX THIS! IT IS LEAKING MEMORY!!!
			constexpr14 bool eraseFast(const _Uint& i){
				if (!m_data || i >= m_size)return false;

				value_type* ndata = (value_type*)(alloc.allocate((m_size-1)*sizeof(value_type)));
				if (!ndata)return false;

				if (m_data && ndata!=m_data){
					__SafeCopy(ndata, m_data, i);
					__SafeCopy(ndata, m_data+i+1, m_size-(i+1));
				}

				alloc.deallocate((uint8_t*)(m_data), m_size*sizeof(value_type));
				m_data = ndata;
				m_size -= 1;
				return true;
			}
			///

			constexpr14 bool erase(const _Uint& i){
				T* p = at(i);
				if (p){ (*p).~value_type(); }
				return eraseFast(i);
			}



			constexpr14 inline void clearFast(){
				reset();
			}

			constexpr14 void clear(){
				for (_Uint i=0; i<m_size; ++i){
					m_data[i].~value_type();
				}
				reset();
			}

		};

		/////////////////////////////////////////////////////////////////////////////////////////////////////












		//////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////
		//////
		////// TypedVector
		////// This vector is used as a replacement to the common std::vector
		/////////////////////////////////////////////////////////////////////////////////////////////////////

		template<typename T, typename U = __DefaultAllocator<uint8_t>>
		struct typed_vector {
			using _Uint = size_t;
			using size_type = _Uint;
			using alloc_type = U;
			using value_type = T;

		private:
			value_type* m_data = nullptr;
			_Uint m_size = 0;
			_Uint m_realsize = 0;

			alloc_type alloc;

		public:

			constexpr inline value_type& operator[](const _Uint& i) const {
				return m_data[i];
			}
			constexpr inline value_type* operator+(const _Uint& i) const {
				return m_data+i;
			}


			struct Iterator {
				typed_vector* parent = nullptr;
				_Uint index = 0;

				constexpr14 inline Iterator& operator++(){
					if (parent){ index += 1; }
					return *this;
				}
				constexpr14 inline Iterator& operator--(){
					if (parent){ index -= (index==0 ? 0 : 1); }
					return *this;
				}
				constexpr14 inline Iterator operator++(int){
					Iterator t = *this;
					++*this; return t;
				}
				constexpr14 inline Iterator operator--(int){
					Iterator t = *this;
					--*this; return t;
				}
				constexpr inline Iterator operator+(const _Uint& num) const {
					return Iterator{ this->parent, this->index+num };
				}
				constexpr inline Iterator operator-(const _Uint& num) const {
					return Iterator{ this->parent, this->index-num };
				}
				constexpr14 inline Iterator& operator+=(const _Uint& num){
					this->index += num;
					return *this;
				}
				constexpr14 inline Iterator& operator-=(const _Uint& num){
					this->index -= num;
					return *this;
				}

				constexpr inline bool operator!=(const Iterator& t) const { return (this->index != t.index); }
				constexpr inline bool operator==(const Iterator& t) const { return (this->index == t.index); }

				constexpr inline value_type* operator*() const {
					if (parent){ return parent->at(index); }
					return nullptr;
				}

			};

			constexpr14 inline Iterator begin() { return Iterator{ this, 0 }; }
			constexpr14 inline Iterator end() { return Iterator{ this, this->m_size }; }


			/// constructors
			constexpr14 typed_vector() : m_size(0), m_realsize(0), m_data(nullptr), alloc() {}
			constexpr14 typed_vector(const alloc_type& a) : m_size(0), m_realsize(0), m_data(nullptr), alloc(a) {}
			~typed_vector(){ clear(); reset(); }

			/// copy semantics
			constexpr14 typed_vector(const typed_vector& other) : m_size(other.m_size), m_realsize(other.m_size), alloc(other.alloc){
				m_data = (value_type*)alloc.allocate(other.m_size*sizeof(value_type));
				__SafeCopy(m_data, other.m_data, other.m_size);
			}
			constexpr14 typed_vector& operator=(const typed_vector& other){
				if (m_data){
					alloc.deallocate((uint8_t*)(m_data), m_realsize*sizeof(value_type));
				}

				m_size = other.m_size;
				m_realsize = other.m_size;
				alloc = other.alloc;
				m_data = (value_type*)alloc.allocate(other.m_size*sizeof(value_type));
				__SafeCopy(m_data, other.m_data, other.m_size);

				return *this;
			}

			/// move semantics
			constexpr14 typed_vector(typed_vector&& other) : m_size(other.m_size), m_realsize(other.m_size), alloc(other.alloc){
				other.transfer(&this->m_data);
			}
			constexpr14 typed_vector& operator=(typed_vector&& other){
				if (m_data){
					alloc.deallocate((uint8_t*)(m_data), m_realsize*sizeof(value_type));
				}

				m_size = other.m_size;
				m_realsize = other.m_size;
				alloc = other.alloc;
				other.transfer(&this->m_data);
				return *this;
			}


			constexpr14 void reset(){
				if (m_data){
					alloc.deallocate((uint8_t*)(m_data), m_realsize*sizeof(value_type));
				}
				m_data = nullptr;
				m_realsize = 0;
				m_size = 0;
			}


			constexpr inline value_type* __GetNoCheck(const _Uint& i) const {
				return m_data + i;
			}

			constexpr inline bool __ValidateIndex(const _Uint& i) const {
				return !(!m_data || i>=m_size);
			}

			constexpr inline value_type* at(const _Uint& i) const {
				if (!m_data || i >= m_size)return nullptr;
				return m_data + i;
			}



			template<typename NT = value_type>
			constexpr inline typename std::enable_if<(std::is_trivially_copyable<NT>::value || !std::is_copy_constructible<NT>::value) == true, void>::type
			__SafeCopy(NT* dst, const NT* src, const size_type& count) const {
				hxmemcpy(dst, src, count*sizeof(NT));
			}
			template<typename NT = value_type>
			constexpr inline typename std::enable_if<(std::is_trivially_copyable<NT>::value || !std::is_copy_constructible<NT>::value) == false, void>::type
			__SafeCopy(NT* dst, const NT* src, const size_type& count) const {
				for (size_t i=0; i<count; ++i){ HX_PLACEMENT_NEW(dst+i, NT{src[i]}); }
			}


			template<typename NT = value_type>
			constexpr inline typename std::enable_if<(std::is_trivially_move_assignable<NT>::value || !std::is_move_constructible<NT>::value) == true, void>::type
			__SafeMove(NT* dst, const NT* src, const size_type& count) const {
				hxmemmove(dst, src, count*sizeof(NT));
			}
			template<typename NT = value_type>
			constexpr inline typename std::enable_if<(std::is_trivially_move_assignable<NT>::value || !std::is_move_constructible<NT>::value) == false, void>::type
			__SafeMove(NT* dst, const NT* src, const size_type& count) const {
				for (size_t i=0; i<count; ++i){ HX_PLACEMENT_NEW(dst+i, NT{ std::move(src[i]) }); }
			}


			template<typename NT, typename... Args>
			constexpr inline typename std::enable_if<std::is_constructible<NT, Args...>::value == true, void>::type
			__ConstructInPlace(_Uint ptr, Args&&... a) const {
				HX_PLACEMENT_NEW(m_data+ptr, NT(std::forward<NT>(a)...));
			}

			template<typename NT, typename... Args>
			constexpr inline typename std::enable_if<std::is_constructible<NT, Args...>::value == false, void>::type
			__ConstructInPlace(_Uint ptr, Args&&... a) const {
				if (std::is_default_constructible<NT>::value){
					HX_PLACEMENT_NEW(m_data+ptr, NT{a...});
				}else{
					/// at this point, the object can never be constructed with the params the user gave it
					/// so just run its trivial default constructor
					HX_PLACEMENT_NEW(m_data+ptr, NT{});
				}
			}


			constexpr14 bool __CheckForReallocate(const _Uint& count){
				if (m_size+count >= m_realsize){
					/// python 3 list: https://github.com/python/cpython/blob/2.6/Objects/listobject.c#L48
					_Uint newsize = m_size+count;
					m_realsize += (newsize >> 3) + (newsize < 9 ? 3 : 6);
					///

					value_type* ndata = (value_type*)(alloc.allocate(m_realsize*sizeof(T)));
					if (!ndata)return false;

					if (m_data && ndata!=m_data){
						__SafeMove(ndata, m_data, m_size);
						alloc.deallocate((uint8_t*)(m_data), m_realsize*sizeof(T));
					}

					m_data = ndata;
				}
				return true;
			}


			constexpr14 bool reserve(const _Uint& size){
				if (m_size+size <= m_realsize)return true;

				m_realsize += size - (m_realsize-m_size);

				value_type* ndata = (value_type*)(alloc.allocate(m_realsize*sizeof(T)));
				if (!ndata)return false;

				if (m_data && ndata != m_data){
					__SafeMove(ndata, m_data, m_size);
				}

				alloc.deallocate((uint8_t*)(m_data), m_realsize*sizeof(T));
				m_data = ndata;
				return true;
			}



			constexpr14 inline void transfer(value_type** out) noexcept { (*out) = m_data; m_data=nullptr, m_size=0, m_realsize=0; }
			constexpr inline value_type* data() const { return this->m_data; }
			constexpr inline _Uint size() const { return this->m_size; }
			constexpr inline _Uint stride() const { return sizeof(value_type); }
			constexpr inline _Uint capacity() const { return this->m_realsize; }
			constexpr inline _Uint last_index() const { return (this->m_size>0) ? (this->m_size-1) : 0; }
			constexpr inline bool empty() const { return this->m_size == 0; }



			template<typename... Args>
			constexpr14 inline _Uint emplace_mult(const _Uint& pos, const _Uint& count, Args&&... a){
				if (!__CheckForReallocate(count))return 0;

				__SafeCopy(m_data+pos+count, m_data+pos, m_size-pos);

				for (_Uint i=0; i<count; ++i){
					__ConstructInPlace<T>(pos+i, a...);
				}
				m_size += count;
				return pos;
			}

			template<typename... Args>
			constexpr14 _Uint emplace(const _Uint& pos, Args&&... a){
				return emplace_mult(pos, 1, a...);
			}

			constexpr14 inline _Uint insert_mult(const _Uint& pos, const _Uint& count, T* data){
				if (!__CheckForReallocate(count))return 0;

				__SafeCopy(m_data + (pos+count), m_data+pos, m_size-pos);
				__SafeCopy(m_data + pos, data, 1);

				m_size += count;
				return pos;
			}

			constexpr14 inline _Uint insert(const _Uint& pos, T&& data){
				return insert_mult(pos, 1, &data);
			}


			template<typename... Args>
			constexpr14 _Uint emplace_back(Args&&... a){
				if (!__CheckForReallocate(1))return 0;

				__ConstructInPlace<T>(m_size, a...);
				m_size += 1;
				return m_size-1;
			}

			constexpr14 _Uint push_back(const value_type& obj){
				if (!__CheckForReallocate(1))return 0;

				__SafeCopy<value_type>(m_data+m_size, &obj, 1);
				m_size += 1;
				return m_size-1;
			}

			template<typename... M>
			constexpr14 inline _Uint push_back_mult(M&&... objs){
				if (!reserve( sizeof...(objs) ))return 0;

				size_type index = this->size();
				int dummy[] = { ((void) push_back(objs), 0)... };
				return index;
			}

			constexpr14 inline _Uint push_back_array(const T* arr, const size_type& sz){
				if (!reserve(sz))return 0;

				size_type index = this->size();
				for (size_type i=0; i<sz; ++i){ push_back(arr[i]); }

				return index;
			}



			constexpr14 void eraseFast(const _Uint& i){
				if (!m_data || i >= m_size)return;
				__SafeMove(__GetNoCheck(i), __GetNoCheck(i+1), m_size-1-i);
				m_size -= 1;
			}

			constexpr14 void erase(const _Uint& i) {
				T* p = at(i);
				if (p){ (*p).~value_type(); }
				eraseFast(i);
			}

			constexpr14 void erase(const Iterator& t) {
				erase(t.index);
			}



			constexpr14 inline void clearFast(){
				m_size = 0;
			}

			constexpr14 void clear(){
				for (_Uint i=0; i<m_size; ++i){
					m_data[i].~value_type();
				}
				clearFast();
			}


		};

		/////////////////////////////////////////////////////////////////////////////////////////////////////












		//////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////
		////// TypelessVector
		////// This vector is used to bypass static typesafety.
		////// The user must ensure that each object inserted is of the same stride and similar (but not the same) type at runtime.
		////// For library builders: to ensure runtime typesafety use the TypesafeTypelessVector
		/////////////////////////////////////////////////////////////////////////////////////////////////////

		struct __NoTypeChecker {
			template<typename T> constexpr14 inline void init_checker() const {}
			template<typename T> constexpr14 inline bool compare_type() const { return true; }
			constexpr14 inline void reset_checker() const {}
		};

		struct DefaultTypeChecker {
			std::type_index sign;

			DefaultTypeChecker() : sign(typeid(void)) {}

			template<typename T> constexpr14 inline void init_checker(){ sign = std::type_index(typeid(std::remove_cv<T>::type)); }
			template<typename T> constexpr14 inline bool compare_type() const { return (std::type_index(typeid(std::remove_cv<T>::type)) == sign); }
			inline void reset_checker(){ sign = std::type_index(typeid(void)); }
		};




		template<typename TypeChecker = __NoTypeChecker, typename U = __DefaultAllocator<uint8_t>>
		struct typeless_vector : TypeChecker {
			using _Uint = size_t;
			using size_type = _Uint;
			using alloc_type = U;
			using value_type = uint8_t;
			using byte_type = value_type;

		private:
			value_type* m_data = nullptr;
			_Uint m_stride = 0;
			_Uint m_size = 0;
			_Uint m_realsize = 0;

			alloc_type alloc;

		public:

			constexpr inline void* operator[](const _Uint& i) const {
				return m_data + i*m_stride;
			}

			template<typename T>
			constexpr inline T& operator[](const _Uint& i) const {
				return *cast<T>( m_data + i*m_stride );
			}

			constexpr inline value_type* operator+(const _Uint& i) const {
				return m_data+i;
			}



			struct Iterator {
				typeless_vector* parent = nullptr;
				_Uint index = 0;

				constexpr14 inline Iterator& operator++(){
					if (parent){ index += 1; }
					return *this;
				}
				constexpr14 inline Iterator& operator--(){
					if (parent){ index -= (index==0 ? 0 : 1); }
					return *this;
				}
				constexpr14 inline Iterator operator++(int){
					Iterator t = *this;
					++*this; return t;
				}
				constexpr14 inline Iterator operator--(int){
					Iterator t = *this;
					--*this; return t;
				}
				constexpr inline Iterator operator+(const _Uint& num) const {
					return Iterator{ this->parent, this->index+num };
				}
				constexpr inline Iterator operator-(const _Uint& num) const {
					return Iterator{ this->parent, this->index-num };
				}
				constexpr14 inline Iterator& operator+=(const _Uint& num){
					this->index += num;
					return *this;
				}
				constexpr14 inline Iterator& operator-=(const _Uint& num){
					this->index -= num;
					return *this;
				}

				constexpr inline bool operator!=(const Iterator& t) const { return (this->index != t.index); }
				constexpr inline bool operator==(const Iterator& t) const { return (this->index == t.index); }

				constexpr inline void* operator*() const {
					if (parent){ return parent->at(index); }
					return nullptr;
				}

			};

			constexpr14 inline Iterator begin() { return Iterator{ this, 0 }; }
			constexpr14 inline Iterator end() { return Iterator{ this, this->m_size }; }



			/// constructors
			constexpr14 typeless_vector() : m_stride(0), m_size(0), m_realsize(0), m_data(nullptr), alloc() {}
			constexpr14 typeless_vector(const alloc_type& a) : m_stride(0), m_size(0), m_realsize(0), m_data(nullptr), alloc(a) {}

			constexpr14 typeless_vector(const _Uint& stride){
				init_raw(stride);
			}
			constexpr14 typeless_vector(const _Uint& stride, const alloc_type& a) : alloc(a) {
				init_raw(stride);
			}

			template<typename T>
			constexpr14 typeless_vector(T&&){
				__Init<T>();
			}
			template<typename T>
			constexpr14 typeless_vector(T&&, const alloc_type& a) : alloc(a) {
				__Init<T>();
			}

			~typeless_vector(){ reset(); }


			/// copy semantics
			constexpr14 typeless_vector(const typeless_vector& other) : m_size(other.m_size), m_stride(other.m_stride), m_realsize(other.m_size), alloc(other.alloc){
				m_data = alloc.allocate(other.m_size*other.m_stride);
				__SafeCopy(m_data, other.m_data, other.m_size);
			}
			constexpr14 typeless_vector& operator=(const typeless_vector& other){
				reset();

				m_size = other.m_size;
				m_realsize = other.m_size;
				m_stride = other.m_stride;
				alloc = other.alloc;
				m_data = alloc.allocate(other.m_size*other.m_stride);
				__SafeCopy(m_data, other.m_data, other.m_size);

				return *this;
			}


			/// move semantics
			constexpr14 typeless_vector(typeless_vector&& other) : m_size(other.m_size), m_stride(other.m_stride), m_realsize(other.m_size), alloc(other.alloc){
				other.transfer(&this->m_data);
			}
			constexpr14 typeless_vector& operator=(typeless_vector&& other){
				reset();

				m_size = other.m_size;
				m_realsize = other.m_size;
				m_stride = other.m_stride;
				alloc = other.alloc;
				other.transfer(&this->m_data);
				return *this;
			}



			template<typename T>
			constexpr14 bool __Init(){
				this->template init_checker<T>();
				m_stride = sizeof(T);
				m_size = 0;
				m_realsize = 0;
				m_data = nullptr;

				if (m_stride == 0)return false;
				return true;
			}

			template<typename T>
			constexpr14 inline bool init(){
				return __Init<T>();
			}
			template<typename T>
			constexpr14 inline bool init(T&&){
				return __Init<T>();
			}

			constexpr14 bool init_raw(const _Uint& stride){
				m_stride = stride;
				m_size = 0;
				m_realsize = 0;
				m_data = nullptr;

				if (stride == 0)return false;
				return true;
			}

			constexpr14 void reset(){
				if (m_data){
					alloc.deallocate(m_data, m_realsize*m_stride);
				}
				m_data = nullptr;
				m_realsize = 0;
				m_size = 0;
				this->reset_checker();
			}




			template <typename T>
			constexpr inline bool __check_type_helper(T*, std::true_type) const noexcept {
				return sizeof(T) == m_stride;
			}

			template <typename T>
			constexpr inline bool __check_type_helper(T*, std::false_type) const noexcept {
				return this->template compare_type<T>();
			}

			/// please don't ever use this function if you are not using a TypeChecker
			template<typename T>
			constexpr inline bool check_type(T* t) const noexcept {
				return __check_type_helper(t, std::is_same<TypeChecker, __NoTypeChecker>::value);
			}




			template<typename T>
			constexpr inline typename std::enable_if<std::is_same<TypeChecker, __NoTypeChecker>::value == false, T*>::type
			cast(void* p) const {
				if ( this->template compare_type<T>() == false ){
					/// throw some error or call quits cuz the user violated typesafety
					#ifndef HEXO_RUNTIME_NO_EXCEPTIONS
					throw std::runtime_error("typeless_vector's TypeChecker detected a different type from the one given at last init/construct");
					#endif
					return nullptr;
				}
				return (T*)p;
			}

			template<typename T>
			constexpr inline typename std::enable_if<std::is_same<TypeChecker, __NoTypeChecker>::value == true, T*>::type
			cast(void* p) const {
				return (T*)p;
			}




			constexpr inline void* __GetNoCheck(const _Uint& i) const {
				return m_data + i*m_stride;
			}

			constexpr inline bool __ValidateIndex(const _Uint& i) const {
				return !(!m_data || i>=m_size);
			}

			constexpr inline void* at(const _Uint& i) const {
				if (!m_data || i >= m_size)return nullptr;
				return m_data + i*m_stride;
			}

			template<typename T>
			constexpr inline T& at(const _Uint& i) const {
				return *(cast<T>(at(i)));
			}



			template<typename NT = value_type>
			constexpr inline typename std::enable_if<(std::is_trivially_copyable<NT>::value || !std::is_copy_constructible<NT>::value) == true, void>::type
			__SafeCopy(NT* dst, const NT* src, const size_type& count) const {
				hxmemcpy(dst, src, count*m_stride);
			}
			template<typename NT = value_type>
			constexpr inline typename std::enable_if<(std::is_trivially_copyable<NT>::value || !std::is_copy_constructible<NT>::value) == false, void>::type
			__SafeCopy(NT* dst, const NT* src, const size_type& count) const {
				for (size_t i=0; i<count; ++i){ HX_PLACEMENT_NEW(dst+i, NT{cast<NT>(src[i])}); }
			}


			template<typename NT = value_type>
			constexpr inline typename std::enable_if<(std::is_trivially_move_assignable<NT>::value || !std::is_move_constructible<NT>::value) == true, void>::type
			__SafeMove(NT* dst, const NT* src, const size_type& count) const {
				hxmemmove(dst, src, count*m_stride);
			}
			template<typename NT = value_type>
			constexpr inline typename std::enable_if<(std::is_trivially_move_assignable<NT>::value || !std::is_move_constructible<NT>::value) == false, void>::type
			__SafeMove(NT* dst, const NT* src, const size_type& count) const {
				for (size_t i=0; i<count; ++i){ HX_PLACEMENT_NEW(dst+i, NT{ std::move(cast<NT>(src[i])) }); }
			}


			template<typename NT, typename... Args>
			constexpr inline typename std::enable_if<std::is_constructible<NT, Args...>::value == true, void>::type
			__ConstructInPlace(_Uint ptr, Args&&... a) const {
				HX_PLACEMENT_NEW(cast<NT>(m_data)+ptr, NT(std::forward<NT>(a)...));
			}

			template<typename NT, typename... Args>
			constexpr inline typename std::enable_if<std::is_constructible<NT, Args...>::value == false, void>::type
			__ConstructInPlace(_Uint ptr, Args&&... a) const {
				if (std::is_default_constructible<NT>::value){
					HX_PLACEMENT_NEW(cast<NT>(m_data)+ptr, NT{a...});
				}else{
					/// at this point, the object can never be constructed with the params the user gave it
					/// so just run its trivial default constructor
					HX_PLACEMENT_NEW(cast<NT>(m_data)+ptr, NT{});
				}
			}


			template<typename NT = value_type>
			constexpr14 bool __CheckForReallocate(const _Uint& count){
				if (m_size+count >= m_realsize){
					/// python 3 list: https://github.com/python/cpython/blob/2.6/Objects/listobject.c#L48
					_Uint newsize = m_size+count;
					m_realsize += (newsize >> 3) + (newsize < 9 ? 3 : 6);
					///
					value_type* ndata = alloc.allocate(m_realsize*m_stride);
					if (!ndata)return false;

					if (m_data && ndata!=m_data){
						__SafeMove<NT>(ndata, m_data, m_size);
						alloc.deallocate(m_data, m_realsize*m_stride);
					}

					m_data = ndata;
				}
				return true;
			}

			template<typename NT = value_type>
			constexpr14 bool reserve(const _Uint& size){
				if (m_size+size <= m_realsize)return true;

				m_realsize += size - (m_realsize-m_size);

				value_type* ndata = alloc.allocate(m_realsize*m_stride);
				if (!ndata)return false;

				if (m_data && ndata != m_data){
					__SafeMove<NT>(ndata, m_data, m_size);
				}
				m_data = ndata;

				return true;
			}



			constexpr14 inline void transfer(void** out) noexcept { (*out) = m_data; m_data=nullptr, m_size=0, m_realsize=0; }
			constexpr inline void* data() const { return this->m_data; }

			template<typename T>
			constexpr inline T* data() const { return cast<T>(m_data); }

			constexpr inline _Uint size() const { return this->m_size; }
			constexpr inline _Uint stride() const { return this->m_stride; }
			constexpr inline _Uint capacity() const { return this->m_realsize; }
			constexpr inline _Uint last_index() const { return (this->m_size>0) ? (this->m_size-1) : 0; }
			constexpr inline bool empty() const { return this->m_size == 0; }




			template<typename T, typename... Args>
			constexpr14 _Uint emplace_mult(const _Uint& pos, const _Uint& count, Args&&... a){
				if (!__CheckForReallocate(count))return 0;

				T* rdata = cast<T>(m_data);
				__SafeCopy(rdata+pos+count, rdata+pos, m_size-pos);

				for (_Uint i=0; i<count; ++i){
					__ConstructInPlace<T>(pos+i, a...);
				}
				m_size += count;
				return pos;
			}

			template<typename T, typename... Args>
			constexpr14 inline _Uint emplace(const _Uint& pos, Args&&... a){
				return emplace_mult<T>(pos, 1, a...);
			}

			template<typename T>
			constexpr14 _Uint insert_mult(const _Uint& pos, const _Uint& count, T* data){
				if (!__CheckForReallocate(count))return 0;

				uint8_t* rdata = (uint8_t*)cast<T>(m_data);
				__SafeCopy(rdata + (pos+count)*m_stride, rdata + pos*m_stride, m_size-pos);
				__SafeCopy(rdata + pos*m_stride, data, 1);

				m_size += count;
				return pos;
			}

			constexpr14 _Uint insert_mult_raw(const _Uint& pos, const _Uint& count, void* data){
				if (!__CheckForReallocate(count))return 0;

				hxmemcpy(m_data + (pos+count)*m_stride, m_data + pos*m_stride, (m_size-pos)*m_stride);
				hxmemcpy(m_data + pos*m_stride, data, m_stride);

				m_size += count;
				return pos;
			}

			template<typename T>
			constexpr14 inline _Uint insert(const _Uint& pos, T&& data){
				return insert_mult(pos, 1, &data);
			}


			template<typename T, typename... Args>
			constexpr14 _Uint emplace_back(Args&&... a){
				if (!__CheckForReallocate(1))return 0;

				__ConstructInPlace<T>(m_size, a...);
				m_size += 1;
				return m_size-1;
			}

			template<typename T>
			constexpr14 _Uint push_back(const T& obj){
				if (!__CheckForReallocate(1))return 0;
				__SafeCopy<T>(m_data + m_size*m_stride, &obj, 1);
				m_size += 1;
				return m_size-1;
			}

			constexpr14 inline _Uint push_back_raw(const void* obj, const size_t& sz){
				if (!__CheckForReallocate(1))return 0;
				hxmemcpy(m_data + (m_size*m_stride), obj, std::min<size_t>(m_stride, sz));
				m_size += 1;
				return m_size-1;
			}

			template<typename... M>
			constexpr14 inline _Uint push_back_mult(M&&... objs){
				if (!reserve<std::tuple_element<0, std::tuple<M...>>::type>( sizeof...(objs) ))return 0;

				size_type index = this->size();
				int dummy[] = { ((void) push_back<M>(objs), 0)... };
				return index;
			}

			template<typename T>
			constexpr14 inline _Uint push_back_array(const T* arr, const size_type& sz){
				if (!reserve<T>(sz))return 0;

				size_type index = this->size();
				for (size_type i=0; i<sz; ++i){ push_back<T>(arr[i]); }

				return index;
			}



			constexpr14 void erase(const _Uint& i){
				if (!m_data || i >= m_size)return;
				hxmemmove(
					__GetNoCheck(i), __GetNoCheck(i+1), m_stride*(m_size-1-i)
				);
				m_size -= 1;
			}

			template<typename T>
			constexpr14 void erase(const _Uint& i) {
				if (!m_data || i >= m_size)return;
				T* p = cast<T>(at(i));
				if (p){ (*p).~T(); }
				__SafeMove<>(__GetNoCheck(i), __GetNoCheck(i+1), m_size-1-i);
				m_size -= 1;
			}

			constexpr14 inline void erase(const Iterator& t) {
				erase(t.index);
			}

			template<typename T>
			constexpr inline void erase(const Iterator& t) {
				erase<T>(t.index);
			}


			constexpr14 inline void clearFast(){
				m_size = 0;
			}

			constexpr14 inline void clear(){
				hxmemset(m_data, 0, m_stride*m_size);
				m_size = 0;
			}

			template<typename T>
			constexpr14 void clear(){
				for (_Uint i=0; i<m_size; ++i){
					at<T>(i).~T();
				}
				clear();
			}


		};

		/////////////////////////////////////////////////////////////////////////////////////////////////////







		template<typename T, typename A = __DefaultAllocator<uint8_t>>
		using MiniTypedVector = mini_typed_vector<T, A>;

		template<typename T, typename A = __DefaultAllocator<uint8_t>>
		using TypedVector = typed_vector<T, A>;

		template<typename T = __DefaultAllocator<uint8_t>>
		using TypelessVector = typeless_vector<__NoTypeChecker, T>;

		template<typename T = __DefaultAllocator<uint8_t>>
		using TypesafeTypelessVector = typeless_vector<DefaultTypeChecker, T>;


	}
}




#endif /* end of include guard: HEXO_UTILS_TYPED_VECTOR_H */


#ifndef HEXO_UTILS_TYPED_LINKED_LIST_H
#define HEXO_UTILS_TYPED_LINKED_LIST_H



namespace Hexo {
	namespace Utils {


		//////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////
		////// TypelessLinkedList (TrulyTypeless)
        ////// This is used when storing polymorphic objects. It is preferred over vector because either way
        ////// the element is a pointer and will be dereferenced, so the benefit of vector does not come into play.
		////// Instead of storing nested pointers, why not directly store the polymorphic data in the node itself?
        /////////////////////////////////////////////////////////////////////////////////////////////////////

		template<typename U>
		struct typless_linked_list {
			using _Uint = size_t;
			using size_type = _Uint;
			using alloc_type = U;

			struct Node {
				Node* next;
				size_type size;
			};

			template<typename T>
			struct NodeData : Node {
				T data;
			};

			using BasicNode = NodeData<uint8_t>;


		private:
			alloc_type alloc;
			Node* m_first;
			Node* m_last;
			_Uint m_size;


		public:
			constexpr14 typless_linked_list() : alloc(), m_first(nullptr), m_last(nullptr){}
			constexpr14 typless_linked_list(const alloc_type& a) : alloc(a), m_first(nullptr), m_last(nullptr){}
			~typless_linked_list(){ clear(); }


			constexpr14 inline void* operator[](const _Uint& i) const {
				return &(at(i).data);
			}
			constexpr14 inline void* operator+(const _Uint& i) const {
				return &(at(i).data);
			}



			constexpr14 inline void transfer(Node** out) noexcept { (*out) = m_first; m_first = nullptr; m_size = 0; }
			constexpr inline Node* first() const { return m_first; }
			constexpr inline Node* last() const { return m_last; }
			constexpr inline _Uint size() const { return m_size; }
			constexpr inline _Uint last_index() const { return (m_size>0) ? (m_size-1) : 0; }
			constexpr inline bool empty() const { return m_size == 0; }


			constexpr14 inline Node* at(const _Uint& i) const {
				Node* p = m_first;
				_Uint ti = 0;
				while (p != nullptr){
					if (ti == i){ return p; }
					p = p->next;
					ti += 1;
				}
				return p;
			}




			template<typename NT>
			constexpr inline typename std::enable_if<(std::is_trivially_copyable<NT>::value || !std::is_copy_constructible<NT>::value) == true, void>::type
			__SafeCopy(NT* dst, const NT* src, const size_type& count) const {
				hxmemcpy(dst, src, count*sizeof(NT));
			}
			template<typename NT>
			constexpr inline typename std::enable_if<(std::is_trivially_copyable<NT>::value || !std::is_copy_constructible<NT>::value) == false, void>::type
			__SafeCopy(NT* dst, const NT* src, const size_type& count) const {
				for (size_t i=0; i<count; ++i){ HX_PLACEMENT_NEW(dst+i, NT{src[i]}); }
			}


			template<typename NT>
			constexpr inline typename std::enable_if<(std::is_trivially_move_assignable<NT>::value || !std::is_move_constructible<NT>::value) == true, void>::type
			__SafeMove(NT* dst, const NT* src, const size_type& count) const {
				hxmemmove(dst, src, count*sizeof(NT));
			}
			template<typename NT>
			constexpr inline typename std::enable_if<(std::is_trivially_move_assignable<NT>::value || !std::is_move_constructible<NT>::value) == false, void>::type
			__SafeMove(NT* dst, const NT* src, const size_type& count) const {
				for (size_t i=0; i<count; ++i){ HX_PLACEMENT_NEW(dst+i, NT{ std::move(src[i]) }); }
			}


			template<typename NT, typename... Args>
			constexpr inline typename std::enable_if<std::is_constructible<NT, Args...>::value == true, void>::type
			__ConstructInPlace(NT* ptr, Args&&... a) const {
				HX_PLACEMENT_NEW(ptr, NT(std::forward<NT>(a)...));
			}

			template<typename NT, typename... Args>
			constexpr inline typename std::enable_if<std::is_constructible<NT, Args...>::value == false, void>::type
			__ConstructInPlace(NT* ptr, Args&&... a) const {
				if (std::is_default_constructible<NT>::value){
					HX_PLACEMENT_NEW(ptr, NT{a...});
				}else{
					/// at this point, the object can never be constructed with the params the user gave it
					/// so just run its trivial default constructor
					HX_PLACEMENT_NEW(ptr, NT{});
				}
			}


			template<typename T>
			constexpr14 NodeData<T>* __AllocateSpace(){
				NodeData<T>* t = (NodeData<T>*)alloc.allocate(sizeof(T) + sizeof(Node));
				t->next = nullptr;
				t->size = sizeof(T);
				return t;
			}



			template<typename T, typename... Args>
			constexpr14 _Uint emplace_back(Args&&... a){
				NodeData<T>* t = __AllocateSpace();
				if (!t)return 0;

				__ConstructInPlace<T>(&(t->data), a...);

				if (!m_first){
					m_first = t;
					m_last = m_first;
				}else{
					m_last->next = t;
				}

				return m_size++;
			}


			template<typename T>
			constexpr14 _Uint push_back(const T& obj){
				NodeData<T>* t = __AllocateSpace();
				if (!t)return 0;

				__SafeCopy<T>(&(t->data), &obj, 1);

				if (!m_first){
					m_first = t;
					m_last = m_first;
				}else{
					m_last->next = t;
				}

				return m_size++;
			}



			constexpr14 void eraseFast(const _Uint& i){
				if (!m_size)return;

				if (i == m_size-1){
					// m_last
					/// TODO:
					return;
				}
			}

			template<typename T>
			constexpr14 void erase(const _Uint& i){
				/// TODO:
			}



			constexpr14 void clear(){
				Node* p = m_first;
				Node* nt = nullptr;
				while (p != nullptr){
					nt = p->next;
					if (p){ alloc.deallocate(p, p->size + sizeof(Node)); }
					p = nt;
				}

				m_first = nullptr;
				m_last = nullptr;
				m_size = 0;
			}



		};


		template<typename U>
		using TyplessLinkedList = typless_linked_list<U>;

	}
}



#endif /* end of include guard: HEXO_UTILS_TYPED_LINKED_LIST_H */




#ifndef HEXO_UTILS_TYPED_QUEUE_H
#define HEXO_UTILS_TYPED_QUEUE_H



namespace Hexo {
	namespace Utils {


		template<typename T, typename U = Utils::__DefaultAllocator<uint8_t>, typename C = TypedVector<T, U>>
		struct TypedQueue {
		    using value_type = T;
		    using container_type = C;
		    using size_type = size_t;

		private:
		    container_type data;

		public:
		    TypedQueue(){}
			TypedQueue(const typename container_type::alloc_type& a) : data(a) {}
		    ~TypedQueue(){}

		    constexpr inline size_type size() const noexcept { return data.size(); }
		    constexpr inline bool empty() const noexcept { return data.empty(); }

		    constexpr inline void push(const value_type& d){
		        data.push_back(d);
		    }

		    template<typename... Args>
		    constexpr inline void emplace(Args&&... a){
		        data.emplace_back(a...);
		    }

		    constexpr inline value_type* front() const {
		        if (data.empty())return nullptr;
		        return &(data[0]);
		    }

		    constexpr inline value_type* back() const {
		        if (data.empty())return nullptr;
		        return data + (data.last_index());
		    }

		    constexpr inline void pop_front(){
				value_type t = std::move(data[0]);
		        data.erase(0);
		    }

		    constexpr inline void pop_back(){
				value_type t = std::move(data[data.last_index()]);
		        data.erase(data.last_index());
		    }

		    constexpr inline void pop(){
		        pop_front();
		    }

		};



	}

}



#endif /* end of include guard: HEXO_UTILS_TYPED_QUEUE_H */




#ifndef HEXO_UTILS_TYPED_SPARSESET_H
#define HEXO_UTILS_TYPED_SPARSESET_H




namespace Hexo {

	namespace Utils {


		template<typename T, typename U = Utils::__DefaultAllocator<uint8_t>>
		struct TypedSparseSet {
		    using value_type = T;
		    using size_type = size_t;
			using key_type = size_type;
			using alloc_type = U;

		    template<typename R>
		    using container_type = Utils::TypedVector<R, U>;


		    struct Data {
		        value_type data;
		        size_type id;
		    };


		private:
		    container_type<Data> m_dense;
		    container_type<size_type> m_sparse;


		public:
		    constexpr14 TypedSparseSet() = default;
			constexpr14 TypedSparseSet(const alloc_type& a) : m_dense(a), m_sparse(a) {}
		    ~TypedSparseSet(){ clear(); }


			constexpr inline size_type full_size() const noexcept { return m_sparse.size(); }
			constexpr inline size_type size() const noexcept { return m_dense.size(); }
			constexpr inline size_type* sparse() noexcept { return m_sparse.data(); }
			constexpr inline Data* dense() noexcept { return m_dense.data(); }
		    constexpr inline Data* data() noexcept { return m_dense.data(); }
			constexpr inline bool empty() const { return m_dense.empty(); }


		    constexpr inline value_type& operator[](const key_type& i) const {
		        return (*at(i));
		    }

		    constexpr inline value_type* operator+(const key_type& i) const {
		        return at(i);
		    }

			template<typename F, typename... Args>
			constexpr inline void iterate(F&& func, Args&&... a){
				for (size_t i=0; i<m_dense.size(); ++i){
					func(m_dense[i].data, i, a...);
				}
			}


		    constexpr14 key_type push_back(const value_type& a){
				/// The counter is the size of the m_sparse array because the array never shrinks
				size_type Counter = m_sparse.size();

		        m_dense.push_back(Data{a, Counter});
		        m_sparse.push_back( m_dense.size() );

		        return key_type{Counter+1};
		    }

		    template<typename... Args>
		    constexpr14 key_type emplace_back(Args&&... a){
				size_type Counter = m_sparse.size();

		        m_dense.push_back(Data{std::forward<Args>(a)..., Counter});
		        m_sparse.push_back( m_dense.size() );

		        return key_type{Counter+1};
		    }

		    constexpr inline bool validate(const key_type& p) const noexcept {
		        if (p < 1)return false;
		        size_type in = m_sparse[p-1];
		        if (!in)return false;

		        return true;
		    }

		    constexpr inline value_type* at(const key_type& p) const {
		        if (p < 1)return nullptr;
		        size_type in = m_sparse[p-1];
		        if (!in)return nullptr;

		        return &(m_dense[in-1].data);
		    }

		    constexpr14 void erase(const key_type& p){
		        if (p < 1)return;
		        size_type in = m_sparse[p-1];
		        if (!in)return;

		        in -= 1;
		        const Data& d = m_dense[in];
				size_type lst = m_dense.last_index();

		        m_sparse[d.id] = 0;
		        m_sparse[m_dense[lst].id] = in+1;

		        d.data.~value_type();
		        std::swap( m_dense[in], m_dense[lst] );
		        m_dense.erase(lst);
		    }

			constexpr14 inline void clear() noexcept {
				m_dense.clear();
				m_sparse.clear();
			}

		};


	}


}





#endif /* end of include guard: HEXO_UTILS_TYPED_SPARSESET_H */




#ifndef HEXO_UTILS_TYPED_HASHMAP_H
#define HEXO_UTILS_TYPED_HASHMAP_H



#include <unordered_map>

namespace Hexo {
	namespace Utils {


		/// TODO: MAKE THIS!!!
		template<typename T, typename U>
		using TypedHashMap = std::unordered_map<T, U>;

		template<typename T>
		using StringHashMap = TypedHashMap<std::string, T>;

		template<typename T>
		using IntHashMap = TypedHashMap<HXSIZE, T>;

	}
}




#endif /* end of include guard: HEXO_UTILS_TYPED_HASHMAP_H */




#ifndef HEXO_UTILS_BITSET_H
#define HEXO_UTILS_BITSET_H




namespace Hexo {
	namespace Utils {



		template<HXSIZE N, typename TN = HXSIZE, typename prec_t = TN>
		struct Bitset {
		    using size_type = HXSIZE;
		    using value_type = TN;
			static constexpr const HXSIZE BitsPerChunk = std::numeric_limits<prec_t>::digits;

		private:
		    value_type bits[size_type(N/BitsPerChunk)+1];


		public:
		    struct reference {
		        Bitset<N, value_type, prec_t>* p;
		        size_type index;

		        constexpr14 reference(Bitset<N, value_type, prec_t>* _p, const size_type& _i) : p(_p), index(_i) {}

		        constexpr operator bool() const { return p->test(index); }
		        constexpr reference& operator=(bool b) noexcept { p->set(index, b); return *this; }
		        constexpr reference& operator=(const reference& b) noexcept { p->set(index, b); return *this; }

		    };



		    constexpr14 Bitset(){ reset(); }
			

		    constexpr inline size_type size() const noexcept { return N; }
		    constexpr14 inline void reset() noexcept {
		        // for (size_type i=0; i<(sizeof(bits)/sizeof(size_type)); bits[i]=0, ++i);
				hxmemset(&bits, 0, sizeof(bits));
		    }


		    constexpr14 inline reference operator[](const size_type& i) noexcept {
		        return reference(this, i);
		    }
		    constexpr inline bool operator[](const size_type& i) const noexcept {
		        return test(i);
		    }


		    constexpr inline bool test(const size_type& i) const noexcept {
		        if (i >= N)return false;
		        size_type chunkIndex = size_type(i/BitsPerChunk);
		        size_type index = (BitsPerChunk-1) - (i - (chunkIndex*BitsPerChunk));
		        return bits[chunkIndex] & (index == BitsPerChunk ? index : size_type(1)<<index);
		    }

		    constexpr14 inline void set(const size_type& i, bool v) noexcept {
		        if (i >= N)return;
		        size_type chunkIndex = size_type(i/BitsPerChunk);
		        size_type index = (BitsPerChunk-1) - (i - (chunkIndex*BitsPerChunk));

		        if (v){
		            bits[chunkIndex] |= (prec_t)(index == BitsPerChunk ? index : size_type(1)<<index);
		        }else{
		            bits[chunkIndex] &= ~(prec_t)(index == BitsPerChunk ? index : size_type(1)<<index);
		        }
		    }

		};




		template<typename Container = TypedVector<HXSIZE>, typename prec_t = typename Container::value_type>
		struct DynamicBitset {
		    using size_type = HXSIZE;
		    using value_type = typename Container::value_type;
			using container_type = Container;
			static constexpr const HXSIZE BitsPerChunk = std::numeric_limits<prec_t>::digits;

		private:
		    container_type bits;
		    size_type lastIndex;


		public:
		    struct reference {
		        DynamicBitset<container_type, prec_t>* p;
		        size_type index;

		        constexpr14 reference(DynamicBitset<container_type, prec_t>* _p, const size_type& _i) : p(_p), index(_i) {}

		        constexpr operator bool() const { return p->test(index); }
		        constexpr reference& operator=(bool b) noexcept { p->set(index, b); return *this; }
		        constexpr reference& operator=(const reference& b) noexcept { p->set(index, b); return *this; }

		    };



		    constexpr14 DynamicBitset() : lastIndex(0) {}
			constexpr14 DynamicBitset(const typename container_type::alloc_type& a) : lastIndex(0), bits(a) {}
		    ~DynamicBitset() = default;

		    constexpr inline size_type size() const noexcept { return bits.size()/BitsPerChunk; }
		    constexpr14 inline void reset() noexcept {
				lastIndex = 0;
				bits.clearFast();
			}


		    constexpr14 inline reference operator[](const size_type& i) noexcept {
		        return reference(this, i);
		    }
		    constexpr inline bool operator[](const size_type& i) const noexcept {
		        return test(i);
		    }


		    constexpr inline bool test(const size_type& i) const noexcept {
		        if (i >= bits.size()*BitsPerChunk)return false;
		        size_type chunkIndex = size_type(i/BitsPerChunk);
		        size_type index = (BitsPerChunk-1) - (i - (chunkIndex*BitsPerChunk));
		        return bits[chunkIndex] & (index == BitsPerChunk ? index : size_type(1)<<index);
		    }

		    constexpr14 inline void set(const size_type& i, bool v) noexcept {
		        if (i >= bits.size()*BitsPerChunk)return;
		        size_type chunkIndex = size_type(i/BitsPerChunk);
		        size_type index = (BitsPerChunk-1) - (i - (chunkIndex*BitsPerChunk));

		        if (v){
		            bits[chunkIndex] |= (prec_t)(index == BitsPerChunk ? index : size_type(1)<<index);
		        }else{
		            bits[chunkIndex] &= ~(prec_t)(index == BitsPerChunk ? index : size_type(1)<<index);
		        }
		    }





		    constexpr14 inline size_type push_back(bool v = false) {
		        if (lastIndex >= (bits.size()*BitsPerChunk) ){
		            bits.push_back(size_type(0));
		        }

		        set(lastIndex, v);
		        lastIndex += 1;
		        return lastIndex-1;
		    }

			constexpr14 inline bool reserve(const size_type i){
				return bits.reserve(size_type(i/BitsPerChunk)+1);
			}

		};




		template<HXSIZE N, typename prec_t = HXSIZE>
		using AtomicBitset = Bitset<N, std::atomic<prec_t>, prec_t>;


		// template<typename prec_t = HXSIZE, typename A = __DefaultAllocator, typename Container = TypedVector<std::atomic<prec_t>, A>, HXSIZE B = std::numeric_limits<HXSIZE>::digits>
		// using AtomicDynamicBitset = DynamicBitset<Container, B>;



	}
}





#endif /* end of include guard: HEXO_UTILS_BITSET_H */





#ifndef HEXO_UTILS_SPINLOCK_H
#define HEXO_UTILS_SPINLOCK_H





namespace Hexo {
	namespace Utils {


		struct Spinlock {
		    std::atomic<bool> locked;

		    Spinlock(){ locked = false; }
		    ~Spinlock(){ locked = false; }

		    Spinlock(const Spinlock&) = delete;
		    Spinlock& operator=(Spinlock const&) = delete;

		    inline void lock() noexcept {
		        for(;;){
		            if (locked == false)break;
		        }
		        locked = true;
		    }

		    inline void unlock() noexcept {
		        locked = false;
		    }

		};


	}
}





#endif /* end of include guard: HEXO_UTILS_SPINLOCK_H */








#endif /* end of include guard: HEXO_UTILS_H */







#endif /* end of include guard: HEXO_DEFINITIONS_H */



#ifndef HEXO_HXRC_H
#define HEXO_HXRC_H


#include <functional>


namespace Hexo {
	typedef uint8_t HXRC;




	enum : HXRC {
		HXRC_OK			= 0,
		HXRC_INFO		= 1,
		HXRC_NOT_OK		= 2,
		HXRC_WARNING	= 3,
		HXRC_FATAL		= 4,
	};


	struct HXRC_STATE {
		HXRC_STATE(const std::string& s, const HXRC& c) : ErrorString(s), Code(c) {}
		std::string ErrorString = nullptr;
		HXRC Code = 0;
	};


	struct HXRC_APP {
		std::function<void(HXRC_STATE)> func;
	};


	struct HXRC_CATCHER {
		std::string pushed;
		HXRC level;
	};


	struct HXRC_SYS {
		Utils::TypedVector<HXRC_APP> Apps;
		HXRC_CATCHER Catcher;

		template<typename F>
		inline size_t AddApp(F&& func) noexcept {
			return Apps.emplace_back(func) + 1;
		}

		static inline HXRC_SYS* Get() noexcept {
			static HXRC_SYS h;
			return &h;
		}

		inline HXRC_APP* GetApp(size_t i) const noexcept {
			return Apps+(i-1);
		}
	};



	template<typename F>
	static inline size_t HXRC_REGISTER_APP(F&& func) noexcept {
		return HXRC_SYS::Get()->AddApp(func);
	}



	static inline HXRC HXRC_BUILD(size_t app, const std::string& string, HXRC code) noexcept {
		if (app >= 1){
			// const HXRC_SYS* p = HXRC_SYS::Get()->Apps[i-1];
			// HXRC_SYS::Get()->Apps.size();
			if (HXRC_SYS::Get()->Apps[app-1].func){
				HXRC_SYS::Get()->Apps[app-1].func(HXRC_STATE{string, code});
			}
		}
		return code;
	}


	static inline HXRC HXRC_PUSH(const std::string& string, HXRC code) noexcept {
		auto sys = HXRC_SYS::Get();

		sys->Catcher.pushed += (string+"\n");
		HXRC prev = sys->Catcher.level;

		sys->Catcher.level = (prev > code ? prev : code);
		return sys->Catcher.level;
	}


	static inline HXRC HXRC_MERGE(const HXRC& a, const HXRC& b) noexcept {
		return a > b ? a : b;
	}




	#define HXRC_FAILED(x) (x != HXRC_OK)
	#define HXRC_SUCCESS(x) (x == HXRC_OK)

	#define HXRC_PRINT(string, app) HXRC_BUILD(app, string, HXRC_INFO);

	#define HXRC_MERGE_RETURN(x, y) HXRC_MERGE(x,y); HXRC_RETURN(x);

	#define HXRC_RETURN_VOID(x) if (x != HXRC_OK)return;
	#define HXRC_RETURN_BOOL(x) if (x != HXRC_OK)return false;
	#define HXRC_RETURN(x) { HXRC c=x; if (c != HXRC_OK)return c; }

	#define HXRC_RETURN_INFO(string, app) return HXRC_BUILD(app, string, HXRC_INFO);
	#define HXRC_RETURN_FATAL(string, app) return HXRC_BUILD(app, string, HXRC_FATAL);
	#define HXRC_RETURN_WARNING(string, app) return HXRC_BUILD(app, string, HXRC_WARNING);

	#define HXRC_RETURN_BOOL_INFO(string, app) HXRC_BUILD(app, string, HXRC_INFO);return false;
	#define HXRC_RETURN_BOOL_FATAL(string, app) HXRC_BUILD(app, string, HXRC_FATAL);return false;
	#define HXRC_RETURN_BOOL_WARNING(string, app) HXRC_BUILD(app, string, HXRC_WARNING);return false;

	#define HXRC_ASSERT(x, string, app, code) if (!(x))return HXRC_BUILD(app, string, code);
	#define HXRC_ASSERT_INFO(x, string, app) if (!(x))return HXRC_BUILD(app, string, HXRC_INFO);
	#define HXRC_ASSERT_FATAL(x, string, app) if (!(x))return HXRC_BUILD(app, string, HXRC_FATAL);
	#define HXRC_ASSERT_WARNING(x, string, app) if (!(x))return HXRC_BUILD(app, string, HXRC_WARNING);

	#define HXRC_NO_RETURN_INFO(string, app) HXRC_BUILD(app, string, HXRC_INFO);
	#define HXRC_NO_RETURN_FATAL(string, app) HXRC_BUILD(app, string, HXRC_FATAL);
	#define HXRC_NO_RETURN_WARNING(string, app) HXRC_BUILD(app, string, HXRC_WARNING);

	#define HXRC_ASSERT_NO_RETURN(x, string, app, code) if (!(x))HXRC_BUILD(app, string, code);
	#define HXRC_ASSERT_INFO_NO_RETURN(x, string, app) if (!(x))HXRC_BUILD(app, string, HXRC_INFO);
	#define HXRC_ASSERT_FATAL_NO_RETURN(x, string, app) if (!(x))HXRC_BUILD(app, string, HXRC_FATAL);
	#define HXRC_ASSERT_WARNING_NO_RETURN(x, string, app) if (!(x))HXRC_BUILD(app, string, HXRC_WARNING);

	#define HXRC_PUSH_NO_RETURN(x, code) HXRC_PUSH(string, code);
	#define HXRC_PUSH_RETURN(string, code) return HXRC_PUSH(string, code);
	#define HXRC_PUSH_RETURN_ASSERT(x, string, code) if (!(x))return HXRC_PUSH(string, code);

};




#endif /* end of include guard: HEXO_HXRC_H */





#ifndef HEXO_MATH_H
#define HEXO_MATH_H




#define GLM_ENABLE_EXPERIMENTAL
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
#include <glm/gtc/reciprocal.hpp> // glm::cot
#include <glm/gtx/quaternion.hpp> // glm::quat
#include <glm/gtc/type_precision.hpp> // glm::f64vec3
#include <glm/ext/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective
#include <glm/ext/scalar_constants.hpp> // glm::pi
#include <glm/gtx/matrix_decompose.hpp> // glm::decompose






namespace Hexo {
	namespace Mathgl = glm;


namespace Math {







#ifndef HEXO_MATH_VECTOR_H
#define HEXO_MATH_VECTOR_H






template<typename F>
struct HXVector4 {
	F x, y, z, w;

	//constructors
	constexpr14 HXVector4(){ x=0; y=0; z=0; w=0; }
	constexpr14 HXVector4(F f){ x=f; y=f; z=f; w=f; }
	constexpr14 HXVector4(F _x, F _y, F _z, F _w){ x=_x; y=_y; z=_z; w=_w; }
	constexpr14 HXVector4(const HXVector4<F>& v){ x=v.x; y=v.y; z=v.z; w=v.w; }


	std::string tostring() const noexcept {
			return
			"x: "+std::to_string(x)+
			" y: "+std::to_string(y)+
			" z: "+std::to_string(z)+
			" w: "+std::to_string(w);
	}


	//Vector3s
	friend constexpr14 inline HXVector4<F> operator + (const HXVector4<F>& v1, const HXVector4<F>& v2) { return HXVector4<F>(v1.x+v2.x, v1.y+v2.y, v1.z+v2.z, v1.w+v2.w); }
	friend constexpr14 inline HXVector4<F> operator - (const HXVector4<F>& v1, const HXVector4<F>& v2) { return HXVector4<F>(v1.x-v2.x, v1.y-v2.y, v1.z-v2.z, v1.w-v2.w); }
	friend constexpr14 inline HXVector4<F> operator * (const HXVector4<F>& v1, const HXVector4<F>& v2) { return HXVector4<F>(v1.x*v2.x, v1.y*v2.y, v1.z*v2.z, v1.w*v2.w); }
	friend constexpr14 inline HXVector4<F> operator / (const HXVector4<F>& v1, const HXVector4<F>& v2) { return HXVector4<F>(v1.x/v2.x, v1.y/v2.y, v1.z/v2.z, v1.w/v2.w); }

	friend constexpr14 HXVector4<F> operator + (const HXVector4<F>& v, const F f) { return HXVector4<F>(v.x+f, v.y+f, v.z+f, v.w+f); }
	friend constexpr14 HXVector4<F> operator - (const HXVector4<F>& v, const F f) { return HXVector4<F>(v.x-f, v.y-f, v.z-f, v.w-f); }
	friend constexpr14 HXVector4<F> operator * (const HXVector4<F>& v, const F f) { return HXVector4<F>(v.x*f, v.y*f, v.z*f, v.w*f); }
	friend constexpr14 HXVector4<F> operator / (const HXVector4<F>& v, const F f) { return HXVector4<F>(v.x/f, v.y/f, v.z/f, v.w/f); }

	friend HXVector4<F> operator + (const F f, const HXVector4<F>& v){ return HXVector4<F>(f+v.x, f+v.y, f+v.z, f+v.w); }
	friend HXVector4<F> operator - (const F f, const HXVector4<F>& v){ return HXVector4<F>(f-v.x, f-v.y, f-v.z, f-v.w); }
	friend HXVector4<F> operator * (const F f, const HXVector4<F>& v){ return HXVector4<F>(f*v.x, f*v.y, f*v.z, f*v.w); }
	friend HXVector4<F> operator / (const F f, const HXVector4<F>& v){ return HXVector4<F>(f/v.x, f/v.y, f/v.z, f/v.w); }

	//booleans
	bool operator > (const HXVector4<F>& v){ return (x > v.x) && (y > v.y) && (z > v.z) && (w > v.w); }
	bool operator < (const HXVector4<F>& v){ return (x < v.x) && (y < v.y) && (z < v.z) && (w < v.w); }
	bool operator <= (const HXVector4<F>& v){ return (x <= v.x) && (y <= v.y) && (z <= v.z) && (w <= v.w); }
	bool operator >= (const HXVector4<F>& v){ return (x >= v.x) && (y >= v.y) && (z >= v.z) && (w >= v.w); }
	bool operator == (const HXVector4<F>& v){ return (x == v.x) && (y == v.y) && (z == v.z) && (w == v.w); }
	bool operator != (const HXVector4<F>& v){ return (x != v.x) || (y != v.y) || (z != v.z) || (w != v.w); }

	bool operator > (const F v){ return (x > v) && (y > v) && (z > v) && (w > v); }
	bool operator < (const F v){ return (x < v) && (y < v) && (z < v) && (w < v); }
	bool operator <= (const F v){ return (x <= v) && (y <= v) && (z <= v) && (w <= v); }
	bool operator >= (const F v){ return (x >= v) && (y >= v) && (z >= v) && (w >= v); }
	bool operator == (const F v){ return (x == v) && (y == v) && (z == v) && (w == v); }
	bool operator != (const F v){ return (x != v) || (y != v) || (z != v) || (w != v); }

	//voids
	void operator += (const HXVector4<F>& v){ x += v.x; y += v.y; z += v.z; w += v.w; }
	void operator -= (const HXVector4<F>& v){ x -= v.x; y -= v.y; z -= v.z; w -= v.w; }
	void operator *= (const HXVector4<F>& v){ x *= v.x; y *= v.y; z *= v.z; w *= v.w; }
	void operator /= (const HXVector4<F>& v){ x /= v.x; y /= v.y; z /= v.z; w /= v.w; }

	void operator += (const F v){ x += v; y += v; z += v; w += v; }
	void operator -= (const F v){ x -= v; y -= v; z -= v; w -= v; }
	void operator *= (const F v){ x *= v; y *= v; z *= v; w *= v; }
	void operator /= (const F v){ x /= v; y /= v; z /= v; w /= v; }


};








template<typename F>
struct HXVector3 {
	F x, y, z;

	//constructors
	HXVector3(){ x=0; y=0; z=0; }
	HXVector3(F f){ x=f; y=f; z=f; }
	HXVector3(F _x, F _y, F _z){ x=_x; y=_y; z=_z; }
	HXVector3(const HXVector3<F>& v){ x=v.x; y=v.y; z=v.z; }


	inline std::string tostring(){
			return
			"x: "+std::to_string(x)+
			" y: "+std::to_string(y)+
			" z: "+std::to_string(z);
	}


	//Vector3s
	friend HXVector3<F> operator + (const HXVector3<F>& v1, const HXVector3<F>& v2){ return HXVector3<F>(v1.x+v2.x, v1.y+v2.y, v1.z+v2.z); }
	friend HXVector3<F> operator - (const HXVector3<F>& v1, const HXVector3<F>& v2){ return HXVector3<F>(v1.x-v2.x, v1.y-v2.y, v1.z-v2.z); }
	friend HXVector3<F> operator * (const HXVector3<F>& v1, const HXVector3<F>& v2){ return HXVector3<F>(v1.x*v2.x, v1.y*v2.y, v1.z*v2.z); }
	friend HXVector3<F> operator / (const HXVector3<F>& v1, const HXVector3<F>& v2){ return HXVector3<F>(v1.x/v2.x, v1.y/v2.y, v1.z/v2.z); }

	friend HXVector3<F> operator + (const HXVector3<F>& v, const F f){ return HXVector3<F>(v.x+f, v.y+f, v.z+f); }
	friend HXVector3<F> operator - (const HXVector3<F>& v, const F f){ return HXVector3<F>(v.x-f, v.y-f, v.z-f); }
	friend HXVector3<F> operator * (const HXVector3<F>& v, const F f){ return HXVector3<F>(v.x*f, v.y*f, v.z*f); }
	friend HXVector3<F> operator / (const HXVector3<F>& v, const F f){ return HXVector3<F>(v.x/f, v.y/f, v.z/f); }

	friend HXVector3<F> operator + (const F f, const HXVector3<F>& v){ return HXVector3<F>(f+v.x, f+v.y, f+v.z); }
	friend HXVector3<F> operator - (const F f, const HXVector3<F>& v){ return HXVector3<F>(f-v.x, f-v.y, f-v.z); }
	friend HXVector3<F> operator * (const F f, const HXVector3<F>& v){ return HXVector3<F>(f*v.x, f*v.y, f*v.z); }
	friend HXVector3<F> operator / (const F f, const HXVector3<F>& v){ return HXVector3<F>(f/v.x, f/v.y, f/v.z); }

	//booleans
	bool operator > (const HXVector3<F>& v){ return (x > v.x) && (y > v.y) && (z > v.z); }
	bool operator < (const HXVector3<F>& v){ return (x < v.x) && (y < v.y) && (z < v.z); }
	bool operator <= (const HXVector3<F>& v){ return (x <= v.x) && (y <= v.y) && (z <= v.z); }
	bool operator >= (const HXVector3<F>& v){ return (x >= v.x) && (y >= v.y) && (z >= v.z); }
	bool operator == (const HXVector3<F>& v){ return (x == v.x) && (y == v.y) && (z == v.z); }
	bool operator != (const HXVector3<F>& v){ return (x != v.x) || (y != v.y) || (z != v.z); }

	bool operator > (const F v){ return (x > v) && (y > v) && (z > v); }
	bool operator < (const F v){ return (x < v) && (y < v) && (z < v); }
	bool operator <= (const F v){ return (x <= v) && (y <= v) && (z <= v); }
	bool operator >= (const F v){ return (x >= v) && (y >= v) && (z >= v); }
	bool operator == (const F v){ return (x == v) && (y == v) && (z == v); }
	bool operator != (const F v){ return (x != v) || (y != v) || (z != v); }

	//voids
	void operator += (const HXVector3<F>& v){ x += v.x; y += v.y; z += v.z; }
	void operator -= (const HXVector3<F>& v){ x -= v.x; y -= v.y; z -= v.z; }
	void operator *= (const HXVector3<F>& v){ x *= v.x; y *= v.y; z *= v.z; }
	void operator /= (const HXVector3<F>& v){ x /= v.x; y /= v.y; z /= v.z; }

	void operator += (const F v){ x += v; y += v; z += v; }
	void operator -= (const F v){ x -= v; y -= v; z -= v; }
	void operator *= (const F v){ x *= v; y *= v; z *= v; }
	void operator /= (const F v){ x /= v; y /= v; z /= v; }


};









template<typename F>
struct HXVector2 {
	F x, y;

	//constructors
	constexpr14 HXVector2(){ x=0; y=0; }
	constexpr14 HXVector2(F f){ x=f; y=f; }
	constexpr14 HXVector2(F _x, F _y){ x=_x; y=_y; }
	constexpr14 HXVector2(const HXVector2<F>& v){ x=v.x; y=v.y; }


	inline std::string tostring() {
			return
			"x: "+std::to_string(x)+
			" y: "+std::to_string(y);
	}


	//Vector3s
	friend constexpr14 inline HXVector2<F> operator + (const HXVector2<F>& v1, const HXVector2<F>& v2){ return HXVector2<F>(v1.x+v2.x, v1.y+v2.y); }
	friend constexpr14 inline HXVector2<F> operator - (const HXVector2<F>& v1, const HXVector2<F>& v2){ return HXVector2<F>(v1.x-v2.x, v1.y-v2.y); }
	friend constexpr14 inline HXVector2<F> operator * (const HXVector2<F>& v1, const HXVector2<F>& v2){ return HXVector2<F>(v1.x*v2.x, v1.y*v2.y); }
	friend constexpr14 inline HXVector2<F> operator / (const HXVector2<F>& v1, const HXVector2<F>& v2){ return HXVector2<F>(v1.x/v2.x, v1.y/v2.y); }

	friend constexpr14 inline HXVector2<F> operator + (const HXVector2<F>& v, const F f){ return HXVector2<F>(v.x+f, v.y+f); }
	friend constexpr14 inline HXVector2<F> operator - (const HXVector2<F>& v, const F f){ return HXVector2<F>(v.x-f, v.y-f); }
	friend constexpr14 inline HXVector2<F> operator * (const HXVector2<F>& v, const F f){ return HXVector2<F>(v.x*f, v.y*f); }
	friend constexpr14 inline HXVector2<F> operator / (const HXVector2<F>& v, const F f){ return HXVector2<F>(v.x/f, v.y/f); }

	friend constexpr14 inline HXVector2<F> operator + (const F f, const HXVector2<F>& v){ return HXVector2<F>(f+v.x, f+v.y); }
	friend constexpr14 inline HXVector2<F> operator - (const F f, const HXVector2<F>& v){ return HXVector2<F>(f-v.x, f-v.y); }
	friend constexpr14 inline HXVector2<F> operator * (const F f, const HXVector2<F>& v){ return HXVector2<F>(f*v.x, f*v.y); }
	friend constexpr14 inline HXVector2<F> operator / (const F f, const HXVector2<F>& v){ return HXVector2<F>(f/v.x, f/v.y); }

	//booleans
	constexpr14 inline bool operator > (const HXVector2<F>& v){ return (x > v.x) && (y > v.y); }
	constexpr14 inline bool operator < (const HXVector2<F>& v){ return (x < v.x) && (y < v.y); }
	constexpr14 inline bool operator <= (const HXVector2<F>& v){ return (x <= v.x) && (y <= v.y); }
	constexpr14 inline bool operator >= (const HXVector2<F>& v){ return (x >= v.x) && (y >= v.y); }
	constexpr14 inline bool operator == (const HXVector2<F>& v){ return (x == v.x) && (y == v.y); }
	constexpr14 inline bool operator != (const HXVector2<F>& v){ return (x != v.x) || (y != v.y); }

	constexpr14 inline bool operator > (const F v){ return (x > v) && (y > v); }
	constexpr14 inline bool operator < (const F v){ return (x < v) && (y < v); }
	constexpr14 inline bool operator <= (const F v){ return (x <= v) && (y <= v); }
	constexpr14 inline bool operator >= (const F v){ return (x >= v) && (y >= v); }
	constexpr14 inline bool operator == (const F v){ return (x == v) && (y == v); }
	constexpr14 inline bool operator != (const F v){ return (x != v) || (y != v); }

	//voids
	constexpr14 inline void operator += (const HXVector2<F>& v){ x += v.x; y += v.y; }
	constexpr14 inline void operator -= (const HXVector2<F>& v){ x -= v.x; y -= v.y; }
	constexpr14 inline void operator *= (const HXVector2<F>& v){ x *= v.x; y *= v.y; }
	constexpr14 inline void operator /= (const HXVector2<F>& v){ x /= v.x; y /= v.y; }

	constexpr14 inline void operator += (const F v){ x += v; y += v; }
	constexpr14 inline void operator -= (const F v){ x -= v; y -= v; }
	constexpr14 inline void operator *= (const F v){ x *= v; y *= v; }
	constexpr14 inline void operator /= (const F v){ x /= v; y /= v; }


};











struct HXColor {
	float r=0, g=0, b=0, a=0;

	HXColor(){ r=0; g=0; b=0; a=0; }
	HXColor(float x){ r=x; g=x; b=x; a=x; }
	HXColor(float _r, float _g, float _b){ r=_r; g=_g; b=_b; a=1; }
	HXColor(float _r, float _g, float _b, float _a){ r=_r; g=_g; b=_b; a=_a; }


	//casts
	operator float*() const {
		return new float[4]{r, g, b, a};
	}


	//HXColor and floats
	friend HXColor operator + (const HXColor& c1, const HXColor& c2){ return HXColor(c1.r+c2.r, c1.g+c2.g, c1.b+c2.b, c1.a+c2.a); }
	friend HXColor operator - (const HXColor& c1, const HXColor& c2){ return HXColor(c1.r-c2.r, c1.g-c2.g, c1.b-c2.b, c1.a-c2.a); }
	friend HXColor operator * (const HXColor& c1, const HXColor& c2){ return HXColor(c1.r*c2.r, c1.g*c2.g, c1.b*c2.b, c1.a*c2.a); }
	friend HXColor operator / (const HXColor& c1, const HXColor& c2){ return HXColor(c1.r/c2.r, c1.g/c2.g, c1.b/c2.b, c1.a/c2.a); }

	friend HXColor operator + (const HXColor& c, const float f){ return HXColor(c.r+f, c.g+f, c.b+f, c.a+f); }
	friend HXColor operator - (const HXColor& c, const float f){ return HXColor(c.r-f, c.g-f, c.b-f, c.a-f); }
	friend HXColor operator * (const HXColor& c, const float f){ return HXColor(c.r*f, c.g*f, c.b*f, c.a*f); }
	friend HXColor operator / (const HXColor& c, const float f){ return HXColor(c.r/f, c.g/f, c.b/f, c.a/f); }

	friend HXColor operator + (const float f, const HXColor& c){ return HXColor(f+c.r, f+c.g, f+c.b, f+c.a); }
	friend HXColor operator - (const float f, const HXColor& c){ return HXColor(f-c.r, f-c.g, f-c.b, f-c.a); }
	friend HXColor operator * (const float f, const HXColor& c){ return HXColor(f*c.r, f*c.g, f*c.b, f*c.a); }
	friend HXColor operator / (const float f, const HXColor& c){ return HXColor(f/c.r, f/c.g, f/c.b, f/c.a); }

	//booleans
	bool operator <= (const HXColor& c){ return (r <= c.r) && (g <= c.g) && (b <= c.b) && (a <= c.a); }
	bool operator >= (const HXColor& c){ return (r >= c.r) && (g >= c.g) && (b >= c.b) && (a >= c.a); }
	bool operator == (const HXColor& c){ return (r == c.r) && (g == c.g) && (b == c.b) && (a == c.a); }
	bool operator != (const HXColor& c){ return (r != c.r) || (g != c.g) || (b != c.b) || (a != c.a); }
	bool operator < (const HXColor& c){ return (r < c.r) && (g < c.g) && (b < c.b) && (a < c.a); }
	bool operator > (const HXColor& c){ return (r > c.r) && (g > c.g) && (b > c.b) && (a > c.a); }

	bool operator <= (const float v){ return (r <= v) && (g <= v) && (b <= v) && (a <= v); }
	bool operator >= (const float v){ return (r >= v) && (g >= v) && (b >= v) && (a >= v); }
	bool operator == (const float v){ return (r == v) && (g == v) && (b == v) && (a == v); }
	bool operator != (const float v){ return (r != v) || (g != v) || (b != v) || (a != v); }
	bool operator < (const float v){ return (r < v) && (g < v) && (b < v) && (a < v); }
	bool operator > (const float v){ return (r > v) && (g > v) && (b > v) && (a > v); }

	//voids
	void operator += (const HXColor& c){ r += c.r; g += c.g; b += c.b; a += c.a; }
	void operator -= (const HXColor& c){ r -= c.r; g -= c.g; b -= c.b; a -= c.a; }
	void operator *= (const HXColor& c){ r *= c.r; g *= c.g; b *= c.b; a *= c.a; }
	void operator /= (const HXColor& c){ r /= c.r; g /= c.g; b /= c.b; a /= c.a; }

	void operator += (const float v){ r += v; g += v; b += v; a += v; }
	void operator -= (const float v){ r -= v; g -= v; b -= v; a -= v; }
	void operator *= (const float v){ r *= v; g *= v; b *= v; a *= v; }
	void operator /= (const float v){ r /= v; g /= v; b /= v; a /= v; }



	static inline HXColor To8Bits(HXColor c){ return c*255; }
	static inline HXColor From8Bits(uint8_t r, uint8_t g, uint8_t b){ return HXColor(r,g,b,1)/255; }
	static inline HXColor From8Bits(uint8_t r, uint8_t g, uint8_t b, uint8_t a){ return HXColor(r,g,b,a)/255; }

	static inline HXColor White(){ return HXColor(1); }
	static inline HXColor Black(){ return HXColor(0); }
	static inline HXColor Red(){ return HXColor(1,0,0); }
	static inline HXColor Blue(){ return HXColor(0,0,1); }
	static inline HXColor Green(){ return HXColor(0,1,0); }

};









#endif /* end of include guard: HEXO_MATH_VECTOR_H */




#ifndef HEXO_MATH_MATRIX
#define HEXO_MATH_MATRIX



template<typename T, unsigned int W, unsigned int H>
struct HXMatrix {
    T m[W*H];

    HXMatrix(){ for (size_t i=0; i<(W*H); m[i]=0, ++i); }
    HXMatrix(const T* _m){ for (size_t i=0; i<(W*H); m[i]=_m[i], ++i); }


    inline const T* getpointer() const { return this->m; }

    inline std::string tostring(){
        std::string out = "";
        for (size_t i=0; i<H; i++){
            for (size_t j=0; j<W; j++){
                if (j != 0) out += ", ";
                out += std::to_string(m[j + i*W]);
            }
            out += "\n";
        }
        return out;
    }

};




#endif /* end of include guard: HEXO_MATH_MATRIX */


	typedef HXVector4<float> Vector4;
	typedef HXVector4<HXSIZE_I> Vector4Int;
	typedef HXVector4<HXSIZE> Vector4Uint;

	typedef HXVector3<float> Vector3;
	typedef HXVector3<HXSIZE_I> Vector3Int;
	typedef HXVector3<HXSIZE> Vector3Uint;

	typedef HXVector2<float> Vector2;
	typedef HXVector2<HXSIZE_I> Vector2Int;
	typedef HXVector2<HXSIZE> Vector2Uint;


	typedef HXMatrix<float, 4, 4> Matrix4f;



	struct Bounds2 {
		Vector2 Position, Dimension;

		Bounds2(){ Position = Vector2(0); Dimension = Vector2(1); }
		Bounds2(Vector2 pos){ Position = pos; Dimension = Vector2(1); }
		Bounds2(Vector2 pos, Vector2 dim){ Position = pos; Dimension = dim; }
		Bounds2(const Bounds2& b){ Position = b.Position; Dimension = b.Dimension; }
		Bounds2(float _x, float _y){ Position = Vector2(_x,_y); Dimension = Vector2(1); }
		Bounds2(float _x, float _y, float _w, float _h){ Position = Vector2(_x,_y); Dimension = Vector2(_w,_h); }

	};








	static const float PI = 3.1415926536f;
	static const char* AlphaNumerics = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";

	static float DegToRad(float a){ return a * (PI/180.0f); }
	static float RadToDeg(float a){ return a * (180.0f/PI); }

	template<typename T>
	static T max(const T& n1, const T& n2){ return (n1 > n2) ? n1 : n2; }
	template<typename T>
	static T min(const T& n1, const T& n2){ return (n1 < n2) ? n1 : n2; }
	template<typename T>
	static T clamp(const T& value, const T& min, const T& max){ return Math::max(Math::min(value, max), min); }
	template<typename T>
	static T saturate(const T& value){ return clamp(value, T(0), T(1)); }

	static float remp(float value, float OldMin, float OldMax, float NewMin, float NewMax){
	    return (((value - OldMin) * (NewMax - NewMin)) / (OldMax - OldMin)) + NewMin;
	}


	//Random
	static float RandomSeed = 1.0f;
	static const uint32_t M1 = 1597334677U;     //1719413*929
	static const uint32_t M2 = 3812015801U;     //140473*2467*11
	static const uint32_t M3 = 3299493293U;     //467549*7057
	static const uint32_t M4 = 3738282773U;
	static const float F0 = 1.0f/float(0xffffffffU);

	static uint32_t IHash(uint32_t n){ return n*(n^(n>>15)); }

	static uint32_t ICoords1(float p){ return uint32_t(p)*M1; }
	static uint32_t ICoords2(Vector2 p){ return (uint32_t(p.x)*M1)^(uint32_t(p.y)*M2); }
	static uint32_t ICoords3(Vector3 p){ return (uint32_t(p.x)*M1)^(uint32_t(p.y)*M2)^(uint32_t(p.z)*M3); }
	// static uint32_t ICoords4(Vector4 p){ return (uint32_t)(p.x*M1)^(uint32_t)(p.y*M2)^(uint32_t)(p.z*M3)^(uint32_t)(p.w*M4); }

	static float Hash11(float n){ return IHash(ICoords1(n)) * F0; }

	static Vector2 Hash12(float n){ return Vector2(IHash(ICoords1(n)*0x1U)*F0, IHash(ICoords1(n)*0x3fffU)*F0); }
	static float Hash21(Vector2 n){ return IHash(ICoords2(n)) * F0; }

	static Vector3 Hash13(float n){ return Vector3(IHash(ICoords1(n)*0x1U)*F0, IHash(ICoords1(n)*0x1ffU)*F0, IHash(ICoords1(n)*0x3ffffU)*F0); }
	static float Hash31(Vector3 n){ return IHash(ICoords3(n)) * F0; }

	// static Vector4 Hash14(float n){ return Vector4(IHash(ICoords1(n)*0x1U)*F0, IHash(ICoords1(n)*0x1ffU)*F0, IHash(ICoords1(n)*0x3ffffU)*F0, IHash(ICoords1(n)*0x1fffffU)*F0); }
	// static float Hash41(Vector4 n){ return IHash(ICoords4(n)) * F0; }


	static float Random(){
		RandomSeed = Hash11(RandomSeed);
		return RandomSeed;
	}
	static Vector2 RandomVector2(){
		Vector2 r = Hash12(RandomSeed);
		RandomSeed = ICoords2(r) * F0;
		return r;
	}
	static Vector3 RandomVector3(){
		Vector3 r = Hash13(RandomSeed);
		RandomSeed = ICoords3(r) * F0;
		return r;
	}
	// static Vector4 RandomVector4(){
	// 	Vector4 r = Hash14(RandomSeed);
	// 	RandomSeed = ICoords4(r) * F0;
	// 	return r;
	// }

	static float Random(float min, float max){
		return min + (Random() * abs(min-max));
	}
	static Vector2 RandomVector2(float min, float max){
		return min + (RandomVector2() * abs(min-max));
	}
	static Vector3 RandomVector3(float min, float max){
		return min + (RandomVector3() * abs(min-max));
	}
	// static Vector4 RandomVector4(float min, float max){
	// 	return min + (RandomVector4() * abs(min-max));
	// }





}
}





#endif /* end of include guard: HEXO_MATH_H */



#ifndef HEXO_MEMORY_BUFFERS_H
#define HEXO_MEMORY_BUFFERS_H


namespace Hexo {
	namespace Memory {


		template<typename U = Utils::__DefaultAllocator<uint8_t>>
		struct IndexedDynamicBuffer {
			using value_type = uint8_t;
			using size_type = size_t;
			using alloc_type = U;

			struct Header {
				size_type id;
				size_type size;
			};


		private:
			value_type* m_data = 0;
			size_type m_realsize = 0;
			size_type m_size = 0;

			alloc_type alloc;


		public:
			IndexedDynamicBuffer() : m_size(0), m_realsize(0), m_data(nullptr) {
				m_size = 0;
				m_data = nullptr;
				m_realsize = 0;
			}
			~IndexedDynamicBuffer(){ reset(); }



			inline void transfer(value_type** out) noexcept { (*out) = m_data; m_data = nullptr; }
			constexpr inline void* data() const noexcept { return this->m_data; }
			constexpr inline size_type size() const noexcept { return this->m_size; }
			constexpr inline size_type stride() const noexcept { return sizeof(value_type); }
			constexpr inline size_type capacity() const noexcept { return this->m_realsize; }
			constexpr inline bool empty() const noexcept { return this->m_size == 0; }



			/// TODO: Make this work with Iterators and range based for loops




			void reset(){
				if (m_data){
					alloc.deallocate((uint8_t*)(m_data), m_realsize);
				}
				m_data = nullptr;
				m_realsize = 0;
				m_size = 0;
			}


			void clear(){
				hxmemset(m_data, 0, m_size);
				m_size = 0;
			}

			void clearFast() noexcept {
				m_size = 0;
			}



			bool __CheckForReallocate(size_type size){
				size += sizeof(Header);

				size_type newsize = m_size+size;
				if (newsize >= m_realsize){
					/// python 3 list: https://github.com/python/cpython/blob/2.6/Objects/listobject.c#L48
					m_realsize += (newsize >> 3) + (newsize < 9 ? 3 : 6) + newsize;
					///

		            value_type* ndata = (value_type*)alloc.allocate(m_realsize);
		            if (!ndata)return false;

		            if (m_data && ndata!=m_data){
		                hxmemcpy(ndata, m_data, m_size);
						alloc.deallocate((uint8_t*)(m_data), m_realsize);
		            }

		            m_data = ndata;
				}
				return true;
			}



			void* allocate(const size_type& id, const size_type& size) {
				if (!__CheckForReallocate(size))return nullptr;

				value_type* p = m_data + m_size;
				Header h = Header{id, size};
				hxmemcpy(p, &h, sizeof(Header));

				m_size += size + sizeof(Header);
				return p + sizeof(Header);
			}


		};












		template<typename U = Utils::__DefaultAllocator<uint8_t>, typename id_type = size_t>
		struct UnindexedIterableDynamicBuffer {
			using value_type = uint8_t;
			using size_type = size_t;
			using alloc_type = U;

			struct Header {
				id_type id;
				size_type size;
			};


		private:
			value_type* m_data = 0;
			size_type m_realsize = 0;
			size_type m_size = 0;

			alloc_type alloc;


		public:
			UnindexedIterableDynamicBuffer() : m_size(0), m_realsize(0), m_data(nullptr) {
				m_size = 0;
				m_data = nullptr;
				m_realsize = 0;
			}
			UnindexedIterableDynamicBuffer(const alloc_type& a) : m_size(0), m_realsize(0), m_data(nullptr), alloc(a) {
				m_size = 0;
				m_data = nullptr;
				m_realsize = 0;
			}
			~UnindexedIterableDynamicBuffer(){ reset(); }



			inline void transfer(value_type** out) noexcept { (*out) = m_data; m_data = nullptr; }
			constexpr inline void* data() const noexcept { return this->m_data; }
			constexpr inline size_type size() const noexcept { return this->m_size; }
			constexpr inline size_type stride() const noexcept { return sizeof(value_type); }
			constexpr inline size_type capacity() const noexcept { return this->m_realsize; }
			constexpr inline bool empty() const noexcept { return this->m_size == 0; }



			/// TODO: Make this work with Iterators and range based for loops




			void reset(){
				if (m_data){
					alloc.deallocate((uint8_t*)(m_data), m_realsize);
				}
				m_data = nullptr;
				m_realsize = 0;
				m_size = 0;
			}


			void clear(){
				hxmemset(m_data, 0, m_size);
				m_size = 0;
			}

			void clearFast() noexcept {
				m_size = 0;
			}



			template<typename NT, typename... Args>
			constexpr inline typename std::enable_if<std::is_constructible<NT, Args...>::value == true, void>::type
			__ConstructInPlace(void* p, Args&&... a) const {
				HX_PLACEMENT_NEW(p, NT(std::forward<NT>(a)...));
			}

			template<typename NT, typename... Args>
			constexpr inline typename std::enable_if<std::is_constructible<NT, Args...>::value == false, void>::type
			__ConstructInPlace(void* p, Args&&... a) const {
				if (std::is_default_constructible<NT>::value){
					HX_PLACEMENT_NEW(p, NT{a...});
				}else{
					/// at this point, the object can never be constructed with the params the user gave it
					/// so just run its trivial default constructor
					HX_PLACEMENT_NEW(p, NT{});
				}
			}



			bool __CheckForReallocate(size_type size){
				size += sizeof(Header);

				size_type newsize = m_size+size;
				if (newsize >= m_realsize){
					/// python 3 list: https://github.com/python/cpython/blob/2.6/Objects/listobject.c#L48
					m_realsize += (newsize >> 3) + (newsize < 9 ? 3 : 6) + newsize;
					///

		            value_type* ndata = (value_type*)alloc.allocate(m_realsize);
		            if (!ndata)return false;

		            if (m_data && ndata!=m_data){
		                hxmemcpy(ndata, m_data, m_size);
						alloc.deallocate((uint8_t*)(m_data), m_realsize);
		            }

		            m_data = ndata;
				}
				return true;
			}



			void* allocate(const id_type& id, const size_type& size) {
				if (!__CheckForReallocate(size))return nullptr;

				value_type* p = m_data + m_size;
				Header h = Header{id, size};
				hxmemcpy(p, &h, sizeof(Header));

				m_size += size + sizeof(Header);
				return p + sizeof(Header);
			}



			template<typename T, typename... Args>
			void* emplace(const id_type& id, Args&&... a){
				void* p = allocate(id, sizeof(T));
				if (p){ __ConstructInPlace<T>(p, a...); }
				return p;
			}



			/// func must be void(id_type id, size_t size, void* data, size_t loop_index, ...)
			template<typename F, typename... Args>
			void iterate(F&& func, Args&&... args) const {
				if (!m_data)return;
				if (!m_size)return;
				value_type* p = m_data;

				for (size_t i=0, tsize=0;; ++i){
					const Header& h = *((Header*)p);
					func(h.id, h.size, p+sizeof(Header), i, args...);

					tsize += sizeof(Header) + h.size;
					if (tsize >= m_size)break;
					p = m_data + tsize;
				}
			}



		};





	}
}



#endif /* end of include guard: HEXO_MEMORY_BUFFERS_H */










#ifdef HEXO_INPUT_ENABLE_ERRORS
	#define HX_INPUT_ERROR_PRINT(str) HXRC_PRINT(str, HX_INPUT_ERROR)
	#define HX_INPUT_ERROR_RETURN(str) HXRC_RETURN_FATAL(str, HX_INPUT_ERROR)
	#define HX_INPUT_WARNING_RETURN(str) HXRC_RETURN_WARNING(str, HX_INPUT_ERROR)
	#define HX_INPUT_ERROR_ASSERT(x, str) HXRC_ASSERT_FATAL(x, str, HX_INPUT_ERROR)
	#define HX_INPUT_WARNING_ASSERT(x, str) HXRC_ASSERT_WARNING(x, str, HX_INPUT_ERROR)

	#define HX_INPUT_ERROR_NO_RETURN(str) HXRC_NO_RETURN_FATAL(str, HX_INPUT_ERROR)
	#define HX_INPUT_WARNING_NO_RETURN(str) HXRC_NO_RETURN_WARNING(str, HX_INPUT_ERROR)
	#define HX_INPUT_ERROR_ASSERT_NO_RETURN(x, str) HXRC_ASSERT_FATAL_NO_RETURN(x, str, HX_INPUT_ERROR)
	#define HX_INPUT_WARNING_ASSERT_NO_RETURN(x, str) HXRC_ASSERT_WARNING_NO_RETURN(x, str, HX_INPUT_ERROR)

#else
	#define HX_INPUT_ERROR_PRINT(str)
	#define HX_INPUT_ERROR_RETURN(str) return HXRC_FATAL
	#define HX_INPUT_WARNING_RETURN(str) return HXRC_WARNING
	#define HX_INPUT_ERROR_ASSERT(x, str) if (!(x)){ return HXRC_FATAL; }
	#define HX_INPUT_WARNING_ASSERT(x, str) if (!(x)){ return HXRC_WARNING; }

	#define HX_INPUT_ERROR_NO_RETURN(str)
	#define HX_INPUT_WARNING_NO_RETURN(str)
	#define HX_INPUT_ERROR_ASSERT_NO_RETURN(x, str) (x);
	#define HX_INPUT_WARNING_ASSERT_NO_RETURN(x, str) (x);
#endif













namespace Hexo {


	class InputEngine;



	namespace Input {

        //////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        //////
        ////// Error app
        //////
        /////////////////////////////////////////////////////////////////////////////////////////////////////

        /// Register Error App for graphics engine
        struct ErrorApp {
            size_t hxrc_error_app = 0;
            static inline ErrorApp* Get(){ static ErrorApp e; return &e; }
        };

	#ifdef HEXO_INPUT_ENABLE_ERRORS
        #define HX_INPUT_ERROR Input::ErrorApp::Get()->hxrc_error_app
		template<typename F>
		static inline void SetErrorCallback(F&& func){
            HX_INPUT_ERROR = HXRC_REGISTER_APP(func);
        }
	#else
		#define HX_INPUT_ERROR 0
		template<typename F>
		static inline void SetErrorCallback(F&& func){}
	#endif

        /////////////////////////////////////////////////////////////////////////////////////////////////////





#ifndef HEXO_INPUT_IHI_IMPL_H
#define HEXO_INPUT_IHI_IMPL_H



typedef uint8_t IHI_InputMessageType;



typedef uint8_t InputDeviceType;
enum : InputDeviceType {
	HX_INPUT_DEVICE_NONE = 0,

	HX_INPUT_DEVICE_MOUSE = (1 << 1),
	HX_INPUT_DEVICE_KEYBOARD = (1 << 2),
	HX_INPUT_DEVICE_GAMEPAD = (1 << 3),
};


typedef uint8_t KeyState;
enum : KeyState {
	HX_INPUT_KEYSTATE_NONE = 0,

	HX_INPUT_KEYSTATE_PRESSED = (1 << 1),
	HX_INPUT_KEYSTATE_DOWN = (1 << 2),
	HX_INPUT_KEYSTATE_RELEASED = (1 << 3),

	HX_INPUT_KEYSTATE_TEMPPRESSED = (1 << 4), /// stateful
};


typedef uint8_t PointerInputAxis;
enum : PointerInputAxis {
	HX_INPUT_PAXIS_ABSOLUTE,
	HX_INPUT_PAXIS_RELATIVE,
	HX_INPUT_PAXIS_WINDOW_RELATIVE,
};


typedef uint8_t MouseKey;
enum : MouseKey {
	HX_INPUT_VK_LMBUTTON,
	HX_INPUT_VK_RMBUTTON,
	HX_INPUT_VK_MMBUTTON,
	HX_INPUT_VK_MOUSE_XBUTTON1,
	HX_INPUT_VK_MOUSE_XBUTTON2,

	/// Count
	HX_INPUT_MOUSE_KEYCOUNT
};


typedef uint8_t KeyboardKey;
enum : KeyboardKey {

	HX_INPUT_VK_BACK,
	HX_INPUT_VK_TAB,
	HX_INPUT_VK_CLEAR,
	HX_INPUT_VK_ENTERKEY,

	HX_INPUT_VK_LSHIFT,
	HX_INPUT_VK_RSHIFT,
	HX_INPUT_VK_SHIFT,
	HX_INPUT_VK_LCONTROL,
	HX_INPUT_VK_RCONTROL,
	HX_INPUT_VK_CONTROL,
	HX_INPUT_VK_LALT,
	HX_INPUT_VK_RALT,
	HX_INPUT_VK_ALT,
	HX_INPUT_VK_LSUPER,
	HX_INPUT_VK_RSUPER,
	HX_INPUT_VK_SUPER,

	HX_INPUT_VK_SPACE,
	HX_INPUT_VK_PAUSE,
	HX_INPUT_VK_CAPSLOCK,
	HX_INPUT_VK_NUMLOCK,
	HX_INPUT_VK_SCROLLLOCK,
	HX_INPUT_VK_ESCAPE,

	HX_INPUT_VK_PAGEUP,
	HX_INPUT_VK_PAGEDOWN,
	HX_INPUT_VK_END,
	HX_INPUT_VK_HOME,

	HX_INPUT_VK_LEFT,
	HX_INPUT_VK_UP,
	HX_INPUT_VK_RIGHT,
	HX_INPUT_VK_DOWN,
	HX_INPUT_VK_SELECT,
	HX_INPUT_VK_PRINT,
	HX_INPUT_VK_EXECUTE,
	HX_INPUT_VK_PRINTSCREEN,
	HX_INPUT_VK_INSERT,
	HX_INPUT_VK_DELETE,
	HX_INPUT_VK_HELPKEY,


	/// characters
	HX_INPUT_VK_CHAR_0,
	HX_INPUT_VK_CHAR_1,
	HX_INPUT_VK_CHAR_2,
	HX_INPUT_VK_CHAR_3,
	HX_INPUT_VK_CHAR_4,
	HX_INPUT_VK_CHAR_5,
	HX_INPUT_VK_CHAR_6,
	HX_INPUT_VK_CHAR_7,
	HX_INPUT_VK_CHAR_8,
	HX_INPUT_VK_CHAR_9,

	HX_INPUT_VK_CHAR_A,
	HX_INPUT_VK_CHAR_B,
	HX_INPUT_VK_CHAR_C,
	HX_INPUT_VK_CHAR_D,
	HX_INPUT_VK_CHAR_E,
	HX_INPUT_VK_CHAR_F,
	HX_INPUT_VK_CHAR_G,
	HX_INPUT_VK_CHAR_H,
	HX_INPUT_VK_CHAR_I,
	HX_INPUT_VK_CHAR_J,
	HX_INPUT_VK_CHAR_K,
	HX_INPUT_VK_CHAR_L,
	HX_INPUT_VK_CHAR_M,
	HX_INPUT_VK_CHAR_N,
	HX_INPUT_VK_CHAR_O,
	HX_INPUT_VK_CHAR_P,
	HX_INPUT_VK_CHAR_Q,
	HX_INPUT_VK_CHAR_R,
	HX_INPUT_VK_CHAR_S,
	HX_INPUT_VK_CHAR_T,
	HX_INPUT_VK_CHAR_U,
	HX_INPUT_VK_CHAR_V,
	HX_INPUT_VK_CHAR_W,
	HX_INPUT_VK_CHAR_X,
	HX_INPUT_VK_CHAR_Y,
	HX_INPUT_VK_CHAR_Z,
	///

	/// Numpad
	HX_INPUT_VK_NUMPAD0,
	HX_INPUT_VK_NUMPAD1,
	HX_INPUT_VK_NUMPAD2,
	HX_INPUT_VK_NUMPAD3,
	HX_INPUT_VK_NUMPAD4,
	HX_INPUT_VK_NUMPAD5,
	HX_INPUT_VK_NUMPAD6,
	HX_INPUT_VK_NUMPAD7,
	HX_INPUT_VK_NUMPAD8,
	HX_INPUT_VK_NUMPAD9,
	HX_INPUT_VK_MULTIPLY,
	HX_INPUT_VK_ADD,
	HX_INPUT_VK_EQUALS,
	HX_INPUT_VK_SEPARATOR,
	HX_INPUT_VK_SUBTRACT,
	HX_INPUT_VK_DECIMAL,
	HX_INPUT_VK_DIVIDE,
	///

	/// Misc
	HX_INPUT_VK_COMMA,
	HX_INPUT_VK_PERIOD,
	HX_INPUT_VK_SEMICOLON,
	HX_INPUT_VK_FORWARDSLASH,
	HX_INPUT_VK_TILDE, /// the "~" key
	HX_INPUT_VK_OPENSQUAREBRACKET,
	HX_INPUT_VK_BACKSLASH,
	HX_INPUT_VK_CLOSESQUAREBRACKET,
	HX_INPUT_VK_SINGLEQUOTE,
	///

	HX_INPUT_VK_F1,
	HX_INPUT_VK_F2,
	HX_INPUT_VK_F3,
	HX_INPUT_VK_F4,
	HX_INPUT_VK_F5,
	HX_INPUT_VK_F6,
	HX_INPUT_VK_F7,
	HX_INPUT_VK_F8,
	HX_INPUT_VK_F9,
	HX_INPUT_VK_F10,
	HX_INPUT_VK_F11,
	HX_INPUT_VK_F12,
	HX_INPUT_VK_F13,
	HX_INPUT_VK_F14,
	HX_INPUT_VK_F15,
	HX_INPUT_VK_F16,
	HX_INPUT_VK_F17,
	HX_INPUT_VK_F18,
	HX_INPUT_VK_F19,
	HX_INPUT_VK_F20,
	HX_INPUT_VK_F21,
	HX_INPUT_VK_F22,
	HX_INPUT_VK_F23,
	HX_INPUT_VK_F24,

	HX_INPUT_VK_VOLUME_MUTE,
	HX_INPUT_VK_VOLUME_DOWN,
	HX_INPUT_VK_VOLUME_UP,
	HX_INPUT_VK_MEDIA_NEXT,
	HX_INPUT_VK_MEDIA_PREV,
	HX_INPUT_VK_MEDIA_STOP,
	HX_INPUT_VK_MEDIA_PLAY,

	HX_INPUT_VK_EXTRA1,
	HX_INPUT_VK_EXTRA2,
	HX_INPUT_VK_EXTRA3,
	HX_INPUT_VK_EXTRA4,
	HX_INPUT_VK_EXTRA5,
	HX_INPUT_VK_EXTRA6,

	/// Count
	HX_INPUT_KEYBOARD_KEYCOUNT
};



typedef uint8_t GamepadKey;
enum : GamepadKey {
	HX_INPUT_VK_PAD_A,						/// A button
	HX_INPUT_VK_PAD_B,						/// B button
	HX_INPUT_VK_PAD_X,						/// X button
	HX_INPUT_VK_PAD_Y,						/// Y button
	HX_INPUT_VK_PAD_RSHOULDER,				/// Right shoulder button
	HX_INPUT_VK_PAD_LSHOULDER,				/// Left shoulder button
	HX_INPUT_VK_PAD_LTRIGGER,				/// Left trigger
	HX_INPUT_VK_PAD_RTRIGGER,				/// Right trigger
	HX_INPUT_VK_PAD_DPAD_UP,				/// Directional pad up
	HX_INPUT_VK_PAD_DPAD_DOWN,				/// Directional pad down
	HX_INPUT_VK_PAD_DPAD_LEFT,				/// Directional pad left
	HX_INPUT_VK_PAD_DPAD_RIGHT,				/// Directional pad right
	HX_INPUT_VK_PAD_START,					/// START button
	HX_INPUT_VK_PAD_BACK,					/// BACK button
	HX_INPUT_VK_PAD_LTHUMB_PRESS,			/// Left thumbstick click
	HX_INPUT_VK_PAD_RTHUMB_PRESS,			/// Right thumbstick click
	HX_INPUT_VK_PAD_LTHUMB_UP,				/// Left thumbstick up
	HX_INPUT_VK_PAD_LTHUMB_DOWN,			/// Left thumbstick down
	HX_INPUT_VK_PAD_LTHUMB_RIGHT,			/// Left thumbstick right
	HX_INPUT_VK_PAD_LTHUMB_LEFT,			/// Left thumbstick left
	HX_INPUT_VK_PAD_LTHUMB_UPLEFT,			/// Left thumbstick up and left
	HX_INPUT_VK_PAD_LTHUMB_UPRIGHT,			/// Left thumbstick up and right
	HX_INPUT_VK_PAD_LTHUMB_DOWNRIGHT,		/// Left thumbstick down and right
	HX_INPUT_VK_PAD_LTHUMB_DOWNLEFT,		/// Left thumbstick down and left
	HX_INPUT_VK_PAD_RTHUMB_UP,				/// Right thumbstick up
	HX_INPUT_VK_PAD_RTHUMB_DOWN,			/// Right thumbstick down
	HX_INPUT_VK_PAD_RTHUMB_RIGHT,			/// Right thumbstick right
	HX_INPUT_VK_PAD_RTHUMB_LEFT,			/// Right thumbstick left
	HX_INPUT_VK_PAD_RTHUMB_UPLEFT,			/// Right thumbstick up and left
	HX_INPUT_VK_PAD_RTHUMB_UPRIGHT,			/// Right thumbstick up and right
	HX_INPUT_VK_PAD_RTHUMB_DOWNRIGHT,		/// Right thumbstick down and right
	HX_INPUT_VK_PAD_RTHUMB_DOWNLEFT,		/// Right thumbstick down and left

	/// Count
	HX_INPUT_GAMEPAD_KEYCOUNT
};





struct IHI_KeyMap;
struct IHI_InputMessage;
struct IHI_Device;
struct IHI_Keyboard;
struct IHI_Mouse;
struct IHI_Gamepad;











#endif /* end of include guard: HEXO_INPUT_IHI_IMPL_H */

		#ifdef HEXO_INPUT_IHI_WIN32_RAWINPUT



#ifndef HEXO_INPUT_IHI_WIN32_RAWINPUT_IMPL_H
#define HEXO_INPUT_IHI_WIN32_RAWINPUT_IMPL_H





enum : IHI_InputMessageType {
	HX_INPUT_PARSING_NONE,

	HX_INPUT_PARSING_WINDOWS_RAWINPUT,
	HX_INPUT_PARSING_WINDOWS_RAWINPUT_MOUSE,
	HX_INPUT_PARSING_WINDOWS_RAWINPUT_KEYBOARD,
	HX_INPUT_PARSING_WINDOWS_RAWINPUT_MOUSE_ABSOLUTEPOS,

	HX_INPUT_PARSING_WINDOWS_XINPUT,
};



struct IHI_Device {
	void Release(){}

	HWND hwnd;
	bool injected;
	WNDPROC OldWindowProcedure;
};



struct IHI_Keyboard {
	IHI_Keyboard() : bound(false) {}
	void Release(){ bound = false; }

	size_t ID;
	bool bound = false;

	uint8_t KeyStates[HX_INPUT_KEYBOARD_KEYCOUNT];
};


struct IHI_Mouse {
	IHI_Mouse() : bound(false) {}
	void Release(){ bound = false; }

	size_t ID;
	bool bound = false;

	uint8_t KeyStates[HX_INPUT_MOUSE_KEYCOUNT];
	Math::Vector2 absolutePos;
	Math::Vector2 deltaPos;
	Math::Vector2 wheelDelta;
};


struct IHI_Gamepad {
	IHI_Gamepad() : bound(false) {}
	void Release(){ bound = false; }

	size_t ID;
	bool bound = false;

	uint8_t KeyStates[HX_INPUT_GAMEPAD_KEYCOUNT];
	Math::Vector2Uint leftaxis;
	Math::Vector2Uint rightaxis;
};



struct IHI_KeyMap {
	constexpr14 IHI_KeyMap() : keys() {

		/// Mouse
		keys[VK_LBUTTON] = 			HX_INPUT_VK_LMBUTTON;
		keys[VK_RBUTTON] = 			HX_INPUT_VK_RMBUTTON;
		keys[VK_MBUTTON] = 			HX_INPUT_VK_MMBUTTON;
		keys[VK_XBUTTON1] = 		HX_INPUT_VK_MOUSE_XBUTTON1;
		keys[VK_XBUTTON2] = 		HX_INPUT_VK_MOUSE_XBUTTON2;
		///

		keys[VK_BACK] = 			HX_INPUT_VK_BACK;
		keys[VK_TAB] = 				HX_INPUT_VK_TAB;
		keys[VK_CLEAR] = 			HX_INPUT_VK_CLEAR;
		keys[VK_RETURN] = 			HX_INPUT_VK_ENTERKEY;

		keys[VK_LSHIFT] = 			HX_INPUT_VK_LSHIFT;
		keys[VK_RSHIFT] = 			HX_INPUT_VK_RSHIFT;
		keys[VK_SHIFT] = 			HX_INPUT_VK_SHIFT;
		keys[VK_LCONTROL] = 		HX_INPUT_VK_LCONTROL;
		keys[VK_RCONTROL] = 		HX_INPUT_VK_RCONTROL;
		keys[VK_CONTROL] = 			HX_INPUT_VK_CONTROL;
		keys[VK_LMENU] = 			HX_INPUT_VK_LALT;
		keys[VK_LMENU] = 			HX_INPUT_VK_RALT;
		keys[VK_MENU] = 			HX_INPUT_VK_ALT;
		keys[VK_LWIN] = 			HX_INPUT_VK_LSUPER;
		keys[VK_RWIN] = 			HX_INPUT_VK_RSUPER;
		// keys[VK_WIN] = 				HX_INPUT_VK_SUPER;

		keys[VK_SPACE] = 			HX_INPUT_VK_SPACE;
		keys[VK_PAUSE] = 			HX_INPUT_VK_PAUSE;
		keys[VK_CAPITAL] = 			HX_INPUT_VK_CAPSLOCK;
		keys[VK_NUMLOCK] =			HX_INPUT_VK_NUMLOCK;
		keys[VK_SCROLL] =			HX_INPUT_VK_SCROLLLOCK;
		keys[VK_ESCAPE] = 			HX_INPUT_VK_ESCAPE;

		keys[VK_PRIOR] = 			HX_INPUT_VK_PAGEUP;
		keys[VK_NEXT] = 			HX_INPUT_VK_PAGEDOWN;
		keys[VK_END] = 				HX_INPUT_VK_END;
		keys[VK_HOME] = 			HX_INPUT_VK_HOME;

		keys[VK_LEFT] = 			HX_INPUT_VK_LEFT;
		keys[VK_UP] = 				HX_INPUT_VK_UP;
		keys[VK_RIGHT] = 			HX_INPUT_VK_RIGHT;
		keys[VK_DOWN] = 			HX_INPUT_VK_DOWN;
		keys[VK_SELECT] = 			HX_INPUT_VK_SELECT;
		keys[VK_PRINT] = 			HX_INPUT_VK_PRINT;
		keys[VK_EXECUTE] = 			HX_INPUT_VK_EXECUTE;
		keys[VK_SNAPSHOT] = 		HX_INPUT_VK_PRINTSCREEN;
		keys[VK_INSERT] = 			HX_INPUT_VK_INSERT;
		keys[VK_DELETE] = 			HX_INPUT_VK_DELETE;
		keys[VK_HELP] = 			HX_INPUT_VK_HELPKEY;


		/// characters
		keys[0x30] = 				HX_INPUT_VK_CHAR_0;
		keys[0x31] = 				HX_INPUT_VK_CHAR_1;
		keys[0x32] = 				HX_INPUT_VK_CHAR_2;
		keys[0x33] = 				HX_INPUT_VK_CHAR_3;
		keys[0x34] = 				HX_INPUT_VK_CHAR_4;
		keys[0x35] = 				HX_INPUT_VK_CHAR_5;
		keys[0x36] = 				HX_INPUT_VK_CHAR_6;
		keys[0x37] = 				HX_INPUT_VK_CHAR_7;
		keys[0x38] = 				HX_INPUT_VK_CHAR_8;
		keys[0x39] = 				HX_INPUT_VK_CHAR_9;

		keys[0x41] = 				HX_INPUT_VK_CHAR_A;
		keys[0x42] = 				HX_INPUT_VK_CHAR_B;
		keys[0x43] = 				HX_INPUT_VK_CHAR_C;
		keys[0x44] = 				HX_INPUT_VK_CHAR_D;
		keys[0x45] = 				HX_INPUT_VK_CHAR_E;
		keys[0x46] = 				HX_INPUT_VK_CHAR_F;
		keys[0x47] = 				HX_INPUT_VK_CHAR_G;
		keys[0x48] = 				HX_INPUT_VK_CHAR_H;
		keys[0x49] = 				HX_INPUT_VK_CHAR_I;
		keys[0x4A] = 				HX_INPUT_VK_CHAR_J;
		keys[0x4B] = 				HX_INPUT_VK_CHAR_K;
		keys[0x4C] = 				HX_INPUT_VK_CHAR_L;
		keys[0x4D] = 				HX_INPUT_VK_CHAR_M;
		keys[0x4E] = 				HX_INPUT_VK_CHAR_N;
		keys[0x4F] = 				HX_INPUT_VK_CHAR_O;
		keys[0x50] = 				HX_INPUT_VK_CHAR_P;
		keys[0x51] = 				HX_INPUT_VK_CHAR_Q;
		keys[0x52] = 				HX_INPUT_VK_CHAR_R;
		keys[0x53] = 				HX_INPUT_VK_CHAR_S;
		keys[0x54] = 				HX_INPUT_VK_CHAR_T;
		keys[0x55] = 				HX_INPUT_VK_CHAR_U;
		keys[0x56] = 				HX_INPUT_VK_CHAR_V;
		keys[0x57] = 				HX_INPUT_VK_CHAR_W;
		keys[0x58] = 				HX_INPUT_VK_CHAR_X;
		keys[0x59] = 				HX_INPUT_VK_CHAR_Y;
		keys[0x5A] = 				HX_INPUT_VK_CHAR_Z;
		///

		/// Numpad
		keys[VK_NUMPAD0] = 			HX_INPUT_VK_NUMPAD0;
		keys[VK_NUMPAD1] = 			HX_INPUT_VK_NUMPAD1;
		keys[VK_NUMPAD2] = 			HX_INPUT_VK_NUMPAD2;
		keys[VK_NUMPAD3] = 			HX_INPUT_VK_NUMPAD3;
		keys[VK_NUMPAD4] = 			HX_INPUT_VK_NUMPAD4;
		keys[VK_NUMPAD5] = 			HX_INPUT_VK_NUMPAD5;
		keys[VK_NUMPAD6] = 			HX_INPUT_VK_NUMPAD6;
		keys[VK_NUMPAD7] = 			HX_INPUT_VK_NUMPAD7;
		keys[VK_NUMPAD8] = 			HX_INPUT_VK_NUMPAD8;
		keys[VK_NUMPAD9] = 			HX_INPUT_VK_NUMPAD9;
		keys[VK_MULTIPLY] = 		HX_INPUT_VK_MULTIPLY;
		keys[VK_ADD] = 				HX_INPUT_VK_ADD;
		keys[VK_SEPARATOR] = 		HX_INPUT_VK_SEPARATOR;
		keys[VK_SUBTRACT] = 		HX_INPUT_VK_SUBTRACT;
		keys[VK_DECIMAL] = 			HX_INPUT_VK_DECIMAL;
		keys[VK_DIVIDE] = 			HX_INPUT_VK_DIVIDE;
		///

		/// Misc
		keys[VK_OEM_PLUS] = 		HX_INPUT_VK_EQUALS;	/// this is the '=' key on windows
		keys[VK_OEM_COMMA] = 		HX_INPUT_VK_COMMA;
		keys[VK_OEM_MINUS] = 		HX_INPUT_VK_SUBTRACT;
		keys[VK_OEM_PERIOD] = 		HX_INPUT_VK_PERIOD;
		keys[VK_OEM_1] = 			HX_INPUT_VK_SEMICOLON;
		keys[VK_OEM_2] = 			HX_INPUT_VK_FORWARDSLASH;
		keys[VK_OEM_3] = 			HX_INPUT_VK_TILDE; /// the "~" key
		keys[VK_OEM_4] = 			HX_INPUT_VK_OPENSQUAREBRACKET;
		keys[VK_OEM_5] = 			HX_INPUT_VK_BACKSLASH;
		keys[VK_OEM_6] = 			HX_INPUT_VK_CLOSESQUAREBRACKET;
		keys[VK_OEM_7] = 			HX_INPUT_VK_SINGLEQUOTE;
		keys[VK_OEM_CLEAR] = 		HX_INPUT_VK_CLEAR;
		///

		keys[VK_F1] = 				HX_INPUT_VK_F1;
		keys[VK_F2] = 				HX_INPUT_VK_F2;
		keys[VK_F3] = 				HX_INPUT_VK_F3;
		keys[VK_F4] = 				HX_INPUT_VK_F4;
		keys[VK_F5] = 				HX_INPUT_VK_F5;
		keys[VK_F6] = 				HX_INPUT_VK_F6;
		keys[VK_F7] = 				HX_INPUT_VK_F7;
		keys[VK_F8] = 				HX_INPUT_VK_F8;
		keys[VK_F9] = 				HX_INPUT_VK_F9;
		keys[VK_F10] = 				HX_INPUT_VK_F10;
		keys[VK_F11] = 				HX_INPUT_VK_F11;
		keys[VK_F12] = 				HX_INPUT_VK_F12;
		keys[VK_F13] = 				HX_INPUT_VK_F13;
		keys[VK_F14] = 				HX_INPUT_VK_F14;
		keys[VK_F15] = 				HX_INPUT_VK_F15;
		keys[VK_F16] = 				HX_INPUT_VK_F16;
		keys[VK_F17] = 				HX_INPUT_VK_F17;
		keys[VK_F18] = 				HX_INPUT_VK_F18;
		keys[VK_F19] = 				HX_INPUT_VK_F19;
		keys[VK_F20] = 				HX_INPUT_VK_F20;
		keys[VK_F21] = 				HX_INPUT_VK_F21;
		keys[VK_F22] = 				HX_INPUT_VK_F22;
		keys[VK_F23] = 				HX_INPUT_VK_F23;
		keys[VK_F24] = 				HX_INPUT_VK_F24;

		keys[VK_VOLUME_MUTE] = 		HX_INPUT_VK_VOLUME_MUTE;
		keys[VK_VOLUME_DOWN] = 		HX_INPUT_VK_VOLUME_DOWN;
		keys[VK_VOLUME_UP] = 		HX_INPUT_VK_VOLUME_UP;
		keys[VK_MEDIA_NEXT_TRACK] = HX_INPUT_VK_MEDIA_NEXT;
		keys[VK_MEDIA_PREV_TRACK] = HX_INPUT_VK_MEDIA_PREV;
		keys[VK_MEDIA_STOP] = 		HX_INPUT_VK_MEDIA_STOP;
		keys[VK_MEDIA_PLAY_PAUSE] = HX_INPUT_VK_MEDIA_PLAY;

	}

	uint8_t keys[256];
};






#endif /* end of include guard: HEXO_INPUT_IHI_WIN32_RAWINPUT_IMPL_H */

		#endif




#ifndef HEXO_INPUT_STANDARD_DEFINITIONS_H
#define HEXO_INPUT_STANDARD_DEFINITIONS_H









#endif /* end of include guard: HEXO_INPUT_STANDARD_DEFINITIONS_H */


	};






#ifndef HEXO_INPUT_ENGINE_H
#define HEXO_INPUT_ENGINE_H








using namespace Hexo::Input;



struct HXKeyboard { HXSIZE id; };
struct HXGamepad { HXSIZE id; };
struct HXMouse { HXSIZE id; };




class InputEngine {
public:
	InputEngine() :
	Device(),
#ifdef HEXO_INPUT_USE_IMMEDIATE_PARSING
	Messages(),
#endif
	Mice(), Gamepads(), Keyboards(), KeyMaps(),
	CursorPointerPosition(0), WindowBindings(), useFocus(false){}
	~InputEngine(){ Release(); }

	HX_MAKE_NON_COPYABLE_TYPE(InputEngine)


	void Release(){

		if (CleanUp() != HXRC_OK){
			HX_INPUT_ERROR_PRINT("CRITICAL WARNING: The InputEngine failed to cleanup, there could be a memory leak or some damage in external programs");
		}

		Device.Release();

	}


private:
	IHI_Device Device;
	IHI_KeyMap KeyMaps;
	Math::Vector2Uint CursorPointerPosition; /// this is the global mouse position
	Utils::TypedVector<size_t> WindowBindings;
	bool useFocus = false;

	/// TODO: replace with int hashmap
	Utils::TypedVector<IHI_Keyboard> Keyboards;
	Utils::TypedVector<IHI_Gamepad> Gamepads;
	Utils::TypedVector<IHI_Mouse> Mice;
	///


#ifndef HEXO_INPUT_USE_IMMEDIATE_PARSING
	Memory::UnindexedIterableDynamicBuffer<> Messages;
#endif


public:
	/// callbacks

	/// for the second param,
	/// 1st -> input engine pointer
	/// 2nd -> bool, false means disconnected, true means connected
	/// 3rd -> type of input device that got passed
	/// 4th -> pointer to the input device handle
	/// 5th -> bool, false means the program doesn't know about the handle, true means the program knows about it and it is valid
	std::function<HXRC(InputEngine*, bool, InputDeviceType, HXUINTP, bool)> OnDeviceConnectionChange;



	inline const IHI_Device& GetDevice() const noexcept { return Device; }
	inline decltype(Mice)* GetMice() noexcept { return &Mice; }
	inline decltype(Keyboards)* GetKeyboards() noexcept { return &Keyboards; }

#ifndef HEXO_INPUT_USE_IMMEDIATE_PARSING
	inline decltype(Messages)* GetMessages() noexcept { return &Messages; }
#endif

	inline void UseFocus(bool t) noexcept { useFocus = t; }





	template<typename T>
	constexpr inline HXSIZE __Get_ID(const T& t) const noexcept { return t.id-1; }
	template<typename T>
	constexpr inline T __Make_ID(const HXSIZE& i) const noexcept { return T{i+1}; }




	HXRC InitInThread(size_t handle) noexcept {
		HXRC_RETURN( CreateDevice(handle) );
		return HXRC_OK;
	}
	inline HXRC InitInThread() noexcept { return InitInThread(0); }


	HXRC CleanUp();
	HXRC Update();



	HXRC ParseInputMessage(size_t, void*, size_t);
	HXRC UpdatePlatformEvents();

	HXRC CreateDevice(HXUINTP);
	HXMouse CreateMouseDevice();
	HXKeyboard CreateKeyboardDevice();
	HXGamepad CreateGamepadDevice();

	HXRC GetInputMessages();
	HXRC GetPluggedDevices();
	Math::Vector2Uint GetCursorAbsolutePosition() noexcept;
	Math::Vector2 GetCursorNormalizedPosition() noexcept;

	bool IsFocused() const noexcept;
	bool IsCursorInWindow() const noexcept;
	void HideAndLockCursorToWindow(HXSIZE) const noexcept;
	void UnhideAndUnlockCursor() const noexcept;

	HXRC ResetInputMessages();
	void ResetMouseDevice(const HXMouse&) noexcept;
	void ResetKeyboardDevice(const HXKeyboard&) noexcept;
	void ResetGamepadDevice(const HXGamepad&) noexcept;








	HXSIZE AddWindowBinding(HXUINTP i) noexcept {
		return WindowBindings.push_back(i);
	}

	HXRC RemoveWindowBinding(HXUINTP i) noexcept {
		for (size_t i=0; i<WindowBindings.size(); ++i){
			if (WindowBindings[i] == i){
				WindowBindings.erase(i);
				return HXRC_OK;
			}
		}
		return HXRC_OK;
	}






	constexpr14 inline KeyState GetKeyState(const HXKeyboard& k, KeyboardKey key) const noexcept {
		if (!k.id || k.id > Keyboards.size())return false;
		return Keyboards[__Get_ID(k)].KeyStates[key];
	}

	constexpr14 inline bool GetKey(const HXKeyboard& k, KeyboardKey key, KeyState state) const noexcept {
		return GetKeyState(k, key) & state;
	}



	constexpr14 inline KeyState GetMouseKeyState(const HXMouse& m, MouseKey key) const noexcept {
		if (!m.id || m.id > Mice.size())return false;
		return Mice[__Get_ID(m)].KeyStates[key];
	}

	constexpr14 inline bool GetMouseKey(const HXMouse& m, MouseKey key, KeyState state) const noexcept {
		return GetMouseKeyState(m, key) & state;
	}

	constexpr14 inline Math::Vector2 GetMousePosition(const HXMouse& m, PointerInputAxis axis) const noexcept {
		if (!m.id || m.id > Mice.size())return Math::Vector2(0,0);
		if (axis == HX_INPUT_PAXIS_ABSOLUTE){ return Mice[__Get_ID(m)].absolutePos; }
		return Mice[__Get_ID(m)].deltaPos;
	}

	constexpr14 inline Math::Vector2 GetMouseWheelDelta(const HXMouse& m) const noexcept {
		if (!m.id || m.id > Mice.size())return false;
		return Mice[__Get_ID(m)].wheelDelta;
	}






	constexpr14 size_t __LookForBinder(InputDeviceType type, size_t id, size_t* empty) const noexcept {
		switch (type){
			case HX_INPUT_DEVICE_MOUSE:
				for (size_t i=0; i<Mice.size(); ++i){
					// std::cout << Mice[i].bound << " -- " << (HANDLE)Mice[i].ID << " -- " << (HANDLE)id << '\n';
					if (Mice[i].bound && Mice[i].ID == id)return (i+1);
					if (!Mice[i].bound && empty){ (*empty) = i+1; }
				}
			break;

			case HX_INPUT_DEVICE_KEYBOARD:
				for (size_t i=0; i<Keyboards.size(); ++i){
					if (Keyboards[i].bound && Keyboards[i].ID == id)return (i+1);
					if (!Keyboards[i].bound && empty){ (*empty) = i+1; }
				}
			break;

			case HX_INPUT_DEVICE_GAMEPAD:
				for (size_t i=0; i<Gamepads.size(); ++i){
					if (Gamepads[i].bound && Gamepads[i].ID == id)return (i+1);
					if (!Gamepads[i].bound && empty){ (*empty) = i+1; }
				}
			break;
		}

		return 0;
	}



	constexpr14 HXRC __BindInputDevice(InputDeviceType type, size_t id, size_t i) noexcept {
		if (!i)return HXRC_OK;

		switch (type) {
			case HX_INPUT_DEVICE_MOUSE:
				if (i > Mice.size())return HXRC_OK;
				Mice[i-1].bound = true;
				Mice[i-1].ID = id;
			break;

			case HX_INPUT_DEVICE_KEYBOARD:
				if (i > Keyboards.size())return HXRC_OK;
				Keyboards[i-1].bound = true;
				Keyboards[i-1].ID = id;
			break;

			case HX_INPUT_DEVICE_GAMEPAD:
				if (i > Gamepads.size())return HXRC_OK;
				Gamepads[i-1].bound = true;
				Gamepads[i-1].ID = id;
			break;
		}

		return HXRC_OK;
	}



	constexpr inline KeyboardKey __GetKeyMap(KeyboardKey key) const noexcept {
		return KeyMaps.keys[key];
	}


	constexpr void __ConfigureKeyState(KeyboardKey& key, bool state) const noexcept {
		if (state){	/// if the key was released
			key = 0;
			key |= HX_INPUT_KEYSTATE_RELEASED;

			/// remove states
			// key &= ~HX_INPUT_KEYSTATE_TEMPPRESSED;
		}else{

			if (key & HX_INPUT_KEYSTATE_TEMPPRESSED){
				key &= ~HX_INPUT_KEYSTATE_PRESSED;
			}else{
				key |= HX_INPUT_KEYSTATE_PRESSED;
				key |= HX_INPUT_KEYSTATE_TEMPPRESSED;
			}
			key |= HX_INPUT_KEYSTATE_DOWN;


			/// check for states
			// if (key & HX_INPUT_KEYSTATE_TEMPPRESSED){
			// 	key |= HX_INPUT_KEYSTATE_DOWN;
			// }else{
			// 	key |= HX_INPUT_KEYSTATE_DOWN;
			// 	key |= HX_INPUT_KEYSTATE_PRESSED;
			// 	key |= HX_INPUT_KEYSTATE_TEMPPRESSED;
			// }
		}
	}


};




#endif /* end of include guard: HEXO_INPUT_ENGINE_H */



};








#endif /* end of include guard: HEXO_INPUT_H */
