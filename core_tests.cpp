#include "VeyronStudio.hpp"

#ifdef NDEBUG
#undef NDEBUG
#endif
#include <cassert>
#include <cmath>
#include <iostream>

int main() {
    using namespace Veyron;

    LevelData source;
    source.name = "Regression Test";
    source.objects = {
        {1, 2, 3, 0.0, 0.0, false},
        {901, 2, 3, 120.0, 0.0, true},
        {211, 5, 4, 240.0, 48.0, false}
    };

    const auto fixture = LevelParser::makeFixture(source);
    LevelData parsed;
    std::string error;
    assert(LevelParser::parseFixture(fixture, parsed, error));
    assert(parsed.name == source.name);
    assert(parsed.objects.size() == 3);
    assert(LevelDataStore::countTriggers(parsed) == 1);

    LevelData invalid;
    assert(!LevelParser::parseFixture("name=No Header\n", invalid, error));
    assert(!error.empty());
    assert(!LevelParser::parseFixture("VEYRON_LEVEL_V1\nname=Bad\nobject=1,2,3,1,2,9\n", invalid, error));

    const auto analysis = LevelAnalyzer().analyze(parsed, 2);
    assert(analysis.objectCount == 3);
    assert(analysis.triggerCount == 1);
    assert(analysis.groupCount == 2);
    assert(analysis.colorCount == 2);
    assert(analysis.errorCount == 0);

    const auto frames = FrameAnalyzer::analyzeInputFrames({10, 11, 19, 32, 34, 35, 58, 83, 84, 85, -1, 10});
    assert(frames.inputCount == 10);
    assert(frames.gapCount == 9);
    assert(frames.smallestGap == 1);
    assert(frames.largestGap == 25);
    assert(frames.oneFrameGaps == 4);
    assert(frames.twoFrameGaps == 1);

    FPSProfiler profiler;
    profiler.recordFrameDelta(1.0 / 60.0);
    profiler.recordFrameDelta(1.0 / 30.0);
    profiler.recordFrameDelta(0.0);
    const auto fps = profiler.getStats();
    assert(fps.samples == 2);
    assert(fps.slowFrames == 1);
    assert(std::abs(fps.averageFPS - 45.0) < 0.01);
    assert(fps.p95FrameMs > 30.0);
    profiler.clear();
    assert(profiler.getStats().samples == 0);

    const auto groupUsage = GroupManager::countUsage(parsed);
    const auto colorUsage = ColorManager::countUsage(parsed);
    assert(groupUsage.at(2) == 2);
    assert(colorUsage.at(3) == 2);
    assert(GroupManager::findUnused(parsed, 1, 3).size() == 2);
    assert(ColorManager::findUnused(parsed, 1, 3).size() == 2);

    SnapshotManager snapshots;
    snapshots.takeSnapshot(parsed);
    auto edited = parsed;
    edited.objects.pop_back();
    edited.objects.push_back({1764, 9, 9, 500.0, 80.0, true});
    const auto diff = snapshots.compareWithLatest(edited);
    assert(diff.added == 1);
    assert(diff.removed == 1);
    assert(diff.unchanged == 2);
    assert(snapshots.getSnapshot(0) != nullptr);

    MacroEngine recorder;
    assert(recorder.beginRecording("Test Macro", 240));
    assert(recorder.recordEvent(10, 1, true));
    assert(recorder.recordEvent(11, 1, false));
    assert(!recorder.recordEvent(9, 1, true));
    auto clip = recorder.finishRecording();
    assert(!recorder.isRecording());
    assert(clip.events.size() == 2);

    const auto serialized = MacroEngine::serialize(clip);
    MacroClip imported;
    assert(MacroEngine::parse(serialized, imported, error));
    assert(imported.name == "Test Macro");
    assert(imported.events.size() == 2);
    assert(!MacroEngine::parse("not-a-macro\n", imported, error));
    assert(!error.empty());

    ProjectManager projects;
    projects.setProjectName("Android Project");
    projects.setNotes("Core checks passed.");
    assert(projects.getProfile().name == "Android Project");
    assert(projects.getProfile().notes == "Core checks passed.");

    assert(!StructureVisualizer::buildSummary(parsed).empty());
    assert(!Diagnostics::format(analysis.diagnostics).empty());

    std::cout << "Veyron Studio core tests passed.\n";
    return 0;
}
