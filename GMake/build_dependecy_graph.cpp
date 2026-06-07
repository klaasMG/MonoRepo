#include "TokeniserGMAKE.h"
#include "SimpleASTGMAKE.h"
#include "assert_print.h"
#include "GMAKE_EXCEPTION.h"
#include "GMakeTypes.h"
#include "file_utils.h"
#include "string_utils.h"
#include "LiteralTypes.h"
#include "GmakeFunctionParser.h"
#include "ExceptionHandler.h"
#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <memory>
#include <ranges>
#include <unordered_set>
#include <windows.h>
#define GMAKE_VERSION "1.0.0"
namespace fs = std::filesystem;

fs::path current_dir;

std::unordered_set<std::string_view> allowed_flags = {"-debug",};

std::string run_command(const fs::path& cmd_path) {
    // mutable command buffer
    std::string cmd = cmd_path.string();
    std::vector<char> cmd_buf(cmd.begin(), cmd.end());
    cmd_buf.push_back('\0');

    // pipe
    HANDLE readPipe = NULL, writePipe = NULL;
    SECURITY_ATTRIBUTES sa{};
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = TRUE;

    CreatePipe(&readPipe, &writePipe, &sa, 0);

    // make sure read end is NOT inherited
    SetHandleInformation(readPipe, HANDLE_FLAG_INHERIT, 0);

    // startup info
    STARTUPINFOA si{};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdOutput = writePipe;
    si.hStdError  = writePipe;
    si.hStdInput  = NULL;

    PROCESS_INFORMATION pi{};

    // create process
    if (!CreateProcessA(
        NULL,
        cmd_buf.data(),
        NULL, NULL,
        TRUE,
        0,
        NULL, NULL,
        &si, &pi
    )) {
        CloseHandle(readPipe);
        CloseHandle(writePipe);
        return "";
    }

    // parent doesn't need write end
    CloseHandle(writePipe);

    // read output
    std::string output;
    char buffer[4096];
    DWORD bytesRead;

    while (true) {
        BOOL success = ReadFile(readPipe, buffer, sizeof(buffer), &bytesRead, NULL);
        if (!success || bytesRead == 0) break;
        output.append(buffer, bytesRead);
    }

    // wait for process
    WaitForSingleObject(pi.hProcess, INFINITE);

    // cleanup
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    CloseHandle(readPipe);

    return output;
}

bool check_param_types(const std::vector<gmake::LiteralType>& allowed_arg_types, const std::vector<std::vector<gmake::LiteralType>>& given_arg_types) {
	std::cout << allowed_arg_types.size() << "function" << given_arg_types.size();

	if (allowed_arg_types.size() != given_arg_types.size()) {
		return false;
	}
	bool result = true;
	for (int i = 0; i < allowed_arg_types.size(); ++i) {
		gmake::LiteralType allowed_arg_type = allowed_arg_types.at(i);
		const std::vector<gmake::LiteralType>& given_arg_type = given_arg_types.at(i);
		bool contains = gmake::contains_on_vector(given_arg_type, allowed_arg_type);
		if (contains) {
			return result;
		}
		std::cout << result << std::endl;
	}
	return result;
};

