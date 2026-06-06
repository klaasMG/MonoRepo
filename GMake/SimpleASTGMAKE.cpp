#include "SimpleASTGMAKE.h"

namespace gmake {

ASTGMAKE::ASTGMAKE(const std::vector<Token> &input_tokens){
    tokens = input_tokens;
    currentToken = 0;
}

std::vector<Node> ASTGMAKE::getNodes(){
    std::vector<Node> nodes = {};
    ProgramNode program = ProgramNode{};
    while (currentToken < tokens.size()){
        Token token = getNextToken();
        if (token.type == TokenType::Identifier){
            auto node = FunctionNode();
            Token left_bracket = getNextToken();
            if (left_bracket.type != TokenType::LeftBracket){
                throw_ast_error("Expected '(' after function name");
            }
            bool func_end = false;
            while (!func_end){
                Token next_token = getNextToken();
                if (next_token.type == TokenType::Identifier || next_token.type == TokenType::Slash){
                    IdentNode ident_node;
                    ident_node.Ident = next_token.value;
                    bool ident_end = false;
                    while (!ident_end){
                        next_token = getNextToken();
                        if (next_token.type == TokenType::Comma){
                            ident_end = true;
                        }
                        else if (next_token.type == TokenType::RightBracket){
                            func_end = true;
                            ident_end = true;
                        }
                        else if (next_token.type == TokenType::Identifier){
                            ident_node.Ident += next_token.value;
                        }
                        else if (next_token.type == TokenType::Slash){
                            ident_node.Ident += "\\";
                        }
                        else{
                            throw_ast_error("Expected a 'identifier' or a ',' for end argument");
                        }
                    }
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
