/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.hpp"
#include "heap_c_malloc.hpp"
#include <vector>

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

        template <typename T>
        inline static T* alloc(MemCType type, size_t size, const char* id) {
            return reinterpret_cast<T*>(alloc(type, sizeof(T) * (size), id));
        }
    };

    template <class T>
    class ALLOCsys {
    public:
        using value_type = T;

        ALLOCsys() noexcept {}
        template <class U> ALLOCsys(ALLOCsys<U> const&) noexcept {}

        value_type* allocate(std::size_t n) {
            return prj::MemoryManager::alloc<value_type>(MemCSystem, n, "ALLOCsys");
        }

        void deallocate(value_type* p, std::size_t) noexcept {
            prj::MemoryManager::free(MemCSystem, reinterpret_cast<void*>(p));
        }
    };

    template <class T, class U>
    bool operator==(ALLOCsys<T> const&, ALLOCsys<U> const&) noexcept {
        return true;
    }

    template <class T, class U>
    bool operator!=(ALLOCsys<T> const& x, ALLOCsys<U> const& y) noexcept {
        return !(x == y);
    }

    template<typename T>
    using sys_vector = std::vector<T, ALLOCsys<T>>;
}
