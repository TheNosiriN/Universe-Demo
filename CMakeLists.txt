cmake_minimum_required(VERSION 3.6)

project(TheUniverse)


SET (CMAKE_CXX_STANDARD 11)


if (MSVC)
    add_compile_options(/permissive)
	SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MD" )
else()
    add_compile_options(-Wall)
endif()


SET(CMAKE_BUILD_TYPE RelWithDebInfo)
add_compile_definitions(DEBUG)


include_directories(
	${CMAKE_CURRENT_LIST_DIR}
	${CMAKE_CURRENT_LIST_DIR}/inc
	${CMAKE_CURRENT_LIST_DIR}/ext
)


add_executable(app
	src/main.cpp
	src/Application.cpp
	src/Camera.cpp
	src/renderers.cpp
	src/Universe.cpp
)


# link_directories(
# 	${CMAKE_CURRENT_LIST_DIR}\\..Hexo Game Engine\\build\\lib
# 	# "C:\\Users\\TheNosiriN\\Documents\\CppProjects\\Hexo Game Engine\\build\\lib"
# )

target_link_libraries(app
	lib/HexoGraphics
	lib/HexoInput
)



add_custom_target(Shaders ALL
	COMMAND hxshc -i shd_compile.args -o ../assets/shaders -p win -v -c ./ ./generic
	WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/shd
	COMMENT "Building shaders..."
)
