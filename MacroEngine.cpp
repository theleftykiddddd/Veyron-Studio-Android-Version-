#include "VeyronStudio.hpp"

#include <charconv>
#include <sstream>
#include <string_view>
#include <utility>

namespace Veyron {
namespace {

bool parseUnsigned(std::string_view text, std::uint32_t& value) {
    if (text.empty()) return false;
    const auto parsed = std::from_chars(text.data(), text.data() + text.size(), value);
    return parsed.ec == std::errc{} && parsed.ptr == text.data() + text.size();
}

} // namespace

bool MacroEngine::beginRecording(std::string name, std::uint16_t framerate) {
    if (m_recording || framerate < 30 || framerate > 1000) return false;
    m_current = {};
    m_current.name = name.empty() ? "Untitled Macro" : std::move(name);
    m_current.framerate = framerate;
    m_recording = true;
    return true;
}

bool MacroEngine::recordEvent(std::uint32_t frame, std::uint8_t player, bool buttonDown) {
    if (!m_recording || (player != 1 && player != 2)) return false;
    if (!m_current.events.empty() && frame < m_current.events.back().frame) return false;
    if (!m_current.events.empty()) {
        const auto& last = m_current.events.back();
        if (last.frame == frame && last.player == player && last.buttonDown == buttonDown) return true;
    }
    m_current.events.push_back({frame, player, buttonDown});
    return true;
}

MacroClip MacroEngine::finishRecording() {
    m_recording = false;
    MacroClip result = std::move(m_current);
    m_current = {};
    return result;
}

bool MacroEngine::isRecording() const { return m_recording; }

bool MacroEngine::validate(const MacroClip& clip, std::string& error) {
    if (clip.name.empty()) { error = "Macro name cannot be empty."; return false; }
    if (clip.framerate < 30 || clip.framerate > 1000) { error = "Framerate must be between 30 and 1000."; return false; }
    std::uint32_t previousFrame = 0;
    bool first = true;
    for (const auto& event : clip.events) {
        if (event.player != 1 && event.player != 2) { error = "Player must be 1 or 2."; return false; }
        if (!first && event.frame < previousFrame) { error = "Macro events must be sorted by frame."; return false; }
        previousFrame = event.frame;
        first = false;
    }
    error.clear();
    return true;
}

std::string MacroEngine::serialize(const MacroClip& clip) {
    std::string error;
    if (!validate(clip, error)) return {};
    std::ostringstream output;
    output << "VEYRON_MACRO_V1\nname=" << clip.name << "\nframerate=" << clip.framerate << '\n';
    for (const auto& event : clip.events) {
        output << "event=" << event.frame << ',' << static_cast<unsigned int>(event.player) << ',' << (event.buttonDown ? 1 : 0) << '\n';
    }
    return output.str();
}

bool MacroEngine::parse(const std::string& text, MacroClip& output, std::string& error) {
    MacroClip parsed;
    std::istringstream input(text);
    std::string line;
    bool header = false;
    std::size_t lineNumber = 0;
    while (std::getline(input, line)) {
        ++lineNumber;
        if (line.empty() || line[0] == '#') continue;
        if (line == "VEYRON_MACRO_V1") { header = true; continue; }
        if (line.rfind("name=", 0) == 0) { parsed.name = line.substr(5); continue; }
        if (line.rfind("framerate=", 0) == 0) {
            std::uint32_t value = 0;
            if (!parseUnsigned(std::string_view(line).substr(10), value) || value > 65535) {
                error = "Line " + std::to_string(lineNumber) + ": invalid framerate."; return false;
            }
            parsed.framerate = static_cast<std::uint16_t>(value);
            continue;
        }
        if (line.rfind("event=", 0) == 0) {
            const auto fields = std::string_view(line).substr(6);
            const auto comma1 = fields.find(',');
            const auto comma2 = comma1 == std::string_view::npos ? comma1 : fields.find(',', comma1 + 1);
            if (comma1 == std::string_view::npos || comma2 == std::string_view::npos || fields.find(',', comma2 + 1) != std::string_view::npos) {
                error = "Line " + std::to_string(lineNumber) + ": event requires frame,player,down."; return false;
            }
            std::uint32_t frame = 0, player = 0, down = 0;
            if (!parseUnsigned(fields.substr(0, comma1), frame) || !parseUnsigned(fields.substr(comma1 + 1, comma2 - comma1 - 1), player) ||
                !parseUnsigned(fields.substr(comma2 + 1), down) || player > 255 || down > 1) {
                error = "Line " + std::to_string(lineNumber) + ": invalid event values."; return false;
            }
            parsed.events.push_back({frame, static_cast<std::uint8_t>(player), down == 1});
            continue;
        }
        error = "Line " + std::to_string(lineNumber) + ": unknown macro directive.";
        return false;
    }
    if (!header) { error = "Missing VEYRON_MACRO_V1 header."; return false; }
    if (!validate(parsed, error)) return false;
    output = std::move(parsed);
    error.clear();
    return true;
}

FrameAnalysis MacroEngine::analyze(const MacroClip& clip) {
    std::vector<int> frames;
    frames.reserve(clip.events.size());
    for (const auto& event : clip.events) frames.push_back(static_cast<int>(event.frame));
    return FrameAnalyzer::analyzeInputFrames(std::move(frames));
}

} // namespace Veyron
