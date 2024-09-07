cmake_minimum_required(VERSION 3.5.0)
project(extreme_ttt VERSION 0.1.0 LANGUAGES C CXX)

# Set the C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add GLFW submodule
add_subdirectory(external/glfw)

include_directories(external/glfw/include)

# Automatically add all .cpp files to the executable
# search recursively for all .cpp files in the current directory
# except in the build directory
file(GLOB_RECURSE CPP_FILES *.cpp)
list(FILTER CPP_FILES EXCLUDE REGEX "build/")

message("CPP_FILES: ${CPP_FILES}")

add_executable(extreme_ttt ${CPP_FILES})

# Specify the path to the glfw3.a library
set(GLFW_LIB_PATH ${CMAKE_BINARY_DIR}/external/glfw/src/libglfw3.a)

# Link with glfw3.a and GL
target_link_libraries(extreme_ttt ${GLFW_LIB_PATH})
target_link_libraries(extreme_ttt GL)
