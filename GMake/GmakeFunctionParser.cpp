#include "GmakeFunctionParser.h"

#include "assert_print.h"
#include "ExceptionHandler.h"

namespace gmake {
    std::vector<std::string> split_spaces(const std::string& str) {
        std::vector<std::string> result;
        std::istringstream iss(str);

        std::string word;
        while (iss >> word) {
            result.push_back(word);
        }
        return result;
    }

    LiteralType parse_literal_type(const std::string& literal_type_str) {
        if (literal_type_str == "Number") {
            return LiteralType::NUMBER;
        } else if (literal_type_str == "Path") {
            return LiteralType::PATH;
        } else if (literal_type_str == "Name") {
            return LiteralType::NAME;
        } else if (literal_type_str == "Version") {
            return LiteralType::VERSION;
        } else if (literal_type_str == "Continue") {
            return LiteralType::CONTINUE;
        }
        ExceptionHandler.error(34 , "Fuck you");
        throw std::runtime_error("for warning");
    }

    std::map<std::string, std::vector<LiteralType>> function_parameters_generator(const fs::path& tool_dir) {
        std::map<std::string, std::vector<LiteralType>> result = {};
        fs::path function_txt_path = tool_dir / "gmake_functions.txt";
        std::string read_file = ReadFilePath(function_txt_path);
        std::vector<std::string> lines;
        std::string line;
        for (const char& char_read: read_file) {
            if (char_read != '\n') {
                line.push_back(char_read);
            }
            else {
                lines.push_back(line);
                line.clear();
            }
        }
        if (!line.empty()) {
            lines.push_back(line);
            line.clear();
        }
        for (const std::string& parse_line : lines) {
            std::vector<std::string> line_data = split_spaces(parse_line);
            const std::string& func_name = line_data.at(0);
            std::vector<LiteralType> literal_types = {};
            for (int i = 1; i < line_data.size(); ++i) {
                const std::string& line_datas = line_data.at(i);
                LiteralType literal_type = parse_literal_type(line_datas);
                literal_types.push_back(literal_type);
            }
            result.emplace(func_name, literal_types);
        }
        return result;
    }
}
