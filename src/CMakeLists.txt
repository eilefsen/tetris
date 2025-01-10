cmake_minimum_required(VERSION 3.30)
project(tetris VERSION 1.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../deps/raylib/ ${CMAKE_CURRENT_SOURCE_DIR}/../lib/raylib/)

add_executable(${PROJECT_NAME} main.cpp tet.cpp "block.cpp" collision.cpp)
target_compile_options(
		${PROJECT_NAME} PUBLIC
		-stdlib=libc++
		-O2 -Wall -Wextra -Wconversion -Wdeprecated
		-Wpedantic -Wcast-align -Wdouble-promotion
		-Wimplicit-fallthrough -Wmisleading-indentation
		-Wnon-virtual-dtor -Wnull-dereference
		-Wold-style-cast -Woverloaded-virtual -Wshadow -Wuninitialized)

target_include_directories(
	${PROJECT_NAME}
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include/
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../lib/raylib/raylib/include/
)
target_link_libraries(tetris raylib)

if (APPLE)
	target_link_libraries(${PROJECT_NAME} "-framework IOKit")
	target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
	target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()
