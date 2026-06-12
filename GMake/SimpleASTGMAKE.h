#ifndef EVENT_STRUCT_SIMPLEASTGMAKE_H
#define EVENT_STRUCT_SIMPLEASTGMAKE_H

#include <string>
#include <variant>
#include <vector>
#include "Tokens.h"
#include "LiteralTypes.h"
#include <filesystem>

namespace gmake {

    template <typename T>
    bool contains_on_vector(const std::vector<T>& vector, const T& element) {
        bool found = false;
        for (const T& element_vec : vector) {
            if (element_vec == element) {
                found = true;
            }
        }
        return found;
    }

    struct LiteralNode {
        std::string Ident;
        std::vector<LiteralType> LiteralTypes;
        std::vector<Token> Tokens;
    };

    struct IdentNode {
        std::string Ident;
        std::vector<Token> Tokens;
    };

    struct FunctionNode {
        IdentNode Ident;
        std::vector<IdentNode> Args;
        std::vector<size_t> ArgsNew;
        std::vector<Token> Tokens;
    };

    struct ErrorNode {
        std::vector<Token> Tokens;
    };

    struct ProgramNode {
        std::vector<size_t> Nodes;
        std::vector<Token> Tokens;
    };
    using Node = std::variant<IdentNode, FunctionNode, ProgramNode, LiteralNode, ErrorNode>;

    class ASTGMAKE {
        std::vector<Token> tokens;
        int currentToken;

    public:
        ASTGMAKE(const std::vector<Token>& input_tokens);

        std::vector<Node> getNodes();

    private:
        Token getNextToken();
    };
}

#endif
