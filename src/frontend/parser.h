#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include <vector>
#include <string>

class ASTNode {
public:
    virtual ~ASTNode() = default;
};

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
    std::string structName;
    TypeNode* baseType;
    int arraySize;

    TypeNode(TypeKind kind)
        : kind(kind), baseType(nullptr), arraySize(0) {}

    ~TypeNode() {
        delete baseType;
    }
};

class VarDeclNode : public ASTNode {
public:
    TypeNode* type;
    std::string name;
    ASTNode* initializer;
    bool isConst;
    bool isStatic;

    VarDeclNode(TypeNode* type, const std::string& name, bool isConst = false, bool isStatic = false)
        : type(type), name(name), initializer(nullptr), isConst(isConst), isStatic(isStatic) {}

    ~VarDeclNode() {
        delete type;
        delete initializer;
    }
};

class ParamNode : public ASTNode {
public:
    TypeNode* type;
    std::string name;

    ParamNode(TypeNode* type, const std::string& name)
        : type(type), name(name) {}

    ~ParamNode() {
        delete type;
    }
};

class StructMemberNode : public ASTNode {
public:
    TypeNode* type;
    std::string name;

    StructMemberNode(TypeNode* type, const std::string& name)
        : type(type), name(name) {}

    ~StructMemberNode() {
        delete type;
    }
};

class StructDeclNode : public ASTNode {
public:
    std::string name;
    std::vector<StructMemberNode*> members;

    StructDeclNode(const std::string& name)
        : name(name) {}

    ~StructDeclNode() {
        for (auto member : members) {
            delete member;
        }
    }
};

class StmtNode : public ASTNode {
};

class CompoundStmtNode : public StmtNode {
public:
    std::vector<ASTNode*> declarations;
    std::vector<StmtNode*> statements;

    ~CompoundStmtNode() {
        for (auto decl : declarations) {
            delete decl;
        }
        for (auto stmt : statements) {
            delete stmt;
        }
    }
};

class ExprStmtNode : public StmtNode {
public:
    ASTNode* expression;

    ExprStmtNode(ASTNode* expression)
        : expression(expression) {}

    ~ExprStmtNode() {
        delete expression;
    }
};

class IfStmtNode : public StmtNode {
public:
    ASTNode* condition;
    StmtNode* thenStmt;
    StmtNode* elseStmt;

    IfStmtNode(ASTNode* condition, StmtNode* thenStmt)
        : condition(condition), thenStmt(thenStmt), elseStmt(nullptr) {}

    ~IfStmtNode() {
        delete condition;
        delete thenStmt;
        delete elseStmt;
    }
};

class WhileStmtNode : public StmtNode {
public:
    ASTNode* condition;
    StmtNode* body;

    WhileStmtNode(ASTNode* condition, StmtNode* body)
        : condition(condition), body(body) {}

    ~WhileStmtNode() {
        delete condition;
        delete body;
    }
};

class ForStmtNode : public StmtNode {
public:
    ASTNode* initialization;
    ASTNode* condition;
    ASTNode* update;
    StmtNode* body;

    ForStmtNode(StmtNode* body)
        : initialization(nullptr), condition(nullptr), update(nullptr), body(body) {}

    ~ForStmtNode() {
        delete initialization;
        delete condition;
        delete update;
        delete body;
    }
};

class ReturnStmtNode : public StmtNode {
public:
    ASTNode* expression;

    ReturnStmtNode(ASTNode* expression = nullptr)
        : expression(expression) {}

    ~ReturnStmtNode() {
        delete expression;
    }
};

class BreakStmtNode : public StmtNode {
};

class ContinueStmtNode : public StmtNode {
};

class ExprNode : public ASTNode {
};

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

    BinaryExprNode(Op op, ASTNode* left, ASTNode* right)
        : op(op), left(left), right(right) {}

    ~BinaryExprNode() {
        delete left;
        delete right;
    }
};

class UnaryExprNode : public ExprNode {
public:
    enum Op {
        OP_NOT, OP_NEG, OP_INC, OP_DEC, OP_ADDR, OP_DEREF
    };

    Op op;
    ASTNode* operand;

    UnaryExprNode(Op op, ASTNode* operand)
        : op(op), operand(operand) {}

    ~UnaryExprNode() {
        delete operand;
    }
};

class CallExprNode : public ExprNode {
public:
    std::string name;
    std::vector<ASTNode*> arguments;

    CallExprNode(const std::string& name)
        : name(name) {}

    ~CallExprNode() {
        for (auto arg : arguments) {
            delete arg;
        }
    }
};

class IdentNode : public ExprNode {
public:
    std::string name;

    IdentNode(const std::string& name)
        : name(name) {}
};

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

    ConstNode(ConstKind kind, const std::string& value)
        : kind(kind), value(value) {}
};

class ArrayAccessNode : public ExprNode {
public:
    ASTNode* array;
    ASTNode* index;

    ArrayAccessNode(ASTNode* array, ASTNode* index)
        : array(array), index(index) {}

    ~ArrayAccessNode() {
        delete array;
        delete index;
    }
};

class MemberAccessNode : public ExprNode {
public:
    ASTNode* structExpr;
    std::string memberName;
    bool isArrow;

    MemberAccessNode(ASTNode* structExpr, const std::string& memberName, bool isArrow = false)
        : structExpr(structExpr), memberName(memberName), isArrow(isArrow) {}

    ~MemberAccessNode() {
        delete structExpr;
    }
};

class FuncDeclNode : public ASTNode {
public:
    TypeNode* returnType;
    std::string name;
    std::vector<ParamNode*> params;
    CompoundStmtNode* body;

    FuncDeclNode(TypeNode* returnType, const std::string& name)
        : returnType(returnType), name(name), body(nullptr) {}

    ~FuncDeclNode() {
        delete returnType;
        for (auto param : params) {
            delete param;
        }
        delete body;
    }
};

class ProgramNode : public ASTNode {
public:
    std::vector<ASTNode*> declarations;

    ~ProgramNode() {
        for (auto decl : declarations) {
            delete decl;
        }
    }
};

class Parser {
private:
    std::vector<Token> tokens;
    size_t position;

    const Token& peek() const;
    const Token& advance();
    bool match(TokenType type);
    void expect(TokenType type, const std::string& message);

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

#endif // PARSER_H