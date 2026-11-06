# Veyron Studio 1.2.0 — Android64 Edition

Veyron Studio is a Geometry Dash 2.2091 creator toolkit packaged as a Geode mod for **Android ARM64 only**. Its GitHub Actions workflow produces one `Android64` `.geode` artifact. It does not build Windows, macOS, iOS or Android32 packages.

## Build target

- Geometry Dash: Android 2.2091
- Geode SDK: `6.0.0-alpha.1`
- Geode target: `Android64` (`arm64-v8a`)
- Android minimum SDK: 23
- C++ standard: C++23

The CI workflow normalizes a leading `v` in the Geode version before CMake configuration because the SDK comparison in the prior Windows build was strict about the prefix. The committed `mod.json` already contains the normalized version.

## Build on GitHub

1. Create an empty repository for the Android64 project.
2. Upload the **contents** of this folder to the repository root. Do not add an extra enclosing folder.
3. Confirm `.github/workflows/android64.yml` exists in the repository.
4. Open **Actions** and run **Veyron Studio - Android64 Build**, or push a commit to `main`.
5. Download the artifact named `Veyron-Studio-Android64-GD-2.2091`.
6. The artifact contains the compiled `.geode` package.

The build action sets `target: Android64` explicitly. It will not create Android32 or Windows packages.

## Modules included

- Mobile main-menu entry and responsive Veyron dashboard.
- Level data model, portable fixture parser and validation diagnostics.
- Frame-gap analysis with min/max/average/median and frame-window buckets.
- Group ID and Color ID usage counters plus unused-ID search.
- In-memory snapshots and record-level diff calculation.
- Horizontal structure summary.
- Dashboard-session FPS/frame-time sampling.
- Macro event model, recorder API, validation, text import/export and frame analysis.
- Persistent project name and notes using Geode saved values.
- Standalone regression tests for the platform-independent core.

See [`docs/FEATURES.md`](docs/FEATURES.md) for exact feature boundaries.

## Important limitations

This is a substantial, working **core-toolkit build**, not a claim that every editor operation is integrated. The current release does not automatically inspect or edit the live Geometry Dash editor, does not inject macro playback, and does not convert native Geometry Dash compressed level strings or third-party macro formats. Those require a separately implemented and device-tested live bridge. The UI uses clearly labelled sample datasets so it does not pretend sample statistics are scans of your level.

## Test the core locally

A regular CMake + C++23 compiler can run the standalone core tests without Geode:

```bash
cmake -S . -B build-core -DVEYRON_BUILD_CORE_TESTS=ON
cmake --build build-core --config Release
ctest --test-dir build-core --output-on-failure
```

## Project tree

```text
.github/workflows/android64.yml
include/StudioUI.hpp
include/ProjectStore.hpp
include/VeyronStudio.hpp
src/main.cpp
src/StudioUI.cpp
src/ProjectStore.cpp
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
tests/core_tests.cpp
examples/sample-level.veylevel
examples/sample-macro.veymacro
```
