cmake_minimum_required(VERSION 3.10)
project(extreme_ttt VERSION 0.1.0 LANGUAGES C CXX)

# Set the C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add submodules
add_subdirectory(external/spdlog)
add_subdirectory(external/glfw)

# Automatically add all .cpp files to the executable
file(GLOB_RECURSE CPP_FILES src/*.cpp)
message("CPP_FILES: ${CPP_FILES}")

add_executable(extreme_ttt ${CPP_FILES})
target_include_directories(extreme_ttt PRIVATE external/spdlog/include)
target_include_directories(extreme_ttt PRIVATE external/glfw/include)
target_include_directories(extreme_ttt PRIVATE src)

# set the debug level depending on the build type
target_compile_definitions(extreme_ttt PRIVATE
    $<$<CONFIG:Debug>:SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE>
    $<$<CONFIG:Release>:SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_INFO>
)
if(MSVC)
  target_compile_options(extreme_ttt PRIVATE /W4 /WX)
  target_link_libraries(extreme_ttt glfw opengl32)
elseif(UNIX)
  target_compile_options(extreme_ttt PRIVATE -Wall -Wextra -Wpedantic -Werror)
  target_link_libraries(extreme_ttt glfw GL)
endif()

# Add precompiled header
set(PCH_FILE src/pch.h)
target_precompile_headers(extreme_ttt PRIVATE ${PCH_FILE})

# Include CPack module
set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_NAME "MyProject")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_CONTACT "your.email@example.com")

install(TARGETS extreme_ttt RUNTIME DESTINATION bin)
include(CPack)