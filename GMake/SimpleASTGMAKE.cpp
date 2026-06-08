#include "SimpleASTGMAKE.h"
#include <iostream>
#include "ExceptionHandler.h"

namespace gmake {

ASTGMAKE::ASTGMAKE(const std::vector<Token> &input_tokens){
    tokens = input_tokens;
    currentToken = 0;
}

std::vector<Node> ASTGMAKE::getNodes(){
    bool is_incorrect = false;
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
            if (token_literal_types.empty()) {
                std::cout << "well great" << std::endl;
            }
            if (!contains_on_vector(token_literal_types, LiteralType::NAME)) {
                std::cout << "Expected name for literal type" << std::endl;
                std::vector<Token> error_tokens = {token};
                ErrorNode error_node = ErrorNode{};
                is_incorrect = true;
                bool is_needed = true;
                while (is_needed) {
                    Token tok = getNextToken();
                    if (tok.type == TokenType::LITERAL && contains_on_vector(tok.literal_types, LiteralType::NAME)) {
                        is_needed = false;
                        error_node.Tokens = error_tokens;
                        nodes.push_back(error_node);
                        token = tok;
                    }
                    error_tokens.push_back(tok);
                }
            }
            FunctionNode node = FunctionNode();
            std::vector<Token> node_tokens = {};
            Token left_bracket = getNextToken();
            if (left_bracket.type != TokenType::LeftBracket){
                std::cout << "Expected '(' after function name" << std::endl;
                std::vector<Token> error_tokens = {left_bracket};
                ErrorNode error_node = ErrorNode{};
                is_incorrect = true;
                bool is_needed = true;
                while (is_needed) {
                    Token tok = getNextToken();
                    if (tok.type == TokenType::LeftBracket) {
                        is_needed = false;
                        error_node.Tokens = error_tokens;
                        nodes.push_back(error_node);
                    }
                    error_tokens.push_back(tok);
                }
            }
            node_tokens.push_back(left_bracket);
            bool func_end = false;
            while (!func_end){
                Token next_token = getNextToken();
                if (next_token.type == TokenType::LITERAL){
                    if (!contains_on_vector(next_token.literal_types, LiteralType::NAME) && !contains_on_vector(next_token.literal_types, LiteralType::VERSION)
                        && !contains_on_vector(next_token.literal_types, LiteralType::PATH)) {
                        std::cout << "not a valid argument type" << std::endl;
                        std::vector<Token> error_tokens = {next_token};
                        ErrorNode error_node = ErrorNode{};
                        is_incorrect = true;
                        bool is_needed = true;
                        while (is_needed) {
                            Token tok = getNextToken();
                            if (tok.type == TokenType::LITERAL) {
                                if (contains_on_vector(tok.literal_types, LiteralType::NAME) || contains_on_vector(tok.literal_types, LiteralType::VERSION)
                                    || contains_on_vector(tok.literal_types, LiteralType::PATH)) {
                                    is_needed = false;
                                    error_node.Tokens = error_tokens;
                                    nodes.push_back(error_node);
                                    next_token = tok;
                                }
                            }
                            error_tokens.push_back(tok);
                        }
                    }
                    LiteralNode ident_node;
                    ident_node.Tokens = {next_token};
                    ident_node.Ident = next_token.value;
                    ident_node.LiteralTypes = next_token.literal_types;
                    size_t node_index = nodes.size();
                    nodes.push_back(ident_node);
                    node.ArgsNew.push_back(node_index);
                }
                else if (next_token.type == TokenType::RightBracket){
                    func_end = true;
                    node_tokens.push_back(next_token);
                }
            }
            IdentNode identifier;
            identifier.Ident = token.value;
            identifier.Tokens = {token};
            node.Ident = identifier;
            node.Tokens = node_tokens;
            size_t node_index = nodes.size();
            program.Tokens = {};
            program.Nodes.push_back(node_index);
            nodes.push_back(std::move(node));
        }
        else if (token.type != TokenType::Semicolon){
            std::cout << "Expected ';' after function name" << std::endl;
            std::vector<Token> error_tokens = {token};
            ErrorNode error_node = ErrorNode{};
            is_incorrect = true;
            bool is_needed = true;
            while (is_needed) {
                Token tok = getNextToken();
                if (tok.type == TokenType::Semicolon) {
                    is_needed = false;
                    error_node.Tokens = error_tokens;
                    nodes.push_back(error_node);
                }
                error_tokens.push_back(tok);
            }
        }
    }
    nodes.push_back(program);
    if (is_incorrect) {
        throw std::runtime_error("Parser did fail");
    }
    return nodes;
}

Token ASTGMAKE::getNextToken(){
    if (currentToken < tokens.size()){
        Token token = tokens[currentToken];
        currentToken++;
        return token;
    }
    throw std::runtime_error("AOF errpr");
    return Token{};
}

}
