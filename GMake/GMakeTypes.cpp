#include "GMakeTypes.h"

namespace gmake {
    GMakeFunction parseFunction(const std::string& name) {
        static const std::unordered_map<std::string, GMakeFunction> functionMap = {
            {"SetProjectDirectory", GMakeFunction::SET_PROJECT_DIRECTORY},
            {"SetProgram", GMakeFunction::SET_PROGRAM},
            {"ExtendStandard", GMakeFunction::EXTEND_STANDARD},
            {"SetLayoutBinding", GMakeFunction::SSBO_LAYOUT_BINDING},
            {"SetMinimalVersion", GMakeFunction::SET_MINIMAL_VERSION}
        };

        std::unordered_map<std::string, GMakeFunction>::const_iterator it = functionMap.find(name);
        return (it != functionMap.end()) ? it->second : GMakeFunction::UNKNOWN;
    }
}
