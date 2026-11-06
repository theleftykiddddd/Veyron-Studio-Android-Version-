#include <Geode/Geode.hpp>

#include "StudioUI.hpp"
#include "ProjectStore.hpp"
#include "VeyronStudio.hpp"

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <string>

using namespace geode::prelude;

namespace Veyron {
namespace {

std::string joinIDs(const std::vector<int>& values, std::size_t limit = 14) {
    if (values.empty()) return "none";
    std::ostringstream out;
    const auto count = std::min(values.size(), limit);
    for (std::size_t i = 0; i < count; ++i) {
        if (i) out << ", ";
        out << values[i];
    }
    if (values.size() > count) out << " ... (" << (values.size() - count) << " more)";
    return out.str();
}

std::string formatDouble(double value, int precision = 2) {
    std::ostringstream out;
    out << std::fixed << std::setprecision(precision) << value;
    return out.str();
}

class StudioDashboard final : public CCLayerColor {
public:
    static StudioDashboard* create() {
        auto* layer = new StudioDashboard();
        if (layer && layer->init()) {
            layer->autorelease();
            return layer;
        }
        CC_SAFE_DELETE(layer);
        return nullptr;
    }

    bool init() override {
        if (!CCLayerColor::initWithColor(ccc4(2, 4, 8, 205))) return false;

        const auto win = CCDirector::sharedDirector()->getWinSize();
        this->setContentSize(win);

        const float panelWidth = win.width * 0.90f;
        const float panelHeight = win.height * 0.88f;
        const float panelX = (win.width - panelWidth) * 0.5f;
        const float panelY = (win.height - panelHeight) * 0.5f;

        auto* panel = CCLayerColor::create(ccc4(13, 18, 28, 250));
        if (!panel) return false;
        panel->setContentSize({panelWidth, panelHeight});
        panel->setPosition({panelX, panelY});
        panel->setID("veyron-studio-panel");
        this->addChild(panel, 1);

        auto* topLine = CCLayerColor::create(ccc4(235, 239, 245, 255));
        topLine->setContentSize({panelWidth, 2.0f});
        topLine->setPosition({0.0f, panelHeight - 2.0f});
        panel->addChild(topLine, 2);

        auto* title = CCLabelBMFont::create("VEYRON STUDIO", "bigFont.fnt");
        if (!title) return false;
        title->setScale(0.62f);
        title->setColor(ccc3(245, 247, 250));
        title->setPosition({panelWidth * 0.5f, panelHeight - 35.0f});
        panel->addChild(title, 3);

        auto* subtitle = CCLabelBMFont::create("ANDROID64  //  CREATOR TOOLKIT  //  BUILD 1.2.0", "bigFont.fnt");
        if (subtitle) {
            subtitle->setScale(0.28f);
            subtitle->setColor(ccc3(155, 167, 183));
            subtitle->setPosition({panelWidth * 0.5f, panelHeight - 60.0f});
            panel->addChild(subtitle, 3);
        }

        auto* divider = CCLayerColor::create(ccc4(65, 75, 90, 255));
        divider->setContentSize({panelWidth - 42.0f, 1.0f});
        divider->setPosition({21.0f, panelHeight - 81.0f});
        panel->addChild(divider, 2);

        auto* menu = CCMenu::create();
        if (!menu) return false;
        menu->setPosition({0.0f, 0.0f});
        panel->addChild(menu, 4);

        const float rowOne = panelHeight - 128.0f;
        const float rowTwo = panelHeight - 190.0f;
        addButton(menu, "HOME", panelWidth * 0.125f, rowOne, menu_selector(StudioDashboard::onHome));
        addButton(menu, "ANALYZER", panelWidth * 0.375f, rowOne, menu_selector(StudioDashboard::onAnalyzer));
        addButton(menu, "FRAME LAB", panelWidth * 0.625f, rowOne, menu_selector(StudioDashboard::onFrames));
        addButton(menu, "ID MAP", panelWidth * 0.875f, rowOne, menu_selector(StudioDashboard::onIDs));
        addButton(menu, "SNAPSHOTS", panelWidth * 0.125f, rowTwo, menu_selector(StudioDashboard::onSnapshots));
        addButton(menu, "VISUALIZER", panelWidth * 0.375f, rowTwo, menu_selector(StudioDashboard::onVisualizer));
        addButton(menu, "PERFORMANCE", panelWidth * 0.625f, rowTwo, menu_selector(StudioDashboard::onPerformance));
        addButton(menu, "MACRO LAB", panelWidth * 0.875f, rowTwo, menu_selector(StudioDashboard::onMacros));
        addButton(menu, "PROJECT", panelWidth * 0.35f, 57.0f, menu_selector(StudioDashboard::onProject));

        auto* closeLabel = CCLabelBMFont::create("X", "bigFont.fnt");
        if (closeLabel) {
            closeLabel->setScale(0.52f);
            closeLabel->setColor(ccc3(235, 235, 240));
            auto* closeButton = CCMenuItemLabel::create(closeLabel, this, menu_selector(StudioDashboard::onClose));
            if (closeButton) {
                closeButton->setPosition({panelWidth - 25.0f, panelHeight - 25.0f});
                menu->addChild(closeButton);
            }
        }

        auto* footer = CCLabelBMFont::create("CORE TOOLS READY  |  LIVE EDITOR BRIDGE NOT ENABLED", "bigFont.fnt");
        if (footer) {
            footer->setScale(0.25f);
            footer->setColor(ccc3(126, 140, 157));
            footer->setPosition({panelWidth * 0.5f, 23.0f});
            panel->addChild(footer, 3);
        }

        this->scheduleUpdate();
        if (Mod::get()->getSettingValue<bool>("animate-ui")) {
            panel->setScale(0.97f);
            panel->runAction(CCScaleTo::create(0.16f, 1.0f));
        }
        return true;
    }

