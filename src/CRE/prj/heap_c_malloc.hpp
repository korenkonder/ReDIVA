/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.hpp"
#include "../../KKdLib/prj/heap_interface.hpp"
#include <map>
#include <mutex>

namespace prj {
    struct MemData {
        const char* id;
        size_t buf_size;
        size_t user_request_size;
        size_t alloc_count;
        bool is_permanent;
    };

    struct HeapDebug {
        std::map<void*, MemData> mem_map;
        const char* id;
        size_t heap_size;
        size_t bytes_alloc;
        size_t max_bytes_alloc;
        size_t count_alloc;
        size_t count_free;
        size_t break_alloc;
        size_t break_free;
        bool permanent_mode;

        HeapDebug();
        ~HeapDebug();

        void init(const char* id, size_t heap_size);

        void alloc_ok(void* data, size_t buf_size, size_t size, const char* id);
        void free_ok(void* data);
        size_t get_size(void* data);
    };

    struct HeapCMalloc : public HeapInterface {
    public:
        HeapDebug mem_debug;
        MemInfo mem_info;
        size_t heap_size;
        const char* name;

    public:
        HeapCMalloc(const HeapInterface::InitParam& init_param);
        virtual ~HeapCMalloc() override;

        virtual void* alloc(size_t size, uint32_t, const char* id) override;
        virtual bool free(void*) override;
        virtual size_t get_size(void*) override;
        virtual void calc_heap_info() override;
        virtual const MemInfo* get_heap_info() override;
        virtual void show_heap(uint32_t) override;

        static HeapCMalloc* create(const HeapInterface::InitParam& init_data);

    private:
        std::mutex M_mutex;

        void* M_alloc(size_t size, uint32_t, const char* id);
        bool M_free(void*);
        size_t M_get_size(void*);
        void M_calc_heap_info();
        const MemInfo* M_get_heap_info();
    };
}
