cmake_minimum_required(VERSION 3.10)
project(extreme_ttt VERSION 0.1.0 LANGUAGES C CXX)

# Set the C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add submodules
add_subdirectory(external/spdlog)
include_directories(external/spdlog/include)
include_directories(.)

# Automatically add all .cpp files to the executable
# search recursively for all .cpp files in the current directory
# except in the build directory
file(GLOB_RECURSE CPP_FILES *.cpp)
list(FILTER CPP_FILES EXCLUDE REGEX "build/")
list(FILTER CPP_FILES EXCLUDE REGEX "external/")

message("CPP_FILES: ${CPP_FILES}")

add_executable(extreme_ttt ${CPP_FILES})
if(MSVC)
  target_compile_options(extreme_ttt PRIVATE /W4 /WX)
else()
  target_compile_options(extreme_ttt PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

# Add precompiled header
set(PCH_FILE pch.h)
target_precompile_headers(extreme_ttt PRIVATE ${PCH_FILE})