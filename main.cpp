#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>

#include "StudioUI.hpp"

using namespace geode::prelude;

class $modify(VeyronMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        auto* menu = this->getChildByID("bottom-menu");
        if (!menu) menu = this->getChildByID("right-menu");
        if (!menu || menu->getChildByID("veyron-studio-button")) return true;

        auto* label = CCLabelBMFont::create("V", "bigFont.fnt");
        if (!label) return true;
        label->setColor(ccc3(240, 244, 250));
        label->setScale(0.8f);

        auto* button = CCMenuItemLabel::create(label, this, menu_selector(VeyronMenuLayer::onVeyronStudio));
        if (!button) return true;
        button->setID("veyron-studio-button");
        menu->addChild(button);
        menu->updateLayout();
        return true;
    }

    void onVeyronStudio(CCObject*) {
        Veyron::StudioUI::showDashboard();
    }
};
