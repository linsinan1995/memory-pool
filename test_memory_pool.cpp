//
// Created by Lin Sinan on 2021-01-03.
//
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

    A(int i, int i2, int i3, double d1, double d2) :
        i(i),i2(i2),i3(i3),d1(d1),d2(d2)
    {}

    std::string to_string_base() {
        return std::to_string(i) + std::to_string(i2) + std::to_string(i3) +
               std::to_string(d1) + std::to_string(d2);
    }
};

struct A_big : public A {
    std::string name_1;
    std::string name_2;

    A_big(int i, int i2, int i3, double d1, double d2, const char *s1, const char *s2) :
        A(i, i2, i3, d1, d2), name_1(s1), name_2(s2)
    {}

    std::string to_string() {
        return to_string_base() + name_1 + name_2;
    }
};

int main () {
    memory_pool pool (50);

    int *p_int = pool.allocate<int>(1);
    auto *p_str = pool.allocate<std::string>(10, '#');
    auto *p_ul = pool.allocate<unsigned long>(212312313);
    auto *p_a_big = pool.allocate<A_big> (1,2,3,1.1,1.2,"woooo", "heyheyhey");

    std::cout << "1. Allocate memory!\n";

    std::cout << "sizeof(int) : " << sizeof(int) << std::endl;
    std::cout << "addr of p_int : " << p_int << std::endl;
    std::cout << "value of p_int : " << *p_int << "\n\n";

    std::cout << "sizeof(string) : " << sizeof(std::string) << std::endl;
    std::cout << "addr of p_str : " << p_str << std::endl;
    std::cout << "value of p_str : " << *p_str << "\n\n";

    std::cout << "sizeof(unsigned long) : " << sizeof(unsigned long) << std::endl;
    std::cout << "addr of p_ul : " << p_ul << std::endl;
    std::cout << "value of p_ul : " << *p_ul << "\n\n";

    std::cout << "sizeof(A_big) : " << sizeof(A_big) << std::endl;
    std::cout << "addr of p_a_big : " << p_a_big << std::endl;
    std::cout << "value of p_a_big : " << p_a_big->to_string() << std::endl;

    std::cout << "\n\n2. Check if new chunk is created!\n";
    std::cout << "pool.chunk_list.size() : " << pool.chunk_list.size() << std::endl;

    std::cout << "\n\n3. Check current memory layout!\n";
    pool.print_memory_layout();

    std::cout << "\n\n4. Deallocate previous memory.\n";
    pool.deallocate(p_int);
    pool.deallocate(p_str);
    pool.deallocate(p_ul);
    pool.deallocate(p_a_big);
    std::cout << "\n5. Check memory layout after deallocation!\n";
    pool.print_memory_layout();

    std::cout << "\n6. Clean memory fragment in memory chunks!\n";
    pool.clean_fragment();

    std::cout << "\n\n7. Check memory layout after memory cleaning!\n";
    pool.print_memory_layout();

}