#pragma once
#include <filesystem>
#include <map>
#include <string>
#include <vector>
#include "LiteralTypes.h"
#include "file_utils.h"

namespace gmake {
    namespace fs = std::filesystem;
    std::map<std::string, std::vector<LiteralType>> function_parameters_generator(const fs::path& tool_dir);
}
