#include "GMakePythonParser.h"

const size_t INVALID = std::numeric_limits<size_t>::max();

bool keyword_check(const token& token, const KeyWord& keyword){
    if (token.type != TokenType::KEYWORD){
        return false;
    }
    if (std::get<KeyWord>(token.value) == keyword){
        return true;
    }
    return false;
}

void Parser::reset_parser(){
    Node_pos = 0;
    AST = {};
    IndentLevel = 0;
    Tokens = {};
    TokenPos = 0;
}

std::vector<Node> Parser::program_parse_ast(const std::vector<token>& tokens) {
    throw std::runtime_error("this is not a valid statement");
}

std::vector<Node> Parser::parse_code(){
    ProgramNode program = {.lines = {}};
    while (peek_token().type != TokenType::EMPTY){
        size_t ast_node = parse_statement();
        if (ast_node == INVALID){}
        program.lines.push_back(ast_node);
    }
    push_node(program);
    std::vector<Node> local_ast = AST;
    reset_parser();
    return local_ast;
}

size_t Parser::parse_statement() {
    throw std::runtime_error("this is not a valid statement");
}

size_t Parser::push_node(const Node& node){
    size_t node_index = AST.size();
    AST.push_back(node);
    return node_index;
}

token Parser::get_token(){
    token tok = peek_token();
    TokenPos++;
    return tok;
}

token Parser::peek_token(const int& look_ahead){
    token tok;
    if (TokenPos + look_ahead >= Tokens.size()){
        tok = {.type = TokenType::EMPTY, .value = ""};
    }
    else{
        tok = Tokens[TokenPos + look_ahead];
    }
    return tok;
}

bool Parser::match_token(const TokenType& type){
    return peek_token().type == type;
}

token Parser::expect_token(TokenType type){
    bool is_expected = match_token(type);
    if (!is_expected){
        throw std::runtime_error("this is not a valid token");
    }
    return get_token();
}