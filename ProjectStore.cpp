#include <Geode/Geode.hpp>
#include "ProjectStore.hpp"

using namespace geode::prelude;

namespace Veyron {

std::string ProjectStore::getProjectName() {
    const auto fallback = Mod::get()->getSettingValue<std::string>("project-name");
    return Mod::get()->getSavedValue<std::string>("project/name", fallback);
}

void ProjectStore::setProjectName(const std::string& name) {
    Mod::get()->setSavedValue<std::string>("project/name", name.empty() ? "Untitled Project" : name);
}

std::string ProjectStore::getProjectNotes() {
    const auto fallback = Mod::get()->getSettingValue<std::string>("project-notes");
    return Mod::get()->getSavedValue<std::string>("project/notes", fallback);
}

void ProjectStore::setProjectNotes(const std::string& notes) {
    Mod::get()->setSavedValue<std::string>("project/notes", notes);
}

} // namespace Veyron
