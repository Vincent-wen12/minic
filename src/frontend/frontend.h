#ifndef FRONTEND_H
#define FRONTEND_H

#include <string>
#include <vector>
#include <unordered_map>
#include <stdexcept>

// Token types
enum TokenType {
    // Keywords
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
    
    // Identifier
    IDENTIFIER,
    
    // Constants
    CONST_INT,
    CONST_FLOAT,
    CONST_CHAR,
    CONST_BOOL,
    CONST_STRING,
    
    // Operators
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
    
    // Punctuation
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
    
    // Other
    PREPROCESSOR,
    END_OF_FILE
};

// Token structure
struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
    
    Token(TokenType type, const std::string& value, int line, int column);
};

// Abstract syntax tree node base class
class ASTNode {
public:
    virtual ~ASTNode() = default;
};

// Type node
class TypeNode : public ASTNode {
public:
    enum TypeKind {
        TYPE_INT,
        TYPE_FLOAT,
        TYPE_DOUBLE,
        TYPE_CHAR,
        TYPE_BOOL,
        TYPE_VOID,
        TYPE_ARRAY,
        TYPE_POINTER,
        TYPE_STRUCT
    };

    TypeKind kind;
    std::string structName; // Struct name
    TypeNode* baseType; // Base type for array or pointer
    int arraySize; // Array size

    TypeNode(TypeKind kind);
    ~TypeNode();
};

// Variable declaration node
class VarDeclNode : public ASTNode {
public:
    TypeNode* type;
    std::string name;
    ASTNode* initializer; // Initialization expression
    bool isConst;
    bool isStatic;

    VarDeclNode(TypeNode* type, const std::string& name, bool isConst = false, bool isStatic = false);
    ~VarDeclNode();
};

// Function parameter node
class ParamNode : public ASTNode {
public:
    TypeNode* type;
    std::string name;

    ParamNode(TypeNode* type, const std::string& name);
    ~ParamNode();
};

// Struct member node
class StructMemberNode : public ASTNode {
public:
    TypeNode* type;
    std::string name;

    StructMemberNode(TypeNode* type, const std::string& name);
    ~StructMemberNode();
};

// Struct declaration node
class StructDeclNode : public ASTNode {
public:
    std::string name;
    std::vector<StructMemberNode*> members;

    StructDeclNode(const std::string& name);
    ~StructDeclNode();
};

// Statement node base class
class StmtNode : public ASTNode {
};

// Compound statement node
class CompoundStmtNode : public StmtNode {
public:
    std::vector<ASTNode*> declarations; // Variable declarations
    std::vector<StmtNode*> statements; // Statements

    CompoundStmtNode();
    ~CompoundStmtNode();
};

// Expression statement node
class ExprStmtNode : public StmtNode {
public:
    ASTNode* expression;

    ExprStmtNode(ASTNode* expression);
    ~ExprStmtNode();
};

// if-else statement node
class IfStmtNode : public StmtNode {
public:
    ASTNode* condition;
    StmtNode* thenStmt;
    StmtNode* elseStmt; // May be nullptr

    IfStmtNode(ASTNode* condition, StmtNode* thenStmt);
    ~IfStmtNode();
};

// while statement node
class WhileStmtNode : public StmtNode {
public:
    ASTNode* condition;
    StmtNode* body;

    WhileStmtNode(ASTNode* condition, StmtNode* body);
    ~WhileStmtNode();
};

// for statement node
class ForStmtNode : public StmtNode {
public:
    ASTNode* initialization; // May be nullptr
    ASTNode* condition; // May be nullptr
    ASTNode* update; // May be nullptr
    StmtNode* body;

    ForStmtNode(StmtNode* body);
    ~ForStmtNode();
};

// return statement node
class ReturnStmtNode : public StmtNode {
public:
    ASTNode* expression; // May be nullptr

    ReturnStmtNode(ASTNode* expression = nullptr);
    ~ReturnStmtNode();
};

// Break statement node
class BreakStmtNode : public StmtNode {
};

// Continue statement node
class ContinueStmtNode : public StmtNode {
};

// Expression node base class
class ExprNode : public ASTNode {
};

// Binary expression node
class BinaryExprNode : public ExprNode {
public:
    enum Op {
        OP_ADD, OP_SUB, OP_MUL, OP_DIV, OP_MOD,
        OP_EQ, OP_NE, OP_LT, OP_GT, OP_LE, OP_GE,
        OP_AND, OP_OR,
        OP_ASSIGN, OP_ADD_ASSIGN, OP_SUB_ASSIGN, OP_MUL_ASSIGN, OP_DIV_ASSIGN, OP_MOD_ASSIGN
    };

    Op op;
    ASTNode* left;
    ASTNode* right;

    BinaryExprNode(Op op, ASTNode* left, ASTNode* right);
    ~BinaryExprNode();
};

// Unary expression node
class UnaryExprNode : public ExprNode {
public:
    enum Op {
        OP_NOT, OP_NEG, OP_INC, OP_DEC, OP_ADDR, OP_DEREF
    };

    Op op;
    ASTNode* operand;

    UnaryExprNode(Op op, ASTNode* operand);
    ~UnaryExprNode();
};

// Function call node
class CallExprNode : public ExprNode {
public:
    std::string name;
    std::vector<ASTNode*> arguments;

    CallExprNode(const std::string& name);
    ~CallExprNode();
};

// Identifier node
class IdentNode : public ExprNode {
public:
    std::string name;

    IdentNode(const std::string& name);
};

