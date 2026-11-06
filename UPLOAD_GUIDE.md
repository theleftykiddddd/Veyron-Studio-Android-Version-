# Upload and install Veyron Studio Android64

## Upload to a new GitHub repository

1. Create a new empty repository, for example `Veyron-Studio-Android64`.
2. Extract `Veyron-Studio-Android64.zip` on your computer.
3. Open the extracted `Veyron-Studio-Android64` folder.
4. Upload everything **inside that folder** to the root of the new repository. Do not upload an extra parent folder.
5. Check that `mod.json`, `CMakeLists.txt`, `src/` and `.github/workflows/android64.yml` appear at the repository root/path.
6. Open **Actions**. Run **Veyron Studio - Android64 Build**, or push a commit to `main`.
7. When the job is green, open the run and download `Veyron-Studio-Android64-GD-2.2091` from Artifacts.

## Install the generated package

1. Extract the downloaded artifact ZIP.
2. Locate the file ending in `.geode`.
3. With Geode Android installed for the matching Geometry Dash version, copy the `.geode` file into:

   `/storage/emulated/0/Android/media/com.geode.launcher/game/geode/mods/`

4. Open Geometry Dash through Geode and check the mod list. If Geode reports an incompatibility, verify the installed Geode and Geometry Dash versions before trying again.

This repository builds **Android64 / ARM64 only**. It is not the correct artifact for a 32-bit Android installation.
