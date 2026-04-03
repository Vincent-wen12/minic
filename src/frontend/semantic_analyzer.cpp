#include "frontend.h"
#include <stdexcept>



// SemanticAnalyzer 类方法实现
SemanticAnalyzer::SemanticAnalyzer() {
    globalScope = new Scope();
    currentScope = globalScope;
    
    // 添加标准库函数
    TypeNode* voidType = new TypeNode(TypeNode::TYPE_VOID);
    TypeNode* intType = new TypeNode(TypeNode::TYPE_INT);
    TypeNode* charPtrType = new TypeNode(TypeNode::TYPE_POINTER);
    charPtrType->baseType = new TypeNode(TypeNode::TYPE_CHAR);
    
    // printf 函数
    TypeNode* printfType = new TypeNode(TypeNode::TYPE_INT);
    Symbol* printfSymbol = new Symbol(Symbol::KIND_FUNC, "printf", printfType);
    globalScope->add("printf", printfSymbol);
    
    // scanf 函数
    TypeNode* scanfType = new TypeNode(TypeNode::TYPE_INT);
    Symbol* scanfSymbol = new Symbol(Symbol::KIND_FUNC, "scanf", scanfType);
    globalScope->add("scanf", scanfSymbol);
    
    // malloc 函数
    TypeNode* mallocType = new TypeNode(TypeNode::TYPE_POINTER);
    mallocType->baseType = new TypeNode(TypeNode::TYPE_VOID);
    Symbol* mallocSymbol = new Symbol(Symbol::KIND_FUNC, "malloc", mallocType);
    globalScope->add("malloc", mallocSymbol);
    
    // free 函数
    TypeNode* freeType = new TypeNode(TypeNode::TYPE_VOID);
    Symbol* freeSymbol = new Symbol(Symbol::KIND_FUNC, "free", freeType);
    globalScope->add("free", freeSymbol);
}

SemanticAnalyzer::~SemanticAnalyzer() {
    // 释放作用域
    Scope* scope = globalScope;
    while (scope) {
        Scope* next = scope->parent;
        delete scope;
        scope = next;
    }
}

void SemanticAnalyzer::enterScope() {
    currentScope = new Scope(currentScope);
}

void SemanticAnalyzer::exitScope() {
    if (currentScope != globalScope) {
        Scope* parent = currentScope->parent;
        delete currentScope;
        currentScope = parent;
    }
}

TypeNode* SemanticAnalyzer::getType(ASTNode* node) {
    if (auto varDecl = dynamic_cast<VarDeclNode*>(node)) {
        return varDecl->type;
    } else if (auto funcDecl = dynamic_cast<FuncDeclNode*>(node)) {
        return funcDecl->returnType;
    } else if (auto constNode = dynamic_cast<ConstNode*>(node)) {
        switch (constNode->kind) {
            case ConstNode::CONST_INT:
                return new TypeNode(TypeNode::TYPE_INT);
            case ConstNode::CONST_FLOAT:
                return new TypeNode(TypeNode::TYPE_FLOAT);
            case ConstNode::CONST_CHAR:
                return new TypeNode(TypeNode::TYPE_CHAR);
            case ConstNode::CONST_BOOL:
                return new TypeNode(TypeNode::TYPE_BOOL);
            case ConstNode::CONST_STRING:
                TypeNode* charPtrType = new TypeNode(TypeNode::TYPE_POINTER);
                charPtrType->baseType = new TypeNode(TypeNode::TYPE_CHAR);
                return charPtrType;
            default:
                throw std::runtime_error("Unknown constant type");
        }
    }
    return nullptr;
}

