#include "linker.h"
#include <iostream>
#include <cstdlib>

// Linker constructor
Linker::Linker() {}

// Linker destructor
Linker::~Linker() {}

// Add object file
void Linker::addObjectFile(const std::string& file) {
    objectFiles.push_back(file);
}

// Set output file
void Linker::setOutputFile(const std::string& file) {
    outputFile = file;
}

// Execute linking
bool Linker::link() {
    std::cout << "Linking files..." << std::endl;
    
    // Check if we have any object files
    if (objectFiles.empty()) {
        std::cerr << "Error: No object files to link" << std::endl;
        return false;
    }
    
    // Check if output file is set
    if (outputFile.empty()) {
        std::cerr << "Error: Output file not set" << std::endl;
        return false;
    }
    
    // Print linking information
    std::cout << "Output file: " << outputFile << std::endl;
    std::cout << "Object files: " << std::endl;
    for (const auto& file : objectFiles) {
        std::cout << "  - " << file << std::endl;
    }
    
    // Integrate with LLVM linker (lld-link)
    std::string command = "D:/LLVM/bin/lld-link";
    for (const auto& file : objectFiles) {
        command += " " + file;
    }
    command += " /OUT:" + outputFile;
    
    std::cout << "Executing command: " << command << std::endl;
    
    int result = system(command.c_str());
    if (result != 0) {
        std::cerr << "Error: Linking failed with exit code " << result << std::endl;
        std::cerr << "Please make sure LLVM toolchain is installed in D:/LLVM/bin" << std::endl;
        return false;
    }
    
    std::cout << "Linking completed successfully!" << std::endl;
    
    return true;
}