gmake::GMAKEConfig runGMAKEFunction(const std::string& function_name, const std::vector<gmake::LiteralNode>& function_args, gmake::GMAKEConfig config,
	const std::map<std::string, std::vector<gmake::LiteralType>>& functions_allowed_arg_types) {
	PRINT(function_name);
	int r = 0;
	for (const gmake::LiteralNode& function_arg : function_args) {
		std::cout << function_arg.Ident << std::endl;
		std::cout << r << std::endl;
		r++;
	}
	gmake::ExceptionHandler.add_to_call_stack(function_name);
	gmake::GMakeFunction func_name = gmake::parseFunction(function_name);
	std::vector<std::vector<gmake::LiteralType>> literal_type_vector = {};
    for (const gmake::LiteralNode& function_arg : function_args) {
	    literal_type_vector.push_back(function_arg.LiteralTypes);
    }
	bool args_allowed = check_param_types(functions_allowed_arg_types.at(function_name), literal_type_vector);
	PRINT("hk");
    if (!args_allowed) {
	    throw std::runtime_error("this is not allowed");
    }
	switch (func_name) {
	case gmake::GMakeFunction::SET_PROJECT_DIRECTORY: {
			fs::path project_dir = function_args.at(0).Ident;
			if (project_dir.is_absolute()) {
				config.ProjectDir = project_dir;
			}
			else {
				config.ProjectDir = (current_dir / project_dir);
			}
			break;
	}
	case gmake::GMakeFunction::SET_PROGRAM: {
			const std::string& shader_program = function_args.at(0).Ident;
			std::vector<fs::path> shaders;
			for (const gmake::LiteralNode& arg : function_args | std::views::drop(1)) {
				fs::path path_arg = arg.Ident;
				shaders.emplace_back(path_arg);
			}
			config.ShaderPrograms[shader_program] = shaders;
			break;
	}

	case gmake::GMakeFunction::EXTEND_STANDARD:{
	    for (const gmake::LiteralNode& arg : function_args){
	        config.StandardExtensions.emplace_back(arg.Ident);
	    }
	    break;
	}

	case gmake::GMakeFunction::SSBO_LAYOUT_BINDING:{
	    PRINT(function_args.size());
        if (function_args.empty()){
	        gmake::ExceptionHandler.error(2,"No program given");
        }
        const std::string& program_name = function_args.at(0).Ident;

        std::cerr << "SSBO layout binding: " << std::to_string(program_name.size()) << std::endl;
        PRINT(program_name);
        fs::path path_program_build_ssbo_layout = config.ProjectDir / program_name;
	    std::string output = run_command(path_program_build_ssbo_layout);

	    PRINT("this is the output:" + output);

	    std::istringstream stream(output);
	    std::string line;
	    std::map<std::string, std::map<std::string, uint64_t>> mappings;
	    bool has_pending = false;
	    while (true) {
	        if (!has_pending) {
	            if (!std::getline(stream, line)){break;}
	        }
	        else {
	            has_pending = false;
	        }
	        line = trim(line);
	        std::pair<std::string, std::string> key_value = split_once(line, ':');

	        if (key_value.first == "header" && !isdigit(key_value.second[0])) {
	            std::string key_to_mapping = key_value.second;
	            std::map<std::string, uint64_t> mapping;

	            while (std::getline(stream, line)) {
	                line = trim(line);
	                std::pair<std::string, std::string> kv = split_once(line, ':');

	                if (kv.first == "header" && !isdigit(kv.second[0])) {
	                    has_pending = true; // reuse this line in outer loop
	                    break;
	                }
	                std::string key = kv.first;
	                key = trim(key);
	                uint64_t value = std::stoull(kv.second);
                    if (mapping.contains(key)){
	                    gmake::ExceptionHandler.error(2,"Key already exists");
                    }
	                mapping.insert_or_assign(key, value);
	            }
	            mappings.insert_or_assign(key_to_mapping, mapping);
	        }
	    }
	    config.SSBO_key_to_value = mappings;
	    break;
	}

	case gmake::GMakeFunction::SET_MINIMAL_VERSION: {
		throw std::runtime_error("not yet");
		break;
	}

	case gmake::GMakeFunction::UNKNOWN:
		gmake::ExceptionHandler.error(1, "Function is not found" + function_name);
		break;
	}

	return config;
}

std::vector<gmake::Node> build_ast(const std::string& gmake_file){
	gmake::TokeniserGMAKE tokeniser(gmake_file);
	std::vector<gmake::Token> tokens = tokeniser.Tokenise();
	gmake::ASTGMAKE ast_builder(tokens);
	std::vector<gmake::Node> nodes = ast_builder.getNodes();
	return nodes;
}

struct SSBOBlock {
	std::string text;
	size_t start;
	size_t end; // one past the last character (like substr)
};

