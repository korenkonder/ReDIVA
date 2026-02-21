/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "heap_c_malloc.hpp"

namespace prj {
    HeapDebug::HeapDebug() : id(""), heap_size(), bytes_alloc(), max_bytes_alloc(),
        count_alloc(), count_free(), break_alloc(), break_free(), permanent_mode() {

    }
    
    HeapDebug::~HeapDebug() {

    }

    void HeapDebug::init(const char* id, size_t heap_size) {
        mem_map.clear();
        this->id = id;
        this->heap_size = heap_size;
        bytes_alloc = 0;
        max_bytes_alloc = 0;
        count_alloc = 0;
        count_free = 0;
        break_alloc = 0;
        break_free = 0;
        permanent_mode = false;
    }

    void HeapDebug::alloc_ok(void* data, size_t buf_size, size_t size, const char* id) {
        if (!data)
            return;

        mem_map.insert({ data, { id, buf_size, size, count_alloc, permanent_mode } });
        bytes_alloc += buf_size;
        max_bytes_alloc = max_def(max_bytes_alloc, bytes_alloc);
    }

    void HeapDebug::free_ok(void* data) {
        if (!data)
            return;

        auto elem = mem_map.find(data);
        if (elem != mem_map.end()) {
            bytes_alloc -= elem->second.buf_size;
            mem_map.erase(elem);
        }
    }

    size_t HeapDebug::get_size(void* data) {
        if (data)
            return -1;

        auto elem = mem_map.find(data);
        if (elem != mem_map.end())
            return elem->second.user_request_size;
        return -1;
    }

    HeapCMalloc::HeapCMalloc(const HeapInterface::InitParam& init_param) {
        name = init_param.name;
        heap_size = init_param.heap_size;

        mem_debug.init(init_param.name, init_param.heap_size);
        M_calc_heap_info();
    }

    HeapCMalloc::~HeapCMalloc() {

    }

    void* HeapCMalloc::alloc(size_t size, uint32_t a3, const char* id) {
        std::unique_lock<std::mutex> u_lock(M_mutex);
        return M_alloc(size, a3, name);
    }

    bool HeapCMalloc::free(void* data) {
        std::unique_lock<std::mutex> u_lock(M_mutex);
        return M_free(data);
    }

    size_t HeapCMalloc::get_size(void* data) {
        std::unique_lock<std::mutex> u_lock(M_mutex);
        return M_get_size(data);
    }

    void HeapCMalloc::calc_heap_info() {
        std::unique_lock<std::mutex> u_lock(M_mutex);
        M_calc_heap_info();
    }

    const prj::MemInfo* HeapCMalloc::get_heap_info() {
        std::unique_lock<std::mutex> u_lock(M_mutex);
        return M_get_heap_info();
    }

    void HeapCMalloc::show_heap(uint32_t) {
        std::unique_lock<std::mutex> u_lock(M_mutex);
    }

    HeapCMalloc* HeapCMalloc::create(const HeapInterface::InitParam& init_data) {
        return new HeapCMalloc(init_data);
    }

    void* HeapCMalloc::M_alloc(size_t size, uint32_t, const char* id) {
        mem_debug.count_alloc++;
        void* data = ::malloc(size);
        mem_debug.alloc_ok(data, size, size, id);
        return data;
    }

    bool HeapCMalloc::M_free(void* data) {
        mem_debug.count_free++;
        mem_debug.free_ok(data);
        ::free(data);
        return true;
    }

    size_t HeapCMalloc::M_get_size(void* data) {
        return mem_debug.get_size(data);
    }

    void HeapCMalloc::M_calc_heap_info() {
        size_t bytes_alloc = mem_debug.bytes_alloc;
        size_t heap_size = this->heap_size;
        mem_info.name = name;

        size_t alloc_num = mem_debug.mem_map.size();
        mem_info.heap_size = heap_size;
        mem_info.alloc_num = alloc_num;

        size_t max_now_alloc = mem_debug.max_bytes_alloc;
        mem_info.now_alloc = bytes_alloc;
        mem_info.max_free = heap_size - bytes_alloc;
        mem_info.max_now_alloc = max_now_alloc;

        memset(&mem_info.mi, 0, sizeof(mem_info.mi));

        mem_info.mi.arena = (int32_t)mem_info.heap_size;
        mem_info.mi.ordblks = (int32_t)mem_info.alloc_num;
        mem_info.mi.uordblks = (int32_t)mem_info.now_alloc;
        mem_info.mi.fordblks = (int32_t)(mem_info.heap_size - mem_info.now_alloc);
    }

    const MemInfo* HeapCMalloc::M_get_heap_info() {
        return &mem_info;
    }
}
