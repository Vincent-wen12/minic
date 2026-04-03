#ifndef STDLIB_H
#define STDLIB_H

// 输入输出函数
int printf(const char* format, ...);
int scanf(const char* format, ...);

// 字符串处理函数
int strlen(const char* str);
char* strcpy(char* dest, const char* src);
char* strcat(char* dest, const char* src);
int strcmp(const char* str1, const char* str2);

// 数学运算函数
int abs(int n);
double sqrt(double x);
double pow(double base, double exponent);
double sin(double x);
double cos(double x);
double tan(double x);

// 内存管理函数
void* malloc(size_t size);
void free(void* ptr);

#endif // STDLIB_H