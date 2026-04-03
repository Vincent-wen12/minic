#ifndef BACKEND_H
#define BACKEND_H

#include <string>
#include <vector>
#include <map>

// Intermediate Representation (IR) Operand Type
enum IROperandType {
    IR_OP_REG,     // Register
    IR_OP_IMM,     // Immediate value
    IR_OP_MEM      // Memory address
};

// Intermediate Representation (IR) Operand
class IROperand {
public:
    IROperandType type;
    std::string value; // Register name or immediate value
    int regNum;        // Register number
    int immValue;      // Immediate value

    IROperand(IROperandType type, const std::string& value);
    IROperand(int regNum);
    IROperand(int immValue, bool isImm);
};

// Intermediate Representation (IR) Instruction Type
enum IRInstructionType {
    IR_ADD,      // Addition
    IR_SUB,      // Subtraction
    IR_MUL,      // Multiplication
    IR_DIV,      // Division
    IR_MOD,      // Modulo
    IR_LOAD,     // Load from memory
    IR_STORE,    // Store to memory
    IR_BRANCH,   // Branch
    IR_JUMP,     // Jump
    IR_RETURN,   // Return
    IR_CALL,     // Function call
    IR_LABEL     // Label
};

// Intermediate Representation (IR) Instruction
class IRInstruction {
public:
    IRInstructionType type;
    IROperand* dest;
    IROperand* src1;
    IROperand* src2;
    std::string label; // For branch and jump instructions

    IRInstruction(IRInstructionType type, IROperand* dest, IROperand* src1, IROperand* src2);
    ~IRInstruction();
};

// Basic Block
class BasicBlock {
public:
    std::string name;
    std::vector<IRInstruction*> instructions;
    BasicBlock* next;
    BasicBlock* branchTrue;
    BasicBlock* branchFalse;

    BasicBlock(const std::string& name);
    ~BasicBlock();
};

// IR Function
class IRFunction {
public:
    std::string name;
    std::vector<BasicBlock*> blocks;
    BasicBlock* entryBlock;

    IRFunction(const std::string& name);
    ~IRFunction();
};

// IR Program
class IRProgram {
public:
    std::vector<IRFunction*> functions;

    IRProgram();
    ~IRProgram();
};

// Register Allocator
class RegisterAllocator {
private:
    std::vector<bool> registerUsed;
    std::map<std::string, int> variableRegisters;

public:
    RegisterAllocator();
    int allocateRegister();
    void freeRegister(int regNum);
    int getVariableRegister(const std::string& varName);
    void setVariableRegister(const std::string& varName, int regNum);
};

// Code Generator
class CodeGenerator {
private:
    IRProgram* program;
    std::string assemblyCode;
    RegisterAllocator* regAllocator;
    std::map<std::string, int> varOffsets;
    int stackSize;

public:
    CodeGenerator(IRProgram* program);
    ~CodeGenerator();
    std::string generate();
    void generateFunction(IRFunction* func);
    void generateBasicBlock(BasicBlock* block);
    void generateInstruction(IRInstruction* instr);
};

// Backend class
class Backend {
private:
    IRProgram* program;
    CodeGenerator* codeGen;

public:
    Backend(IRProgram* program);
    ~Backend();
    std::string generateCode();
};

#endif // BACKEND_H