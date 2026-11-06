cmake_minimum_required(VERSION 3.25 FATAL_ERROR)
project(VeyronStudio VERSION 1.2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(VEYRON_CORE_SOURCES
    src/LevelData.cpp
    src/LevelParser.cpp
    src/LevelAnalyzer.cpp
    src/Diagnostics.cpp
    src/FrameAnalyzer.cpp
    src/FPSProfiler.cpp
    src/IDManager.cpp
    src/SnapshotManager.cpp
    src/MacroEngine.cpp
    src/ProjectManager.cpp
    src/StructureVisualizer.cpp
    src/StudioManager.cpp
)

option(VEYRON_BUILD_CORE_TESTS "Build Veyron's standalone core tests without Geode" OFF)
if (VEYRON_BUILD_CORE_TESTS)
    enable_testing()
    add_executable(veyron-core-tests
        tests/core_tests.cpp
        ${VEYRON_CORE_SOURCES}
    )
    target_include_directories(veyron-core-tests PRIVATE include)
    add_test(NAME veyron-core-tests COMMAND veyron-core-tests)
    return()
endif()

find_package(Geode REQUIRED)

add_geode_mod(veyron-studio
    src/main.cpp
    src/StudioUI.cpp
    src/ProjectStore.cpp
    ${VEYRON_CORE_SOURCES}
)

target_include_directories(veyron-studio PRIVATE include)
