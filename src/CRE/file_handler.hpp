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
#include "../KKdLib/default.hpp"
#include "../KKdLib/farc.hpp"

struct file_handler_callback_func {
    void(*func)(void*, const void*, size_t);
    void* data;
    bool ready;
};

struct file_handler {
    int32_t count;
    std::mutex mtx;
    bool not_ready;
    bool reading;
    std::string dir;
    std::string farc_file;
    std::string file;
    bool cache;
    file_handler_callback_func callback[2];
    ssize_t size;
    void* data;
    void* ds;

    file_handler();
    virtual ~file_handler();

    void call_callback(int32_t index);
    void free_data_lock();
    void set_file(const char* file);
    void set_farc_file(const char* farc_file, bool cache);
    void set_dir(const char* dir);
    void set_callback_data(int32_t index, void(*func)(void*, const void*, size_t), void* data);
};

struct p_file_handler {
    file_handler* ptr;

    p_file_handler();
    virtual ~p_file_handler();

    void call_free_callback();
    bool check_not_ready();
    void free_data();
    const void* get_data();
    ssize_t get_size();
    bool read_file(void* data, const char* path);
    bool read_file(void* data, const char* dir, const char* farc_file, const char* file, bool cache);
    bool read_file(void* data, const char* dir, const char* file);
    bool read_file(void* data, const char* dir, uint32_t hash, const char* ext);
    void read_now();
    void set_callback_data(int32_t index, void(* func)(void*, const void*, size_t), void* data);
};

extern void file_handler_storage_init();
extern void file_handler_storage_ctrl();
extern void file_handler_storage_free();