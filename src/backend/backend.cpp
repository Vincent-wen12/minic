#include "backend.h"

// IROperand constructor
IROperand::IROperand(IROperandType type, const std::string& value)
    : type(type), value(value), regNum(-1), immValue(0) {}

IROperand::IROperand(int regNum)
    : type(IR_OP_REG), value("r" + std::to_string(regNum)), regNum(regNum), immValue(0) {}

IROperand::IROperand(int immValue, bool isImm)
    : type(IR_OP_IMM), value(std::to_string(immValue)), regNum(-1), immValue(immValue) {}

// IRInstruction constructor
IRInstruction::IRInstruction(IRInstructionType type, IROperand* dest, IROperand* src1, IROperand* src2)
    : type(type), dest(dest), src1(src1), src2(src2) {}

// IRInstruction destructor
IRInstruction::~IRInstruction() {
    delete dest;
    delete src1;
    delete src2;
}

// BasicBlock constructor
BasicBlock::BasicBlock(const std::string& name)
    : name(name), next(nullptr), branchTrue(nullptr), branchFalse(nullptr) {}

// BasicBlock destructor
BasicBlock::~BasicBlock() {
    for (auto instr : instructions) {
        delete instr;
    }
}

// IRFunction constructor
IRFunction::IRFunction(const std::string& name)
    : name(name), entryBlock(nullptr) {}

// IRFunction destructor
IRFunction::~IRFunction() {
    for (auto block : blocks) {
        delete block;
    }
}

// IRProgram constructor
IRProgram::IRProgram() {}

// IRProgram destructor
IRProgram::~IRProgram() {
    for (auto func : functions) {
        delete func;
    }
}

// RegisterAllocator constructor
RegisterAllocator::RegisterAllocator() {
    // Initialize register usage, assuming r0-r15
    registerUsed.resize(16, false);
}

// RegisterAllocator::allocateRegister method
int RegisterAllocator::allocateRegister() {
    for (int i = 0; i < registerUsed.size(); i++) {
        if (!registerUsed[i]) {
            registerUsed[i] = true;
            return i;
        }
    }
    // If no register available, return -1
    return -1;
}

// RegisterAllocator::freeRegister method
void RegisterAllocator::freeRegister(int regNum) {
    if (regNum >= 0 && regNum < registerUsed.size()) {
        registerUsed[regNum] = false;
    }
}

// RegisterAllocator::getVariableRegister method
int RegisterAllocator::getVariableRegister(const std::string& varName) {
    auto it = variableRegisters.find(varName);
    if (it != variableRegisters.end()) {
        return it->second;
    }
    return -1;
}

// RegisterAllocator::setVariableRegister method
void RegisterAllocator::setVariableRegister(const std::string& varName, int regNum) {
    variableRegisters[varName] = regNum;
}

// CodeGenerator constructor
CodeGenerator::CodeGenerator(IRProgram* program)
    : program(program), regAllocator(new RegisterAllocator()), stackSize(0) {}

// CodeGenerator destructor
CodeGenerator::~CodeGenerator() {
    delete regAllocator;
}

// CodeGenerator::generate method
std::string CodeGenerator::generate() {
    assemblyCode = ".section .text\n.globl main\n";
    
    for (auto func : program->functions) {
        generateFunction(func);
    }
    
    return assemblyCode;
}

// CodeGenerator::generateFunction method
void CodeGenerator::generateFunction(IRFunction* func) {
    assemblyCode += "\n" + func->name + ":\n";
    assemblyCode += "    push rbp\n";
    assemblyCode += "    mov rbp, rsp\n";
    
    // Allocate stack space
    if (stackSize > 0) {
        assemblyCode += "    sub rsp, " + std::to_string(stackSize) + "\n";
    }
    
    for (auto block : func->blocks) {
        generateBasicBlock(block);
    }
    
    assemblyCode += "    pop rbp\n";
    assemblyCode += "    ret\n";
}

// CodeGenerator::generateBasicBlock method
void CodeGenerator::generateBasicBlock(BasicBlock* block) {
    assemblyCode += "\n" + block->name + ":\n";
    for (auto instr : block->instructions) {
        generateInstruction(instr);
    }
}

// CodeGenerator::generateInstruction method
void CodeGenerator::generateInstruction(IRInstruction* instr) {
    switch (instr->type) {
        case IR_ADD:
            assemblyCode += "    add " + instr->src1->value + ", " + instr->dest->value + "\n";
            break;
        case IR_SUB:
            assemblyCode += "    sub " + instr->src1->value + ", " + instr->dest->value + "\n";
            break;
        case IR_MUL:
            assemblyCode += "    imul " + instr->src1->value + ", " + instr->dest->value + "\n";
            break;
        case IR_DIV:
            assemblyCode += "    mov rax, " + instr->dest->value + "\n";
            assemblyCode += "    cqo\n";
            assemblyCode += "    idiv " + instr->src1->value + "\n";
            assemblyCode += "    mov " + instr->dest->value + ", rax\n";
            break;
        case IR_MOD:
            assemblyCode += "    mov rax, " + instr->dest->value + "\n";
            assemblyCode += "    cqo\n";
            assemblyCode += "    idiv " + instr->src1->value + "\n";
            assemblyCode += "    mov " + instr->dest->value + ", rdx\n";
            break;
        case IR_LOAD:
            assemblyCode += "    mov " + instr->dest->value + ", " + instr->src1->value + "\n";
            break;
        case IR_STORE:
            assemblyCode += "    mov " + instr->dest->value + ", " + instr->src1->value + "\n";
            break;
        case IR_BRANCH:
            assemblyCode += "    cmp " + instr->src1->value + ", " + instr->src2->value + "\n";
            assemblyCode += "    jne " + instr->label + "\n";
            break;
        case IR_JUMP:
            assemblyCode += "    jmp " + instr->label + "\n";
            break;
        case IR_RETURN:
            if (instr->src1) {
                assemblyCode += "    mov rax, " + instr->src1->value + "\n";
            }
            break;
        case IR_CALL:
            assemblyCode += "    call " + instr->label + "\n";
            break;
        case IR_LABEL:
            assemblyCode += instr->label + ":\n";
            break;
        default:
            break;
    }
}

// Backend constructor
Backend::Backend(IRProgram* program)
    : program(program), codeGen(new CodeGenerator(program)) {}

// Backend destructor
Backend::~Backend() {
    delete codeGen;
}

// Backend::generateCode method
std::string Backend::generateCode() {
    return codeGen->generate();
}
