#ifndef EVENT_STRUCT_SIMPLEASTGMAKE_H
#define EVENT_STRUCT_SIMPLEASTGMAKE_H

#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>
#include "Tokens.h"
#include <filesystem>
#include <iostream>

namespace gmake {
    struct IdentNode {
        std::string Ident;
    };
    struct FunctionNode {
        IdentNode Ident;
        std::vector<IdentNode> Args;
        std::vector<size_t> ArgsNew;
    };

    struct ProgramNode {
        std::vector<size_t> Nodes;
    };
    using Node = std::variant<IdentNode, FunctionNode, ProgramNode>;

    class ASTGMAKE {
        std::vector<Token> tokens;
        int currentToken;

    public:
        ASTGMAKE(const std::vector<Token>& input_tokens);

        std::vector<Node> getNodes();

    private:
        Token getNextToken();
        static void throw_ast_error(const std::string& message);
    };
}

#endif
