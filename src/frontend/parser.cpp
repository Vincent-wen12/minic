#include "parser.h"
#include <stdexcept>

Parser::Parser(const std::vector<Token>& tokens)
    : tokens(tokens), position(0) {}

const Token& Parser::peek() const {
    if (position >= tokens.size()) {
        throw std::runtime_error("Unexpected end of input");
    }
    return tokens[position];
}

const Token& Parser::advance() {
    if (position >= tokens.size()) {
        throw std::runtime_error("Unexpected end of input");
    }
    return tokens[position++];
}

bool Parser::match(TokenType type) {
    if (peek().type == type) {
        advance();
        return true;
    }
    return false;
}

void Parser::expect(TokenType type, const std::string& message) {
    if (!match(type)) {
        const Token& token = peek();
        std::string errorMessage = message + " at line " + std::to_string(token.line) + ", column " + std::to_string(token.column) + ". Got '" + token.value + "' instead.";
        throw std::runtime_error(errorMessage);
    }
}

ProgramNode* Parser::parse() {
    return parseProgram();
}

ProgramNode* Parser::parseProgram() {
    ProgramNode* program = new ProgramNode();
    
    while (peek().type != END_OF_FILE) {
        program->declarations.push_back(parseDeclaration());
    }
    
    return program;
}

ASTNode* Parser::parseDeclaration() {
    if (match(KEYWORD_STRUCT)) {
        return parseStructDecl();
    }
    
    bool isConst = match(KEYWORD_CONST);
    bool isStatic = match(KEYWORD_STATIC);
    
    TypeNode* type = parseType();
    
    if (peek().type == IDENTIFIER) {
        const Token& ident = peek();
        advance();
        
        if (peek().type == PUNCT_LPAREN) {
            // 函数声明
            FuncDeclNode* funcDecl = new FuncDeclNode(type, ident.value);
            
            // 解析参数列表
            expect(PUNCT_LPAREN, "Expected '(' after function name");
            
            if (!match(PUNCT_RPAREN)) {
                do {
                    TypeNode* paramType = parseType();
                    expect(IDENTIFIER, "Expected parameter name");
                    ParamNode* param = new ParamNode(paramType, peek().value);
                    advance();
                    funcDecl->params.push_back(param);
                } while (match(PUNCT_COMMA));
                expect(PUNCT_RPAREN, "Expected ')' after parameters");
            }
            
            // 解析函数体
            funcDecl->body = parseCompoundStmt();
            
            return funcDecl;
        } else {
            // 变量声明
            VarDeclNode* varDecl = new VarDeclNode(type, ident.value, isConst, isStatic);
            
            // 解析数组大小
            while (match(PUNCT_LBRACKET)) {
                expect(CONST_INT, "Expected array size");
                int size = std::stoi(peek().value);
                advance();
                expect(PUNCT_RBRACKET, "Expected ']' after array size");
                
                TypeNode* arrayType = new TypeNode(TypeNode::TYPE_ARRAY);
                arrayType->baseType = varDecl->type;
                arrayType->arraySize = size;
                varDecl->type = arrayType;
            }
            
            // 解析初始化器
            if (match(OP_ASSIGN)) {
                varDecl->initializer = parseExpression();
            }
            
            expect(PUNCT_SEMICOLON, "Expected ';' after variable declaration");
            
            return varDecl;
        }
    }
    
    throw std::runtime_error("Expected declaration");
}

TypeNode* Parser::parseType() {
    TypeNode* type;
    
    if (match(KEYWORD_INT)) {
        type = new TypeNode(TypeNode::TYPE_INT);
    } else if (match(KEYWORD_FLOAT)) {
        type = new TypeNode(TypeNode::TYPE_FLOAT);
    } else if (match(KEYWORD_DOUBLE)) {
        type = new TypeNode(TypeNode::TYPE_DOUBLE);
    } else if (match(KEYWORD_CHAR)) {
        type = new TypeNode(TypeNode::TYPE_CHAR);
    } else if (match(KEYWORD_BOOL)) {
        type = new TypeNode(TypeNode::TYPE_BOOL);
    } else if (match(KEYWORD_VOID)) {
        type = new TypeNode(TypeNode::TYPE_VOID);
    } else if (match(KEYWORD_STRUCT)) {
        expect(IDENTIFIER, "Expected struct name");
        TypeNode* structType = new TypeNode(TypeNode::TYPE_STRUCT);
        structType->structName = peek().value;
        advance();
        type = structType;
    } else {
        throw std::runtime_error("Expected type");
    }
    
    // 处理指针
    while (match(OP_MUL)) {
        TypeNode* ptrType = new TypeNode(TypeNode::TYPE_POINTER);
        ptrType->baseType = type;
        type = ptrType;
    }
    
    return type;
}

