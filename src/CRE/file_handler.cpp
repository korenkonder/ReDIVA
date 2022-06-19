/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include <thread>
#include "../KKdLib/hash.hpp"
#include "file_handler.hpp"
#include "data.hpp"
#include <sys/stat.h>

struct farc_read_handler {
    std::string file_path;
    bool cache;
    farc* farc;

    farc_read_handler();
    virtual ~farc_read_handler();

    size_t get_file_size(std::string& file);
    bool read(std::string& file_path, bool cache);
    bool read_data(void* data, size_t size, std::string& file);
};

struct file_handler_storage {
    std::list<file_handler*> list;
    farc_read_handler* farc_read_handler;
    std::deque<file_handler*> deque;
    std::mutex farc_mtx;
    std::thread* thread;
    std::mutex mtx;
    std::condition_variable cnd;
    bool exit;
    bool field_61;
    int32_t field_64;
    bool read_in_thread;
    bool field_69;
    int32_t field_6C;
    int32_t field_70;
    int32_t field_74;

    file_handler_storage();
    virtual ~file_handler_storage();
};

file_handler_storage* file_handler_storage_data;

static bool file_handler_load_file(void* data, const char* dir, const char* file, uint32_t hash);
static bool file_handler_load_farc_file(void* data, const char* dir, const char* file, uint32_t hash);
static void file_handler_storage_ctrl_list();
static void file_handler_storage_thread_ctrl();

file_handler::file_handler() : count(), not_ready(true),
reading(), cache(), callback(), size(), data(), ds() {

}

file_handler::~file_handler() {
    free(data);
}

void file_handler::call_callback(int32_t index) {
    if (index >= 2)
        return;

    std::unique_lock<std::mutex> u_lock(mtx);
    const void* data = this->data;
    void(*callback_func)(void*, const void*, size_t) = this->callback[index].func;
    void* callback_data = this->callback[index].data;
    bool ready = this->callback[index].ready;
    this->callback[index].ready = true;
    u_lock.unlock();

    if (!ready && callback_func)
        callback_func(callback_data, data, this->size);
}

void file_handler::set_file(const char* file) {
    std::unique_lock<std::mutex> u_lock(mtx);
    this->file = file;
    u_lock.unlock();
}

void file_handler::set_farc_file(const char* farc_file, bool cache) {
    std::unique_lock<std::mutex> u_lock(mtx);
    this->farc_file = farc_file;
    this->cache = cache;
    u_lock.unlock();
}

void file_handler::set_dir(const char* dir) {
    std::unique_lock<std::mutex> u_lock(mtx);
    this->dir = dir;
    u_lock.unlock();
}

void file_handler::set_callback_data(int32_t index,
    void(*func)(void*, const void*, size_t), void* data) {
    std::unique_lock<std::mutex> u_lock(mtx);
    if (index < 2) {
        callback[index].func = func;
        callback[index].data = data;
        callback[index].ready = false;
    }
    u_lock.unlock();
}

void file_handler::free_data_lock() {
    std::unique_lock<std::mutex> u_lock(mtx);
    if (count > 0) {
        count--;
        bool free_fhndl = count == 0;
        u_lock.unlock();
        if (free_fhndl)
            delete this;
    }
    else
        u_lock.unlock();
}

farc_read_handler::farc_read_handler() : cache(), farc() {

}

farc_read_handler::~farc_read_handler() {
    delete farc;
}

size_t farc_read_handler::get_file_size(std::string& file) {
    if (farc)
        return farc->get_file_size(file.c_str());
    return 0;
}

bool farc_read_handler::read(std::string& file_path, bool cache) {
    if (farc) {
        if (this->file_path == file_path && this->cache == cache)
            return true;

        delete farc;
        farc = 0;
    }

    this->file_path = file_path;
    this->cache = cache;

    struct stat st;
    if (!stat(this->file_path.c_str(), &st) && st.st_size) {
        size_t size = st.st_size;
        farc = new ::farc;
        if (farc) {
            farc->read(this->file_path.c_str(), this->cache, false);
            return true;
        }
    }
    return false;
}

bool farc_read_handler::read_data(void* data, size_t size, std::string& file) {
    if (farc) {
        farc_file* ff = farc->read_file(file.c_str());
        if (ff) {
            memcpy(data, ff->data, min(size, ff->size));
            if (!cache) {
                free(ff->data);
                ff->data = 0;
            }
            return true;
        }
    }
    return false;
}

file_handler_storage::file_handler_storage() : farc_read_handler(), thread(), cnd(), exit(),
field_61(), field_64(), read_in_thread(), field_69(), field_6C(), field_70(), field_74() {

}

file_handler_storage::~file_handler_storage() {
    delete farc_read_handler;
}

