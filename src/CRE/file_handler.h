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

struct file_handler_read_func_data {
    void(*func)(void*, void*, size_t);
    void* data;
    bool ready;
};

struct file_handler {
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
    void* ds;

    file_handler();
    virtual ~file_handler();

    void call_read_free_func(int32_t index);
    void free_data_lock();
    void set_file(const char* file);
    void set_farc_file(const char* farc_file, bool cache);
    void set_path(const char* path);
    void set_read_free_func_data(int32_t index, void(*func)(void*, void*, size_t), void* data);
};

struct p_file_handler {
    file_handler* ptr;

    p_file_handler();
    virtual ~p_file_handler();

    void call_free_func_free_data();
    bool check_not_ready();
    void free_data();
    void* get_data();
    ssize_t get_size();
    bool read_file(void* data, const char* path, const char* farc_file, const char* file, bool cache);
    bool read_file(void* data, const char* path, const char* file);
    bool read_file(void* data, const char* path, uint32_t hash, const char* ext);
    void read_now();
    void set_read_free_func_data(int32_t index, void(* func)(void*, void*, size_t), void* data);
};

extern void file_handler_storage_init();
extern void file_handler_storage_ctrl();
extern void file_handler_storage_free();
