/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"

struct Wrap_data {
    int32_t value;
    const char* name;
};

class WrapInterface {
public:
    size_t default_index;
    size_t min_index;
    size_t max_index;
    const Wrap_data* arr;

    WrapInterface();
    virtual ~WrapInterface();

    virtual uint8_t get_index() = 0;
    virtual void set_index(uint8_t value) = 0;
    virtual bool operator ==(uint8_t value) = 0;
    virtual bool operator <(uint8_t value) = 0;
    virtual bool operator <=(uint8_t value) = 0;
    virtual bool operator >(uint8_t value) = 0;
    virtual bool operator >=(uint8_t value) = 0;
};

struct p_WrapInterface {
    friend struct Wrap_collection;

    WrapInterface* ptr;

    const char* get_name() const;
    int32_t get_value() const;

protected:
    void init(size_t default_index, size_t min_index, size_t max_index, const Wrap_data* arr);
    void reset();
    void step(int32_t direction);

    template <size_t size>
    inline void init(size_t default_index, const Wrap_data(&arr)[size]) {
        init(default_index, 0, size, arr);
    }
};

struct Wrap_collection {
    p_WrapInterface advertise_sound_volume;
    p_WrapInterface give_up;
    p_WrapInterface stage_num;
    p_WrapInterface internal_unable_to_assure_next_stage;
    p_WrapInterface printer;
    p_WrapInterface internal_allow_continue;
    p_WrapInterface deprecated_advertise_sound;

    void init();
    void reset();
};

extern void am_data_init();

extern const Wrap_collection* wrap_collection_get();
extern Wrap_collection* wrap_collection_get_write();