    void update(float dt) override {
        CCLayerColor::update(dt);
        m_profiler.recordFrameDelta(static_cast<double>(dt));
    }

private:
    FPSProfiler m_profiler;
    SnapshotManager m_snapshots;

    void addButton(CCMenu* menu, const char* labelText, float x, float y, SEL_MenuHandler handler) {
        auto* label = CCLabelBMFont::create(labelText, "bigFont.fnt");
        if (!label) return;
        label->setScale(0.36f);
        label->setColor(ccc3(222, 230, 240));
        auto* item = CCMenuItemLabel::create(label, this, handler);
        if (!item) return;
        item->setPosition({x, y});
        menu->addChild(item);
    }

    void showPage(const char* title, const std::string& body) {
        FLAlertLayer::create(title, body, "BACK")->show();
    }

    void onHome(CCObject*) {
        std::ostringstream text;
        text << "WELCOME TO YOUR CREATOR WORKSPACE\n\n"
             << "Project: " << ProjectStore::getProjectName() << "\n"
             << "Target: Geometry Dash 2.2091 / Android64\n\n"
             << "MODULES\n"
             << "- Level diagnostics and validation\n"
             << "- Frame-gap and macro data analysis\n"
             << "- Group ID / Color ID usage maps\n"
             << "- Snapshot comparison\n"
             << "- Structure visualization\n"
             << "- Runtime frame-time monitor\n\n"
             << "The core modules are implemented. The current release does not yet read or modify the live editor level automatically.";
        showPage("VEYRON // OVERVIEW", text.str());
    }

    void onAnalyzer(CCObject*) {
        const auto level = StudioManager::get().makeDemoLevel();
        const int sensitivity = static_cast<int>(Mod::get()->getSettingValue<int64_t>("diagnostic-sensitivity"));
        const auto analysis = LevelAnalyzer().analyze(level, sensitivity);
        std::ostringstream text;
        text << "LEVEL DIAGNOSTICS // SAMPLE DATA\n\n"
             << "Dataset: " << level.name << '\n'
             << "Object records: " << analysis.objectCount << '\n'
             << "Trigger records: " << analysis.triggerCount << '\n'
             << "Unique groups: " << analysis.groupCount << '\n'
             << "Unique colors: " << analysis.colorCount << '\n'
             << "Errors: " << analysis.errorCount << " | Warnings: " << analysis.warningCount << "\n\n"
             << Diagnostics::format(analysis.diagnostics)
             << "\nThis is the bundled sample dataset, not your currently open editor level.";
        showPage("VEYRON // ANALYZER", text.str());
    }

    void onFrames(CCObject*) {
        const auto macro = StudioManager::get().makeDemoMacro();
        const auto stats = MacroEngine::analyze(macro);
        std::ostringstream text;
        text << "FRAME LAB // MACRO TIMING ANALYSIS\n\n"
             << "Macro: " << macro.name << '\n'
             << "Input events: " << macro.events.size() << '\n'
             << "Unique input frames: " << stats.inputCount << '\n'
             << "Frame gaps: " << stats.gapCount << '\n'
             << "Smallest gap: " << stats.smallestGap << '\n'
             << "Largest gap: " << stats.largestGap << '\n'
             << "Average gap: " << formatDouble(stats.averageGap) << '\n'
             << "Median gap: " << formatDouble(stats.medianGap) << '\n'
             << "1-frame gaps: " << stats.oneFrameGaps << '\n'
             << "2-frame gaps: " << stats.twoFrameGaps << '\n'
             << "3+ frame gaps: " << stats.threeOrMoreFrameGaps << "\n\n"
             << "The frame analyzer and macro data format are working. Live touch/input capture is not connected in this build.";
        showPage("VEYRON // FRAME LAB", text.str());
    }

