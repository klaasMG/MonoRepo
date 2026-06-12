#include "TokeniserGMAKE.h"

#include "ExceptionHandler.h"

namespace gmake {
    TokeniserGMAKE::TokeniserGMAKE(const std::string& input) {
        file = input;
        TokenPos = 0;
        none_symbols = {TokenType::None, TokenType::LITERAL};
        symbols = getOtherSymbols(none_symbols);
    }

    std::vector<Token> TokeniserGMAKE::Tokenise() {
        std::vector<Token> tokens;
        while (TokenPos<file.length()) {
            Token token = Token{.type = TokenType::None, .value = "", .literal_types = {LiteralType::NAME}};
            if (is_alphabet() || '/' == peek_char()) {
                std::string ident = "";
                char c = consume_char();
                ident.push_back(c);
                bool path = false;
                while (is_alphabet() || peek_char() == '/') {
                    c = consume_char();
                    if (c == '/') {
                        path = true;
                    }
                    ident.push_back(c);
                }
                if (!path) {
                    token = Token(TokenType::LITERAL, ident, {LiteralType::NAME, LiteralType::PATH});
                } else {
                    token = Token(TokenType::LITERAL, ident, {LiteralType::PATH});
                }
                std::cout << ident << std::endl;
            }
            else if (is_digit()) {
                std::string num = "";
                char c = consume_char();
                num.push_back(c);
                while (is_digit()) {
                    c = consume_char();
                    num.push_back(c);
                }
                if (peek_char() != '.') {
                    token = Token(TokenType::LITERAL, num, {LiteralType::NUMBER});
                    tokens.push_back(token);
                    continue;
                }
                char dot = consume_char();
                num.push_back(dot);
                while (is_digit()) {
                    c = consume_char();
                    num.push_back(c);
                }
                if (peek_char() != '.') {
                    ExceptionHandler.error(34 , "no float support yet");
                }
                char dot2 = consume_char();
                num.push_back(dot2);
                while (is_digit()) {
                    c = consume_char();
                    num.push_back(c);
                }
                if (peek_char() != '.') {
                    ExceptionHandler.error(34 , "unknown type");
                }
                char dot3 = consume_char();
                num.push_back(dot3);
                while (is_digit()) {
                    c = consume_char();
                    num.push_back(c);
                }
                token = Token{.type = TokenType::LITERAL, .value = num, .literal_types = {LiteralType::VERSION}};
            }
            else {
                TokenType t = get_token_type();
                char c = consume_char();
                if (t != TokenType::None) {
                    token = Token(t, "");
                }
            }
            if (token.type != TokenType::None) {
                tokens.push_back(token);
            }
        }
        return tokens;
    }

    char TokeniserGMAKE::consume_char() {
        char c = peek_char();
        TokenPos++;
        return c;
    }

    char TokeniserGMAKE::peek_char() {
        if (TokenPos <= file.length()) {
            return file[TokenPos];
        }
        return '\0';
    }

    bool TokeniserGMAKE::is_alphabet() {
        char c = peek_char();
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '.') || (c == '_');
    }

    bool TokeniserGMAKE::is_digit() {
        char c = peek_char();
        return (c >= '0' && c <= '9');
    }

    TokenType TokeniserGMAKE::get_token_type() {
        char c = peek_char();
        for (const auto& symbol : symbols) {
            char value_symbol = static_cast<char>(symbol);
            if (value_symbol == c) {
                return symbol;
            }
        }
        return TokenType::None;
    }

    std::vector<TokenType> TokeniserGMAKE::getOtherSymbols(const std::vector<TokenType>& exclude) {
        std::vector<TokenType> all = {
            TokenType::LeftBracket, TokenType::RightBracket, TokenType::Comma, TokenType::Semicolon, TokenType::None, TokenType::LITERAL,
        };
        std::vector<TokenType> result;
        for (const auto& token : all) {
            if (std::find(exclude.begin(), exclude.end(), token) == exclude.end()) {
                result.push_back(token);
            }
        }
        return result;
    }
}
