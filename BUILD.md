# Android64 build notes

The only CI target in this repository is `Android64`. The workflow uses the official `geode-sdk/build-geode-mod` action with `target: Android64`, Android min SDK 23, and the SDK version declared in `mod.json`.

If a build fails, open the failed `Build Veyron Studio for Android64` step and copy the first compiler or CMake error block. Do not infer a cause from the workflow graph status alone.

A successful artifact upload proves that the package was produced. It does not prove runtime compatibility on every Android device or that the unfinished editor/input bridge exists.