    void onIDs(CCObject*) {
        const auto level = StudioManager::get().makeDemoLevel();
        const auto groups = GroupManager::countUsage(level);
        const auto colors = ColorManager::countUsage(level);
        const auto freeGroups = GroupManager::findUnused(level, 1, 12);
        const auto freeColors = ColorManager::findUnused(level, 1, 12);
        std::ostringstream text;
        text << "ID MAP // SAMPLE DATA\n\nGROUP ID USAGE\n";
        for (const auto& [id, count] : groups) text << "Group " << id << "  ->  " << count << " object(s)\n";
        text << "Free IDs 1-12: " << joinIDs(freeGroups) << "\n\nCOLOR ID USAGE\n";
        for (const auto& [id, count] : colors) text << "Color " << id << "  ->  " << count << " object(s)\n";
        text << "Free IDs 1-12: " << joinIDs(freeColors)
             << "\n\nThe usage and free-ID calculations operate on the supplied dataset; automatic editor assignment is not enabled.";
        showPage("VEYRON // ID MAP", text.str());
    }

    void onSnapshots(CCObject*) {
        const auto baseline = StudioManager::get().makeDemoLevel();
        m_snapshots.takeSnapshot(baseline);
        auto changed = baseline;
        if (!changed.objects.empty()) changed.objects.pop_back();
        changed.objects.push_back({211, 12, 9, 2100.0, 96.0, false});
        const auto diff = m_snapshots.compareWithLatest(changed);
        std::ostringstream text;
        text << "SNAPSHOT CORE // COMPARISON TEST\n\n"
             << "Stored snapshots this session: " << m_snapshots.snapshotCount() << '\n'
             << "Added records: " << diff.added << '\n'
             << "Removed records: " << diff.removed << '\n'
             << "Unchanged records: " << diff.unchanged << "\n\n"
             << "Snapshot storage and object-by-object comparison are implemented in the core. Automatic capture/rollback of the live editor level is not connected yet.";
        showPage("VEYRON // SNAPSHOTS", text.str());
    }

    void onVisualizer(CCObject*) {
        const auto level = StudioManager::get().makeDemoLevel();
        showPage("VEYRON // STRUCTURE MAP", StructureVisualizer::buildSummary(level, 500.0));
    }

    void onPerformance(CCObject*) {
        const auto stats = m_profiler.getStats();
        std::ostringstream text;
        text << "RUNTIME PROFILER // DASHBOARD SESSION\n\n"
             << "Samples: " << stats.samples << '\n';
        if (stats.samples == 0) {
            text << "No frame samples recorded yet. Close this panel and keep the Veyron dashboard open briefly before checking again.\n";
        } else {
            text << "Average FPS: " << formatDouble(stats.averageFPS, 1) << '\n'
                 << "Lowest sampled FPS: " << formatDouble(stats.lowestFPS, 1) << '\n'
                 << "P95 frame time: " << formatDouble(stats.p95FrameMs, 2) << " ms\n"
                 << "Frames slower than 45 FPS: " << stats.slowFrames << '\n';
        }
        text << "\nThese measurements are captured while the Veyron dashboard is open, not during gameplay.";
        showPage("VEYRON // PERFORMANCE", text.str());
    }

    void onMacros(CCObject*) {
        auto sample = StudioManager::get().makeDemoMacro();
        const auto encoded = MacroEngine::serialize(sample);
        MacroClip decoded;
        std::string error;
        const bool roundTrip = MacroEngine::parse(encoded, decoded, error);
        std::ostringstream text;
        text << "MACRO LAB // DATA ENGINE\n\n"
             << "Sample macro: " << sample.name << '\n'
             << "Framerate: " << sample.framerate << " FPS\n"
             << "Events: " << sample.events.size() << '\n'
             << "Export format: VEYRON_MACRO_V1\n"
             << "Import/export round-trip: " << (roundTrip ? "PASS" : "FAIL") << "\n"
             << "Serialized bytes: " << encoded.size() << "\n\n"
             << "Macro recording data structures, validation and text import/export are implemented. The game input hook and playback injection are not connected in this Android build.";
        showPage("VEYRON // MACRO LAB", text.str());
    }

    void onProject(CCObject*) {
        std::ostringstream text;
        text << "PROJECT WORKSPACE\n\n"
             << "Name: " << ProjectStore::getProjectName() << '\n'
             << "Notes: " << (ProjectStore::getProjectNotes().empty() ? "No notes saved." : ProjectStore::getProjectNotes()) << "\n\n"
             << "Project name and notes use Geode's per-mod saved values. You can edit the default name and notes in Veyron Studio's Geode settings.";
        showPage("VEYRON // PROJECT", text.str());
    }

    void onClose(CCObject*) {
        this->removeFromParentAndCleanup(true);
    }
};

} // namespace

void StudioUI::showDashboard() {
    auto* scene = CCDirector::sharedDirector()->getRunningScene();
    if (!scene) return;
    if (scene->getChildByID("veyron-studio-dashboard")) return;
    auto* dashboard = StudioDashboard::create();
    if (!dashboard) {
        FLAlertLayer::create("Veyron Studio", "The Android dashboard could not be initialized.", "OK")->show();
        return;
    }
    dashboard->setID("veyron-studio-dashboard");
    scene->addChild(dashboard, 10000);
}

} // namespace Veyron