bool SemanticAnalyzer::checkTypeCompatibility(TypeNode* type1, TypeNode* type2) {
    if (type1->kind == type2->kind) {
        if (type1->kind == TypeNode::TYPE_ARRAY) {
            return checkTypeCompatibility(type1->baseType, type2->baseType) &&
                   type1->arraySize == type2->arraySize;
        } else if (type1->kind == TypeNode::TYPE_POINTER) {
            return checkTypeCompatibility(type1->baseType, type2->baseType);
        } else if (type1->kind == TypeNode::TYPE_STRUCT) {
            return type1->structName == type2->structName;
        }
        return true;
    }
    
    // 允许数值类型之间的隐式转换
    if ((type1->kind == TypeNode::TYPE_INT && (type2->kind == TypeNode::TYPE_FLOAT || type2->kind == TypeNode::TYPE_DOUBLE)) ||
        (type1->kind == TypeNode::TYPE_FLOAT && type2->kind == TypeNode::TYPE_DOUBLE) ||
        (type2->kind == TypeNode::TYPE_INT && (type1->kind == TypeNode::TYPE_FLOAT || type1->kind == TypeNode::TYPE_DOUBLE)) ||
        (type2->kind == TypeNode::TYPE_FLOAT && type1->kind == TypeNode::TYPE_DOUBLE)) {
        return true;
    }
    
    return false;
}

void SemanticAnalyzer::analyze(ProgramNode* program) {
    analyzeProgram(program);
}

void SemanticAnalyzer::analyzeProgram(ProgramNode* program) {
    for (auto decl : program->declarations) {
        analyzeDeclaration(decl);
    }
}

void SemanticAnalyzer::analyzeDeclaration(ASTNode* declaration) {
    if (auto varDecl = dynamic_cast<VarDeclNode*>(declaration)) {
        analyzeVarDecl(varDecl);
    } else if (auto funcDecl = dynamic_cast<FuncDeclNode*>(declaration)) {
        analyzeFuncDecl(funcDecl);
    } else if (auto structDecl = dynamic_cast<StructDeclNode*>(declaration)) {
        analyzeStructDecl(structDecl);
    }
}

void SemanticAnalyzer::analyzeVarDecl(VarDeclNode* varDecl) {
    // 检查变量是否已声明
    if (currentScope->find(varDecl->name)) {
        throw std::runtime_error("Variable " + varDecl->name + " already declared");
    }
    
    // 分析初始化表达式
    if (varDecl->initializer) {
        TypeNode* initType = analyzeExpression(varDecl->initializer);
        if (!checkTypeCompatibility(initType, varDecl->type)) {
            throw std::runtime_error("Type mismatch in variable initialization");
        }
    }
    
    // 添加到符号表
    Symbol* symbol = new Symbol(Symbol::KIND_VAR, varDecl->name, varDecl->type);
    currentScope->add(varDecl->name, symbol);
}

void SemanticAnalyzer::analyzeFuncDecl(FuncDeclNode* funcDecl) {
    // 检查函数是否已声明
    if (currentScope->find(funcDecl->name)) {
        throw std::runtime_error("Function " + funcDecl->name + " already declared");
    }
    
    // 添加函数到符号表
    Symbol* symbol = new Symbol(Symbol::KIND_FUNC, funcDecl->name, funcDecl->returnType);
    currentScope->add(funcDecl->name, symbol);
    
    // 进入函数作用域
    enterScope();
    
    // 分析参数
    for (auto param : funcDecl->params) {
        Symbol* paramSymbol = new Symbol(Symbol::KIND_VAR, param->name, param->type);
        currentScope->add(param->name, paramSymbol);
    }
    
    // 分析函数体
    analyzeCompoundStmt(funcDecl->body);
    
    // 退出函数作用域
    exitScope();
}

