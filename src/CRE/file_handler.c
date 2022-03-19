/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include <thread>
#include "file_handler.h"
#include "data.h"
#include "render_context.h"
#include <sys/stat.h>

class file_handler_storage {
public:
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
    ~file_handler_storage();
};

file_handler_storage file_handler_storage_data;

static bool file_handler_load_file(void* data, const char* path, const char* file, uint32_t hash);
static bool file_handler_load_farc_file(void* data, const char* path, const char* file, uint32_t hash);
static void file_handler_storage_ctrl_list();
static void file_handler_storage_thread_ctrl();
static size_t farc_read_handler_get_file_size(farc_read_handler* frh, std::string* file);
static bool farc_read_handler_read(farc_read_handler* frh, std::string* file_path, bool cache);
static bool farc_read_handler_read_data(farc_read_handler* frh, void* data, size_t size, std::string* file);

file_handler::file_handler() : count(), not_ready(true),
reading(), cache(), read_free_func(), size(), data() {

}

file_handler::~file_handler() {
    free(data);
}

void file_handler::call_read_free_func(int32_t index) {
    if (index >= 2)
        return;

    std::unique_lock<std::mutex> u_lock(mtx);
    void* data = this->data;
    void(*read_free_func_func)(void*, void*, size_t) = this->read_free_func[index].func;
    void* read_free_func_data = this->read_free_func[index].data;
    bool ready = this->read_free_func[index].ready;
    this->read_free_func[index].ready = true;
    u_lock.unlock();

    if (!ready && read_free_func_func)
        read_free_func_func(read_free_func_data, data, this->size);
}

void file_handler::set_file(const char* file) {
    std::unique_lock<std::mutex> u_lock(mtx);
    this->file = std::string(file);
    u_lock.unlock();
}

void file_handler::set_farc_file(const char* farc_file, bool cache) {
    std::unique_lock<std::mutex> u_lock(mtx);
    this->farc_file = std::string(farc_file);
    this->cache = cache;
    u_lock.unlock();
}

void file_handler::set_path(const char* path) {
    std::unique_lock<std::mutex> u_lock(mtx);
    this->path = std::string(path);
    u_lock.unlock();
}

