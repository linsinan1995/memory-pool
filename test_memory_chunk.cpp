#include <iostream>
#include <string>
#include <memory>
#include "memory_pool.h"

struct A {
    int i;
    int i2;
    int i3;
    double d1;
    double d2;
};

struct A_big : public A {
    std::string name_1;
    std::string name_2;
};

int main() {
    memory_chunk chunk(78);
    std::cout << "1. allocate memory from the memory chunk\n\n";

    std::cout << "1.1. allocate mem for int, which size is " << sizeof(int) << "\n";
    auto _int  = reinterpret_cast<int*>(chunk.malloc(sizeof(int)));
    std::cout << "addr of int :" << _int << "\n\n";

    std::cout << "1.2. allocate mem for long double, which size is " << sizeof(long double) << "\n";
    auto *_ld = reinterpret_cast<long double*>(chunk.malloc(sizeof(long double)));
    std::cout << "addr of long double :" << _ld << "\n\n";

    std::cout << "1.3. allocate mem for string, which size is " << sizeof(std::string) << "\n";
    auto _str  = reinterpret_cast<std::string*>(chunk.malloc(sizeof(std::string)));
    std::cout << "addr of string :" << _str << "\n\n";

    std::cout << "1.4. allocate mem for class A, which size is " << sizeof(int) << "\n";
    auto _A  = reinterpret_cast<A*>(chunk.malloc(sizeof(A)));
    std::cout << "addr of class A :" << _A << "\n\n";

    // deallocate
    std::cout << "2. deallocate previous memory segment\n";
    chunk.deallocate(_A);
    chunk.deallocate(_int);
    chunk.deallocate(_ld);
    chunk.deallocate(_str);

    std::cout << "3. memory layout after deallocatation\n";
    chunk.print_chunk_table();

    // allocate a mem chunk for a big class
    std::cout << "4. allocate mem for big class (A_big), which size is " << sizeof(A_big) << "\n";
    chunk.malloc(sizeof(A_big));
    return 0;
}