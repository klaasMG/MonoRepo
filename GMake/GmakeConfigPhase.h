#pragma once
#include <iostream>
#include <minwindef.h>
#include <ranges>
#include <windef.h>
#include <winbase.h>

#include "assert_print.h"
#include "ExceptionHandler.h"
#include "GMakeTypes.h"
#include "LiteralTypes.h"
#include "SimpleASTGMAKE.h"
#include "string_utils.h"

namespace gmake {
	inline std::string run_command(const fs::path& cmd_path) {
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

	inline bool check_param_types(const std::vector<gmake::LiteralType>& allowed_arg_types, const std::vector<std::vector<gmake::LiteralType>>& given_arg_types) {
		std::cout << allowed_arg_types.size() << "function" << given_arg_types.size() << std::endl;
		bool continue_cond = false;
		if (allowed_arg_types.back() == gmake::LiteralType::CONTINUE) {
			continue_cond = true;
			PRINT("true is cond");
		}
		if (!continue_cond) {
			if (allowed_arg_types.size() != given_arg_types.size()) {
				return false;
			}
		} else {
			PRINT("hy there");
			if (allowed_arg_types.size() - 1 > given_arg_types.size()) {
				PRINT("hy here");
				return false;
			}
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
		if (continue_cond) {
			gmake::LiteralType literal_type = allowed_arg_types.back();
			for (size_t i = allowed_arg_types.size(); i < given_arg_types.size(); ++i) {
				if (!gmake::contains_on_vector(given_arg_types.at(i), literal_type)) {
					gmake::ExceptionHandler.error(87, "this is not a allowed type");
				}
			}
		}
		return result;
	};

    inline gmake::GMAKEConfig runGMAKEFunction(const std::string& function_name, const std::vector<gmake::LiteralNode>& function_args, gmake::GMAKEConfig config,
	const std::map<std::string, std::vector<gmake::LiteralType>>& functions_allowed_arg_types, const fs::path& current_dir) {
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
	    gmake::ExceptionHandler.error(34 , "this is not allowed");
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
		std::string min_version = function_args.at(0).Ident;
		std::string version = GMAKE_VERSION;
		std::erase(min_version, '.');
		std::erase(version, '.');
		uint64_t version_number = std::stoull(version);
		uint64_t min_version_number = std::stoull(min_version);
		if (min_version_number > version_number) {
			gmake::ExceptionHandler.error(56,"To low of a GMake version to run this script");
		}
		break;
	}

	case gmake::GMakeFunction::UNKNOWN:
		gmake::ExceptionHandler.error(1, "Function is not found" + function_name);
		break;
	}

	return config;
}
}
