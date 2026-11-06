#include "VeyronStudio.hpp"

#include <sstream>

namespace Veyron {

std::string Diagnostics::severityName(DiagnosticSeverity severity) {
    switch (severity) {
        case DiagnosticSeverity::Info: return "INFO";
        case DiagnosticSeverity::Warning: return "WARNING";
        case DiagnosticSeverity::Error: return "ERROR";
    }
    return "UNKNOWN";
}

std::string Diagnostics::format(const std::vector<Diagnostic>& diagnostics) {
    if (diagnostics.empty()) return "No diagnostics were generated.";
    std::ostringstream output;
    for (const auto& item : diagnostics) {
        output << '[' << severityName(item.severity) << "] " << item.code << " - " << item.message << '\n';
    }
    return output.str();
}

} // namespace Veyron
