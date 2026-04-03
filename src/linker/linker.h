#ifndef LINKER_H
#define LINKER_H

#include <string>
#include <vector>

// 链接器类
class Linker {
private:
    std::vector<std::string> objectFiles;
    std::string outputFile;

public:
    Linker();
    ~Linker();
    
    // 添加目标文件
    void addObjectFile(const std::string& file);
    
    // 设置输出文件
    void setOutputFile(const std::string& file);
    
    // 执行链接
    bool link();
};

#endif // LINKER_H