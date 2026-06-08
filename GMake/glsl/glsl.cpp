#include "glsl.h"

namespace glsl {
    Tokeniser::Tokeniser() {
        text = "";
        CharPos = 0;
    }

    void Tokeniser::reset_tokeniser() {
        text = "";
        CharPos = 0;
    }

    char Tokeniser::peek_char() {
        if (CharPos > text.size()) {
            return '\0';
        }
        char c = text.at(CharPos);
        return c;
    }

    char Tokeniser::consume_char() {
        char c = peek_char();
        CharPos++;
        return c;
    }

    std::vector<Token> Tokeniser::tokenize(const fs::path& source_file) {
        std::vector<Token> tokens;
        throw std::runtime_error("this is not implemented yet");
        reset_tokeniser();
        return tokens;
    }
}
