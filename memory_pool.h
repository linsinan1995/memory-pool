//
// Created by Lin Sinan on 2021-01-03.
//

#ifndef MEMORY_POOL_MEMORY_POOL_H
#define MEMORY_POOL_MEMORY_POOL_H

#include <cstdio>
#include <memory>
#include <list>
#include <vector>
#include "helper.h"

static const int ALIGN_SIZE = 8;
static const int DEFAULT_POOL_SIZE = 78;

// basic element for recording memory fragment
struct memory_cell {
    uint m_start;
    uint m_size;
    memory_cell() = default;
    memory_cell(uint start, uint size) :
        m_start(start), m_size(size)
    {}
};

struct memory_chunk {
    explicit memory_chunk(uint size, bool print_log = false) :
        m_align_size(ALIGN_SIZE),
        m_print_log(print_log)
    {
        this->m_cell_num = ((size - 1) / m_align_size) + 1;
        this->m_size = m_cell_num * m_cell_num;
        this->m_data = calloc(m_size, sizeof(char));

        if (this->m_data == nullptr) {
            panic_nptr("Fail to allocate memory for memory_chunk!\n");
            return;
        }

        this->m_used_cell_num = 0;
        this->avai_cell_list.push_front(new memory_cell(0, m_cell_num));
    }

    void *malloc(uint size, bool warning = true) {
        void *ret = nullptr;

        if (size == 0)
            return panic_nptr("Warning: malloc a memory chunk with size 0!\n");
        uint required_cell_num = ((size - 1) / m_align_size) + 1;

        if (required_cell_num > m_cell_num - m_used_cell_num) {
            return warning ? panic_nptr("Warning: required memory (%u byte) is larger "
                                        "than the available size (%u byte)!\n",
                                        required_cell_num * m_align_size,
                                        (m_cell_num - m_used_cell_num) * m_align_size) :
                             nullptr;
        }

        for (auto & cell : avai_cell_list) {
            if (cell->m_size >= required_cell_num) {
                ret = malloc_from_cell(cell, required_cell_num);
                break;
            }
        }

        if (ret == nullptr) {
            panic_nptr("Warning: does not find available size of memory_cell, and memory "
                       "pool starts to clean small memory fragment!\n");
            clean_mem();
            for (auto & cell : avai_cell_list) {
                if (cell->m_size >= required_cell_num) {
                    ret = malloc_from_cell(cell, required_cell_num);
                    break;
                }
            }
        }
        if (ret == nullptr)
            return panic_nptr("Warning: Fail to find available memory in this chunk!\n");

        if (m_print_log) print_chunk_table();

        return ret;
    }

    void *malloc_from_cell(memory_cell *cell, uint cell_num) {
        void *ret = reinterpret_cast<void *> (
                addr_to_ulong(m_data) +
                cell->m_start * ALIGN_SIZE
        );

        used_cell_list.push_front(new memory_cell(cell->m_start, cell_num));

        cell->m_start += cell_num;
        cell->m_size  -= cell_num;
        m_used_cell_num += cell_num;

        return ret;
    }

    void deallocate(void *mem) {
        for (auto cell_iter = used_cell_list.begin(); cell_iter != used_cell_list.end(); cell_iter++) {
            unsigned long start_addr_cell = addr_to_ulong(m_data) +
                                            (*cell_iter)->m_start * ALIGN_SIZE;

            if (start_addr_cell == addr_to_ulong(mem)) {
                m_used_cell_num -= (*cell_iter)->m_size;
                avai_cell_list.push_front(*cell_iter);
                used_cell_list.erase(cell_iter);
                break;
            }
        }
    }

    void sort_list() {
        // sort cell lists
        auto cmp = [] (memory_cell *p_cell, memory_cell *p_cell_2) {
            return p_cell->m_start < p_cell_2->m_start;
        };
        avai_cell_list.sort(cmp);
        used_cell_list.sort(cmp);
    }

    void clean_mem() {
        print_red("clean memory fragment\n");

        sort_list();
        auto avai_iter  = avai_cell_list.begin();
        for (auto next_iter = std::next(avai_iter); next_iter != avai_cell_list.end(); )
        {
            if ((*avai_iter)->m_start + (*avai_iter)->m_size == (*next_iter)->m_start) {
                (*avai_iter)->m_size += (*next_iter)->m_size;
                auto tmp = next_iter++;
                delete *tmp;
                avai_cell_list.erase(tmp);
            } else {
                next_iter++;
                avai_iter = next_iter;
            }
        }

        if (m_print_log) print_chunk_table();
    }

