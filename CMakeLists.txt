
cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

project(VeyronStudio VERSION 1.2.0 LANGUAGES CXX)

# ============================================================
# C++ CONFIGURATION
# ============================================================

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ============================================================
# VEYRON CORE SOURCES
# ============================================================

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

# ============================================================
# BUILD OPTIONS
# ============================================================

option(
    VEYRON_BUILD_CORE_TESTS
    "Build Veyron standalone core tests without Geode"
    OFF
)

# ============================================================
# OPTIONAL STANDALONE TESTS
# ============================================================

if(VEYRON_BUILD_CORE_TESTS AND NOT ANDROID)

    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/core_tests.cpp")

        enable_testing()

        add_executable(veyron-core-tests
            tests/core_tests.cpp
            ${VEYRON_CORE_SOURCES}
        )

        target_include_directories(veyron-core-tests
            PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/include
        )

        add_test(
            NAME veyron-core-tests
            COMMAND veyron-core-tests
        )

        message(STATUS "Veyron standalone tests enabled.")

    else()

        message(WARNING
            "VEYRON_BUILD_CORE_TESTS requested, but "
            "tests/core_tests.cpp was not found. Tests disabled."
        )

    endif()

    return()

endif()

# ============================================================
# GEODE SDK
# ============================================================

find_package(Geode REQUIRED)

# ============================================================
# VEYRON STUDIO GEODE MOD
# ============================================================

add_geode_mod(veyron-studio

    src/main.cpp
    src/StudioUI.cpp
    src/ProjectStore.cpp

    ${VEYRON_CORE_SOURCES}

)

# ============================================================
# INCLUDE DIRECTORIES
# ============================================================

target_include_directories(veyron-studio
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# ============================================================
# ANDROID CONFIGURATION
# ============================================================

if(ANDROID)

    message(STATUS "====================================")
    message(STATUS "Building Veyron Studio for Android")
    message(STATUS "Android ABI: ${ANDROID_ABI}")
    message(STATUS "Android API: ${ANDROID_PLATFORM}")
    message(STATUS "Core tests: DISABLED")
    message(STATUS "====================================")

endif()
