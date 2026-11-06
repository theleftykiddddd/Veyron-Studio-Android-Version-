#include "VeyronStudio.hpp"

namespace Veyron {

StudioManager& StudioManager::get() {
    static StudioManager instance;
    return instance;
}

LevelData StudioManager::makeDemoLevel() const {
    LevelData level;
    level.name = "Veyron Studio Diagnostic Sample";
    level.objects = {
        {1, 1, 1, 0.0, 0.0, false},
        {901, 4, 2, 120.0, 0.0, true},
        {1815, 4, 2, 240.0, 48.0, false},
        {2067, 4, 3, 360.0, 48.0, true},
        {211, 5, 4, 500.0, 48.0, false},
        {1, 6, 4, 720.0, 0.0, false},
        {1764, 1004, 1008, 150000.0, 0.0, true},
        {1755, 7, 5, 1700.0, 96.0, false},
        {899, 7, 5, 1820.0, 96.0, false}
    };
    return level;
}

MacroClip StudioManager::makeDemoMacro() const {
    MacroClip clip;
    clip.name = "Veyron Frame Lab Sample";
    clip.framerate = 240;
    clip.events = {
        {10, 1, true}, {11, 1, false}, {19, 1, true}, {32, 1, false},
        {34, 1, true}, {35, 1, false}, {58, 1, true}, {83, 1, false},
        {84, 1, true}, {85, 1, false}
    };
    return clip;
}

std::string StudioManager::getVersionLabel() const { return "Veyron Studio v1.2.0 - Android64"; }

} // namespace Veyron
