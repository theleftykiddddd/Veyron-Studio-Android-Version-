#include "VeyronStudio.hpp"

namespace Veyron {

std::size_t LevelDataStore::countTriggers(const LevelData& level) {
    std::size_t count = 0;
    for (const auto& object : level.objects) {
        if (object.isTrigger) ++count;
    }
    return count;
}

} // namespace Veyron
