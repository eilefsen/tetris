cmake_minimum_required(VERSION 3.30..3.31)
project( tetris
	VERSION 1.0
	DESCRIPTION "It's Tetris!"
	LANGUAGES CXX
)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(default_build_type "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
	set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
						STRING "Choose the type of build." FORCE)
	# Set the possible values of build type for cmake-gui
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
				"Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()


# Dependencies
set(RAYLIB_VERSION 5.5)
include(FetchContent)
FetchContent_Declare(
	raylib
	DOWNLOAD_EXTRACT_TIMESTAMP OFF
	URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
)
FetchContent_MakeAvailable(raylib)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples

add_subdirectory(src)

