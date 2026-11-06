#include "VeyronStudio.hpp"

namespace Veyron {
namespace {

std::vector<int> findUnusedIDs(const std::map<int, std::size_t>& usage, int firstID, int lastID) {
    std::vector<int> result;
    if (firstID > lastID) return result;
    for (int id = firstID; id <= lastID; ++id) {
        if (usage.find(id) == usage.end()) result.push_back(id);
        if (id == 2147483647) break;
    }
    return result;
}

} // namespace

std::map<int, std::size_t> ColorManager::countUsage(const LevelData& level) {
    std::map<int, std::size_t> usage;
    for (const auto& object : level.objects) if (object.colorID > 0) ++usage[object.colorID];
    return usage;
}

std::vector<int> ColorManager::findUnused(const LevelData& level, int firstID, int lastID) {
    return findUnusedIDs(countUsage(level), firstID, lastID);
}

std::map<int, std::size_t> GroupManager::countUsage(const LevelData& level) {
    std::map<int, std::size_t> usage;
    for (const auto& object : level.objects) if (object.groupID > 0) ++usage[object.groupID];
    return usage;
}

std::vector<int> GroupManager::findUnused(const LevelData& level, int firstID, int lastID) {
    return findUnusedIDs(countUsage(level), firstID, lastID);
}

} // namespace Veyron
