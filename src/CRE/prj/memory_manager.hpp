/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.hpp"
#include "heap_c_malloc.hpp"

namespace prj {
    enum MemCType {
        MemCSystem = 0,
        MemCTemp,
        MemCMode,
        MemCEvent,
        MemCDebug,
        MemCMax,
    };

    struct MemoryManager {
        static void init();
        static void free(MemCType type, void* data);
        static void* alloc(MemCType type, size_t size, const char* id);
        static void show_heap(MemCType type, uint32_t a2);
        static HeapInterface* get(MemCType type);
    };
}
