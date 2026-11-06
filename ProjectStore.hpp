#pragma once

#include <string>

namespace Veyron {
    class ProjectStore {
    public:
        static std::string getProjectName();
        static void setProjectName(const std::string& name);
        static std::string getProjectNotes();
        static void setProjectNotes(const std::string& notes);
    };
}
