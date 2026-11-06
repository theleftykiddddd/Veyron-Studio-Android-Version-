# Veyron Studio feature map

## Implemented core modules

- **Level data model and parser:** validates the documented `VEYRON_LEVEL_V1` interchange format and converts in-memory level records back to that format.
- **Diagnostics:** validates object IDs, Group IDs, Color IDs and coordinate values; reports info, warning and error counts.
- **Frame Lab:** sorts and deduplicates frame indices, then calculates min/max/average/median gaps and 1-, 2- and 3+-frame buckets.
- **ID Map:** counts Group ID and Color ID use and identifies unoccupied IDs in a requested range.
- **Snapshots:** keeps a bounded set of in-memory level snapshots and compares an input dataset with the latest snapshot.
- **Structure Visualizer:** groups records by horizontal coordinate ranges and produces a text map.
- **Runtime profiler:** records frame deltas, average/lowest sampled FPS, P95 frame time and slow-frame count. The dashboard currently samples only while it is open.
- **Macro data engine:** records event data supplied by a caller, validates order/player/framerate, parses and serializes `VEYRON_MACRO_V1`, and reuses Frame Lab statistics.
- **Project storage:** saves project name and notes with Geode's per-mod saved values.

## Not connected to live Geometry Dash editor/game state yet

The current package does **not** automatically read the user's open editor level, mutate objects, assign IDs, save/restore editor changes, capture real player input, or inject macro playback. The Android64 UI explicitly identifies these boundaries. The data engines are usable with supplied/imported datasets; a live editor/input bridge requires separate hooks against the exact 2.2091 Android bindings and device testing.

The two example files are Veyron interchange samples, not native Geometry Dash level or macro files. They must not be renamed to `.gmd` or treated as compatible with third-party macro formats.