StructDeclNode* Parser::parseStructDecl() {
    expect(IDENTIFIER, "Expected struct name");
    StructDeclNode* structDecl = new StructDeclNode(peek().value);
    advance();
    
    expect(PUNCT_LBRACE, "Expected '{' after struct name");
    
    while (!match(PUNCT_RBRACE)) {
        TypeNode* type = parseType();
        expect(IDENTIFIER, "Expected member name");
        StructMemberNode* member = new StructMemberNode(type, peek().value);
        advance();
        expect(PUNCT_SEMICOLON, "Expected ';' after member declaration");
        structDecl->members.push_back(member);
    }
    
    expect(PUNCT_SEMICOLON, "Expected ';' after struct declaration");
    
    return structDecl;
}

StmtNode* Parser::parseStatement() {
    if (match(PUNCT_LBRACE)) {
        CompoundStmtNode* compoundStmt = parseCompoundStmt();
        expect(PUNCT_RBRACE, "Expected '}' after compound statement");
        return compoundStmt;
    } else if (match(KEYWORD_IF)) {
        return parseIfStmt();
    } else if (match(KEYWORD_WHILE)) {
        return parseWhileStmt();
    } else if (match(KEYWORD_FOR)) {
        return parseForStmt();
    } else if (match(KEYWORD_RETURN)) {
        return parseReturnStmt();
    } else if (match(KEYWORD_BREAK)) {
        expect(PUNCT_SEMICOLON, "Expected ';' after break");
        return new BreakStmtNode();
    } else if (match(KEYWORD_CONTINUE)) {
        expect(PUNCT_SEMICOLON, "Expected ';' after continue");
        return new ContinueStmtNode();
    } else {
        // 表达式语句
        ExprNode* expr = parseExpression();
        expect(PUNCT_SEMICOLON, "Expected ';' after expression");
        return new ExprStmtNode(expr);
    }
}

CompoundStmtNode* Parser::parseCompoundStmt() {
    CompoundStmtNode* compoundStmt = new CompoundStmtNode();
    
    while (peek().type != PUNCT_RBRACE && peek().type != END_OF_FILE) {
        if (peek().type == KEYWORD_CONST || peek().type == KEYWORD_STATIC ||
            peek().type == KEYWORD_INT || peek().type == KEYWORD_FLOAT ||
            peek().type == KEYWORD_DOUBLE || peek().type == KEYWORD_CHAR ||
            peek().type == KEYWORD_BOOL || peek().type == KEYWORD_VOID) {
            // 变量声明
            compoundStmt->declarations.push_back(parseDeclaration());
        } else {
            // 语句
            compoundStmt->statements.push_back(parseStatement());
        }
    }
    
    return compoundStmt;
}

IfStmtNode* Parser::parseIfStmt() {
    expect(PUNCT_LPAREN, "Expected '(' after if");
    ExprNode* condition = parseExpression();
    expect(PUNCT_RPAREN, "Expected ')' after condition");
    
    StmtNode* thenStmt = parseStatement();
    IfStmtNode* ifStmt = new IfStmtNode(condition, thenStmt);
    
    if (match(KEYWORD_ELSE)) {
        ifStmt->elseStmt = parseStatement();
    }
    
    return ifStmt;
}

WhileStmtNode* Parser::parseWhileStmt() {
    expect(PUNCT_LPAREN, "Expected '(' after while");
    ExprNode* condition = parseExpression();
    expect(PUNCT_RPAREN, "Expected ')' after condition");
    
    StmtNode* body = parseStatement();
    return new WhileStmtNode(condition, body);
}

ForStmtNode* Parser::parseForStmt() {
    expect(PUNCT_LPAREN, "Expected '(' after for");
    
    ForStmtNode* forStmt = new ForStmtNode(nullptr);
    
    // 初始化部分
    if (!match(PUNCT_SEMICOLON)) {
        forStmt->initialization = parseExpression();
        expect(PUNCT_SEMICOLON, "Expected ';' after initialization");
    }
    
    // 条件部分
    if (!match(PUNCT_SEMICOLON)) {
        forStmt->condition = parseExpression();
        expect(PUNCT_SEMICOLON, "Expected ';' after condition");
    }
    
    // 更新部分
    if (!match(PUNCT_RPAREN)) {
        forStmt->update = parseExpression();
        expect(PUNCT_RPAREN, "Expected ')' after update");
    }
    
    // 循环体
    forStmt->body = parseStatement();
    
    return forStmt;
}

