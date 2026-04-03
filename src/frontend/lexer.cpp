#include "lexer.h"
#include <cctype>
#include <unordered_map>

Lexer::Lexer(const std::string& source)
    : source(source), position(0), line(1), column(1) {}

char Lexer::peek() const {
    if (position >= source.length()) {
        return '\0';
    }
    return source[position];
}

char Lexer::advance() {
    char c = source[position++];
    if (c == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
    return c;
}

bool Lexer::isWhitespace(char c) const {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

bool Lexer::isDigit(char c) const {
    return std::isdigit(c);
}

bool Lexer::isLetter(char c) const {
    return std::isalpha(c) || c == '_';
}

bool Lexer::isIdentifierStart(char c) const {
    return std::isalpha(c) || c == '_';
}

bool Lexer::isIdentifierChar(char c) const {
    return std::isalnum(c) || c == '_';
}

Token Lexer::parseIdentifier() {
    std::string value;
    int startColumn = column;
    
    while (isIdentifierChar(peek())) {
        value += advance();
    }
    
    // 检查是否是关键字
    static const std::unordered_map<std::string, TokenType> keywords = {
        {"int", KEYWORD_INT},
        {"float", KEYWORD_FLOAT},
        {"double", KEYWORD_DOUBLE},
        {"char", KEYWORD_CHAR},
        {"bool", KEYWORD_BOOL},
        {"void", KEYWORD_VOID},
        {"if", KEYWORD_IF},
        {"else", KEYWORD_ELSE},
        {"while", KEYWORD_WHILE},
        {"for", KEYWORD_FOR},
        {"return", KEYWORD_RETURN},
        {"const", KEYWORD_CONST},
        {"static", KEYWORD_STATIC},
        {"struct", KEYWORD_STRUCT},
        {"union", KEYWORD_UNION},
        {"enum", KEYWORD_ENUM},
        {"typedef", KEYWORD_TYPEDEF},
        {"extern", KEYWORD_EXTERN},
        {"break", KEYWORD_BREAK},
        {"continue", KEYWORD_CONTINUE},
        {"true", CONST_BOOL},
        {"false", CONST_BOOL}
    };
    
    auto it = keywords.find(value);
    if (it != keywords.end()) {
        return Token(it->second, value, line, startColumn);
    }
    
    return Token(IDENTIFIER, value, line, startColumn);
}

Token Lexer::parseNumber() {
    std::string value;
    int startColumn = column;
    bool isFloat = false;
    
    // 处理整数部分
    while (isDigit(peek())) {
        value += advance();
    }
    
    // 处理小数部分
    if (peek() == '.') {
        isFloat = true;
        value += advance();
        while (isDigit(peek())) {
            value += advance();
        }
    }
    
    // 处理指数部分
    if (peek() == 'e' || peek() == 'E') {
        isFloat = true;
        value += advance();
        if (peek() == '+' || peek() == '-') {
            value += advance();
        }
        while (isDigit(peek())) {
            value += advance();
        }
    }
    
    if (isFloat) {
        return Token(CONST_FLOAT, value, line, startColumn);
    } else {
        return Token(CONST_INT, value, line, startColumn);
    }
}

Token Lexer::parseChar() {
    std::string value;
    int startColumn = column;
    
    advance(); // 跳过单引号
    
    if (peek() == '\\') {
        value += advance(); // 跳过转义字符
    }
    value += advance(); // 读取字符
    advance(); // 跳过结束单引号
    
    return Token(CONST_CHAR, value, line, startColumn);
}

Token Lexer::parseString() {
    std::string value;
    int startColumn = column;
    
    advance(); // 跳过大引号
    
    while (peek() != '"' && peek() != '\0') {
        if (peek() == '\\') {
            value += advance(); // 跳过转义字符
        }
        value += advance();
    }
    
    advance(); // 跳过结束大引号
    
    return Token(CONST_STRING, value, line, startColumn);
}

Token Lexer::parseOperator() {
    std::string value;
    int startColumn = column;
    
    char c = advance();
    value += c;
    
    // 处理双字符运算符
    switch (c) {
        case '=':
            if (peek() == '=') {
                value += advance();
                return Token(OP_EQ, value, line, startColumn);
            }
            return Token(OP_ASSIGN, value, line, startColumn);
        case '!':
            if (peek() == '=') {
                value += advance();
                return Token(OP_NE, value, line, startColumn);
            }
            return Token(OP_NOT, value, line, startColumn);
        case '<':
            if (peek() == '=') {
                value += advance();
                return Token(OP_LE, value, line, startColumn);
            } else if (peek() == '<') {
                value += advance();
                return Token(OP_SHIFT_LEFT, value, line, startColumn);
            }
            return Token(OP_LT, value, line, startColumn);
        case '>':
            if (peek() == '=') {
                value += advance();
                return Token(OP_GE, value, line, startColumn);
            } else if (peek() == '>') {
                value += advance();
                return Token(OP_SHIFT_RIGHT, value, line, startColumn);
            }
            return Token(OP_GT, value, line, startColumn);
        case '&':
            if (peek() == '&') {
                value += advance();
                return Token(OP_AND, value, line, startColumn);
            }
            return Token(OP_BIT_AND, value, line, startColumn);
        case '|':
            if (peek() == '|') {
                value += advance();
                return Token(OP_OR, value, line, startColumn);
            }
            return Token(OP_BIT_OR, value, line, startColumn);
        case '+':
            if (peek() == '+') {
                value += advance();
                return Token(OP_INC, value, line, startColumn);
            } else if (peek() == '=') {
                value += advance();
                return Token(OP_ADD_ASSIGN, value, line, startColumn);
            }
            return Token(OP_PLUS, value, line, startColumn);
        case '-':
            if (peek() == '-') {
                value += advance();
                return Token(OP_DEC, value, line, startColumn);
            } else if (peek() == '=') {
                value += advance();
                return Token(OP_SUB_ASSIGN, value, line, startColumn);
            } else if (peek() == '>') {
                value += advance();
                return Token(PUNCT_ARROW, value, line, startColumn);
            }
            return Token(OP_MINUS, value, line, startColumn);
        case '*':
            if (peek() == '=') {
                value += advance();
                return Token(OP_MUL_ASSIGN, value, line, startColumn);
            }
            return Token(OP_MUL, value, line, startColumn);
        case '/':
            if (peek() == '=') {
                value += advance();
                return Token(OP_DIV_ASSIGN, value, line, startColumn);
            }
            return Token(OP_DIV, value, line, startColumn);
        case '%':
            if (peek() == '=') {
                value += advance();
                return Token(OP_MOD_ASSIGN, value, line, startColumn);
            }
            return Token(OP_MOD, value, line, startColumn);
        case '~':
            return Token(OP_BIT_NOT, value, line, startColumn);
        default:
            return Token(END_OF_FILE, value, line, startColumn);
    }
}

Token Lexer::parsePunctuation() {
    char c = advance();
    int startColumn = column - 1;
    
    switch (c) {
        case '(':
            return Token(PUNCT_LPAREN, "(", line, startColumn);
        case ')':
            return Token(PUNCT_RPAREN, ")", line, startColumn);
        case '{':
            return Token(PUNCT_LBRACE, "{", line, startColumn);
        case '}':
            return Token(PUNCT_RBRACE, "}", line, startColumn);
        case '[':
            return Token(PUNCT_LBRACKET, "[", line, startColumn);
        case ']':
            return Token(PUNCT_RBRACKET, "]", line, startColumn);
        case ';':
            return Token(PUNCT_SEMICOLON, ";", line, startColumn);
        case ',':
            return Token(PUNCT_COMMA, ",", line, startColumn);
        case '.':
            return Token(PUNCT_DOT, ".", line, startColumn);
        default:
            return Token(END_OF_FILE, std::string(1, c), line, startColumn);
    }
}

Token Lexer::parsePreprocessor() {
    std::string value;
    int startColumn = column;
    
    value += advance(); // 读取 #
    
    while (peek() != '\n' && peek() != '\0') {
        value += advance();
    }
    
    return Token(PREPROCESSOR, value, line, startColumn);
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    
    while (position < source.length()) {
        char c = peek();
        
        if (isWhitespace(c)) {
            advance();
        } else if (isIdentifierStart(c)) {
            tokens.push_back(parseIdentifier());
        } else if (isDigit(c)) {
            tokens.push_back(parseNumber());
        } else if (c == '\'') {
            tokens.push_back(parseChar());
        } else if (c == '"') {
            tokens.push_back(parseString());
        } else if (c == '#') {
            tokens.push_back(parsePreprocessor());
        } else if (c == '+' || c == '-' || c == '*' || c == '/' || c == '%' ||
                   c == '=' || c == '!' || c == '<' || c == '>' || c == '&' ||
                   c == '|' || c == '~') {
            tokens.push_back(parseOperator());
        } else if (c == '(' || c == ')' || c == '{' || c == '}' ||
                   c == '[' || c == ']' || c == ';' || c == ',' || c == '.') {
            tokens.push_back(parsePunctuation());
        } else {
            // 处理未知字符
            std::string value(1, advance());
            tokens.push_back(Token(ERROR, value, line, column - 1));
        }
    }
    
    tokens.push_back(Token(END_OF_FILE, "", line, column));
    return tokens;
}