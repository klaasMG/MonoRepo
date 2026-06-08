#include "GmakeImplementationPhase.h"
#include "TokeniserGMAKE.h"
#include "SimpleASTGMAKE.h"
#include "GMAKE_EXCEPTION.h"
#include "GMakeTypes.h"
#include "file_utils.h"
#include "string_utils.h"
#include "LiteralTypes.h"
#include "GmakeFunctionParser.h"
#include "ExceptionHandler.h"
#include "glsl/glsl.h"
#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <memory>
#include <ranges>
#include <unordered_set>
#include <windows.h>
#define GMAKE_VERSION "0.1.0.0"
#include "GmakeConfigPhase.h"
namespace fs = std::filesystem;

fs::path current_dir;

std::unordered_set<std::string_view> allowed_flags = {"-debug", "-warn"};

std::vector<gmake::Node> build_ast(const std::string& gmake_file){
	gmake::TokeniserGMAKE tokeniser(gmake_file);
	std::vector<gmake::Token> tokens = tokeniser.Tokenise();
	gmake::ASTGMAKE ast_builder(tokens);
	std::vector<gmake::Node> nodes = ast_builder.getNodes();
	return nodes;
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
	        } else if (arg.starts_with("MODE")) {
	        	std::string arg_copy = arg;
	        	arg_copy = arg_copy.substr(4);
		        if (!arg_copy.starts_with('=')) {
			        gmake::ExceptionHandler.error(89,"Mode is a var and so must be given");
		        }
	        	arg_copy = arg_copy.substr(1);
		        if (arg_copy == "debug") {
		        	config.Build = gmake::DebugType::DEBUG;
		        } else if (arg_copy == "release") {
			        config.Build = gmake::DebugType::BUILD;
		        } else {
			        gmake::ExceptionHandler.error(90,"No valid build type is given");
		        }
	        } else if (arg == "-warn") {
	        	//GLSL_PARSER.WARN()
		        gmake::ExceptionHandler.error(7, "-warn is not yet supported");
	        }else if (!allowed_flags.contains(arg)){
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
            config = runGMAKEFunction(function_name, function_args, config, functions_allowed_arg_types, current_dir);
        }
	    std::cout << config.ProjectDir << std::endl;
		if (config.Build == gmake::DebugType::NONE) {
			config.Build = gmake::DebugType::BUILD;
		}
	    gmake::include_run("path", config);
	    //ssbo_layout_bindings();
	}
	else{
		std::cout << "wrong number of arguments" << std::endl;
	}

	return 0;
}