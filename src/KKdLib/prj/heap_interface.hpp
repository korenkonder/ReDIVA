/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../default.hpp"

namespace prj {
    struct mallinfo {
        int32_t arena;
        int32_t ordblks;
        int32_t smblks;
        int32_t hblks;
        int32_t hblkhd;
        int32_t usmblks;
        int32_t fsmblks;
        int32_t uordblks;
        int32_t fordblks;
        int32_t keepcost;
    };

    struct MemInfo {
        const char* name;
        size_t heap_size;
        size_t alloc_num;
        size_t now_alloc;
        size_t max_now_alloc;
        size_t max_free;
        mallinfo mi;
    };

    class HeapInterface {
    public:
        struct InitParam {
            size_t heap_size;
            size_t align;
            const char* name;
            void* arena;
            int32_t use_fill;
        };

        HeapInterface();
        virtual ~HeapInterface();

        virtual void* alloc(size_t size, uint32_t, const char* id) = 0;
        virtual bool free(void*) = 0;
        virtual size_t get_size(void*) = 0;
        virtual void calc_heap_info()  = 0;
        virtual const MemInfo* get_heap_info() = 0;
        virtual void show_heap(uint32_t) = 0;
    };
}
