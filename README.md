# Minic Compiler

Minic is a lightweight programming language similar to C, designed to be faster and easier to understand and learn.

## Features

- **Lexical Analyzer**: Enhanced tokenization with support for keywords, operators, constants, and identifiers
- **Syntax Analyzer**: Complete parsing with abstract syntax tree (AST) construction and error reporting
- **Semantic Analyzer**: Comprehensive type checking and scope analysis
- **Code Generator**: X86-64 assembly code generation with register allocation and optimization
- **Standard Library**: Basic functions for input/output, string handling, and mathematical operations
- **Linker**: Integration with LLVM linker (lld-link) for object file linking

## Build Instructions

1. **Install LLVM**: Download and install LLVM from [LLVM.org](https://llvm.org/) and add the bin directory to your PATH
2. **Build with CMake**: 
   ```bash
   cmake -B build
   cmake --build build
   ```
3. **Run the compiler**: 
   ```bash
   build\Debug\minic.exe <source_file>
   ```

## Example

```c
// test.minic
int main() {
    printf("Hello, Minic!\n");
    return 0;
}
```

## Project Structure

```
minip/
├── src/
│   ├── frontend/         # Frontend components (lexer, parser, semantic analyzer)
│   ├── backend/          # Backend components (code generator)
│   ├── stdlib/           # Standard library functions
│   ├── linker/           # Linker integration with LLVM
│   └── main.cpp          # Compiler driver
├── CMakeLists.txt        # Build configuration
└── README.md             # This file
```

## Future Work

- **Complete compilation logic**: Implement full code generation for Minic programs
- **Optimization**: Improve compiler and linker performance
- **Testing**: Add more test cases for various language features
- **Documentation**: Expand documentation with usage examples and API reference

## License

MIT