ReturnStmtNode* Parser::parseReturnStmt() {
    ASTNode* expr = nullptr;
    if (peek().type != PUNCT_SEMICOLON) {
        expr = parseExpression();
    }
    expect(PUNCT_SEMICOLON, "Expected ';' after return");
    return new ReturnStmtNode(expr);
}

ExprNode* Parser::parseExpression() {
    return parseAssignmentExpr();
}

ExprNode* Parser::parseAssignmentExpr() {
    ExprNode* left = parseLogicalOrExpr();
    
    if (match(OP_ASSIGN)) {
        ExprNode* right = parseAssignmentExpr();
        return new BinaryExprNode(BinaryExprNode::OP_ASSIGN, left, right);
    } else if (match(OP_ADD_ASSIGN)) {
        ExprNode* right = parseAssignmentExpr();
        return new BinaryExprNode(BinaryExprNode::OP_ADD_ASSIGN, left, right);
    } else if (match(OP_SUB_ASSIGN)) {
        ExprNode* right = parseAssignmentExpr();
        return new BinaryExprNode(BinaryExprNode::OP_SUB_ASSIGN, left, right);
    } else if (match(OP_MUL_ASSIGN)) {
        ExprNode* right = parseAssignmentExpr();
        return new BinaryExprNode(BinaryExprNode::OP_MUL_ASSIGN, left, right);
    } else if (match(OP_DIV_ASSIGN)) {
        ExprNode* right = parseAssignmentExpr();
        return new BinaryExprNode(BinaryExprNode::OP_DIV_ASSIGN, left, right);
    } else if (match(OP_MOD_ASSIGN)) {
        ExprNode* right = parseAssignmentExpr();
        return new BinaryExprNode(BinaryExprNode::OP_MOD_ASSIGN, left, right);
    }
    
    return left;
}

ExprNode* Parser::parseLogicalOrExpr() {
    ExprNode* left = parseLogicalAndExpr();
    
    while (match(OP_OR)) {
        ExprNode* right = parseLogicalAndExpr();
        left = new BinaryExprNode(BinaryExprNode::OP_OR, left, right);
    }
    
    return left;
}

ExprNode* Parser::parseLogicalAndExpr() {
    ExprNode* left = parseEqualityExpr();
    
    while (match(OP_AND)) {
        ExprNode* right = parseEqualityExpr();
        left = new BinaryExprNode(BinaryExprNode::OP_AND, left, right);
    }
    
    return left;
}

ExprNode* Parser::parseEqualityExpr() {
    ExprNode* left = parseRelationalExpr();
    
    if (match(OP_EQ)) {
        ExprNode* right = parseRelationalExpr();
        return new BinaryExprNode(BinaryExprNode::OP_EQ, left, right);
    } else if (match(OP_NE)) {
        ExprNode* right = parseRelationalExpr();
        return new BinaryExprNode(BinaryExprNode::OP_NE, left, right);
    }
    
    return left;
}

ExprNode* Parser::parseRelationalExpr() {
    ExprNode* left = parseAdditiveExpr();
    
    if (match(OP_LT)) {
        ExprNode* right = parseAdditiveExpr();
        return new BinaryExprNode(BinaryExprNode::OP_LT, left, right);
    } else if (match(OP_GT)) {
        ExprNode* right = parseAdditiveExpr();
        return new BinaryExprNode(BinaryExprNode::OP_GT, left, right);
    } else if (match(OP_LE)) {
        ExprNode* right = parseAdditiveExpr();
        return new BinaryExprNode(BinaryExprNode::OP_LE, left, right);
    } else if (match(OP_GE)) {
        ExprNode* right = parseAdditiveExpr();
        return new BinaryExprNode(BinaryExprNode::OP_GE, left, right);
    }
    
    return left;
}

ExprNode* Parser::parseAdditiveExpr() {
    ExprNode* left = parseMultiplicativeExpr();
    
    while (true) {
        if (match(OP_PLUS)) {
            ExprNode* right = parseMultiplicativeExpr();
            left = new BinaryExprNode(BinaryExprNode::OP_ADD, left, right);
        } else if (match(OP_MINUS)) {
            ExprNode* right = parseMultiplicativeExpr();
            left = new BinaryExprNode(BinaryExprNode::OP_SUB, left, right);
        } else {
            break;
        }
    }
    
    return left;
}