void file_handler::set_read_free_func_data(int32_t index, void(* func)(void*, void*, size_t), void* data) {
    std::unique_lock<std::mutex> u_lock(mtx);
    if (index < 2) {
        read_free_func[index].func = func;
        read_free_func[index].data = data;
        read_free_func[index].ready = false;
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

file_handler_storage::file_handler_storage() : farc_read_handler(), thread(), cnd(), exit(),
field_61(), field_64(), read_in_thread(), field_69(), field_6C(), field_70(), field_74() {

}

file_handler_storage::~file_handler_storage() {

}

void file_handler_storage_ctrl() {
    file_handler_storage_ctrl_list();

    std::unique_lock<std::mutex> u_lock(file_handler_storage_data.mtx);
    if (!file_handler_storage_data.deque.size()) {
        for (file_handler*& i : file_handler_storage_data.list)
            if (i->not_ready && !i->reading) {
                std::unique_lock<std::mutex> u_lock(i->mtx);
                i->count++;
                u_lock.unlock();
                file_handler_storage_data.deque.push_back(i);
            }

        if (file_handler_storage_data.deque.size())
            file_handler_storage_data.cnd.notify_all();
    }
    u_lock.unlock();
}

void file_handler_storage_free_thread() {
    std::unique_lock<std::mutex> u_lock(file_handler_storage_data.mtx);
    file_handler_storage_data.exit = true;
    file_handler_storage_data.cnd.notify_one();
    u_lock.unlock();

    file_handler_storage_data.thread->join();
    delete file_handler_storage_data.thread;
    file_handler_storage_data.thread = 0;
}

void file_handler_storage_init_thread() {
    file_handler_storage_data.thread = new std::thread(file_handler_storage_thread_ctrl);
    if (file_handler_storage_data.thread) {
        SetThreadDescription((HANDLE)file_handler_storage_data.thread->native_handle(), L"File Thread");
        //file_handler_storage_data.thread->detach();
    }
}

static bool file_handler_load_file(void* data, const char* path, const char* file, uint32_t hash) {
    std::string file_path = std::string(path);
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
    io_open(&s, file_path.c_str(), "rb");
    bool ret = s.io.stream && io_read(&s, fhndl->data, fhndl->size);
    io_free(&s);
    if (!ret)
        free(fhndl->data);
    return ret;
}

static bool file_handler_load_farc_file(void* data, const char* path, const char* file, uint32_t hash) {
    std::string file_path = std::string(path);
    file_path += file;

    file_handler* fhndl = (file_handler*)data;

    std::unique_lock<std::mutex> u_lock(file_handler_storage_data.mtx);
    bool cache = fhndl->cache;

    farc_read_handler* farc_read_hndl = file_handler_storage_data.farc_read_handler;
    if (farc_read_hndl && farc_read_hndl->file_path != file_path) {
        delete farc_read_hndl;
        file_handler_storage_data.farc_read_handler = 0;
    }

    if (!file_handler_storage_data.farc_read_handler)
        file_handler_storage_data.farc_read_handler = new farc_read_handler;

    farc_read_hndl = file_handler_storage_data.farc_read_handler;
    if (farc_read_hndl && farc_read_handler_read(farc_read_hndl, &file_path, cache)) {
        fhndl->size = farc_read_handler_get_file_size(farc_read_hndl, &fhndl->file);
        if (!fhndl->size)
            fhndl->size = 1;

        fhndl->data = malloc(fhndl->size);
        if (fhndl->data && farc_read_handler_read_data(farc_read_hndl,
            fhndl->data, fhndl->size, &fhndl->file)) {
            u_lock.unlock();
            return true;
        }
    }

    free(fhndl->data);
    u_lock.unlock();
    return false;
}

static void file_handler_storage_ctrl_list() {
    for (std::list<file_handler*>::iterator i = file_handler_storage_data.list.begin();
        i != file_handler_storage_data.list.end();) {
        file_handler* pfhndl = i._Ptr->_Myval;
        if (pfhndl->count == 1) {
            pfhndl->free_data_lock();
            i = file_handler_storage_data.list.erase(i);
        }
        else {
            if (!pfhndl->not_ready && !pfhndl->read_free_func[0].ready)
                pfhndl->call_read_free_func(0);
            i++;
        }
    }
}

extern render_context* rctx_ptr;

static void file_handler_storage_thread_ctrl() {
    std::unique_lock<std::mutex> u_lock(file_handler_storage_data.mtx);
    while (!file_handler_storage_data.exit) {
        file_handler_storage_data.cnd.wait(u_lock);
        if (file_handler_storage_data.exit)
            break;

        for (file_handler*& i : file_handler_storage_data.deque) {
            if (i->not_ready) {
                std::unique_lock<std::mutex> u_lock(i->mtx);
                i->reading = true;
                if (i->not_ready) {
                    if (i->farc_file.size())
                        rctx_ptr->data->load_file(i,
                            i->path.c_str(), i->farc_file.c_str(), file_handler_load_farc_file);
                    else
                        rctx_ptr->data->load_file(i,
                            i->path.c_str(), i->file.c_str(), file_handler_load_file);
                    i->not_ready = false;
                }
                i->reading = false;
                u_lock.unlock();
            }
            i->free_data_lock();
        }
        file_handler_storage_data.deque.clear();
    }
    file_handler_storage_data.exit = false;
    u_lock.unlock();
}

static size_t farc_read_handler_get_file_size(farc_read_handler* frh, std::string* file) {
    if (frh->farc)
        return frh->farc->get_file_size(file->c_str());
    return 0;
}

static bool farc_read_handler_read(farc_read_handler* frh, std::string* file_path, bool cache) {
    if (frh->farc) {
        if (frh->file_path == *file_path && frh->cache == cache)
            return true;

        delete frh->farc;
        frh->farc = 0;
    }

    frh->file_path = *file_path;
    frh->cache = cache;

    struct stat st;
    if (!stat(frh->file_path.c_str(), &st) && st.st_size) {
        size_t size = st.st_size;
        frh->farc = new farc;
        if (frh->farc) {
            frh->farc->read(frh->file_path.c_str(), cache, false);
            return true;
        }
    }
    return false;
}

static bool farc_read_handler_read_data(farc_read_handler* frh, void* data, size_t size, std::string* file) {
    if (frh->farc) {
        farc_file* ff = frh->farc->read_file(file->c_str());
        if (ff) {
            memcpy(data, ff->data, min(size, ff->size));
            if (!frh->cache) {
                free(ff->data);
                ff->data;
            }
            return true;
        }
    }
    return false;
}

p_file_handler::p_file_handler() {
    ptr = 0;
}

p_file_handler::~p_file_handler() {
    if (ptr) {
        if (check_not_ready())
            call_free_func_free_data();
        else
            free_data();
    }
}

void p_file_handler::call_free_func_free_data() {
    if (ptr)
        ptr->call_read_free_func(1);
    free_data();
}

bool p_file_handler::check_not_ready() {
    if (!ptr)
        return false;
    else if (ptr->not_ready)
        return true;
    else
        return !ptr->read_free_func[0].ready;
}

void p_file_handler::free_data() {
    if (!ptr)
        return;

    if (ptr->mtx.try_lock()) {
        for (int32_t i = 0; i < 2; i++)
            ptr->read_free_func[i] = {};
        ptr->mtx.unlock();
    }
    ptr->free_data_lock();
    ptr = 0;
}

void* p_file_handler::get_data() {
    if (!ptr || ptr->not_ready)
        return 0;
    return ptr->data;
}

ssize_t p_file_handler::get_size() {
    if (!ptr || ptr->not_ready || ptr->size < 0)
        return 0;
    return ptr->size;
}

bool p_file_handler::read_file(const char* path, const char* farc_file, const char* file, bool cache) {
    if (!path || !file)
        return false;

    if (ptr) {
        if (ptr->not_ready)
            return false;
        free_data();
    }

    if (!rctx_ptr->data->check_file_exists(path, farc_file ? farc_file : file))
        return false;

    ptr = new file_handler;
    if (!ptr)
        return false;

    std::unique_lock<std::mutex> u_lock(ptr->mtx);
    ptr->count++;
    u_lock.unlock();

    ptr->set_path(path);
    if (farc_file)
        ptr->set_farc_file(farc_file, cache);
    ptr->set_file(file);

    std::unique_lock<std::mutex> u_lock1(ptr->mtx);
    ptr->count++;
    u_lock1.unlock();
    file_handler_storage_data.list.push_back(ptr);
    return true;
}

bool p_file_handler::read_file_path(const char* path, const char* file) {
    return read_file(path, 0, file, false);
}

void p_file_handler::read_now() {
    while (check_not_ready()) {
        if (file_handler_storage_data.read_in_thread)
            break;

        if (ptr) {
            std::unique_lock<std::mutex> u_lock(ptr->mtx);
            ptr->reading = true;
            if (ptr->not_ready) {
                bool ret;
                if (ptr->farc_file.size())
                    ret = ((data_struct*)rctx_ptr->data)->load_file(
                        ptr, ptr->path.c_str(), ptr->farc_file.c_str(), file_handler_load_farc_file);
                else
                    ret = ((data_struct*)rctx_ptr->data)->load_file(
                        ptr, ptr->path.c_str(), ptr->file.c_str(), file_handler_load_file);

                if (ret)
                    ptr->not_ready = false;
            }
            ptr->reading = false;
            u_lock.unlock();
        }

        file_handler_storage_ctrl_list();
    }
}

void p_file_handler::set_read_free_func_data(int32_t index, void(*func)(void*, void*, size_t), void* data) {
    if (ptr)
        ptr->set_read_free_func_data(index, func, data);
}
