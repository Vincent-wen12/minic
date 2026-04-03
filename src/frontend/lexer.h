#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>

// 词法单元类型
enum TokenType {
    // 关键字
    KEYWORD_INT,
    KEYWORD_FLOAT,
    KEYWORD_DOUBLE,
    KEYWORD_CHAR,
    KEYWORD_BOOL,
    KEYWORD_VOID,
    KEYWORD_IF,
    KEYWORD_ELSE,
    KEYWORD_WHILE,
    KEYWORD_FOR,
    KEYWORD_RETURN,
    KEYWORD_CONST,
    KEYWORD_STATIC,
    KEYWORD_STRUCT,
    KEYWORD_UNION,
    KEYWORD_ENUM,
    KEYWORD_TYPEDEF,
    KEYWORD_EXTERN,
    KEYWORD_BREAK,
    KEYWORD_CONTINUE,
    
    // 标识符
    IDENTIFIER,
    
    // 常量
    CONST_INT,
    CONST_FLOAT,
    CONST_CHAR,
    CONST_BOOL,
    CONST_STRING,
    
    // 运算符
    OP_PLUS,
    OP_MINUS,
    OP_MUL,
    OP_DIV,
    OP_MOD,
    OP_EQ,
    OP_NE,
    OP_LT,
    OP_GT,
    OP_LE,
    OP_GE,
    OP_AND,
    OP_OR,
    OP_NOT,
    OP_ASSIGN,
    OP_ADD_ASSIGN,
    OP_SUB_ASSIGN,
    OP_MUL_ASSIGN,
    OP_DIV_ASSIGN,
    OP_MOD_ASSIGN,
    OP_INC,
    OP_DEC,
    OP_BIT_AND,
    OP_BIT_OR,
    OP_BIT_XOR,
    OP_BIT_NOT,
    OP_SHIFT_LEFT,
    OP_SHIFT_RIGHT,
    OP_SIZEOF,
    
    // 标点符号
    PUNCT_LPAREN,
    PUNCT_RPAREN,
    PUNCT_LBRACE,
    PUNCT_RBRACE,
    PUNCT_LBRACKET,
    PUNCT_RBRACKET,
    PUNCT_SEMICOLON,
    PUNCT_COMMA,
    PUNCT_DOT,
    PUNCT_ARROW,
    
    // 其他
    PREPROCESSOR,
    END_OF_FILE,
    ERROR
};

// 词法单元结构
struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
    
    Token(TokenType type, const std::string& value, int line, int column)
        : type(type), value(value), line(line), column(column) {}
};

// 词法分析器类
class Lexer {
private:
    std::string source;
    size_t position;
    int line;
    int column;
    
    // 辅助函数
    char peek() const;
    char advance();
    bool isWhitespace(char c) const;
    bool isDigit(char c) const;
    bool isLetter(char c) const;
    bool isIdentifierStart(char c) const;
    bool isIdentifierChar(char c) const;
    
    // 词法分析函数
    Token parseIdentifier();
    Token parseNumber();
    Token parseChar();
    Token parseString();
    Token parseOperator();
    Token parsePunctuation();
    Token parsePreprocessor();
    
public:
    Lexer(const std::string& source);
    std::vector<Token> tokenize();
};

#endif // LEXER_H