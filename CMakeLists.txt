cmake_minimum_required(VERSION 3.20)
project(LeviFrameGenSmooth VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Source files
file(GLOB_RECURSE SOURCES
    "src/*.cpp"
    "src/*.c"
)

file(GLOB_RECURSE HEADERS
    "include/*.h"
    "include/*.hpp"
)

# Include directories
include_directories(
    include
)

# Build dynamic library for LeviLaunchroid plugin
add_library(LeviFrameGenSmooth SHARED ${SOURCES} ${HEADERS})

target_compile_definitions(LeviFrameGenSmooth PRIVATE
    LEVI_MOD_EXPORTS
    BUILDING_LEVI_MOD
)

# Build standalone test executable for physics & shader reprojection verification
file(GLOB_RECURSE TEST_SOURCES
    "tests/*.cpp"
)

if (TEST_SOURCES)
    add_executable(mod_tests ${TEST_SOURCES} "src/camera/CameraSmoothing.cpp" "src/framegen/FrameGenEngine.cpp" "src/math/MatrixMath.cpp")
    target_include_directories(mod_tests PRIVATE include)
endif()
