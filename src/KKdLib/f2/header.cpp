/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "header.hpp"

static uint8_t calc_crc8(uint8_t* data, uint32_t size, uint8_t seed = 0xFF);
static uint16_t calc_crc16_ccitt(uint8_t* data, uint32_t size, uint16_t seed = 0xFFFF);

f2_header::f2_header() : signature(), data_size(), length(), attrib(), depth(), crc_header(), crc_data(),
section_size(), version(), custom(), murmurhash(), unknown1(), inner_signature(), unknown2() {
    signature = '    ';
    set_length(F2_HEADER_DEFAULT_LENGTH);
    attrib.set_type(0x01);
}

f2_header::f2_header(uint32_t signature, uint32_t size, uint32_t depth, bool align) : f2_header() {
    this->signature = reverse_endianness_uint32_t(signature);
    set_length(F2_HEADER_DEFAULT_LENGTH);
    set_section_size(size, align);
    set_data_size(size, align);

    attrib.m.unk0 = 0x00;
    attrib.set_align(align);
    attrib.set_big_endian(false);
    set_depth(depth);
    version &= 0xFFF0FFFF;
}

void f2_header::apply_xor() {
    if (attrib.get_xor_data() > 0x01)
        return;

    if (attrib.get_xor_data()) {
        remove_xor();
        apply_xor();
        return;
    }

    uint8_t* data = get_section_data();
    uint8_t* data_end = data + get_section_size();

    uint8_t* iv_begin = (uint8_t*)this;
    uint8_t* iv = iv_begin;

    uint8_t xor_val = 0xA5;
    while (iv != iv_begin + 0x08)
        xor_val ^= *iv++;

    if (!xor_val || xor_val == 0xFF)
        xor_val = 0x01;

    while (data != data_end) {
        xor_val ^= *data;
        *data++ = xor_val;
    }

    attrib.set_xor_data(0x02);
}

void f2_header::calc_crc() {
    attrib.set_crc(true);

    uint8_t crc_header = calc_crc8((uint8_t*)&section_size, 0x0C, calc_crc8((uint8_t*)this, 0x11));
    if (attrib.get_type())
        this->crc_header = crc_header;
    else
        abort();

    uint16_t crc_data = calc_crc_data();
    if (attrib.get_type())
        this->crc_data = crc_data;
    else
        abort();
}

uint16_t f2_header::calc_crc_data() const {
    uint16_t hash = calc_crc16_ccitt((uint8_t*)this + F2_HEADER_DEFAULT_LENGTH,
        get_length() - F2_HEADER_DEFAULT_LENGTH);
    return calc_crc16_ccitt(get_section_data(), get_section_size(), hash);
}

void f2_header::read(stream& s) {
    *this = {};

    if (s.check_null())
        return;

    s.read(this, F2_HEADER_DEFAULT_LENGTH);
    if (get_length() > F2_HEADER_DEFAULT_LENGTH)
        s.read((uint8_t*)this + F2_HEADER_DEFAULT_LENGTH,
            min_def(get_length(), F2_HEADER_EXTENDED_LENGTH) - F2_HEADER_DEFAULT_LENGTH);
}

void f2_header::remove_xor() {
    if (attrib.get_xor_data() > 0x01) {
        if (attrib.get_xor_data() != 0x03) {
            uint8_t* data = get_section_data();
            uint8_t* data_end = data + get_section_size();

            uint8_t* iv_begin = (uint8_t*)this;
            uint8_t* iv = iv_begin;

            uint8_t xor_val = 0xA5;

            while (iv != iv_begin + 0x08)
                xor_val ^= *iv++;

            if (!xor_val || xor_val == 0xFF)
                xor_val = 0x01;

            while (data != data_end) {
                uint8_t temp = *data;
                *data++ ^= xor_val;
                xor_val = temp;
            }

            attrib.set_xor_data(0x00);
        }
    }
    else {
        if (attrib.get_xor_data() != 0x00) {
            uint8_t* data = get_section_data();
            uint8_t* data_end = data + min_def(0x200U, get_section_size());

            uint8_t* iv_begin = (uint8_t*)this;
            uint8_t* iv = iv_begin;

            uint8_t xor_val = 0xA5;

            while (iv != iv_begin + 0x08)
                xor_val ^= *iv++;

            if (!xor_val || xor_val == 0xFF)
                xor_val = 0x01;

            while (data != data_end) {
                uint8_t temp = *data;
                *data++ ^= xor_val;
                xor_val = temp;
            }

            attrib.set_xor_data(0x00);
        }
    }
}

bool f2_header::validate_crc(int32_t not_ignore_crc) {
    if (!not_ignore_crc && !attrib.get_crc())
        return true;

    uint8_t crc_header;
    if (attrib.get_type())
        crc_header = this->crc_header;
    else {
        abort();
        crc_header = 0xDE;
    }

    if (crc_header == calc_crc8((uint8_t*)&this->section_size, 0x0C, calc_crc8((uint8_t*)this, 0x11))) {
        uint16_t crc_data;
        if (attrib.get_type())
            crc_data = this->crc_data;
        else {
            abort();
            crc_data = 0xDEAD;
        }

        if (crc_data == calc_crc_data())
            return true;
    }
    return false;
}

void f2_header::write(stream& s) {
    if (s.check_null())
        return;

    s.write(this, get_length());
}

// CRC8 Polynomial 0x8D
// 0x813495E0 in PCSB01007
static uint8_t calc_crc8(uint8_t* data, uint32_t size, uint8_t seed) {
    static const uint8_t crc8_table_small[] = {
        0x00, 0x8D, 0x97, 0x1A, 0xA3, 0x2E, 0x34, 0xB9,
        0xCB, 0x46, 0x5C, 0xD1, 0x68, 0xE5, 0xFF, 0x72,
    };

    uint8_t crc = seed;
    uint8_t* data_end = data + size;
    while (data != data_end) {
        crc ^= *data++;
        crc = crc8_table_small[crc >> 0x04] ^ (crc << 0x04);
        crc = crc8_table_small[crc >> 0x04] ^ (crc << 0x04);
    }
    return crc;
}

// CRC16-CCITT
// 0x81349476 in PCSB01007
static uint16_t calc_crc16_ccitt(uint8_t* data, uint32_t size, uint16_t seed) {
    static const uint16_t crc16_ccitt_table_small[] = {
        0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
        0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
    };

    uint16_t crc = seed;
    uint8_t* data_end = data + size;
    while (data != data_end) {
        crc ^= (uint16_t)*data++ << 0x08;
        crc = crc16_ccitt_table_small[crc >> 0x0C] ^ (crc << 0x04);
        crc = crc16_ccitt_table_small[crc >> 0x0C] ^ (crc << 0x04);
    }
    return crc;
}

