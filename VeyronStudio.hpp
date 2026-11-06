#pragma once

#include <cstddef>
#include <cstdint>
#include <deque>
#include <map>
#include <string>
#include <vector>

namespace Veyron {

struct LevelObject {
    int objectID = 0;
    int groupID = 0;
    int colorID = 0;
    double x = 0.0;
    double y = 0.0;
    bool isTrigger = false;
};

struct LevelData {
    std::string name;
    std::vector<LevelObject> objects;
};

enum class DiagnosticSeverity { Info, Warning, Error };

struct Diagnostic {
    DiagnosticSeverity severity = DiagnosticSeverity::Info;
    std::string code;
    std::string message;
};

struct LevelAnalysis {
    std::size_t objectCount = 0;
    std::size_t triggerCount = 0;
    std::size_t groupCount = 0;
    std::size_t colorCount = 0;
    std::size_t errorCount = 0;
    std::size_t warningCount = 0;
    std::vector<Diagnostic> diagnostics;
};

struct FrameAnalysis {
    std::size_t inputCount = 0;
    std::size_t gapCount = 0;
    int smallestGap = 0;
    int largestGap = 0;
    double averageGap = 0.0;
    double medianGap = 0.0;
    std::size_t oneFrameGaps = 0;
    std::size_t twoFrameGaps = 0;
    std::size_t threeOrMoreFrameGaps = 0;
};

struct ProfilerStats {
    std::size_t samples = 0;
    double averageFPS = 0.0;
    double lowestFPS = 0.0;
    double p95FrameMs = 0.0;
    std::size_t slowFrames = 0;
};

struct SnapshotDiff {
    std::size_t added = 0;
    std::size_t removed = 0;
    std::size_t unchanged = 0;
};

struct ProjectProfile {
    std::string name = "Untitled Project";
    std::string target = "Geometry Dash 2.2091 / Android64";
    std::string notes;
};

struct MacroEvent {
    std::uint32_t frame = 0;
    std::uint8_t player = 1;
    bool buttonDown = false;
};

struct MacroClip {
    std::string name = "Untitled Macro";
    std::uint16_t framerate = 240;
    std::vector<MacroEvent> events;
};

class LevelDataStore {
public:
    static std::size_t countTriggers(const LevelData& level);
};

class LevelParser {
public:
    // Parses Veyron's documented, human-readable .veylevel fixture format.
    // This is not a decoder for Geometry Dash's compressed level string.
    static bool parseFixture(const std::string& text, LevelData& output, std::string& error);
    static std::string makeFixture(const LevelData& level);
};

class Diagnostics {
public:
    static std::string severityName(DiagnosticSeverity severity);
    static std::string format(const std::vector<Diagnostic>& diagnostics);
};

class LevelAnalyzer {
public:
    LevelAnalysis analyze(const LevelData& level, int sensitivity = 2) const;
};

class FrameAnalyzer {
public:
    static FrameAnalysis analyzeInputFrames(std::vector<int> frames);
};

class FPSProfiler {
public:
    void recordFrameDelta(double deltaSeconds);
    ProfilerStats getStats() const;
    void clear();

private:
    std::deque<double> m_frameDeltas;
    static constexpr std::size_t MAX_SAMPLES = 600;
};

class ColorManager {
public:
    static std::map<int, std::size_t> countUsage(const LevelData& level);
    static std::vector<int> findUnused(const LevelData& level, int firstID = 1, int lastID = 999);
};

class GroupManager {
public:
    static std::map<int, std::size_t> countUsage(const LevelData& level);
    static std::vector<int> findUnused(const LevelData& level, int firstID = 1, int lastID = 999);
};

class SnapshotManager {
public:
    void takeSnapshot(const LevelData& level);
    SnapshotDiff compareWithLatest(const LevelData& level) const;
    bool hasSnapshot() const;
    std::size_t snapshotCount() const;
    const LevelData* getSnapshot(std::size_t index) const;

private:
    std::vector<LevelData> m_snapshots;
    static constexpr std::size_t MAX_SNAPSHOTS = 8;
};

class MacroEngine {
public:
    bool beginRecording(std::string name, std::uint16_t framerate = 240);
    bool recordEvent(std::uint32_t frame, std::uint8_t player, bool buttonDown);
    MacroClip finishRecording();
    bool isRecording() const;

    static bool validate(const MacroClip& clip, std::string& error);
    static std::string serialize(const MacroClip& clip);
    static bool parse(const std::string& text, MacroClip& output, std::string& error);
    static FrameAnalysis analyze(const MacroClip& clip);

private:
    bool m_recording = false;
    MacroClip m_current;
};

class ProjectManager {
public:
    const ProjectProfile& getProfile() const;
    void setProjectName(std::string name);
    void setNotes(std::string notes);

private:
    ProjectProfile m_profile;
};

class StructureVisualizer {
public:
    static std::string buildSummary(const LevelData& level, double segmentWidth = 500.0);
};

class StudioManager {
public:
    static StudioManager& get();
    LevelData makeDemoLevel() const;
    MacroClip makeDemoMacro() const;
    std::string getVersionLabel() const;

private:
    StudioManager() = default;
};

} // namespace Veyron
