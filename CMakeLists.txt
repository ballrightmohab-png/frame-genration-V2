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

# Exclude test_mod.cpp and main.cpp from shared library target
list(FILTER SOURCES EXCLUDE REGEX "tests/.*")
list(FILTER SOURCES EXCLUDE REGEX "src/main.cpp")

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

# Custom target to package the mod into a .levipack zip archive
add_custom_target(levipack
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/manifest.json ${CMAKE_BINARY_DIR}/manifest.json
    COMMAND ${CMAKE_COMMAND} -E tar "cfv" "${CMAKE_BINARY_DIR}/LeviFrameGenSmooth.levipack" --format=zip
            "${CMAKE_BINARY_DIR}/libLeviFrameGenSmooth.so"
            "${CMAKE_BINARY_DIR}/manifest.json"
    DEPENDS LeviFrameGenSmooth
    COMMENT "Packaging LeviLaunchroid mod into LeviFrameGenSmooth.levipack..."
)

# Build main mod executable runner
add_executable(levi_mod_runner
    "src/main.cpp"
    "src/LeviMod.cpp"
    "src/camera/CameraSmoothing.cpp"
    "src/framegen/FrameGenEngine.cpp"
    "src/math/MatrixMath.cpp"
    "src/gui/ModMenu.cpp"
)
target_include_directories(levi_mod_runner PRIVATE include)

# Build standalone test executable for physics & shader reprojection verification
file(GLOB_RECURSE TEST_SOURCES
    "tests/*.cpp"
)

if (TEST_SOURCES)
    add_executable(mod_tests ${TEST_SOURCES} "src/camera/CameraSmoothing.cpp" "src/framegen/FrameGenEngine.cpp" "src/math/MatrixMath.cpp" "src/LeviMod.cpp" "src/gui/ModMenu.cpp")
    target_include_directories(mod_tests PRIVATE include)
endif()
