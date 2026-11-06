#include "VeyronStudio.hpp"

#include <cmath>
#include <map>
#include <sstream>

namespace Veyron {

std::string StructureVisualizer::buildSummary(const LevelData& level, double segmentWidth) {
    if (level.objects.empty()) return "No object records are available for the structure map.";
    if (!std::isfinite(segmentWidth) || segmentWidth <= 0.0) segmentWidth = 500.0;
    std::map<long long, std::size_t> sections;
    for (const auto& object : level.objects) {
        if (!std::isfinite(object.x)) continue;
        const auto section = static_cast<long long>(std::floor(object.x / segmentWidth));
        ++sections[section];
    }
    std::ostringstream output;
    output << "STRUCTURE MAP // " << level.name << "\nSEGMENT WIDTH: " << segmentWidth << " UNITS\n\n";
    for (const auto& [section, count] : sections) {
        output << "SEG " << section << " | ";
        const std::size_t bars = count > 28 ? 28 : count;
        for (std::size_t i = 0; i < bars; ++i) output << '#';
        output << "  " << count << " objects\n";
    }
    return output.str();
}

} // namespace Veyron
