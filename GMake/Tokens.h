#ifndef EVENT_STRUCT_TOKENS_H
#define EVENT_STRUCT_TOKENS_H
#include <string>
#include <vector>
#include "LiteralTypes.h"

namespace gmake {
    enum class TokenType : char{
        LITERAL = 1,
        LeftBracket = '(',
        RightBracket = ')',
        Comma = ',',
        Semicolon = ';',
        None = '\0'
    };

    struct Token{
        TokenType type;
        std::string value;
        std::vector<LiteralType> literal_types;
    };
}

#endif //EVENT_STRUCT_TOKENS_H