std::vector<SSBOBlock> extractSSBOs(const std::string& src) {
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

std::string do_includes(const std::string& shader, std::map<fs::path, std::string>& open_shaders, const gmake::GMAKEConfig &config){
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
					new_line = gmake::ReadFilePath(shader_path);
				}
				else{
					fs::path shader_path_comb = config.ProjectDir / shader_path;
					new_line = gmake::ReadFilePath(shader_path_comb);
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

void include_run(const fs::path& shader_directory, const gmake::GMAKEConfig &config) {
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

			std::string shader_content = gmake::ReadFilePath(actual_file_path);
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
						gmake::ExceptionHandler.error(4, "Expected '.' after header");
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
			fs::path output_file = new_dir / file.filename();
			open_include_files[output_file] = included_shader;
		}

		for (const std::pair<const fs::path, std::string> &write_file : open_include_files) {
			PRINT("Writing to: " << write_file.first);
			gmake::WriteFile(write_file.first, write_file.second);
		}
	}
}

fs::path get_exe_dir() {
	char buffer[MAX_PATH];
	GetModuleFileNameA(nullptr, buffer, MAX_PATH);
	return std::filesystem::path(buffer).parent_path();
}

int main(int argc, char* argv[]) {
	if (argc >= 2){
		fs::path tool_dir = get_exe_dir();
		tool_dir = fs::absolute(tool_dir);
	    current_dir = fs::current_path();
	    std::cout << current_dir << std::endl;
	    char* gmake_file_path = argv[1];
	    std::string gmake_file = gmake::readFile(gmake_file_path);
	    std::vector<gmake::Node> nodes = build_ast(gmake_file);
	    gmake::GMAKEConfig config = gmake::GMAKEConfig();
	    std::vector<std::string> flags;
	    for (int i = 2; i < argc; i++){
	        const std::string& arg = argv[i];
	        if (arg == "-debug"){
	            config.debug = true;
	        }
	        else if (!allowed_flags.contains(arg)){
	            std::string error_message = "This flag: " + arg + " is not allowed\n" + "Do you wish to proceed?(Y/N)";
	            std::cout << error_message << std::endl;
	            std::string continue_program;
	            std::cin >> continue_program;
	            continue_program = toLower(continue_program);
	            bool is_solved = false;
	            while (!is_solved){
	                if (continue_program == "y"){
	                    is_solved = true;
	                }
	                else if (continue_program == "n"){
	                    const int& exit_code = 1;
	                    is_solved = true;
	                    std::exit(exit_code);
	                }
	            }
	            flags.push_back(arg);
	        }
	    }
	    if (config.debug){
		    gmake::ExceptionHandler.set_debug(true);
	    }
		std::map<std::string, std::vector<gmake::LiteralType>> functions_allowed_arg_types = gmake::function_parameters_generator(tool_dir);
	    gmake::Node program_node_maybe = nodes.at(nodes.size() - 1);
	    gmake::ProgramNode program = std::get<gmake::ProgramNode>(program_node_maybe);
        for (const size_t& function_node : program.Nodes){
            gmake::Node function_node_maybe = nodes.at(function_node);
            gmake::FunctionNode function = std::get<gmake::FunctionNode>(function_node_maybe);
            std::vector<size_t> ident_node_pos = function.ArgsNew;
            std::vector<gmake::LiteralNode> function_args = {};
            for (const size_t& node_pos : ident_node_pos){
                gmake::LiteralNode ident_node = std::get<gmake::LiteralNode>(nodes.at(node_pos));
                function_args.push_back(ident_node);
            }
            std::string function_name = function.Ident.Ident;
            config = runGMAKEFunction(function_name, function_args, config, functions_allowed_arg_types);
        }
	    std::cout << config.ProjectDir << std::endl;
	    include_run("path", config);
	    //ssbo_layout_bindings();
	}
	else{
		std::cout << "wrong number of arguments" << std::endl;
	}

	return 0;
}