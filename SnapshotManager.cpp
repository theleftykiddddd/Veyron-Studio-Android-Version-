#include "VeyronStudio.hpp"

#include <algorithm>
#include <sstream>
#include <unordered_map>

namespace Veyron {
namespace {

std::string objectKey(const LevelObject& object) {
    std::ostringstream key;
    key.precision(12);
    key << object.objectID << ':' << object.groupID << ':' << object.colorID << ':'
        << object.x << ':' << object.y << ':' << object.isTrigger;
    return key.str();
}

} // namespace

void SnapshotManager::takeSnapshot(const LevelData& level) {
    m_snapshots.push_back(level);
    if (m_snapshots.size() > MAX_SNAPSHOTS) m_snapshots.erase(m_snapshots.begin());
}

SnapshotDiff SnapshotManager::compareWithLatest(const LevelData& level) const {
    SnapshotDiff diff;
    if (m_snapshots.empty()) return diff;
    std::unordered_map<std::string, std::size_t> previous;
    std::unordered_map<std::string, std::size_t> current;
    for (const auto& object : m_snapshots.back().objects) ++previous[objectKey(object)];
    for (const auto& object : level.objects) ++current[objectKey(object)];
    for (const auto& [key, count] : previous) {
        const auto it = current.find(key);
        const std::size_t now = it == current.end() ? 0 : it->second;
        if (count > now) diff.removed += count - now;
    }
    for (const auto& [key, count] : current) {
        const auto it = previous.find(key);
        const std::size_t before = it == previous.end() ? 0 : it->second;
        if (count > before) diff.added += count - before;
    }
    const auto previousSize = m_snapshots.back().objects.size();
    diff.unchanged = previousSize > diff.removed ? previousSize - diff.removed : 0;
    return diff;
}

bool SnapshotManager::hasSnapshot() const { return !m_snapshots.empty(); }
std::size_t SnapshotManager::snapshotCount() const { return m_snapshots.size(); }

const LevelData* SnapshotManager::getSnapshot(std::size_t index) const {
    return index < m_snapshots.size() ? &m_snapshots[index] : nullptr;
}

} // namespace Veyron
