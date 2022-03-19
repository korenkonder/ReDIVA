/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <deque>
#include <list>
#include <mutex>
#include <string>
#include <thread>
#include "../KKdLib/default.h"
#include "../KKdLib/farc.h"

typedef struct file_handler_read_func_data {
    void(*func)(void*, void*, size_t);
    void* data;
    bool ready;
} file_handler_read_func_data;

class file_handler {
public:
    int32_t count;
    std::mutex mtx;
    bool not_ready;
    bool reading;
    std::string path;
    std::string farc_file;
    std::string file;
    bool cache;
    file_handler_read_func_data read_free_func[2];
    ssize_t size;
    void* data;

    file_handler();
    ~file_handler();

    void call_read_free_func(int32_t index);
    void free_data_lock();
    void set_file(const char* file);
    void set_farc_file(const char* farc_file, bool cache);
    void set_path(const char* path);
    void set_read_free_func_data(int32_t index, void(*func)(void*, void*, size_t), void* data);
};

class p_file_handler {
public:
    file_handler* ptr;

    p_file_handler();
    ~p_file_handler();

    void call_free_func_free_data();
    bool check_not_ready();
    void free_data();
    void* get_data();
    ssize_t get_size();
    bool read_file(const char* path, const char* farc_file, const char* file, bool cache);
    bool read_file_path(const char* path, const char* file);
    void read_now();
    void set_read_free_func_data(int32_t index, void(* func)(void*, void*, size_t), void* data);
};

class farc_read_handler {
public:
    std::string file_path;
    bool cache;
    farc* farc;

    farc_read_handler();
    ~farc_read_handler();
};

extern void file_handler_storage_ctrl();
extern void file_handler_storage_free_thread();
extern void file_handler_storage_init_thread();