void file_handler_storage_init() {
    file_handler_storage_data = new file_handler_storage;
    file_handler_storage_data->thread = new std::thread(file_handler_storage_thread_ctrl);
    if (file_handler_storage_data->thread) {
        SetThreadDescription((HANDLE)file_handler_storage_data->thread->native_handle(), L"File Thread");
        //file_handler_storage_data->thread->detach();
    }
}

void file_handler_storage_ctrl() {
    file_handler_storage_ctrl_list();

    std::unique_lock<std::mutex> u_lock(file_handler_storage_data->mtx);
    if (!file_handler_storage_data->deque.size()) {
        for (file_handler*& i : file_handler_storage_data->list)
            if (i->not_ready && !i->reading) {
                std::unique_lock<std::mutex> u_lock(i->mtx);
                i->count++;
                u_lock.unlock();
                file_handler_storage_data->deque.push_back(i);
            }

        if (file_handler_storage_data->deque.size())
            file_handler_storage_data->cnd.notify_all();
    }
    u_lock.unlock();
}

void file_handler_storage_free() {
    std::unique_lock<std::mutex> u_lock(file_handler_storage_data->mtx);
    file_handler_storage_data->exit = true;
    file_handler_storage_data->cnd.notify_one();
    u_lock.unlock();

    file_handler_storage_data->thread->join();
    delete file_handler_storage_data->thread;
    file_handler_storage_data->thread = 0;
    delete file_handler_storage_data;
}

static bool file_handler_load_file(void* data, const char* dir, const char* file, uint32_t hash) {
    std::string file_path = dir;
    file_path += file;

    struct stat st;
    if (stat(file_path.c_str(), &st) || !st.st_size)
        return false;

    file_handler* fhndl = (file_handler*)data;
    fhndl->size = st.st_size;
    fhndl->data = malloc(st.st_size);
    if (!fhndl->data)
        return false;

    stream s;
    s.open(file_path.c_str(), "rb");
    bool ret = s.io.stream && s.read(fhndl->data, fhndl->size);
    if (!ret)
        free(fhndl->data);
    return ret;
}

static bool file_handler_load_farc_file(void* data, const char* dir, const char* file, uint32_t hash) {
    std::string file_path = dir;
    file_path += file;

    file_handler* fhndl = (file_handler*)data;

    std::unique_lock<std::mutex> u_lock(file_handler_storage_data->farc_mtx);
    bool cache = fhndl->cache;

    farc_read_handler* farc_read_hndl = file_handler_storage_data->farc_read_handler;
    if (farc_read_hndl && farc_read_hndl->file_path != file_path) {
        delete file_handler_storage_data->farc_read_handler;
        file_handler_storage_data->farc_read_handler = 0;
    }

    if (!file_handler_storage_data->farc_read_handler)
        file_handler_storage_data->farc_read_handler = new farc_read_handler;

    farc_read_hndl = file_handler_storage_data->farc_read_handler;
    if (farc_read_hndl && farc_read_hndl->read(file_path, cache)) {
        fhndl->size = farc_read_hndl->get_file_size(fhndl->file);
        if (!fhndl->size)
            fhndl->size = 1;

        fhndl->data = malloc(fhndl->size);
        if (fhndl->data && farc_read_hndl->read_data(fhndl->data, fhndl->size, fhndl->file)) {
            u_lock.unlock();
            return true;
        }
    }

    free(fhndl->data);
    u_lock.unlock();
    return false;
}

static void file_handler_storage_ctrl_list() {
    for (std::list<file_handler*>::iterator i = file_handler_storage_data->list.begin();
        i != file_handler_storage_data->list.end();) {
        file_handler* pfhndl = i._Ptr->_Myval;
        if (pfhndl->count == 1) {
            pfhndl->free_data_lock();
            i = file_handler_storage_data->list.erase(i);
        }
        else {
            if (!pfhndl->not_ready && !pfhndl->callback[0].ready)
                pfhndl->call_callback(0);
            i++;
        }
    }
}

static void file_handler_storage_thread_ctrl() {
    std::unique_lock<std::mutex> u_lock(file_handler_storage_data->mtx);
    while (!file_handler_storage_data->exit) {
        file_handler_storage_data->cnd.wait(u_lock);
        if (file_handler_storage_data->exit)
            break;

        for (file_handler*& i : file_handler_storage_data->deque) {
            if (i->not_ready) {
                std::unique_lock<std::mutex> u_lock(i->mtx);
                i->reading = true;
                if (i->not_ready) {
                    if (i->farc_file.size())
                        i->not_ready = !((data_struct*)i->ds)->load_file(i,
                            i->dir.c_str(), i->farc_file.c_str(), file_handler_load_farc_file);
                    else
                        i->not_ready = !((data_struct*)i->ds)->load_file(i,
                            i->dir.c_str(), i->file.c_str(), file_handler_load_file);
                }
                i->reading = false;
                u_lock.unlock();
            }
            i->free_data_lock();
        }
        file_handler_storage_data->deque.clear();
    }
    file_handler_storage_data->exit = false;
    u_lock.unlock();
}

