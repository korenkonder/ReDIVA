/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "diva.hpp"
#include "wav.hpp"
#include "../io/file_stream.hpp"
#include "../io/path.hpp"
#include "../str_utils.hpp"

struct ima_storage {
    int32_t step_index;
    int32_t current;
    int32_t current_clamp;
};

static const int8_t ima_index_table[] = {
    -1, -1, -1, -1, 2, 4, 6, 8,
    -1, -1, -1, -1, 2, 4, 6, 8,
};

static const int16_t ima_step_table[] = {
        7,     8,     9,    10,    11,    12,    13,    14,
       16,    17,    19,    21,    23,    25,    28,    31,
       34,    37,    41,    45,    50,    55,    60,    66,
       73,    80,    88,    97,   107,   118,   130,   143,
      157,   173,   190,   209,   230,   253,   279,   307,
      337,   371,   408,   449,   494,   544,   598,   658,
      724,   796,   876,   963,  1060,  1166,  1282,  1411,
     1552,  1707,  1878,  2066,  2272,  2499,  2749,  3024,
     3327,  3660,  4026,  4428,  4871,  5358,  5894,  6484,
     7132,  7845,  8630,  9493, 10442, 11487, 12635, 13899,
    15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794,
    32767
};

static bool diva_read_inner(diva* d, stream& s, float_t*& data);
static void diva_write_inner(diva* d, stream& s, float_t* data);
static void diva_read_wav(diva* d, const wchar_t* path, float_t*& data);
static void diva_write_wav(diva* d, const wchar_t* path, float_t* data);
static void ima_decode(uint8_t value, ima_storage* storage);
static uint8_t ima_encode(int32_t sample, ima_storage* storage);

diva::diva() : size(), sample_rate(), samples_count(), loop_start(), loop_end(), channels() {

}

diva::~diva() {

}

void diva::read(const char* path) {
    if (!path)
        return;

    char* path_diva = str_utils_add(path, ".diva");
    file_stream s;
    s.open(path_diva, "rb");
    if (s.check_not_null()) {
        float_t* data = 0;
        if (diva_read_inner(this, s, data)) {
            wchar_t* path_temp = utf8_to_utf16(path);
            diva_write_wav(this, path_temp, data);
            free_def(path_temp);
            free_def(data);
        }
    }
    free_def(path_diva);
}

void diva::read(const wchar_t* path) {
    if (!path)
        return;

    wchar_t* path_diva = str_utils_add(path, L".diva");
    file_stream s;
    s.open(path_diva, L"rb");
    if (s.check_not_null()) {
        float_t* data = 0;
        if (diva_read_inner(this, s, data)) {
            diva_write_wav(this, path, data);
            free_def(data);
        }
    }
    free_def(path_diva);
}

void diva::write(const char* path) {
    if (!path)
        return;

    wchar_t* path_temp = utf8_to_utf16(path);
    float_t* data = 0;
    diva_read_wav(this, path_temp, data);
    if (!data) {
        free_def(path_temp);
        return;
    }

    char* path_diva = str_utils_add(path, ".diva");
    file_stream s;
    s.open(path_diva, "wb");
    if (s.check_not_null())
        diva_write_inner(this, s, data);
    free_def(path_diva);
    free_def(data);
    free_def(path_temp);
}

void diva::write(const wchar_t* path) {
    if (!path)
        return;

    float_t* data = 0;
    diva_read_wav(this, path, data);
    if (!data)
        return;

    wchar_t* path_diva = str_utils_add(path, L".diva");
    file_stream s;
    s.open(path_diva, L"wb");
    if (s.check_not_null())
        diva_write_inner(this, s, data);
    free_def(path_diva);
    free_def(data);
}

void diva_dispose(diva* d) {
    free_def(d);
}

static bool diva_read_inner(diva* d, stream& s, float_t*& data) {
    uint32_t signature = s.read_uint32_t_reverse_endianness(true);
    if (signature != 'DIVA')
        return false;

    s.read_uint32_t();
    d->size = s.read_uint32_t();
    d->sample_rate = s.read_uint32_t();
    d->samples_count = s.read_uint32_t();
    d->loop_start = s.read_uint32_t();
    d->loop_end = s.read_uint32_t();
    d->channels = s.read_uint32_t();
    s.set_position(s.get_position() + 0x20, SEEK_SET);

    uint8_t nibble = 0;
    uint8_t value = 0;
    size_t current_sample = 0;
    size_t ch = d->channels;
    size_t samples_count = d->samples_count;

    data = force_malloc_s(float_t, samples_count * ch);
    ima_storage* storage = force_malloc_s(ima_storage, ch);

    float_t* temp_data = data;
    for (size_t i = 0; i < samples_count; i++)
        for (size_t c = 0; c < ch; c++, current_sample++) {
            if (!(current_sample & 0x01))
                value = s.read_uint8_t();

            nibble = (current_sample & 1 ? value : value >> 4) & 0x0F;
            ima_decode(nibble, &storage[c]);
            *temp_data++ = (float_t)(storage[c].current / 32768.0);
        }
    free_def(storage);
    return true;
}