// Constant node
class ConstNode : public ExprNode {
public:
    enum ConstKind {
        CONST_INT,
        CONST_FLOAT,
        CONST_CHAR,
        CONST_BOOL,
        CONST_STRING
    };

    ConstKind kind;
    std::string value;

    ConstNode(ConstKind kind, const std::string& value);
};

// Array access node
class ArrayAccessNode : public ExprNode {
public:
    ASTNode* array;
    ASTNode* index;

    ArrayAccessNode(ASTNode* array, ASTNode* index);
    ~ArrayAccessNode();
};

// Struct member access node
class MemberAccessNode : public ExprNode {
public:
    ASTNode* structExpr;
    std::string memberName;
    bool isArrow; // true for ->, false for .

    MemberAccessNode(ASTNode* structExpr, const std::string& memberName, bool isArrow = false);
    ~MemberAccessNode();
};

// Function declaration node
class FuncDeclNode : public ASTNode {
public:
    TypeNode* returnType;
    std::string name;
    std::vector<ParamNode*> params;
    CompoundStmtNode* body; // Function body (compound statement)

    FuncDeclNode(TypeNode* returnType, const std::string& name);
    ~FuncDeclNode();
};

// Program node
class ProgramNode : public ASTNode {
public:
    std::vector<ASTNode*> declarations;

    ProgramNode();
    ~ProgramNode();
};

// Symbol table entry
class Symbol {
public:
    enum SymbolKind {
        KIND_VAR,
        KIND_FUNC,
        KIND_STRUCT
    };

    SymbolKind kind;
    std::string name;
    TypeNode* type;

    Symbol(SymbolKind kind, const std::string& name, TypeNode* type);
    ~Symbol();
};

// Scope
class Scope {
public:
    std::unordered_map<std::string, Symbol*> symbols;
    Scope* parent;

    Scope(Scope* parent = nullptr);
    ~Scope();

    Symbol* find(const std::string& name) const;
    void add(const std::string& name, Symbol* symbol);
};

// Lexer class
class Lexer {
private:
    std::string source;
    size_t position;
    int line;
    int column;
    
    // Helper functions
    char peek() const;
    char advance();
    bool isWhitespace(char c) const;
    bool isDigit(char c) const;
    bool isLetter(char c) const;
    bool isIdentifierStart(char c) const;
    bool isIdentifierChar(char c) const;
    
    // Lexical analysis functions
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

// Parser class
class Parser {
private:
    std::vector<Token> tokens;
    size_t position;
    
    // Helper functions
    const Token& peek() const;
    const Token& advance();
    bool match(TokenType type);
    void expect(TokenType type, const std::string& message);
    
    // Parsing functions
    ProgramNode* parseProgram();
    ASTNode* parseDeclaration();
    TypeNode* parseType();
    VarDeclNode* parseVarDecl(bool isConst = false, bool isStatic = false);
    FuncDeclNode* parseFuncDecl();
    StructDeclNode* parseStructDecl();
    StmtNode* parseStatement();
    CompoundStmtNode* parseCompoundStmt();
    IfStmtNode* parseIfStmt();
    WhileStmtNode* parseWhileStmt();
    ForStmtNode* parseForStmt();
    ReturnStmtNode* parseReturnStmt();
    ExprNode* parseExpression();
    ExprNode* parseAssignmentExpr();
    ExprNode* parseLogicalOrExpr();
    ExprNode* parseLogicalAndExpr();
    ExprNode* parseEqualityExpr();
    ExprNode* parseRelationalExpr();
    ExprNode* parseAdditiveExpr();
    ExprNode* parseMultiplicativeExpr();
    ExprNode* parseUnaryExpr();
    ExprNode* parsePrimaryExpr();
    
public:
    Parser(const std::vector<Token>& tokens);
    ProgramNode* parse();
};

// Semantic analyzer class
class SemanticAnalyzer {
private:
    Scope* globalScope;
    Scope* currentScope;
    
    // Helper functions
    void enterScope();
    void exitScope();
    TypeNode* getType(ASTNode* node);
    bool checkTypeCompatibility(TypeNode* type1, TypeNode* type2);
    
    // Analysis functions
    void analyzeProgram(ProgramNode* program);
    void analyzeDeclaration(ASTNode* declaration);
    void analyzeVarDecl(VarDeclNode* varDecl);
    void analyzeFuncDecl(FuncDeclNode* funcDecl);
    void analyzeStructDecl(StructDeclNode* structDecl);
    void analyzeStatement(StmtNode* statement);
    void analyzeCompoundStmt(CompoundStmtNode* compoundStmt);
    void analyzeIfStmt(IfStmtNode* ifStmt);
    void analyzeWhileStmt(WhileStmtNode* whileStmt);
    void analyzeForStmt(ForStmtNode* forStmt);
    void analyzeReturnStmt(ReturnStmtNode* returnStmt, TypeNode* returnType);
    TypeNode* analyzeExpression(ASTNode* expression);
    TypeNode* analyzeBinaryExpr(BinaryExprNode* binaryExpr);
    TypeNode* analyzeUnaryExpr(UnaryExprNode* unaryExpr);
    TypeNode* analyzeCallExpr(CallExprNode* callExpr);
    TypeNode* analyzeIdent(IdentNode* ident);
    TypeNode* analyzeConst(ConstNode* constNode);
    TypeNode* analyzeArrayAccess(ArrayAccessNode* arrayAccess);
    TypeNode* analyzeMemberAccess(MemberAccessNode* memberAccess);
    
public:
    SemanticAnalyzer();
    ~SemanticAnalyzer();
    void analyze(ProgramNode* program);
};

#endif // FRONTEND_H