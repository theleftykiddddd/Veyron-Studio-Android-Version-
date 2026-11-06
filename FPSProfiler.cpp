#include "VeyronStudio.hpp"

#include <algorithm>
#include <cmath>
#include <vector>

namespace Veyron {

void FPSProfiler::recordFrameDelta(double deltaSeconds) {
    if (!std::isfinite(deltaSeconds) || deltaSeconds <= 0.0 || deltaSeconds > 1.0) return;
    m_frameDeltas.push_back(deltaSeconds);
    while (m_frameDeltas.size() > MAX_SAMPLES) m_frameDeltas.pop_front();
}

ProfilerStats FPSProfiler::getStats() const {
    ProfilerStats stats;
    stats.samples = m_frameDeltas.size();
    if (m_frameDeltas.empty()) return stats;

    double fpsTotal = 0.0;
    stats.lowestFPS = 1000000.0;
    std::vector<double> frameTimes;
    frameTimes.reserve(m_frameDeltas.size());
    for (double delta : m_frameDeltas) {
        const double fps = 1.0 / delta;
        fpsTotal += fps;
        stats.lowestFPS = std::min(stats.lowestFPS, fps);
        if (delta > (1.0 / 45.0)) ++stats.slowFrames;
        frameTimes.push_back(delta * 1000.0);
    }
    stats.averageFPS = fpsTotal / static_cast<double>(m_frameDeltas.size());
    std::sort(frameTimes.begin(), frameTimes.end());
    const double percentilePosition = std::ceil(static_cast<double>(frameTimes.size()) * 0.95) - 1.0;
    const std::size_t percentileIndex = static_cast<std::size_t>(std::max(0.0, percentilePosition));
    const std::size_t index = std::min(frameTimes.size() - 1, percentileIndex);
    stats.p95FrameMs = frameTimes[index];
    return stats;
}

void FPSProfiler::clear() {
    m_frameDeltas.clear();
}

} // namespace Veyron
