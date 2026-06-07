#include "SimpleASTGMAKE.h"

#include <iostream>

namespace gmake {

ASTGMAKE::ASTGMAKE(const std::vector<Token> &input_tokens){
    tokens = input_tokens;
    currentToken = 0;
}

std::vector<Node> ASTGMAKE::getNodes(){
    std::vector<Node> nodes = {};
    ProgramNode program = ProgramNode{};
    int p = 8;
    while (currentToken < tokens.size()){
        p++;
        Token token = getNextToken();
        if (token.type == TokenType::LITERAL){
            std::vector<LiteralType> token_literal_types = token.literal_types;
            for (LiteralType literal_type : token_literal_types) {
                std::cout << static_cast<int>(literal_type) << std::endl;
            }
            if (token_literal_types.size() == 0) {
                std::cout << "well great" << std::endl;
            }
            std::cout << p << std::endl;
            if (!contains_on_vector(token_literal_types, LiteralType::NAME)) {
                throw std::runtime_error("Expected name for literal type");
            }
            FunctionNode node = FunctionNode();
            Token left_bracket = getNextToken();
            if (left_bracket.type != TokenType::LeftBracket){
                throw_ast_error("Expected '(' after function name");
            }
            bool func_end = false;
            while (!func_end){
                Token next_token = getNextToken();
                if (next_token.type == TokenType::LITERAL){
                    if (!contains_on_vector(next_token.literal_types, LiteralType::NAME) && !contains_on_vector(next_token.literal_types, LiteralType::VERSION)
                        && !contains_on_vector(next_token.literal_types, LiteralType::PATH)) {
                        throw_ast_error("not a valid argument type");
                    }
                    LiteralNode ident_node;
                    ident_node.Ident = next_token.value;
                    ident_node.LiteralTypes = next_token.literal_types;
                    size_t node_index = nodes.size();
                    nodes.push_back(ident_node);
                    node.ArgsNew.push_back(node_index);
                }
                else if (next_token.type == TokenType::RightBracket){
                    func_end = true;
                }
            }
            IdentNode identifier;
            identifier.Ident = token.value;
            node.Ident = identifier;
            size_t node_index = nodes.size();
            program.Nodes.push_back(node_index);
            nodes.push_back(std::move(node));
        }
        else if (token.type != TokenType::Semicolon){
            std::cout << token.value << std::endl;
            std::cout << static_cast<int>(token.type) << "this" << std::endl;
            std::cout << static_cast<int>(TokenType::LITERAL) << std::endl;
            std::cout << static_cast<int>(TokenType::None) << std::endl;
            std::cout << static_cast<int>(TokenType::Comma) << std::endl;
            std::cout << static_cast<int>(TokenType::LeftBracket) << "llk" << std::endl;
            std::cout << static_cast<int>(TokenType::RightBracket) << std::endl;
            std::cout << static_cast<int>(TokenType::Semicolon) << std::endl;

            throw_ast_error("Expected ';' after function name");
        }
    }
    nodes.push_back(program);
    return nodes;
}

Token ASTGMAKE::getNextToken(){
    if (currentToken < tokens.size()){
        Token token = tokens[currentToken];
        currentToken++;
        return token;
    }
    throw_ast_error("EOF error");
    return Token{};
}

void ASTGMAKE::throw_ast_error(const std::string& message){
    std::cerr << "ast error" << std::endl;
    throw std::runtime_error(message);
}

}