    void print_chunk_table() {
        sort_list();
        // print legend
        print_red("@");
        printf(":used cell ");
        print_yellow("#");
        printf(":available cell\n");

        // print index of cell
        for(int i = 0; i < m_cell_num; i++) printf("%d ", i);
        printf("\n");

        int idx_align = 0;

        auto avai_iter = avai_cell_list.begin(), used_iter = used_cell_list.begin();
        for (; ;)
        {
            // avoid overflow -> when cell is empty
            while (avai_iter != avai_cell_list.end() && (*avai_iter)->m_size == 0) { avai_iter ++; }
            while (used_iter != used_cell_list.end() && (*used_iter)->m_size == 0) { used_iter ++; } // optional

            if (avai_iter == avai_cell_list.end() ||
                used_iter == used_cell_list.end())
                break;

            if ((*avai_iter)->m_start < (*used_iter)->m_start) {
                cmd_color_yellow();

                for(int i = 0; i < (*avai_iter)->m_size - 1; i++)
                    print_symbol_align("#", idx_align++);
                print_symbol_align("#|", idx_align++);

                cmd_color_reset();
                avai_iter ++;
            } else {
                cmd_color_red();

                for(int i = 0; i < (*used_iter)->m_size - 1; i++)
                    print_symbol_align("@", idx_align++);
                print_symbol_align("@|", idx_align++);
                cmd_color_reset();

                used_iter ++;
            }
        }

        while (true) {
            while (avai_iter != avai_cell_list.end() && (*avai_iter)->m_size == 0) { avai_iter ++; }
            if (avai_iter == avai_cell_list.end()) break;

            cmd_color_yellow();
            for(int i = 0; i < (*avai_iter)->m_size - 1; i++)
                print_symbol_align("#", idx_align++);
            print_symbol_align("#|", idx_align++);
            cmd_color_reset();
            avai_iter++;
        }

        while (true) {
            while (used_iter != used_cell_list.end() && (*used_iter)->m_size == 0) { used_iter ++; }
            if (used_iter == used_cell_list.end()) break;

            cmd_color_red();
            for(int i = 0; i < (*used_iter)->m_size - 1; i++)
                print_symbol_align("@", idx_align++);
            print_symbol_align("@|", idx_align++);
            cmd_color_reset();
            used_iter++;
        }

        printf("\n");
    }

    ~memory_chunk() {
        for (auto & cell : used_cell_list) delete cell;
        for (auto & cell : avai_cell_list) delete cell;

        free(m_data);
    }

    static unsigned long addr_to_ulong(void *addr) {
        return reinterpret_cast<unsigned long> (addr);
    }

    uint m_size, m_align_size;
    uint m_cell_num, m_used_cell_num;

    bool m_print_log;

    std::list<memory_cell *> avai_cell_list;
    std::list<memory_cell *> used_cell_list;

    void *m_data;
}; // end of memory chunk

struct memory_pool {
    void *get_available_mem(uint size) {
        for (auto &chunk : chunk_list) {
            // if there are a lot of memory fragmentation, it still can fail to allocate.
            void *mem = chunk->malloc(size, false);
            if (mem == nullptr) continue;
            return mem;
        }

        // add new chunk
        this->add_new_chunk(static_cast<uint>(size * 1.5));
        void *mem = chunk_list.back()->malloc(size, false);

        return mem;
    }

    // warning! addr is not monotonically increasing for many scenarios!
    // re-implement it if you gonna use it.
    memory_chunk *get_dealloc_chunk(unsigned long mem_addr) {
        // binary search
        int l = 0, r = chunk_list.size() - 1;

        while (l < r) {
            int mid = (l + r + 1) / 2;
            auto addr_chunk_data = memory_chunk::addr_to_ulong(chunk_list[mid]->m_data);

            if (addr_chunk_data >= mem_addr) l = mid;
            else r = mid - 1;
        }

        return chunk_list[l].get();
    }

    template <class Type, typename ...Args>
    Type *allocate(Args &&...args) {
        void *mem = this->get_available_mem(sizeof(Type));
        Type *new_ptr = reinterpret_cast<Type *> (mem);
        new (new_ptr) Type(std::forward<Args>(args)...);
        return new_ptr;
    }

    template <class Type>
    void deallocate(Type *ptr) {
        ptr->~Type();
        unsigned long addr_value = memory_chunk::addr_to_ulong(ptr);
        auto chunk = this->get_dealloc_chunk(addr_value);
        chunk->deallocate(ptr);
    }

    void add_new_chunk(uint size) {
        auto p_chunk = new memory_chunk(size);

        if (!chunk_list.empty() &&
            memory_chunk::addr_to_ulong(chunk_list.back()->m_data) > memory_chunk::addr_to_ulong(p_chunk)) {
            delete p_chunk;
            panic_nptr("Error in memory addr! See the comment on get_dealloc_chunk\n");
            return;
        }

        chunk_list.emplace_back(p_chunk);
    }

    void clean_fragment() {
        for (auto &chunk : chunk_list) {
           chunk->clean_mem();
        }
    }

    void print_memory_layout() {
        int i = 0;
        for (auto & chunk : chunk_list) {
            printf("=====================Chunk %d=====================\n", ++i);
            printf("Memory addr starts from : %p\n", chunk->m_data);
            printf("Total memory size:\t\t %d byte\n", chunk->m_size);
            printf("Unused memory size:\t\t %d byte\n", chunk->m_used_cell_num * ALIGN_SIZE);
            printf("Available memory size:\t %d byte\n", chunk->m_size - chunk->m_used_cell_num * ALIGN_SIZE);
            chunk->print_chunk_table();
            printf("==================================================\n\n");
        }
    }

    explicit memory_pool(uint size = DEFAULT_POOL_SIZE, bool is_print = false) {
        add_new_chunk(size);
    }

    ~memory_pool() = default;

public:
    // todo better structure -> bit mask + priority_queue + vector?
    std::vector<std::unique_ptr<memory_chunk>> chunk_list;
};
#endif //MEMORY_POOL_MEMORY_POOL_H