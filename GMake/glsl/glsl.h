#pragma once
#include <filesystem>
#include <vector>
#include <algorithm>
#include <array>
#include <map>
#include "../file_utils.h"
#include "Error.h"
#include <map>
#include "../../lib/vendor/cpp/fmt/include/fmt/chrono.h"

namespace glsl {
    namespace fs = std::filesystem;
    std::string ReadFilePath(const fs::path& path);



    inline int whore = 0;
    namespace fs = std::filesystem;

    enum class ParserError {
        UNEXPECTED_TOKEN,
    };

    std::string to_string(ParserError error);

    enum class TokenType {
        LEFT_BRACKET,
        RIGHT_BRACKET,
        SEMICOLON,
        IDENT,
        KEYWORD,
        LITERAL_TYPE,
        VERSION,
        LEFT_BRACE,
        RIGHT_BRACE,
        COMMA,
        DOT,
        LEFT_SQAURE_BRACKET,
        RIGHT_SQAURE_BRACKET,
        ASSIGN,
        STAR_ASSIGN,
        PLUS_ASSIGN,
        MINUS_ASSIGN,
        SLASH_ASSIGN,
        PLUS,
        MINUS,
        STAR,
        SLASH,
        PRECENT,
        EQUAL,
        GREATER,
        LESS,
        GREATER_EQUAL,
        LESS_EQUAL,
        NOT_EQUAL,
        NOT,
        OR,
        AND,
        BITWISE_AND,
        BITWISE_OR,
        BITWISE_XOR,
        BITWISE_LEFT_SHIFT,
        BITWISE_RIGHT_SHIFT,
        BITWISE_NOT,
        QUESTION_MARK,
        COLON,
        INCREMENT,
        DECREMENT,
        ARROW,
        DOUBLE_STAR,
        ERROR
    };

    enum class LiteralType {
        INT,
        FLOAT,
    };

    struct Token {
        TokenType type;
        std::string value;
        size_t line_number;
        std::pair<size_t, size_t> position_range;
        std::vector<LiteralType> literal_types;
    };

    struct Define {
        std::string name;
        std::vector<std::string> params;
        std::string body;
        size_t line_number;
    };

    struct Undef {
        std::string name;
        size_t line_number;
    };

    struct IfdefState {
        bool active;
        bool taken;
        size_t line_number;
    };

    enum class GLSLVersion : uint8_t {
        GLSL_110,
        GLSL_120,
        GLSL_130,
        GLSL_140,
        GLSL_150,
        GLSL_330,
        GLSL_400,
        GLSL_410,
        GLSL_420,
        GLSL_430,
        GLSL_440,
        GLSL_450,
        GLSL_460
    };

    enum class TokenErrorType {
        NONE,
        UNKNOWN
    };

    class Tokeniser {
    public:
        Tokeniser();
        void reset_tokeniser();
        [[nodiscard]] char peek_char(const int& look_ahead = 0) const;
        char consume_char();
        void push_token(const TokenType& type, const std::string& value, const std::pair<size_t, size_t>& char_in_line_pos, const std::vector<LiteralType>& literal_types = {}, const TokenErrorType& error_type = TokenErrorType::NONE);
        static bool is_alpha(char c);
        static bool is_num(char c);
        static bool is_whitespace(char c);
        Result<std::vector<Token>, ParserError> tokenize(const fs::path& text_in);
        std::string text;
        size_t char_pos;
        size_t line_number;
        size_t number_of_lines_processed;
        size_t pos_in_line;
        std::vector<Token> tokens;
        std::map<std::string, Define> defines;
        std::map<std::string, Undef> undefs;
        std::vector<IfdefState> ifdef_stack;
        bool tokenizing = true;
    };

    class Parser {
    public:
        Parser();
    };
}