void SemanticAnalyzer::analyzeStructDecl(StructDeclNode* structDecl) {
    // 检查结构体是否已声明
    if (currentScope->find(structDecl->name)) {
        throw std::runtime_error("Struct " + structDecl->name + " already declared");
    }
    
    // 添加结构体到符号表
    TypeNode* structType = new TypeNode(TypeNode::TYPE_STRUCT);
    structType->structName = structDecl->name;
    Symbol* symbol = new Symbol(Symbol::KIND_STRUCT, structDecl->name, structType);
    currentScope->add(structDecl->name, symbol);
    
    // 分析结构体成员
    enterScope();
    for (auto member : structDecl->members) {
        Symbol* memberSymbol = new Symbol(Symbol::KIND_VAR, member->name, member->type);
        currentScope->add(member->name, memberSymbol);
    }
    exitScope();
}

void SemanticAnalyzer::analyzeStatement(StmtNode* statement) {
    if (auto compoundStmt = dynamic_cast<CompoundStmtNode*>(statement)) {
        analyzeCompoundStmt(compoundStmt);
    } else if (auto ifStmt = dynamic_cast<IfStmtNode*>(statement)) {
        analyzeIfStmt(ifStmt);
    } else if (auto whileStmt = dynamic_cast<WhileStmtNode*>(statement)) {
        analyzeWhileStmt(whileStmt);
    } else if (auto forStmt = dynamic_cast<ForStmtNode*>(statement)) {
        analyzeForStmt(forStmt);
    } else if (auto returnStmt = dynamic_cast<ReturnStmtNode*>(statement)) {
        // 需要知道函数的返回类型，这里简化处理
        analyzeReturnStmt(returnStmt, nullptr);
    } else if (auto exprStmt = dynamic_cast<ExprStmtNode*>(statement)) {
        analyzeExpression(exprStmt->expression);
    } else if (dynamic_cast<BreakStmtNode*>(statement)) {
        // break语句，需要在循环或switch语句中
    } else if (dynamic_cast<ContinueStmtNode*>(statement)) {
        // continue语句，需要在循环语句中
    }
}

void SemanticAnalyzer::analyzeCompoundStmt(CompoundStmtNode* compoundStmt) {
    enterScope();
    
    // 分析变量声明
    for (auto decl : compoundStmt->declarations) {
        analyzeDeclaration(decl);
    }
    
    // 分析语句
    for (auto stmt : compoundStmt->statements) {
        analyzeStatement(stmt);
    }
    
    exitScope();
}

void SemanticAnalyzer::analyzeIfStmt(IfStmtNode* ifStmt) {
    // 分析条件表达式（应该是布尔类型）
    TypeNode* condType = analyzeExpression(ifStmt->condition);
    if (condType->kind != TypeNode::TYPE_BOOL && condType->kind != TypeNode::TYPE_INT) {
        throw std::runtime_error("Condition must be boolean or integer type");
    }
    
    // 分析 then 语句
    analyzeStatement(ifStmt->thenStmt);
    
    // 分析 else 语句
    if (ifStmt->elseStmt) {
        analyzeStatement(ifStmt->elseStmt);
    }
}

void SemanticAnalyzer::analyzeWhileStmt(WhileStmtNode* whileStmt) {
    // 分析条件表达式（应该是布尔类型）
    TypeNode* condType = analyzeExpression(whileStmt->condition);
    if (condType->kind != TypeNode::TYPE_BOOL && condType->kind != TypeNode::TYPE_INT) {
        throw std::runtime_error("Condition must be boolean or integer type");
    }
    
    // 分析循环体
    analyzeStatement(whileStmt->body);
}

void SemanticAnalyzer::analyzeForStmt(ForStmtNode* forStmt) {
    // 分析初始化表达式
    if (forStmt->initialization) {
        analyzeExpression(forStmt->initialization);
    }
    
    // 分析条件表达式（应该是布尔类型）
    if (forStmt->condition) {
        TypeNode* condType = analyzeExpression(forStmt->condition);
        if (condType->kind != TypeNode::TYPE_BOOL && condType->kind != TypeNode::TYPE_INT) {
            throw std::runtime_error("Condition must be boolean or integer type");
        }
    }
    
    // 分析更新表达式
    if (forStmt->update) {
        analyzeExpression(forStmt->update);
    }
    
    // 分析循环体
    analyzeStatement(forStmt->body);
}

