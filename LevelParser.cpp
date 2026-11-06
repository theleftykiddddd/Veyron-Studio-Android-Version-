#include "VeyronStudio.hpp"

#include <charconv>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <string_view>
#include <utility>

namespace Veyron {
namespace {

std::string_view trim(std::string_view value) {
    while (!value.empty() && (value.front() == ' ' || value.front() == '\t' || value.front() == '\r')) value.remove_prefix(1);
    while (!value.empty() && (value.back() == ' ' || value.back() == '\t' || value.back() == '\r')) value.remove_suffix(1);
    return value;
}

bool parseInt(std::string_view value, int& output) {
    value = trim(value);
    if (value.empty()) return false;
    const auto result = std::from_chars(value.data(), value.data() + value.size(), output);
    return result.ec == std::errc{} && result.ptr == value.data() + value.size();
}

bool parseDouble(std::string_view value, double& output) {
    value = trim(value);
    if (value.empty()) return false;
    std::string owned(value);
    try {
        std::size_t consumed = 0;
        output = std::stod(owned, &consumed);
        return consumed == owned.size() && std::isfinite(output);
    } catch (...) {
        return false;
    }
}

} // namespace

bool LevelParser::parseFixture(const std::string& text, LevelData& output, std::string& error) {
    LevelData parsed;
    std::istringstream input(text);
    std::string line;
    std::size_t lineNumber = 0;
    bool sawHeader = false;

    while (std::getline(input, line)) {
        ++lineNumber;
        auto view = trim(line);
        if (view.empty() || view.front() == '#') continue;
        if (view == "VEYRON_LEVEL_V1") {
            sawHeader = true;
            continue;
        }
        if (view.starts_with("name=")) {
            parsed.name = std::string(trim(view.substr(5)));
            continue;
        }
        if (view.starts_with("object=")) {
            if (!sawHeader) {
                error = "Line " + std::to_string(lineNumber) + ": missing VEYRON_LEVEL_V1 header";
                return false;
            }
            auto fields = view.substr(7);
            double x = 0.0;
            double y = 0.0;
            const auto c1 = fields.find(',');
            const auto c2 = c1 == std::string_view::npos ? c1 : fields.find(',', c1 + 1);
            const auto c3 = c2 == std::string_view::npos ? c2 : fields.find(',', c2 + 1);
            const auto c4 = c3 == std::string_view::npos ? c3 : fields.find(',', c3 + 1);
            const auto c5 = c4 == std::string_view::npos ? c4 : fields.find(',', c4 + 1);
            if (c1 == std::string_view::npos || c2 == std::string_view::npos || c3 == std::string_view::npos ||
                c4 == std::string_view::npos || c5 == std::string_view::npos || fields.find(',', c5 + 1) != std::string_view::npos) {
                error = "Line " + std::to_string(lineNumber) + ": object requires 6 fields (id,group,color,x,y,trigger)";
                return false;
            }
            const auto f0 = fields.substr(0, c1);
            const auto f1 = fields.substr(c1 + 1, c2 - c1 - 1);
            const auto f2 = fields.substr(c2 + 1, c3 - c2 - 1);
            const auto f3 = fields.substr(c3 + 1, c4 - c3 - 1);
            const auto f4 = fields.substr(c4 + 1, c5 - c4 - 1);
            const auto f5 = fields.substr(c5 + 1);
            int trigger = 0;
            LevelObject object;
            if (!parseInt(f0, object.objectID) || !parseInt(f1, object.groupID) || !parseInt(f2, object.colorID) ||
                !parseDouble(f3, x) || !parseDouble(f4, y) || !parseInt(f5, trigger) || (trigger != 0 && trigger != 1)) {
                error = "Line " + std::to_string(lineNumber) + ": invalid object field";
                return false;
            }
            object.x = x;
            object.y = y;
            object.isTrigger = trigger == 1;
            parsed.objects.push_back(object);
            continue;
        }
        error = "Line " + std::to_string(lineNumber) + ": unknown directive";
        return false;
    }

    if (!sawHeader) {
        error = "Missing VEYRON_LEVEL_V1 header";
        return false;
    }
    if (parsed.name.empty()) {
        error = "Missing name=<level name> header";
        return false;
    }

    output = std::move(parsed);
    error.clear();
    return true;
}

std::string LevelParser::makeFixture(const LevelData& level) {
    std::ostringstream output;
    output << "VEYRON_LEVEL_V1\nname=" << level.name << '\n';
    output << std::setprecision(12);
    for (const auto& object : level.objects) {
        output << "object=" << object.objectID << ',' << object.groupID << ',' << object.colorID << ','
               << object.x << ',' << object.y << ',' << (object.isTrigger ? 1 : 0) << '\n';
    }
    return output.str();
}

} // namespace Veyron
