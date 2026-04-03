#include "frontend.h"

// Token 构造函数
Token::Token(TokenType type, const std::string& value, int line, int column)
    : type(type), value(value), line(line), column(column) {}

// TypeNode 构造函数
TypeNode::TypeNode(TypeKind kind)
    : kind(kind), baseType(nullptr), arraySize(0) {}

// TypeNode 析构函数
TypeNode::~TypeNode() {
    delete baseType;
}

// VarDeclNode 构造函数
VarDeclNode::VarDeclNode(TypeNode* type, const std::string& name, bool isConst, bool isStatic)
    : type(type), name(name), initializer(nullptr), isConst(isConst), isStatic(isStatic) {}

// VarDeclNode 析构函数
VarDeclNode::~VarDeclNode() {
    delete type;
    delete initializer;
}

// ParamNode 构造函数
ParamNode::ParamNode(TypeNode* type, const std::string& name)
    : type(type), name(name) {}

// ParamNode 析构函数
ParamNode::~ParamNode() {
    delete type;
}

// StructMemberNode 构造函数
StructMemberNode::StructMemberNode(TypeNode* type, const std::string& name)
    : type(type), name(name) {}

// StructMemberNode 析构函数
StructMemberNode::~StructMemberNode() {
    delete type;
}

// StructDeclNode 构造函数
StructDeclNode::StructDeclNode(const std::string& name)
    : name(name) {}

// StructDeclNode 析构函数
StructDeclNode::~StructDeclNode() {
    for (auto member : members) {
        delete member;
    }
}

// CompoundStmtNode 构造函数
CompoundStmtNode::CompoundStmtNode() {}

// CompoundStmtNode 析构函数
CompoundStmtNode::~CompoundStmtNode() {
    for (auto decl : declarations) {
        delete decl;
    }
    for (auto stmt : statements) {
        delete stmt;
    }
}

// ExprStmtNode 构造函数
ExprStmtNode::ExprStmtNode(ASTNode* expression)
    : expression(expression) {}

// ExprStmtNode 析构函数
ExprStmtNode::~ExprStmtNode() {
    delete expression;
}

// IfStmtNode 构造函数
IfStmtNode::IfStmtNode(ASTNode* condition, StmtNode* thenStmt)
    : condition(condition), thenStmt(thenStmt), elseStmt(nullptr) {}

// IfStmtNode 析构函数
IfStmtNode::~IfStmtNode() {
    delete condition;
    delete thenStmt;
    delete elseStmt;
}

// WhileStmtNode 构造函数
WhileStmtNode::WhileStmtNode(ASTNode* condition, StmtNode* body)
    : condition(condition), body(body) {}

// WhileStmtNode 析构函数
WhileStmtNode::~WhileStmtNode() {
    delete condition;
    delete body;
}

// ForStmtNode 构造函数
ForStmtNode::ForStmtNode(StmtNode* body)
    : initialization(nullptr), condition(nullptr), update(nullptr), body(body) {}

// ForStmtNode 析构函数
ForStmtNode::~ForStmtNode() {
    delete initialization;
    delete condition;
    delete update;
    delete body;
}

// ReturnStmtNode 构造函数
ReturnStmtNode::ReturnStmtNode(ASTNode* expression)
    : expression(expression) {}

// ReturnStmtNode 析构函数
ReturnStmtNode::~ReturnStmtNode() {
    delete expression;
}

// BinaryExprNode 构造函数
BinaryExprNode::BinaryExprNode(Op op, ASTNode* left, ASTNode* right)
    : op(op), left(left), right(right) {}

// BinaryExprNode 析构函数
BinaryExprNode::~BinaryExprNode() {
    delete left;
    delete right;
}

// UnaryExprNode 构造函数
UnaryExprNode::UnaryExprNode(Op op, ASTNode* operand)
    : op(op), operand(operand) {}

// UnaryExprNode 析构函数
UnaryExprNode::~UnaryExprNode() {
    delete operand;
}

// CallExprNode 构造函数
CallExprNode::CallExprNode(const std::string& name)
    : name(name) {}

// CallExprNode 析构函数
CallExprNode::~CallExprNode() {
    for (auto arg : arguments) {
        delete arg;
    }
}

// IdentNode 构造函数
IdentNode::IdentNode(const std::string& name)
    : name(name) {}

// ConstNode 构造函数
ConstNode::ConstNode(ConstKind kind, const std::string& value)
    : kind(kind), value(value) {}

// ArrayAccessNode 构造函数
ArrayAccessNode::ArrayAccessNode(ASTNode* array, ASTNode* index)
    : array(array), index(index) {}

// ArrayAccessNode 析构函数
ArrayAccessNode::~ArrayAccessNode() {
    delete array;
    delete index;
}

// MemberAccessNode 构造函数
MemberAccessNode::MemberAccessNode(ASTNode* structExpr, const std::string& memberName, bool isArrow)
    : structExpr(structExpr), memberName(memberName), isArrow(isArrow) {}

// MemberAccessNode 析构函数
MemberAccessNode::~MemberAccessNode() {
    delete structExpr;
}

// FuncDeclNode 构造函数
FuncDeclNode::FuncDeclNode(TypeNode* returnType, const std::string& name)
    : returnType(returnType), name(name), body(nullptr) {}

// FuncDeclNode 析构函数
FuncDeclNode::~FuncDeclNode() {
    delete returnType;
    for (auto param : params) {
        delete param;
    }
    delete body;
}

// ProgramNode 构造函数
ProgramNode::ProgramNode() {}

// ProgramNode 析构函数
ProgramNode::~ProgramNode() {
    for (auto decl : declarations) {
        delete decl;
    }
}

// Symbol 构造函数
Symbol::Symbol(SymbolKind kind, const std::string& name, TypeNode* type)
    : kind(kind), name(name), type(type) {}

// Symbol 析构函数
Symbol::~Symbol() {
    delete type;
}

// Scope 构造函数
Scope::Scope(Scope* parent)
    : parent(parent) {}

// Scope 析构函数
Scope::~Scope() {
    for (auto& pair : symbols) {
        delete pair.second;
    }
}

// Scope::find 方法
Symbol* Scope::find(const std::string& name) const {
    auto it = symbols.find(name);
    if (it != symbols.end()) {
        return it->second;
    }
    if (parent) {
        return parent->find(name);
    }
    return nullptr;
}

// Scope::add 方法
void Scope::add(const std::string& name, Symbol* symbol) {
    if (symbols.find(name) != symbols.end()) {
        throw std::runtime_error("Symbol " + name + " already declared in this scope");
    }
    symbols[name] = symbol;
}
