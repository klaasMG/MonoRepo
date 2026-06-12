#pragma once
#include <filesystem>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "assert_print.h"
#include "ExceptionHandler.h"
#include "file_utils.h"
#include "GMakeTypes.h"

namespace fs = std::filesystem;

namespace gmake {
	struct SSBOBlock {
		std::string text;
		size_t start;
		size_t end; // one past the last character (like substr)
	};

	inline std::vector<SSBOBlock> extractSSBOs(const std::string& src) {
		std::vector<SSBOBlock> result;
		size_t pos = 0;

		while ((pos = src.find("layout(", pos)) != std::string::npos) {
			size_t start = pos;

			// --- match layout(...) ---
			size_t i = pos + 7;
			int parenDepth = 1;

			while (i < src.size() && parenDepth > 0) {
				if (src[i] == '(') parenDepth++;
				else if (src[i] == ')') parenDepth--;
				i++;
			}
			if (parenDepth != 0) break;

			// skip whitespace
			size_t after = src.find_first_not_of(" \t\r\n", i);

			// must be "buffer"
			if (after == std::string::npos ||
				src.compare(after, 6, "buffer") != 0) {
				pos = i;
				continue;
				}

			// find '{'
			size_t braceStart = src.find('{', after);
			if (braceStart == std::string::npos) break;

			// --- match { ... } ---
			size_t j = braceStart + 1;
			int braceDepth = 1;

			while (j < src.size() && braceDepth > 0) {
				if (src[j] == '{') braceDepth++;
				else if (src[j] == '}') braceDepth--;
				j++;
			}
			if (braceDepth != 0) break;

			// find ';' after closing '}'
			size_t semicolon = src.find(';', j);
			if (semicolon == std::string::npos) break;

			size_t end = semicolon + 1;

			result.push_back({
				src.substr(start, end - start),
				start,
				end
			});

			pos = end;
		}

		return result;
	}

	inline std::string do_includes(const std::string& shader, std::map<fs::path, std::string>& open_shaders, const GMAKEConfig &config){
		std::istringstream stream(shader);
		std::string line;
		std::string rebuild;
		while (getline(stream, line)){
			std::string new_line;
			if (line.starts_with("#include")) {
				// Extract the filename from #include "filename" or #include <filename>
				size_t first_quote = line.find('"');
				size_t last_quote = line.rfind('"');

				// Handle both "filename" and <filename> formats
				if (first_quote == std::string::npos) {
					first_quote = line.find('<');
					last_quote = line.rfind('>');
				}

				if (first_quote != std::string::npos && last_quote != std::string::npos && first_quote != last_quote) {
					std::string include_path = line.substr(first_quote + 1, last_quote - first_quote - 1);
					fs::path shader_path(include_path);

					if (shader_path.is_absolute()){
						new_line = ReadFilePath(shader_path);
					}
					else{
						fs::path shader_path_comb = config.ProjectDir / shader_path;
						new_line = ReadFilePath(shader_path_comb);
					}
				}
			}
			else{
				new_line = line;
			}
			rebuild.append(new_line);
			rebuild.append("\n");  // Add newline back
		}

		if (rebuild.contains("#include")){
			rebuild = do_includes(rebuild, open_shaders, config);
		}
		return rebuild;
	}

    inline void include_run(const fs::path& shader_directory, const GMAKEConfig &config) {
	std::map<fs::path, std::string> open_shader_files;
	std::map<fs::path, std::string> open_include_files;

	fs::path new_dir = config.ProjectDir.parent_path() / "preprocessed_shaders";//preprocessed_shaders
	if (!fs::exists(new_dir)) {
		fs::create_directory(new_dir);
	}

	for (const std::pair<const std::string, std::vector<fs::path>>& shader : config.ShaderPrograms) {
		std::vector<fs::path> shaders = shader.second;
		for (const fs::path& file : shaders){
			fs::path actual_file_path;

			if (file.is_absolute()) {
				actual_file_path = file;
			} else {
				actual_file_path = config.ProjectDir / file;
			}

			std::string shader_content = ReadFilePath(actual_file_path);
			for (const fs::path& standard_path : config.StandardExtensions){
				std::string path_string = standard_path.string();
				std::string standard_file_path_include = "#include " + path_string;
				shader_content = insertLine(shader_content, 1, standard_file_path_include);
			}
			std::string included_shader = do_includes(shader_content, open_shader_files, config);
			std::vector<SSBOBlock> ssbo_blocks = extractSSBOs(included_shader);
				for ( SSBOBlock& ssbo_block : ssbo_blocks) {
					std::string ssbo_content = ssbo_block.text;
					std::string target = "binding";
					size_t pos = 0;
					pos = ssbo_content.find(target);
					uint64_t target_lenght = 7;
					ASSERT_MSG(pos != std::string::npos, "binding must be in the return of find ssbo this is a bug");
					size_t binding_pos = ssbo_content.find("binding");
					ASSERT_MSG(binding_pos != std::string::npos, "binding not found");
					size_t eq_pos = ssbo_content.find('=', binding_pos);
					ASSERT_MSG(eq_pos != std::string::npos, "binding missing '='");
					// find first non-space after '='
					size_t i = eq_pos + 1;
					while (i < ssbo_content.size() && std::isspace(static_cast<unsigned char>(ssbo_content[i]))) {
						i++;
					}
					if (i >= ssbo_content.size()) {
						continue;
					}
					// ✅ STOP if numeric binding
					if (std::isdigit(static_cast<unsigned char>(ssbo_content[i]))) {
						PRINT("Numeric binding found, skipping");
						continue;
					}
					// ✅ Parse symbolic binding
					if (std::isalpha(static_cast<unsigned char>(ssbo_content[i])) || static_cast<unsigned char>(ssbo_content[i]) == '_') {
						std::string header_name;
						while (i < ssbo_content.size() && std::isalpha(static_cast<unsigned char>(ssbo_content[i])) || static_cast<unsigned char>(ssbo_content[i]) == '_') {
							header_name += ssbo_content[i++];
						}
						PRINT("Header: " + header_name);
						if (i >= ssbo_content.size() || ssbo_content[i] != '.') {
							ExceptionHandler.error(4, "Expected '.' after header");
						}
						i++; // skip '.'
						std::string attribute;
						while (i < ssbo_content.size() && std::isalpha(static_cast<unsigned char>(ssbo_content[i])) || static_cast<unsigned char>(ssbo_content[i]) == '_') {
							attribute += ssbo_content[i++];
						}
						PRINT("Attribute: " + attribute);
						auto& mapping = config.SSBO_key_to_value.at(header_name);
						uint64_t value = mapping.at(attribute);
						std::string full_expr = header_name + "." + attribute;
						ssbo_content = replace_first(ssbo_content, full_expr, std::to_string(value));
						included_shader = replace_first(included_shader, ssbo_block.text, ssbo_content);
					}
				}
			if (config.Build == DebugType::DEBUG) {
				included_shader = insertLine(included_shader, 1, "#define DEBUG");
			} else if (config.Build == DebugType::NONE) {
				ExceptionHandler.error(4, "The build type is set to None this should never happen");
			}
			fs::path output_file = new_dir / file.filename();
			open_include_files[output_file] = included_shader;
			}
		}
		for (const std::pair<const fs::path, std::string> &write_file : open_include_files) {
			PRINT("Writing to: " << write_file.first);
			WriteFile(write_file.first, write_file.second);
		}
	}
}
