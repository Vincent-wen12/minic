#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include <unordered_map>
#include <vector>
#include <string>

// 前向声明
class TypeNode;
class ASTNode;
class VarDeclNode;
class FuncDeclNode;
class StructDeclNode;
class StmtNode;
class CompoundStmtNode;
class IfStmtNode;
class WhileStmtNode;
class ForStmtNode;
class ReturnStmtNode;
class ExprNode;
class BinaryExprNode;
class UnaryExprNode;
class CallExprNode;
class IdentNode;
class ConstNode;
class ArrayAccessNode;
class MemberAccessNode;
class ProgramNode;

// 符号表条目
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

// 作用域
class Scope {
public:
    std::unordered_map<std::string, Symbol*> symbols;
    Scope* parent;

    Scope(Scope* parent = nullptr);
    ~Scope();

    Symbol* find(const std::string& name) const;
    void add(const std::string& name, Symbol* symbol);
};

// 语义分析器类
class SemanticAnalyzer {
private:
    Scope* globalScope;
    Scope* currentScope;

    // 辅助函数
    void enterScope();
    void exitScope();
    TypeNode* getType(ASTNode* node);
    bool checkTypeCompatibility(TypeNode* type1, TypeNode* type2);

    // 分析函数
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

#endif // SEMANTIC_ANALYZER_H