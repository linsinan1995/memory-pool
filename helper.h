//
// Created by Lin Sinan on 2021-01-03.
//

#ifndef MEMORY_POOL_HELPER_H
#define MEMORY_POOL_HELPER_H

#include <cstdio>
#include <string>

static void cmd_color_red()    { printf("\033[1;31m"); }
static void cmd_color_yellow() { printf("\033[1;33m"); }
static void cmd_color_reset()  { printf("\033[0m"); }
static void print_red(const char *c)  { printf("\033[1;31m%s\033[0m", c); }
static void print_yellow(const char *c)  { printf("\033[1;33m%s\033[0m", c); }

static void print_symbol_align(const char *c, int idx)  {
    std::string ret (c);
    // if more than one char, we should reduce the number of whitespace.
    int size = ret.size() - 1;

    int i = 0;
    for (; idx != 0; idx /= 10) {
        i ++;
    }

    i = i ? i : 1;
    i = i - size < 0 ? 0 : i - size;
    printf("%s", (ret + std::string(i, ' ')).c_str());
}

static std::nullptr_t panic_nptr(char const* const format, ...) {
    cmd_color_red();
    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
    cmd_color_reset();
    return nullptr;
}

#endif //MEMORY_POOL_HELPER_H
