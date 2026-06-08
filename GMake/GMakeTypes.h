#ifndef EVENT_STRUCT_GMAKETYPES_H
#define EVENT_STRUCT_GMAKETYPES_H
#include <filesystem>
#include <string>
#include <vector>
#include <unordered_map>
#include <map>

namespace gmake {
    namespace fs = std::filesystem;

    enum class GMakeFunction {
        SET_PROJECT_DIRECTORY,
        SET_PROGRAM,
        EXTEND_STANDARD,
        SSBO_LAYOUT_BINDING,
        SET_MINIMAL_VERSION,
        UNKNOWN
    };

    enum class DebugType {
        BUILD,
        DEBUG,
        NONE,
    };

    struct GMAKEConfig {
        bool debug = false;
        DebugType Build = DebugType::NONE;
        fs::path ProjectDir;
        std::map<std::string, std::vector<fs::path>> ShaderPrograms;
        std::vector<fs::path> StandardExtensions;
        std::map<std::string, std::map<std::string, uint64_t>> SSBO_key_to_value = {};
    };

    GMakeFunction parseFunction(const std::string& name);
}

#endif
