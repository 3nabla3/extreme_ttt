cmake_minimum_required(VERSION 3.10)
project(extreme_ttt VERSION 0.1.0 LANGUAGES C CXX)

# Set the C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# This sets the policy to NEW, which means the timestamps
# of the extracted files will be set to time of extraction
cmake_policy(SET CMP0135 NEW)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/d144031940543e15423a25ae5a8a74141044862f.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Add submodules
add_subdirectory(external/spdlog)
add_subdirectory(external/glfw)

# Automatically add all .cpp files to the executable
file(GLOB_RECURSE CPP_FILES src/*.cpp)

add_executable(extreme_ttt ${CPP_FILES})
target_include_directories(extreme_ttt PRIVATE external/spdlog/include)
target_include_directories(extreme_ttt PRIVATE external/glfw/include)
target_include_directories(extreme_ttt PRIVATE external/INIReader/include)
target_include_directories(extreme_ttt PRIVATE src)

# set the debug level depending on the build type
target_compile_definitions(extreme_ttt PRIVATE
    $<$<CONFIG:Debug>:SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE>
    $<$<CONFIG:Release>:SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_INFO>
    $<$<CONFIG:RelWithDebInfo>:SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE>
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

enable_testing()
file(GLOB_RECURSE TEST_FILES tests/*.cpp)
list(FILTER CPP_FILES EXCLUDE REGEX ".*Main\\.cpp$")
add_executable(unit_tests ${TEST_FILES} ${CPP_FILES})
target_link_libraries(unit_tests GTest::gtest_main)
target_link_libraries(unit_tests spdlog::spdlog)
target_include_directories(unit_tests PRIVATE src)
target_include_directories(unit_tests PRIVATE external/INIReader/include)

if(MSVC)
  target_link_libraries(unit_tests glfw opengl32)
elseif(UNIX)
  target_link_libraries(unit_tests glfw GL)
endif()

include(GoogleTest)
gtest_discover_tests(unit_tests)