void SemanticAnalyzer::analyzeReturnStmt(ReturnStmtNode* returnStmt, TypeNode* returnType) {
    if (returnStmt->expression) {
        TypeNode* exprType = analyzeExpression(returnStmt->expression);
        if (returnType && !checkTypeCompatibility(exprType, returnType)) {
            throw std::runtime_error("Return type mismatch");
        }
    }
}

TypeNode* SemanticAnalyzer::analyzeExpression(ASTNode* expression) {
    if (auto binaryExpr = dynamic_cast<BinaryExprNode*>(expression)) {
        return analyzeBinaryExpr(binaryExpr);
    } else if (auto unaryExpr = dynamic_cast<UnaryExprNode*>(expression)) {
        return analyzeUnaryExpr(unaryExpr);
    } else if (auto callExpr = dynamic_cast<CallExprNode*>(expression)) {
        return analyzeCallExpr(callExpr);
    } else if (auto ident = dynamic_cast<IdentNode*>(expression)) {
        return analyzeIdent(ident);
    } else if (auto constNode = dynamic_cast<ConstNode*>(expression)) {
        return analyzeConst(constNode);
    } else if (auto arrayAccess = dynamic_cast<ArrayAccessNode*>(expression)) {
        return analyzeArrayAccess(arrayAccess);
    } else if (auto memberAccess = dynamic_cast<MemberAccessNode*>(expression)) {
        return analyzeMemberAccess(memberAccess);
    }
    throw std::runtime_error("Unknown expression type");
}

TypeNode* SemanticAnalyzer::analyzeBinaryExpr(BinaryExprNode* binaryExpr) {
    TypeNode* leftType = analyzeExpression(binaryExpr->left);
    TypeNode* rightType = analyzeExpression(binaryExpr->right);
    
    // 检查类型兼容性
    if (!checkTypeCompatibility(leftType, rightType)) {
        throw std::runtime_error("Type mismatch in binary expression");
    }
    
    // 返回结果类型
    switch (binaryExpr->op) {
        case BinaryExprNode::OP_ADD:
        case BinaryExprNode::OP_SUB:
        case BinaryExprNode::OP_MUL:
        case BinaryExprNode::OP_DIV:
        case BinaryExprNode::OP_MOD:
            return leftType;
        case BinaryExprNode::OP_EQ:
        case BinaryExprNode::OP_NE:
        case BinaryExprNode::OP_LT:
        case BinaryExprNode::OP_GT:
        case BinaryExprNode::OP_LE:
        case BinaryExprNode::OP_GE:
        case BinaryExprNode::OP_AND:
        case BinaryExprNode::OP_OR:
            return new TypeNode(TypeNode::TYPE_BOOL);
        case BinaryExprNode::OP_ASSIGN:
        case BinaryExprNode::OP_ADD_ASSIGN:
        case BinaryExprNode::OP_SUB_ASSIGN:
        case BinaryExprNode::OP_MUL_ASSIGN:
        case BinaryExprNode::OP_DIV_ASSIGN:
        case BinaryExprNode::OP_MOD_ASSIGN:
            return leftType;
        default:
            throw std::runtime_error("Unknown binary operator");
    }
}