p_file_handler::p_file_handler() {
    ptr = 0;
}

p_file_handler::~p_file_handler() {
    if (ptr) {
        if (check_not_ready())
            call_free_callback();
        else
            free_data();
    }
}

void p_file_handler::call_free_callback() {
    if (ptr)
        ptr->call_callback(1);
    free_data();
}

bool p_file_handler::check_not_ready() {
    if (!ptr)
        return false;
    else if (ptr->not_ready)
        return true;
    else
        return !ptr->callback[0].ready;
}

void p_file_handler::free_data() {
    if (!ptr)
        return;

    if (ptr->mtx.try_lock()) {
        for (int32_t i = 0; i < 2; i++)
            ptr->callback[i] = {};
        ptr->mtx.unlock();
    }
    ptr->free_data_lock();
    ptr = 0;
}

const void* p_file_handler::get_data() {
    if (!ptr || ptr->not_ready)
        return 0;
    return ptr->data;
}

ssize_t p_file_handler::get_size() {
    if (!ptr || ptr->not_ready || ptr->size < 0)
        return 0;
    return ptr->size;
}

bool p_file_handler::read_file(void* data, const char* path) {
    const char* t = strrchr(path, '/');
    if (t) {
        std::string dir = std::string(path, t - path + 1);
        return read_file(data, dir.c_str(), 0, t + 1, false);
    }

    t = strrchr(path, '\\');
    if (t) {
        std::string dir = std::string(path, t - path + 1);
        return read_file(data, dir.c_str(), 0, t + 1, false);
    }
    return false;
}

bool p_file_handler::read_file(void* data, const char* dir,
    const char* farc_file, const char* file, bool cache) {
    if (!dir || !file)
        return false;

    if (ptr) {
        if (ptr->not_ready)
            return false;
        free_data();
    }

    if (!((data_struct*)data)->check_file_exists(dir, farc_file ? farc_file : file))
        return false;

    ptr = new file_handler;
    if (!ptr)
        return false;

    ptr->ds = data;

    std::unique_lock<std::mutex> u_lock(ptr->mtx);
    ptr->count++;
    u_lock.unlock();

    ptr->set_dir(dir);
    if (farc_file)
        ptr->set_farc_file(farc_file, cache);
    ptr->set_file(file);

    std::unique_lock<std::mutex> u_lock1(ptr->mtx);
    ptr->count++;
    u_lock1.unlock();
    file_handler_storage_data->list.push_back(ptr);
    return true;
}

bool p_file_handler::read_file(void* data, const char* dir, const char* file) {
    return read_file(data, dir, 0, file, false);
}

bool p_file_handler::read_file(void* data, const char* dir, uint32_t hash, const char* ext) {
    if (!dir || !hash || hash == hash_murmurhash_empty)
        return false;

    if (ptr) {
        if (ptr->not_ready)
            return false;
        free_data();
    }

    std::string file;
    if (!((data_struct*)data)->get_file(dir, hash, ext, file))
        return false;

    ptr = new file_handler;
    if (!ptr)
        return false;

    ptr->ds = data;

    std::unique_lock<std::mutex> u_lock(ptr->mtx);
    ptr->count++;
    u_lock.unlock();

    ptr->set_dir(dir);
    ptr->set_file(file.c_str());

    std::unique_lock<std::mutex> u_lock1(ptr->mtx);
    ptr->count++;
    u_lock1.unlock();
    file_handler_storage_data->list.push_back(ptr);
    return true;
}

void p_file_handler::read_now() {
    while (check_not_ready()) {
        if (file_handler_storage_data->read_in_thread)
            break;

        if (ptr) {
            std::unique_lock<std::mutex> u_lock(ptr->mtx);
            ptr->reading = true;
            if (ptr->not_ready) {
                bool ret;
                if (ptr->farc_file.size())
                    ret = ((data_struct*)ptr->ds)->load_file(
                        ptr, ptr->dir.c_str(), ptr->farc_file.c_str(), file_handler_load_farc_file);
                else
                    ret = ((data_struct*)ptr->ds)->load_file(
                        ptr, ptr->dir.c_str(), ptr->file.c_str(), file_handler_load_file);

                if (ret)
                    ptr->not_ready = false;
            }
            ptr->reading = false;
            u_lock.unlock();
        }

        file_handler_storage_ctrl_list();
    }
}

void p_file_handler::set_callback_data(int32_t index,
    void(*func)(void*, const void*, size_t), void* data) {
    if (ptr)
        ptr->set_callback_data(index, func, data);
}
