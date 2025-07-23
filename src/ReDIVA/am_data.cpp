/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "am_data.hpp"

#define WrapDef(name) \
class name : public WrapInterface { \
public: \
    name(); \
    virtual ~name(); \
\
    virtual uint8_t get_index() override; \
    virtual void set_index(uint8_t value) override; \
    virtual bool operator ==(uint8_t value) override; \
    virtual bool operator <(uint8_t value) override; \
    virtual bool operator <=(uint8_t value) override; \
    virtual bool operator >(uint8_t value) override; \
    virtual bool operator >=(uint8_t value) override; \
}

#define WrapImpl(name, val) \
name::name() { } \
name::~name() { } \
uint8_t name::get_index() { return val; } \
void name::set_index(uint8_t value) { val = value; } \
bool name::operator ==(uint8_t value) { return val == value; } \
bool name::operator <(uint8_t value) { return val < value; } \
bool name::operator <=(uint8_t value) { return val <= value; } \
bool name::operator >(uint8_t value) { return val > value; } \
bool name::operator >=(uint8_t value) { return val >= value; }

WrapDef(Wrap_advertise_sound_volume);
WrapDef(Wrap_give_up);
WrapDef(Wrap_stage_num);
WrapDef(Wrap_internal_unable_to_assure_next_stage);
WrapDef(Wrap_printer);
WrapDef(Wrap_internal_allow_continue);
WrapDef(Wrap_deprecated_advertise_sound);

struct struc_887 {
    uint32_t field_0;
    uint32_t field_4;
    union {
        struct {
            uint32_t flag_0 : 3;
            uint32_t flag_3 : 1;
            uint32_t flag_4 : 1;
            uint32_t flag_5 : 1;
            uint32_t flag_6 : 1;
            uint32_t dummy : 25;
        } m;
        uint32_t w;
    } field_8;
    union {
        struct {
            uint32_t dummy : 30;
            uint32_t flag_30 : 1;
            uint32_t flag_31 : 1;
        } m;
        uint32_t w;
    } field_C;
};

Wrap_advertise_sound_volume wrap_advertise_sound_volume;
Wrap_give_up wrap_give_up;
Wrap_stage_num wrap_stage_num;
Wrap_internal_unable_to_assure_next_stage wrap_internal_unable_to_assure_next_stage;
Wrap_printer wrap_printer;
Wrap_internal_allow_continue wrap_internal_allow_continue;
Wrap_deprecated_advertise_sound wrap_deprecated_advertise_sound;

Wrap_collection wrap_collection;

struc_887 stru_140FA2470 = { 0x10031519, (uint32_t)-1, 0, 0 };

static const Wrap_data stru_1409BE040[] = {
    {   0,   "0" },
    {  25,  "25" },
    {  50,  "50" },
    {  75,  "75" },
    { 100, "100" },
};

static const Wrap_data stru_1409BE018[] = {
    { 0, "OFF" },
    { 1,  "ON" },
};

static const Wrap_data stru_1409BE0A8[] = {
    { 0, "2" },
    { 1, "3" },
};

static void sub_1401E8700();

WrapInterface::WrapInterface() : default_index(), min_index(), max_index(), arr() {

}

WrapInterface::~WrapInterface() {

}

const char* p_WrapInterface::get_name() const {
    size_t index = (uint8_t)ptr->get_index();
    if (ptr->min_index > index || index >= ptr->max_index)
        index = ptr->default_index;
    return ptr->arr[index].name;
}

int32_t p_WrapInterface::get_value() const {
    size_t index = (uint8_t)ptr->get_index();
    if (ptr->min_index > index || index >= ptr->max_index)
        index = ptr->default_index;
    return ptr->arr[index].value;
}

inline void p_WrapInterface::init(size_t default_index, size_t min_index, size_t max_index, const Wrap_data* arr) {
    ptr->default_index = default_index;
    ptr->min_index = min_index;
    ptr->max_index = max_index;
    ptr->arr = arr;
}

void p_WrapInterface::reset() {
    ptr->set_index((uint8_t)ptr->default_index);
}

void p_WrapInterface::step(int32_t direction) {
    uint8_t index = (uint8_t)ptr->get_index();
    if (direction > 0) {
        index++;
        if (ptr->min_index > index || index >= ptr->max_index)
            index = (uint8_t)ptr->min_index;
    }
    else if (direction < 0) {
        index--;
        if (ptr->min_index > index || index >= ptr->max_index) {
            index = (uint8_t)ptr->max_index;
            index--;
        }
    }
    ptr->set_index(index);
}

