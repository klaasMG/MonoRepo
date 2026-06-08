#pragma once
#include <filesystem>
#include <vector>

namespace glsl {
    namespace fs = std::filesystem;

    enum class TokenType {
        LEFT_BRACKET,
        RIGHT_BRACKET,
    };
    struct Token {
        TokenType type;
        std::string value;
        size_t line_number;
        std::pair<size_t, size_t> position_range;
    };

    class Tokeniser {
    public:
        Tokeniser();
        void reset_tokeniser();
        char peek_char();
        char consume_char();
        std::vector<Token> tokenize(const fs::path& source_file);
        std::string text;
        size_t CharPos;
    };
}
