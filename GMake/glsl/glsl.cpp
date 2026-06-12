#include "glsl.h"

namespace glsl {

    constexpr std::array<std::string, 43> key_words = { "attribute", "const", "uniform", "varying", "buffer", "shared", "coherent", "volatile", "restrict",
        "readonly", "writeonly", "precision", "highp", "mediump", "lowp", "layout", "centroid", "flat", "smooth", "noperspective", "patch",
        "sample", "invariant", "precise", "break", "continue", "do", "for", "while", "switch", "case", "default", "if", "else", "subroutine",
        "discard", "return", "in", "out", "inout", "true", "false", "struct" };
    constexpr std::array<std::string, 12> preprocessor_commands = {"version","define", "undef","ifdef","ifndef", "include","else","endif","extension","error","line","pragma"};

    template <typename T,size_t N>
    bool contains_on_array(const std::array<T, N>& vector, const T& element) {
        bool found = false;
        for (const T& element_vec : vector) {
            if (element_vec == element) {
                found = true;
            }
        }
        return found;
    }

    std::string remove_backslashes(const std::string& input) {
        std::string result;
        result.reserve(input.size());

        for (char c : input) {
            if (c != '\\') {
                result.push_back(c);
            }
        }

        return result;
    }

    std::string replace_token(std::string text, const std::string& find, const std::string& replace) {
        size_t pos = 0;

        while ((pos = text.find(find, pos)) != std::string::npos) {
            bool left_ok = pos == 0 || !std::isalnum(static_cast<unsigned char>(text[pos - 1]));
            bool right_ok = pos + find.size() >= text.size() ||
                            !std::isalnum(static_cast<unsigned char>(text[pos + find.size()]));

            if (left_ok && right_ok) {
                text.replace(pos, find.size(), replace);
                pos += replace.size();
            } else {
                pos += find.size();
            }
        }

        return text;
    }

    char continue_token_type_to_char(TokenType type) {
        switch (type) {
        case TokenType::PLUS: return '+';
        case TokenType::MINUS: return '-';
        case TokenType::STAR: return '*';
        case TokenType::SLASH: return '/';
        case TokenType::ASSIGN: return '=';
        case TokenType::NOT: return '!';
        case TokenType::LESS: return '<';
        case TokenType::GREATER: return '>';
        case TokenType::BITWISE_AND: return '&';
        case TokenType::BITWISE_OR: return '|';
        default: return '\0';
        }
    }

    std::string ReadFilePath(const fs::path& path) {
        std::ifstream file(path, std::ios::binary);
        if (!file) return "";

        file.seekg(0, std::ios::end);
        size_t size = file.tellg();
        file.seekg(0);

        std::string data(size, '\0');
        file.read(data.data(), size);

        return data;
    }

    Tokeniser::Tokeniser() {
        number_of_lines_processed = 0;
        text = "";
        char_pos = 0;
        tokens = {};
        line_number = 0;
        pos_in_line = 0;
    }