TypeNode* SemanticAnalyzer::analyzeUnaryExpr(UnaryExprNode* unaryExpr) {
    TypeNode* operandType = analyzeExpression(unaryExpr->operand);
    
    switch (unaryExpr->op) {
        case UnaryExprNode::OP_NOT:
            if (operandType->kind != TypeNode::TYPE_BOOL && operandType->kind != TypeNode::TYPE_INT) {
                throw std::runtime_error("Operand of ! must be boolean or integer");
            }
            return new TypeNode(TypeNode::TYPE_BOOL);
        case UnaryExprNode::OP_NEG:
            if (operandType->kind != TypeNode::TYPE_INT && operandType->kind != TypeNode::TYPE_FLOAT && operandType->kind != TypeNode::TYPE_DOUBLE) {
                throw std::runtime_error("Operand of - must be numeric");
            }
            return operandType;
        case UnaryExprNode::OP_INC:
        case UnaryExprNode::OP_DEC:
            return operandType;
        case UnaryExprNode::OP_ADDR:
            TypeNode* ptrType = new TypeNode(TypeNode::TYPE_POINTER);
            ptrType->baseType = operandType;
            return ptrType;
        case UnaryExprNode::OP_DEREF:
            if (operandType->kind != TypeNode::TYPE_POINTER) {
                throw std::runtime_error("Operand of * must be pointer");
            }
            return operandType->baseType;
        default:
            throw std::runtime_error("Unknown unary operator");
    }
}

TypeNode* SemanticAnalyzer::analyzeCallExpr(CallExprNode* callExpr) {
    // 查找函数
    Symbol* symbol = currentScope->find(callExpr->name);
    if (!symbol || symbol->kind != Symbol::KIND_FUNC) {
        throw std::runtime_error("Function " + callExpr->name + " not declared");
    }
    
    // 分析参数
    for (auto arg : callExpr->arguments) {
        analyzeExpression(arg);
    }
    
    // 返回函数的返回类型
    return symbol->type;
}

TypeNode* SemanticAnalyzer::analyzeIdent(IdentNode* ident) {
    // 查找标识符
    Symbol* symbol = currentScope->find(ident->name);
    if (!symbol) {
        throw std::runtime_error("Identifier " + ident->name + " not declared");
    }
    
    return symbol->type;
}

TypeNode* SemanticAnalyzer::analyzeConst(ConstNode* constNode) {
    switch (constNode->kind) {
        case ConstNode::CONST_INT:
            return new TypeNode(TypeNode::TYPE_INT);
        case ConstNode::CONST_FLOAT:
            return new TypeNode(TypeNode::TYPE_FLOAT);
        case ConstNode::CONST_CHAR:
            return new TypeNode(TypeNode::TYPE_CHAR);
        case ConstNode::CONST_BOOL:
            return new TypeNode(TypeNode::TYPE_BOOL);
        case ConstNode::CONST_STRING:
            TypeNode* charPtrType = new TypeNode(TypeNode::TYPE_POINTER);
            charPtrType->baseType = new TypeNode(TypeNode::TYPE_CHAR);
            return charPtrType;
        default:
            throw std::runtime_error("Unknown constant type");
    }
}

TypeNode* SemanticAnalyzer::analyzeArrayAccess(ArrayAccessNode* arrayAccess) {
    TypeNode* arrayType = analyzeExpression(arrayAccess->array);
    if (arrayType->kind != TypeNode::TYPE_ARRAY) {
        throw std::runtime_error("Array access requires array type");
    }
    
    // 分析索引表达式
    TypeNode* indexType = analyzeExpression(arrayAccess->index);
    if (indexType->kind != TypeNode::TYPE_INT) {
        throw std::runtime_error("Array index must be integer");
    }
    
    // 返回数组元素类型
    return arrayType->baseType;
}

TypeNode* SemanticAnalyzer::analyzeMemberAccess(MemberAccessNode* memberAccess) {
    TypeNode* structType = analyzeExpression(memberAccess->structExpr);
    if (structType->kind != TypeNode::TYPE_STRUCT) {
        throw std::runtime_error("Member access requires struct type");
    }
    
    // 查找结构体定义
    Symbol* structSymbol = currentScope->find(structType->structName);
    if (!structSymbol || structSymbol->kind != Symbol::KIND_STRUCT) {
        throw std::runtime_error("Struct " + structType->structName + " not declared");
    }
    
    // 这里简化处理，实际应该查找结构体成员
    // 假设结构体成员都是int类型
    return new TypeNode(TypeNode::TYPE_INT);
}