ExprNode* Parser::parseMultiplicativeExpr() {
    ExprNode* left = parseUnaryExpr();
    
    while (true) {
        if (match(OP_MUL)) {
            ExprNode* right = parseUnaryExpr();
            left = new BinaryExprNode(BinaryExprNode::OP_MUL, left, right);
        } else if (match(OP_DIV)) {
            ExprNode* right = parseUnaryExpr();
            left = new BinaryExprNode(BinaryExprNode::OP_DIV, left, right);
        } else if (match(OP_MOD)) {
            ExprNode* right = parseUnaryExpr();
            left = new BinaryExprNode(BinaryExprNode::OP_MOD, left, right);
        } else {
            break;
        }
    }
    
    return left;
}

ExprNode* Parser::parseUnaryExpr() {
    if (match(OP_NOT)) {
        ExprNode* operand = parseUnaryExpr();
        return new UnaryExprNode(UnaryExprNode::OP_NOT, operand);
    } else if (match(OP_MINUS)) {
        ExprNode* operand = parseUnaryExpr();
        return new UnaryExprNode(UnaryExprNode::OP_NEG, operand);
    } else if (match(OP_INC)) {
        ExprNode* operand = parseUnaryExpr();
        return new UnaryExprNode(UnaryExprNode::OP_INC, operand);
    } else if (match(OP_DEC)) {
        ExprNode* operand = parseUnaryExpr();
        return new UnaryExprNode(UnaryExprNode::OP_DEC, operand);
    } else if (match(OP_MUL)) {
        ExprNode* operand = parseUnaryExpr();
        return new UnaryExprNode(UnaryExprNode::OP_DEREF, operand);
    } else if (match(OP_BIT_AND)) {
        ExprNode* operand = parseUnaryExpr();
        return new UnaryExprNode(UnaryExprNode::OP_ADDR, operand);
    } else {
        return parsePrimaryExpr();
    }
}

ExprNode* Parser::parsePrimaryExpr() {
    if (match(PUNCT_LPAREN)) {
        ExprNode* expr = parseExpression();
        expect(PUNCT_RPAREN, "Expected ')' after expression");
        return expr;
    } else if (peek().type == IDENTIFIER) {
        const Token& ident = advance();
        
        if (match(PUNCT_LPAREN)) {
            // 函数调用
            CallExprNode* callExpr = new CallExprNode(ident.value);
            
            if (!match(PUNCT_RPAREN)) {
                do {
                    callExpr->arguments.push_back(parseExpression());
                } while (match(PUNCT_COMMA));
                expect(PUNCT_RPAREN, "Expected ')' after arguments");
            }
            
            return callExpr;
        } else if (match(PUNCT_LBRACKET)) {
            // 数组访问
            ExprNode* index = parseExpression();
            expect(PUNCT_RBRACKET, "Expected ']' after index");
            return new ArrayAccessNode(new IdentNode(ident.value), index);
        } else if (match(PUNCT_DOT)) {
            // 结构体成员访问（.）
            expect(IDENTIFIER, "Expected member name");
            return new MemberAccessNode(new IdentNode(ident.value), peek().value, false);
        } else if (match(PUNCT_ARROW)) {
            // 结构体成员访问（->）
            expect(IDENTIFIER, "Expected member name");
            return new MemberAccessNode(new IdentNode(ident.value), peek().value, true);
        } else {
            // 普通标识符
            return new IdentNode(ident.value);
        }
    } else if (peek().type == CONST_INT) {
        const Token& token = advance();
        return new ConstNode(ConstNode::CONST_INT, token.value);
    } else if (peek().type == CONST_FLOAT) {
        const Token& token = advance();
        return new ConstNode(ConstNode::CONST_FLOAT, token.value);
    } else if (peek().type == CONST_CHAR) {
        const Token& token = advance();
        return new ConstNode(ConstNode::CONST_CHAR, token.value);
    } else if (peek().type == CONST_BOOL) {
        const Token& token = advance();
        return new ConstNode(ConstNode::CONST_BOOL, token.value);
    } else if (peek().type == CONST_STRING) {
        const Token& token = advance();
        return new ConstNode(ConstNode::CONST_STRING, token.value);
    } else {
        throw std::runtime_error("Expected expression");
    }
}