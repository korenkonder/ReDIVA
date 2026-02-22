/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "memory_manager.hpp"

namespace prj {
    struct MemoryManagerData {
        HeapInterface* arr[5];
    };

    MemoryManagerData memory_manager;
    bool memory_manager_init;

    const std::pair<MemCType, HeapInterface::InitParam> memory_manager_init_param[] = {
        { MemCSystem, { 0x12000000, 0x20, "MEMC_SYS"  , 0, 0 } },
        { MemCTemp  , {  0x8000000, 0x20, "MEMC_TMP"  , 0, 0 } },
        { MemCMode  , {  0x1000000, 0x20, "MEMC_MODE" , 0, 0 } },
        { MemCEvent , {  0x1000000, 0x20, "MEMC_EVENT", 0, 0 } },
        { MemCDebug , {  0x1000000, 0x20, "MEMC_DEBUG", 0, 0 } },
    };

    void MemoryManager::init() {
        if (memory_manager_init)
            return;

        for (const auto& i : memory_manager_init_param)
            memory_manager.arr[i.first] = HeapCMalloc::create(i.second);
        memory_manager_init = true;
    }

    void MemoryManager::free(MemCType type, void* data) {
        HeapInterface* heap_interface = get(type);
        if (heap_interface)
            heap_interface->free(data);
    }

    void* MemoryManager::alloc(MemCType type, size_t size, const char* id) {
        HeapInterface* heap_interface = get(type);
        if (heap_interface) {
            void* data = heap_interface->alloc(size, 0, id);
            if (!data)
                MemoryManager::show_heap(MemCSystem, 0);
            return data;
        }
        return 0;
    }

    void MemoryManager::show_heap(MemCType type, uint32_t a2) {
        HeapInterface* heap_interface = get(type);
        if (heap_interface)
            heap_interface->show_heap(a2);
    }

    inline HeapInterface* MemoryManager::get(MemCType type) {
        if ((memory_manager_init || (MemoryManager::init(), memory_manager_init))
            && type >= 0 && type < MemCMax)
            return memory_manager.arr[type];
        return 0;
    }
}
