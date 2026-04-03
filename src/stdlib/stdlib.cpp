#include "stdlib.h"
#include <cstdio>
#include <cstring>
#include <cmath>
#include <cstdlib>
#include <cstdarg>

// 输入输出函数
int printf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    int result = vprintf(format, args);
    va_end(args);
    return result;
}

int scanf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    int result = vscanf(format, args);
    va_end(args);
    return result;
}

// 字符串处理函数
int strlen(const char* str) {
    return std::strlen(str);
}

char* strcpy(char* dest, const char* src) {
    return std::strcpy(dest, src);
}

char* strcat(char* dest, const char* src) {
    return std::strcat(dest, src);
}

int strcmp(const char* str1, const char* str2) {
    return std::strcmp(str1, str2);
}

// 数学运算函数
int abs(int n) {
    return std::abs(n);
}

double sqrt(double x) {
    return std::sqrt(x);
}

double pow(double base, double exponent) {
    return std::pow(base, exponent);
}

double sin(double x) {
    return std::sin(x);
}

double cos(double x) {
    return std::cos(x);
}

double tan(double x) {
    return std::tan(x);
}

// 内存管理函数
void* malloc(size_t size) {
    return std::malloc(size);
}

void free(void* ptr) {
    std::free(ptr);
}