    Result<std::vector<Token>, ParserError> Tokeniser::tokenize(const fs::path& text_in) {
        text = remove_backslashes(ReadFilePath(text_in));
        bool is_unknown_char = false;
        std::string unknown_chars;
        while (char_pos < text.size()) {
            char c = consume_char();
            if (c == ' ' || c == '\t' || c == '\r') {
                continue;
            }
            if (!tokenizing) {
                if ((c == '\n' && peek_char() == '#') || c == '#') {
                } else {
                    continue;
                }
            }
            if (is_unknown_char){
                push_token(TokenType::ERROR, unknown_chars, {pos_in_line, pos_in_line});
                is_unknown_char = false;
                unknown_chars.clear();
            }
            if (is_alpha(c) || c == '_') {
                size_t pos_one = pos_in_line;
                std::string ident;
                ident += c;
                while (peek_char() == '_' || isalpha(peek_char()) || is_num(peek_char())) {
                    char c1 = consume_char();
                    ident.push_back(c1);
                }
                size_t pos_two = pos_in_line;
                bool is_keyword = false;
                for (const std::string& key_word : key_words) {
                    if (key_word == ident) {
                        is_keyword = true;
                    }
                }
                if (is_keyword) {
                    push_token(TokenType::KEYWORD, ident, {pos_one, pos_two});
                }
                else {
                    bool is_define = false;
                    if (defines.contains(ident)){
                        Define define = defines.at(ident);
                        size_t def_line_number = define.line_number;
                        size_t undef_line_number = std::numeric_limits<size_t>::max();
                        if (undefs.contains(ident)){
                            Undef undef = undefs.at(ident);
                            undef_line_number = undef.line_number;
                        }
                        if ((def_line_number < number_of_lines_processed) && (number_of_lines_processed < undef_line_number)){
                            is_define = true;
                        }
                        constexpr std::array<TokenType, 10> use_last_token = {TokenType::PLUS, TokenType::MINUS, TokenType::STAR, TokenType::SLASH, TokenType::ASSIGN, TokenType::NOT, TokenType::LESS,TokenType::GREATER,
                            TokenType::BITWISE_AND, TokenType::BITWISE_OR};
                        Token token_last = tokens.back();
                        bool is_check = contains_on_array(use_last_token, token_last.type);
                        std::string define_body = define.body;
                        std::vector<std::string> param_input = {};
                        if (peek_char() == '('){
                            consume_char();
                            std::string param;
                            while (peek_char() != ')'){
                                char c_new = consume_char();
                                if (c_new == ' ' || c_new == '\t' || c_new == '\r'){
                                    continue;
                                }
                                else if (c_new != ','){
                                    param_input.push_back(param);
                                    param.clear();
                                }
                                else{
                                    param.push_back(c_new);
                                }
                            }
                        }
                        if (define.params.size() != param_input.size()){
                            throw std::runtime_error("not enough off them bitches");
                        }
                        for (size_t i = 0; i < param_input.size(); i++){
                            const std::string& param_replace = param_input.at(i);
                            const std::string& param = define.params.at(i);
                            define_body = replace_token(define_body, param, param_replace);
                        }
                        if (is_check){
                            TokenType token_type_check = token_last.type;
                            char token_value = continue_token_type_to_char(token_type_check);
                            define_body = fmt::format("{}{}", token_value, define_body);
                            tokens.pop_back();
                        }
                        text.erase(0, char_pos);
                        text = fmt::format("{}{}", define_body, text);
                        char_pos = 0;
                    }
                    if (!is_define){
                        push_token(TokenType::IDENT, ident, {pos_one, pos_two});
                    }
                }
            }
            else if (is_num(c) || (c == '.' && is_num(peek_char(1)))) {
                size_t pos_one = pos_in_line;
                std::string number;
                number += c;
                bool is_dot_seen = false;
                if (c == '.'){
                    is_dot_seen = true;
                    number.push_back('.');
                }
                else{
                    is_dot_seen = false;
                }
                while (is_num(peek_char()) || peek_char() == '.') {
                    char c1 = consume_char();
                    if (c1 == '.'){
                        if (!is_dot_seen){
                            is_dot_seen = true;
                        }
                        else{
                            throw std::runtime_error("saw dot twice in float");
                        }
                    }
                    number.push_back(c1);
                }
                size_t pos_two = pos_in_line;
                if (!is_dot_seen){
                    push_token(TokenType::LITERAL_TYPE, number, {pos_one, pos_two}, {LiteralType::INT});
                }
                else{
                    push_token(TokenType::LITERAL_TYPE, number, {pos_one, pos_two}, {LiteralType::FLOAT});
                }
            }
            else if (c == '\n' || c == '#'){
                if (c == '#'){
                    std::string preprocessor_directive;
                    preprocessor_directive += c;
                    while (peek_char() != '\n'){
                        char preprocessor_char = consume_char();
                        preprocessor_directive += preprocessor_char;
                    }
                    size_t space_pos = preprocessor_directive.find_first_of(" \t");
                    std::string command = preprocessor_directive.substr(0, space_pos);
                    std::string extra;
                    if (space_pos != std::string::npos) {
                        extra = preprocessor_directive.substr(space_pos + 1);
                    }
                    if (command == "#version") {
                        size_t i = 0;
                        while (i < extra.size() && (extra[i] == ' ' || extra[i] == '\t')) i++;
                        std::string version;
                        while (i < extra.size() && extra[i] != ' ' && extra[i] != '\t') {
                            version.push_back(extra[i]);
                            i++;
                        }
                        if (!version.empty()) {
                            push_token(TokenType::VERSION, version, {pos_in_line, pos_in_line + version.size()});
                        }
                    }
                    else if (command == "#define") {
                        std::string name;
                        std::vector<std::string> params;
                        std::string body;
                        size_t i = 0;
                        while (i < extra.size() && (extra[i] == ' ' || extra[i] == '\t')) i++;
                        while (i < extra.size() && (std::isalnum(extra[i]) || extra[i] == '_')) {
                            name.push_back(extra[i]);
                            i++;
                        }
                        while (i < extra.size() && (extra[i] == ' ' || extra[i] == '\t')) i++;
                        if (i < extra.size() && extra[i] == '(') {
                            i++;
                            std::string param;
                            while (i < extra.size() && extra[i] != ')') {
                                if (extra[i] == ',' || extra[i] == ' ') {
                                    if (!param.empty()) {
                                        params.push_back(param);
                                        param.clear();
                                    }
                                    if (extra[i] == ',') i++;
                                    while (i < extra.size() && extra[i] == ' ') i++;
                                } else {
                                    param.push_back(extra[i]);
                                    i++;
                                }
                            }
                            if (!param.empty()) params.push_back(param);
                            if (i < extra.size()) i++;
                            while (i < extra.size() && (extra[i] == ' ' || extra[i] == '\t')) i++;
                        }
                        body = extra.substr(i);
                        if (!name.empty()) {
                            defines[name] = Define{name, params, body, number_of_lines_processed + 1};
                        }
                    }
                    else if (command == "#undef") {
                        size_t j = 0;
                        while (j < extra.size() && (extra[j] == ' ' || extra[j] == '\t')) j++;
                        std::string macro_name;
                        while (j < extra.size() && (std::isalnum(extra[j]) || extra[j] == '_')) {
                            macro_name.push_back(extra[j]);
                            j++;
                        }
                        if (!macro_name.empty()) {
                            undefs[macro_name] = Undef{macro_name, number_of_lines_processed + 1};
                        }
                    }
                    else if (command == "#ifdef") {
                        size_t k = 0;
                        while (k < extra.size() && (extra[k] == ' ' || extra[k] == '\t')) k++;
                        std::string ident_str;
                        while (k < extra.size() && (std::isalnum(extra[k]) || extra[k] == '_')) {
                            ident_str.push_back(extra[k]);
                            k++;
                        }
                        IfdefState state{tokenizing, false, line_number + 1};
                        ifdef_stack.push_back(state);
                        if (state.active) {
                            bool defined = defines.find(ident_str) != defines.end() && undefs.find(ident_str) == undefs.end();
                            if (defined) {
                                tokenizing = true;
                                ifdef_stack.back().taken = true;
                            } else {
                                tokenizing = false;
                            }
                        }
                    }
                    else if (command == "#ifndef") {
                        size_t l = 0;
                        while (l < extra.size() && (extra[l] == ' ' || extra[l] == '\t')) l++;
                        std::string ident2;
                        while (l < extra.size() && (std::isalnum(extra[l]) || extra[l] == '_')) {
                            ident2.push_back(extra[l]);
                            l++;
                        }
                        IfdefState state2{tokenizing, false, line_number + 1};
                        ifdef_stack.push_back(state2);
                        if (state2.active) {
                            bool defined = defines.find(ident2) != defines.end() && undefs.find(ident2) == undefs.end();
                            if (!defined) {
                                tokenizing = true;
                                ifdef_stack.back().taken = true;
                            } else {
                                tokenizing = false;
                            }
                        }
                    }
                    else if (command == "#else") {
                        if (ifdef_stack.empty()) {
                            throw std::runtime_error(fmt::format("#else at line {} without matching #ifdef/#ifndef", line_number + 1));
                        }
                        auto& top = ifdef_stack.back();
                        if (top.active) {
                            if (top.taken) {
                                throw std::runtime_error(fmt::format("duplicate #else at line {}", line_number + 1));
                            } else {
                                tokenizing = true;
                                top.taken = true;
                            }
                        }
                    }
                    else if (command == "#endif") {
                        if (ifdef_stack.empty()) {
                            throw std::runtime_error(fmt::format("#endif at line {} without matching #ifdef/#ifndef", line_number + 1));
                        }
                        IfdefState state3 = ifdef_stack.back();
                        ifdef_stack.pop_back();
                        tokenizing = state3.active;
                    }
                    else if (!tokenizing) {
                        continue;
                    }
                    else if (command == "#include") {
                        std::string include_target = extra;
                        size_t s = include_target.find_first_not_of(" \t");
                        size_t e = include_target.find_last_not_of(" \t");
                        if (s != std::string::npos && e != std::string::npos) {
                            include_target = include_target.substr(s, e - s + 1);
                        }
                        if (include_target.size() >= 2 && include_target.front() == '"' && include_target.back() == '"') {
                            include_target = include_target.substr(1, include_target.size() - 2);
                        } else if (include_target.size() >= 2 && include_target.front() == '<' && include_target.back() == '>') {
                            include_target = include_target.substr(1, include_target.size() - 2);
                        }
                        fs::path include_path(include_target);
                        if (include_path.is_relative()) {
                            include_path = text_in.parent_path() / include_path;
                        }
                        text.erase(0, char_pos);
                        std::string include_str = ReadFilePath(include_path);
                        text = fmt::format("{}{}", include_str, text);
                        char_pos = 0;
                        line_number = 0;
                        pos_in_line = 0;
                    }
                    else if (command == "#extension") {
                        continue;
                    }
                    else if (command == "#error") {
                        throw std::runtime_error(fmt::format("#error at line {}: {}", line_number + 1, extra));
                    }
                    else if (command == "#line") {
                        size_t num_end = extra.find_first_of(" \t");
                        std::string num_str = extra.substr(0, num_end);
                        if (!num_str.empty()) {
                            int new_line = std::stoi(num_str);
                            line_number = static_cast<size_t>(new_line) - 1;
                        }
                    } else if (command == "#pragma") {
                        throw std::runtime_error("unsupported \"pragma\"");
                    } else {
                        throw std::runtime_error("unknown preprocessor_command \"" + command + "\"");
                    }
                }
            } else {
                switch (c) {
                    case '{': {
                        push_token(TokenType::LEFT_BRACKET, "", {pos_in_line, pos_in_line});
                        break;
                    }
                    case '}': {
                        push_token(TokenType::RIGHT_BRACKET, "", {pos_in_line, pos_in_line});
                        break;
                    }
                    case ';': {
                        push_token(TokenType::SEMICOLON, "", {pos_in_line, pos_in_line});
                        break;
                    }
                    case '+':{
                        if (peek_char() == '='){
                            consume_char();
                            push_token(TokenType::PLUS_ASSIGN, "", {pos_in_line -1, pos_in_line});
                        }
                        else if (peek_char() == '+'){
                            consume_char();
                            push_token(TokenType::INCREMENT, "", {pos_in_line - 1, pos_in_line});
                        }
                        else{
                            push_token(TokenType::PLUS, "", {pos_in_line, pos_in_line});
                        }
                        break;
                    }
                    case '-':{
                        if (peek_char() == '='){
                            consume_char();
                            push_token(TokenType::MINUS_ASSIGN, "", {pos_in_line -1, pos_in_line});
                        }
                        else if (peek_char() == '-'){
                            consume_char();
                            push_token(TokenType::DECREMENT, "", {pos_in_line - 1, pos_in_line});
                        }
                        else if (peek_char() == '>'){
                            consume_char();
                            push_token(TokenType::ARROW, "", {pos_in_line - 1, pos_in_line});
                        }
                        else{
                            push_token(TokenType::MINUS, "", {pos_in_line, pos_in_line});
                        }
                        break;
                    }
                    case '*':{
                        if (peek_char() == '='){
                            consume_char();
                            push_token(TokenType::STAR_ASSIGN, "", {pos_in_line -1, pos_in_line});
                        }
                        else if (peek_char() == '*'){
                            consume_char();
                            push_token(TokenType::DOUBLE_STAR, "", {pos_in_line - 1, pos_in_line});
                        }
                        else{
                            push_token(TokenType::STAR, "", {pos_in_line, pos_in_line});
                        }
                        break;
                    }
                    case '/':{
                        if (peek_char() == '='){
                            consume_char();
                            push_token(TokenType::SLASH_ASSIGN, "", {pos_in_line -1, pos_in_line});
                        }
                        else if (peek_char() == '/'){
                            while (peek_char() != '\n'){
                                consume_char();
                            }
                        }
                        else {
                            push_token(TokenType::SLASH, "", {pos_in_line, pos_in_line});
                        }
                        break;
                    }
                    case '%':{
                        push_token(TokenType::PRECENT,"", {pos_in_line, pos_in_line});
                        break;
                    }
                    case '=':{
                        if (peek_char() == '='){
                            consume_char();
                            push_token(TokenType::EQUAL, "", {pos_in_line - 1, pos_in_line});
                        }
                        else{
                            push_token(TokenType::ASSIGN, "", {pos_in_line, pos_in_line});
                        }
                        break;
                    }
                    case '!':{
                        if (peek_char() == '='){
                            consume_char();
                            push_token(TokenType::NOT_EQUAL, "", {pos_in_line - 1, pos_in_line});
                        }
                        else{
                            push_token(TokenType::NOT, "", {pos_in_line, pos_in_line});
                        }
                        break;
                    }
                    case '<':{
                        if (peek_char() == '='){
                            consume_char();
                            push_token(TokenType::LESS_EQUAL, "", {pos_in_line - 1, pos_in_line});
                        }
                        else if (peek_char() == '<'){
                            consume_char();
                            push_token(TokenType::BITWISE_LEFT_SHIFT, "", {pos_in_line - 1, pos_in_line});
                        }
                        else{
                            push_token(TokenType::LESS, "", {pos_in_line, pos_in_line});
                        }
                        break;
                    }
                    case '>':{
                        if (peek_char() == '='){
                            consume_char();
                            push_token(TokenType::GREATER_EQUAL, "", {pos_in_line - 1, pos_in_line});
                        }
                        else if (peek_char() == '>'){
                            consume_char();
                            push_token(TokenType::BITWISE_RIGHT_SHIFT, "", {pos_in_line - 1, pos_in_line});
                        }
                        else{
                            push_token(TokenType::GREATER, "", {pos_in_line, pos_in_line});
                        }
                        break;
                    }
                    case '&':{
                        if (peek_char() == '&'){
                            consume_char();
                            push_token(TokenType::AND, "", {pos_in_line - 1, pos_in_line});
                        }
                        else{
                            push_token(TokenType::BITWISE_AND, "", {pos_in_line, pos_in_line});
                        }
                        break;
                    }
                    case '|':{
                        if (peek_char() == '|'){
                            consume_char();
                            push_token(TokenType::OR, "", {pos_in_line - 1, pos_in_line});
                        }
                        else{
                            push_token(TokenType::BITWISE_OR, "", {pos_in_line, pos_in_line});
                        }
                        break;
                    }
                    case'^':{
                        push_token(TokenType::BITWISE_XOR, "", {pos_in_line, pos_in_line});
                        break;
                    }
                    case'~':{
                        push_token(TokenType::BITWISE_NOT, "", {pos_in_line, pos_in_line});
                        break;
                    }
                    case'?':{
                        push_token(TokenType::QUESTION_MARK, "", {pos_in_line, pos_in_line});
                        break;
                    }
                    case':':{
                        push_token(TokenType::COLON, "", {pos_in_line, pos_in_line});
                        break;
                    }
                    case'(':{
                        push_token(TokenType::LEFT_BRACE, "", {pos_in_line, pos_in_line});
                        break;
                    }
                    case')':{
                        push_token(TokenType::RIGHT_BRACE, "", {pos_in_line, pos_in_line});
                        break;
                    }
                    case'[':{
                        push_token(TokenType::LEFT_SQAURE_BRACKET, "", {pos_in_line, pos_in_line});
                        break;
                    }
                    case']':{
                        push_token(TokenType::RIGHT_SQAURE_BRACKET, "", {pos_in_line, pos_in_line});
                        break;
                    }
                    case'.':{
                        push_token(TokenType::DOT, "", {pos_in_line, pos_in_line});
                        break;
                    }
                    case',':{
                        push_token(TokenType::COMMA, "", {pos_in_line, pos_in_line});
                        break;
                    }
                    default:{
                        is_unknown_char = true;
                        unknown_chars.push_back(c);
                    }
                }
            }
        }
        if (!ifdef_stack.empty()) {
            throw std::runtime_error(fmt::format("unmatched #ifdef/#ifndef at line {}", ifdef_stack.back().line_number));
        }
        std::vector<Token> tokens_out = tokens;
        reset_tokeniser();
        return Result<std::vector<Token>, ParserError>(tokens_out);
    }


