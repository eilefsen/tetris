add_executable(${PROJECT_NAME} main.cpp tet.cpp "block.cpp" collision.cpp)
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin/)
target_compile_options(
	${PROJECT_NAME} PUBLIC
	-stdlib=libc++
	PRIVATE
	-O2 -Wall -Wextra -Wconversion
	-Wpedantic -Wcast-align -Wdouble-promotion
	-Wimplicit-fallthrough -Wmisleading-indentation
	-Wnon-virtual-dtor -Wnull-dereference
	-Wold-style-cast -Woverloaded-virtual -Wshadow -Wuninitialized
)

target_link_libraries(${PROJECT_NAME} raylib)

if (APPLE)
	target_link_libraries(${PROJECT_NAME} "-framework IOKit")
	target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
	target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()
