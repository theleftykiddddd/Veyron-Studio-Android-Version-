#include "VeyronStudio.hpp"

#include <utility>

namespace Veyron {

const ProjectProfile& ProjectManager::getProfile() const { return m_profile; }

void ProjectManager::setProjectName(std::string name) {
    if (!name.empty()) m_profile.name = std::move(name);
}

void ProjectManager::setNotes(std::string notes) {
    m_profile.notes = std::move(notes);
}

} // namespace Veyron
