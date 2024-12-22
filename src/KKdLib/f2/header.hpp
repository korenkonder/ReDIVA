/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../default.hpp"
#include "../io/stream.hpp"

#define F2_HEADER_BASE_ALIGNMENT (0x10U)
#define F2_HEADER_DEFAULT_LENGTH (0x20U)
#define F2_HEADER_EXTENDED_LENGTH (0x40U)

struct f2_header_attrib_member {
friend struct f2_header;
friend union f2_header_attrib;
private:
    uint32_t unk0 : 3;
    uint32_t align : 1;
    uint32_t unk1 : 5;
    uint32_t aes : 1;
    uint32_t gzip : 1;
    uint32_t unk2 : 6;
    uint32_t xor_data : 2;
    uint32_t crc : 1;
    uint32_t unk3 : 7;
    uint32_t big_endian : 1;
    uint32_t type : 4;
};

union f2_header_attrib {
    f2_header_attrib_member m;
    uint32_t w;

    inline bool get_aes() const {
        return !!m.aes;
    }

    inline bool get_align() const {
        return !!m.align;
    }

    inline bool get_big_endian() const {
        return !!m.big_endian;
    }

    inline bool get_crc() const {
        return !!m.crc;
    }

    inline bool get_gzip() const {
        return !!m.gzip;
    }

    inline uint32_t get_type() const {
        return m.type;
    }

    inline uint32_t get_xor_data() const {
        return m.xor_data;
    }

    inline void set_aes(bool value) {
        m.aes = value ? 0x01 : 0x00;
    }

    inline void set_align(bool value) {
        m.align = value ? 0x01 : 0x00;
    }

    inline void set_big_endian(bool value) {
        m.big_endian = value ? 0x01 : 0x00;
    }

    inline void set_crc(bool value) {
        m.crc = value ? 0x01 : 0x00;
    }

    inline void set_gzip(bool value) {
        m.gzip = value ? 0x01 : 0x00;
    }

    inline void set_type(uint32_t value) {
        m.type = value;
    }

    inline void set_xor_data(uint32_t value) {
        m.xor_data = value;
    }
};

struct f2_header {
    union {
        char signature_char[0x04];  // 0x00
        uint32_t signature;         // 0x00
    };
private:
    uint32_t data_size;             // 0x04
    uint32_t length;                // 0x08
public:
    f2_header_attrib attrib;        // 0x0C
private:
    uint8_t depth;                  // 0x10
public:
    uint8_t crc_header;             // 0x11
    uint16_t crc_data;              // 0x12
private:
    uint32_t section_size;          // 0x14
public:
    uint32_t version;               // 0x18
    uint32_t custom;                // 0x1C
    uint32_t murmurhash;            // 0x20
    uint32_t unknown1[3];           // 0x24
    uint32_t inner_signature;       // 0x30
    uint32_t unknown2[3];           // 0x34

    f2_header();
    f2_header(uint32_t signature, uint32_t size = 0, uint32_t depth = 0, bool align = false);

    void apply_xor();
    void calc_crc();
    uint16_t calc_crc_data() const;
    void read(stream& s);
    void remove_xor();
    bool validate_crc(int32_t not_ignore_crc = 0);
    void write(stream& s);

    inline uint32_t get_data_size() const {
        if (attrib.get_type() < 0x02)
            return data_size;
        else
            return F2_HEADER_BASE_ALIGNMENT * data_size;
    }

    inline uint32_t get_depth() const {
        return depth;
    }

    inline uint32_t get_length() const {
        if (attrib.get_type() < 0x02)
            return length;
        else
            return F2_HEADER_BASE_ALIGNMENT * length;
    }

    inline uint32_t get_raw_data_size() const {
        return data_size;
    }

    inline uint8_t* get_section_data() const {
        if (get_section_size())
            return (uint8_t*)this + get_length();
        return 0;
    }

    inline uint32_t get_section_size() const {
        if (!attrib.get_type())
            return data_size;
        else if (attrib.get_type() == 0x01)
            return section_size;
        else
            return F2_HEADER_BASE_ALIGNMENT * section_size;
    }

    inline void reset_crc() {
        attrib.set_crc(false);
        crc_header = 0x00;
        crc_data = 0x00;
    }

    inline void set_data_size(uint32_t value, bool align = false) {
        if (attrib.get_type() < 0x02)
            data_size = value;
        else if (!(value & 0x0F) || !align)
            data_size = value / F2_HEADER_BASE_ALIGNMENT;
        else
            abort();
    }

    inline void set_depth(uint32_t value) {
        if (attrib.get_type())
            depth = value;
        else
            abort();
    }

    inline void set_length(uint32_t value) {
        if (attrib.get_type() < 0x02)
            length = value;
        else
            length = value / F2_HEADER_BASE_ALIGNMENT;
    }

    inline void set_section_size(uint32_t value, bool align = false) {
        if (attrib.get_type()) {
            if (attrib.get_type() < 0x02)
                section_size = value;
            else if (!(section_size & 0x0F) || !align)
                section_size = value / F2_HEADER_BASE_ALIGNMENT;
            else
                abort();
        }
    }
};
