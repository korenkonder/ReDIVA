/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "stream.hpp"

class memory_stream : public stream {
private:
    struct data {
        std::vector<uint8_t>::iterator data;
        std::vector<uint8_t> vec;
    } data;

public:
    memory_stream();
    virtual ~memory_stream();

    virtual int flush() override;
    virtual void close() override;
    virtual bool can_be_null() override;
    virtual bool check_null() override;
    virtual bool check_not_null() override;

    virtual void align_read(size_t align) override;
    virtual void align_write(size_t align) override;
    virtual size_t read(size_t count) override;
    virtual size_t read(void* buf, size_t count) override;
    virtual size_t read(void* buf, size_t size, size_t count) override;
    virtual size_t write(size_t count) override;
    virtual size_t write(const void* buf, size_t count) override;
    virtual size_t write(const void* buf, size_t size, size_t count) override;
    virtual int32_t read_char() override;
    virtual int32_t write_char(char c) override;
    virtual int64_t get_length() override;
    virtual int64_t get_position() override;
    virtual int32_t set_position(int64_t pos, int32_t seek) override;

    void open();
    void open(const void* data, size_t size);
    void open(std::vector<uint8_t>& data);
    void copy(void** data, size_t* size);
    void copy(std::vector<uint8_t>& data);

    template <typename T>
    size_t read_data(T& data) {
        return read(&data, sizeof(T));
    }

    template <typename T>
    size_t write_data(const T& data) {
        return write(&data, sizeof(T));
    }
};