static void diva_write_inner(diva* d, stream& s, float_t* data) {
    s.write_uint32_t_reverse_endianness('DIVA', true);
    s.write_uint32_t(0);
    s.write_uint32_t(align_val_divide(d->samples_count * d->channels, 2, 2));
    s.write_uint32_t(d->sample_rate);
    s.write_uint32_t(d->samples_count);
    s.write_uint32_t(0);
    s.write_uint32_t(0);
    s.write_uint32_t(d->channels);
    s.write_uint64_t(0);
    s.write_uint64_t(0);
    s.write_uint64_t(0);
    s.write_uint64_t(0);

    uint8_t nibble = 0;
    uint8_t value = 0;
    size_t current_sample = 0;
    size_t ch = d->channels;
    size_t samples_count = d->samples_count;

    ima_storage* storage = force_malloc_s(ima_storage, ch);

    float_t* temp_data = data;
    for (size_t i = 0; i < samples_count; i++)
        for (size_t c = 0; c < ch; c++, current_sample++) {
            nibble = ima_encode((int32_t)round(*temp_data++ * 32768.0), &storage[c]);
            value |= current_sample & 1 ? nibble : nibble << 4;
            if (current_sample & 1) {
                s.write_uint8_t(value);
                value = 0;
            }
        }

    if (current_sample & 1)
        s.write_uint8_t(value);
    free_def(storage);
}

static void diva_read_wav(diva* d, const wchar_t* path, float_t*& data) {
    data = 0;
    size_t samples = 0;
    wchar_t* path_av = str_utils_add(path, L".wav");
    wav w;
    w.read(path_av, data, samples);
    d->channels = w.channels;
    d->sample_rate = w.sample_rate;
    d->samples_count = (uint32_t)samples;
    free_def(path_av);
}

static void diva_write_wav(diva* d, const wchar_t* path, float_t* data) {
    wchar_t* path_av = str_utils_add(path, L".wav");
    wav w;
    w.bytes = 4;
    w.channels = d->channels;
    w.format = 0x03;
    w.sample_rate = d->sample_rate;
    w.write(path_av, data, d->samples_count);
    free_def(path_av);
}

static void ima_decode(uint8_t value, ima_storage* storage) {
    int32_t current = storage->current;
    int32_t current_clamp = storage->current_clamp;
    int32_t step_index = storage->step_index;
    int16_t step = ima_step_table[step_index];

    int32_t diff = step >> 3;
    if (value & 1)
        diff += step >> 2;
    if (value & 2)
        diff += step >> 1;
    if (value & 4)
        diff += step;
    if ((value & 8) == 8)
        diff = -diff;

    current_clamp += diff;
    current = current_clamp;
    current_clamp = clamp_def(current_clamp, -0x8000, 0x7FFF);

    step_index += ima_index_table[value];
    step_index = clamp_def(step_index, 0, 88);

    storage->current = current;
    storage->current_clamp = current_clamp;
    storage->step_index = step_index;
}

static uint8_t ima_encode(int32_t sample, ima_storage* storage) {
    int32_t current = storage->current;
    int32_t current_clamp = storage->current_clamp;
    int32_t step_index = storage->step_index;
    uint8_t value = 0;
    int16_t step = ima_step_table[step_index];

    int32_t delta = sample - current;

    if (delta < 0) {
        value |= 8;
        delta = -delta;
    }

    int32_t diff = step >> 3;
    if (delta > step) {
        value |= 4;
        diff += step;
        delta -= step;
    }

    step >>= 1;
    if (delta > step) {
        value |= 2;
        diff += step;
        delta -= step;
    }

    step >>= 1;
    if (delta > step) {
        value |= 1;
        diff += step;
    }

    if ((value & 8) == 8)
        diff = -diff;

    current_clamp += diff;
    current = current_clamp;
    current_clamp = clamp_def(current_clamp, -0x8000, 0x7FFF);

    step_index += ima_index_table[value];
    step_index = clamp_def(step_index, 0, 88);

    storage->current = current;
    storage->current_clamp = current_clamp;
    storage->step_index = step_index;
    return value;
}
