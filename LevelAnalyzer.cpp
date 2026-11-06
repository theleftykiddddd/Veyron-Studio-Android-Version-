#include "VeyronStudio.hpp"

#include <cmath>
#include <set>
#include <string>

namespace Veyron {

LevelAnalysis LevelAnalyzer::analyze(const LevelData& level, int sensitivity) const {
    LevelAnalysis result;
    result.objectCount = level.objects.size();
    if (sensitivity < 1) sensitivity = 1;
    if (sensitivity > 5) sensitivity = 5;

    auto add = [&](DiagnosticSeverity severity, std::string code, std::string message) {
        result.diagnostics.push_back({severity, std::move(code), std::move(message)});
        if (severity == DiagnosticSeverity::Error) ++result.errorCount;
        if (severity == DiagnosticSeverity::Warning) ++result.warningCount;
    };

    if (level.name.empty()) add(DiagnosticSeverity::Error, "LEVEL_NAME", "The supplied dataset has no level name.");
    if (level.objects.empty()) {
        add(DiagnosticSeverity::Info, "EMPTY_DATA", "No object records were supplied.");
        return result;
    }

    std::set<int> groups;
    std::set<int> colors;
    std::size_t triggers = 0;
    const double coordinateLimit = sensitivity >= 4 ? 50000.0 : 100000.0;

    for (std::size_t index = 0; index < level.objects.size(); ++index) {
        const auto& object = level.objects[index];
        const std::string label = "Record " + std::to_string(index + 1) + ": ";

        if (object.objectID <= 0) add(DiagnosticSeverity::Error, "OBJECT_ID", label + "object ID must be positive.");
        if (object.groupID < 0 || object.groupID > 999) add(DiagnosticSeverity::Warning, "GROUP_RANGE", label + "Group ID is outside the common 0-999 range.");
        if (object.colorID < 0 || object.colorID > 999) add(DiagnosticSeverity::Warning, "COLOR_RANGE", label + "Color ID is outside the common 0-999 range.");
        if (object.groupID > 0) groups.insert(object.groupID);
        if (object.colorID > 0) colors.insert(object.colorID);
        if (object.isTrigger) ++triggers;

        if (!std::isfinite(object.x) || !std::isfinite(object.y)) {
            add(DiagnosticSeverity::Error, "POSITION_INVALID", label + "position is not finite.");
        } else if (std::abs(object.x) > coordinateLimit || std::abs(object.y) > coordinateLimit) {
            add(DiagnosticSeverity::Warning, "POSITION_EXTREME", label + "position is unusually far from the origin; review it manually.");
        }
    }

    result.triggerCount = triggers;
    result.groupCount = groups.size();
    result.colorCount = colors.size();
    if (triggers == 0) add(DiagnosticSeverity::Info, "NO_TRIGGERS", "No records are marked as triggers in this dataset.");
    if (result.errorCount == 0 && result.warningCount == 0) add(DiagnosticSeverity::Info, "CLEAN", "No validation issues were found in the supplied records.");
    return result;
}

} // namespace Veyron