void Wrap_collection::init() {
    advertise_sound_volume.ptr = &wrap_advertise_sound_volume;
    give_up.ptr = &wrap_give_up;
    stage_num.ptr = &wrap_stage_num;
    internal_unable_to_assure_next_stage.ptr = &wrap_internal_unable_to_assure_next_stage;
    printer.ptr = &wrap_printer;
    internal_allow_continue.ptr = &wrap_internal_allow_continue;
    deprecated_advertise_sound.ptr = &wrap_deprecated_advertise_sound;

    internal_allow_continue.init(0, stru_1409BE018);
    advertise_sound_volume.init(4, stru_1409BE040);
    give_up.init(0, stru_1409BE018);
    stage_num.init(0, stru_1409BE0A8);
    internal_unable_to_assure_next_stage.init(0, stru_1409BE018);
    printer.init(0, stru_1409BE018);
}

void Wrap_collection::reset() {
    advertise_sound_volume.reset();
    give_up.reset();
    stage_num.reset();
    internal_unable_to_assure_next_stage.reset();
    printer.reset();
    internal_allow_continue.reset();
    deprecated_advertise_sound.reset();
}

void am_data_init() {
    wrap_collection.init();

    //sub_1401F4430();

    /*if (SUCCEEDED(sub_14066C240(0, &stru_140FA2470, 0x10))) {
        if (FAILED(sub_14066C240(2, &stru_140FA2480, 0x14)))
            sub_1401E9210(&stru_140FA2480);

        if (FAILED(sub_14066C240(3, &stru_140FA2494, 0x28)))
            sub_1401E92D0(&stru_140FA2494);

        if (FAILED(sub_14066C240(1, &stru_140FA24BC, 0x19CB8))) {
            stru_140FA24BC.field_0 = 0x15052916;
            stru_140FA24BC.field_4 = 0x19CB8;
            stru_140FA24BC.game_assignments.init();
            stru_140FA24BC.field_48 = 0;
            stru_140FA24BC.field_4C = 0;
            stru_140FA24BC.field_50 = 0;
            sub_1401E8FE0(&stru_140FA24BC.field_54);
            sub_1401E9060(&stru_140FA24BC.field_F4);
            stru_140FA24BC.field_19CB0 = 0;
        }

        if (stru_140FA2470.field_0 != 0x10031519)
            sub_1401E8700();

        //sub_1401E9920(&stru_140FA24BC);
    }
    else*/
        sub_1401E8700();

    //sub_1401E87E0();
}

const Wrap_collection* wrap_collection_get() {
    return &wrap_collection;
}

Wrap_collection* wrap_collection_get_write() {
    return &wrap_collection;
}

WrapImpl(Wrap_advertise_sound_volume, stru_140FA2470.field_8.m.flag_0);
WrapImpl(Wrap_give_up, stru_140FA2470.field_8.m.flag_3);
WrapImpl(Wrap_stage_num, stru_140FA2470.field_8.m.flag_4);
WrapImpl(Wrap_internal_unable_to_assure_next_stage, stru_140FA2470.field_8.m.flag_5);
WrapImpl(Wrap_printer, stru_140FA2470.field_8.m.flag_6);
WrapImpl(Wrap_internal_allow_continue, stru_140FA2470.field_C.m.flag_30);
WrapImpl(Wrap_deprecated_advertise_sound, stru_140FA2470.field_C.m.flag_31);

static void sub_1401E8700() {
    stru_140FA2470 = {};
    wrap_collection.reset();
    //sub_1401E92D0(&stru_140FA2494);
    //sub_1401E9210(&stru_140FA2480);
    /*stru_140FA24BC.field_0 = 0x15052916;
    stru_140FA24BC.field_4 = 0x19CB8;
    stru_140FA24BC.game_assignments.init();
    stru_140FA24BC.field_48 = 0;
    stru_140FA24BC.field_4C = 0;
    stru_140FA24BC.field_50 = 0;
    sub_1401E8FE0(&stru_140FA24BC.field_54);
    sub_1401E9060(&stru_140FA24BC.field_F4);
    stru_140FA24BC.field_19CB0 = 0;*/
}
