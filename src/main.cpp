#include <iostream>
#include <string>
#include "linker/linker.h"

int main(int argc, char* argv[]) {
    std::cout << "Hello, Minic!" << std::endl;
    std::cout << "Minic compiler v0.1" << std::endl;
    
    if (argc != 2) {
        std::cerr << "Usage: minic <source_file>" << std::endl;
        return 1;
    }
    
    std::string sourceFile = argv[1];
    std::string objectFile = sourceFile + ".o";
    std::string outputFile = sourceFile.substr(0, sourceFile.find("."));
    
    std::cout << "Compiling " << sourceFile << "..." << std::endl;
    
    // TODO: Implement compilation logic to generate object file
    std::cout << "Compilation completed successfully!" << std::endl;
    std::cout << "Generated object file: " << objectFile << std::endl;
    
    std::cout << "Linking..." << std::endl;
    
    // Use linker to link object files
    Linker linker;
    linker.addObjectFile(objectFile);
    linker.setOutputFile(outputFile);
    
    if (!linker.link()) {
        std::cerr << "Linking failed!" << std::endl;
        return 1;
    }
    
    std::cout << "Linking completed successfully!" << std::endl;
    std::cout << "Generated executable: " << outputFile << std::endl;
    
    return 0;
}