    void Tokeniser::reset_tokeniser() {
        number_of_lines_processed = 0;
        text = "";
        char_pos = 0;
        tokens = {};
        line_number = 0;
        pos_in_line = 0;

    }

    char Tokeniser::peek_char(const int& look_ahead) const {
        if (look_ahead + char_pos > text.size()) {
            return '\0';
        }
        char c = text.at(char_pos + look_ahead);
        return c;
    }

    char Tokeniser::consume_char() {
        char c = peek_char();
        char_pos++;
        pos_in_line++;
        if (c == '\n') {
            line_number++;
            number_of_lines_processed++;
            pos_in_line = 0;
        }
        return c;
    }

    void Tokeniser::push_token(const TokenType& type, const std::string& value, const std::pair<size_t, size_t>& char_in_line_pos, const std::vector<LiteralType>& literal_types, const TokenErrorType& error_type) {
        Token token = Token{.type = type, .value = value, .line_number = line_number, .position_range = char_in_line_pos, .literal_types = literal_types};
        tokens.push_back(token);
    }

    bool Tokeniser::is_alpha(char c) {
        std::vector<char> alpha_chars = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};
        bool is_alpha = false;
        for (const char& alpha_char : alpha_chars) {
            if (alpha_char == c) {
                is_alpha = true;
            }
        }
        return is_alpha;
    }

    bool Tokeniser::is_num(char c) {
        std::vector<char> num_chars = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
        bool is_num = false;
        for (const char& num_char : num_chars) {
            if (num_char == c) {
                is_num = true;
            }
        }
        return is_num;
    }

    bool Tokeniser::is_whitespace(char c) {
        std::vector<char> whitespace_chars = {' ', '\t', '\r', '\n'};
        bool is_whitespace = false;
        for (const char& whitespace_char : whitespace_chars) {
            if (whitespace_char == c) {
                is_whitespace = true;
            }
        }
        return is_whitespace;
    }

    std::string to_string(ParserError error) {
        std::string error_string;
        switch (error) {
        case ParserError::UNEXPECTED_TOKEN:{
                return "unexpected token";
            };
        }
        throw std::runtime_error("how did this happen from this fucking position");
    }
}
