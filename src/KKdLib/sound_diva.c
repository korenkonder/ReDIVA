/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "sound_diva.h"
#include "sound_wav.h"
#include "io_path.h"
#include "io_stream.h"

static const int8_t ima_index_table[] = {
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

static void diva_read_wav(diva* d, wchar_t* path, float_t** data);
static void diva_write_wav(diva* d, wchar_t* path, float_t* data);
static void ima_decode(uint8_t value, int32_t* current, int32_t* current_clamp, int8_t* step_index);
static uint8_t ima_encode(int32_t sample, int32_t* current, int32_t* current_clamp, int8_t* step_index);

diva* diva_init() {
    diva* d = force_malloc(sizeof(diva));
    return d;
}

void diva_dispose(diva* d) {
    free(d);
}

void diva_read(diva* d, char* path) {
    wchar_t* path_buf = char_string_to_wchar_t_string(path);
    diva_wread(d, path_buf);
    free(path_buf);
}

void diva_wread(diva* d, wchar_t* path) {
    wchar_t* path_diva = path_wadd_extension(path, L".diva");
    stream* s = io_wopen(path_diva, L"rb");
    if (s->io) {
        uint32_t signature = io_read_uint32_t(s);
        if (signature != 0x41564944)
            goto End;

        io_read_uint32_t(s);
        d->size = io_read_uint32_t(s);
        d->sample_rate = io_read_uint32_t(s);
        d->samples_count = io_read_uint32_t(s);
        d->loop_start = io_read_uint32_t(s);
        d->loop_end = io_read_uint32_t(s);
        d->channels = io_read_uint32_t(s);
        io_set_position(s, io_get_position(s) + 0x20, IO_SEEK_SET);

        uint8_t nibble = 0;
        uint8_t value = 0;
        size_t current_sample = 0;
        size_t ch = d->channels;
        size_t samples_count = d->samples_count;

        float_t* data = force_malloc_s(sizeof(float_t), samples_count * ch);
        int32_t* current = force_malloc_s(sizeof(int32_t), ch);
        int32_t* current_clamp = force_malloc_s(sizeof(int32_t), ch);
        int8_t* step_index = force_malloc(ch);

        float_t* temp_data = data;
        for (size_t i = 0; i < samples_count; i++)
            for (size_t c = 0; c < ch; c++, current_sample++) {
                if (!(current_sample & 1))
                    value = io_read_uint8_t(s);

                nibble = (current_sample & 1 ? value : value >> 4) & 0x0F;
                ima_decode(nibble, current + c, current_clamp + c, step_index + c);
                *temp_data++ = (float_t)(current[c] / 32768.0);
            }
        free(step_index);
        free(current_clamp);
        free(current);

        diva_write_wav(d, path, data);
        free(data);
    }
End:
    io_dispose(s);
    free(path_diva);
}

void diva_write(diva* d, char* path) {
    wchar_t* path_buf = char_string_to_wchar_t_string(path);
    diva_wwrite(d, path_buf);
    free(path_buf);
}

void diva_wwrite(diva* d, wchar_t* path) {
    float_t* data;

    diva_read_wav(d, path, &data);
    if (!data)
        return;

    wchar_t* path_diva = path_wadd_extension(path, L".diva");
    stream* s = io_wopen(path_diva, L"wb");
    if (s->io) {
        io_write_uint32_t(s, 0x41564944);
        io_write_uint32_t(s, 0);
        io_write_uint32_t(s, align_val_divide(d->samples_count * d->channels, 2, 2));
        io_write_uint32_t(s, d->sample_rate);
        io_write_uint32_t(s, d->samples_count);
        io_write_uint32_t(s, 0);
        io_write_uint32_t(s, 0);
        io_write_uint32_t(s, d->channels);
        io_write_uint64_t(s, 0);
        io_write_uint64_t(s, 0);
        io_write_uint64_t(s, 0);
        io_write_uint64_t(s, 0);

        uint8_t nibble = 0;
        uint8_t value = 0;
        size_t current_sample = 0;
        size_t ch = d->channels;
        size_t samples_count = d->samples_count;

        int32_t* current = force_malloc_s(sizeof(int32_t), ch);
        int32_t* current_clamp = force_malloc_s(sizeof(int32_t), ch);
        int8_t* step_index = force_malloc(ch);

        float_t* temp_data = data;
        for (size_t i = 0; i < samples_count; i++)
            for (size_t c = 0; c < ch; c++, current_sample++) {
                nibble = ima_encode((int32_t)round(*temp_data++ * 32768.0),
                    current + c, current_clamp + c, step_index + c);
                value |= current_sample & 1 ? nibble : nibble << 4;
                if (current_sample & 1) {
                    io_write_uint8_t(s, value);
                    value = 0;
                }
            }

        if (current_sample & 1)
            io_write_uint8_t(s, value);
        free(step_index);
        free(current_clamp);
        free(current);
    }
    io_dispose(s);
    free(path_diva);
    free(data);
}

static void diva_read_wav(diva* d, wchar_t* path, float_t** data) {
    *data = 0;
    size_t samples = 0;
    wchar_t* path_wav = path_wadd_extension(path, L".wav");
    wav* w = wav_init();
    wav_wread(w, path_wav, data, &samples);
    d->channels = w->channels;
    d->sample_rate = w->sample_rate;
    d->samples_count = (uint32_t)samples;
    wav_dispose(w);
    free(path_wav);
}

static void diva_write_wav(diva* d, wchar_t* path, float_t* data) {
    wchar_t* path_wav = path_wadd_extension(path, L".wav");
    wav* w = wav_init();
    w->bytes = 4;
    w->channels = d->channels;
    w->format = 0x03;
    w->sample_rate = d->sample_rate;
    wav_wwrite(w, path_wav, data, d->samples_count);
    wav_dispose(w);
    free(path_wav);
}

static void ima_decode(uint8_t value, int32_t* current, int32_t* current_clamp, int8_t* step_index) {
    int32_t c = *current;
    int32_t cc = *current_clamp;
    int32_t si = *step_index;
    int16_t step = ima_step_table[si];

    int32_t diff = step >> 3;
    if ((value & 1) == 1) diff += step >> 2;
    if ((value & 2) == 2) diff += step >> 1;
    if ((value & 4) == 4) diff += step;

    if ((value & 8) == 8) {
        cc -= diff;
        c = cc;
        if (cc < -0x8000)
            cc = -0x8000;
    }
    else {
        cc += diff;
        c = cc;
        if (cc > 0x7FFF)
            cc = 0x7FFF;
    }

    si += ima_index_table[value & 0x07];
    if (si < 0)
        si = 0;
    if (si > 88)
        si = 88;
    *current = c;
    *current_clamp = cc;
    *step_index = si;
}

static uint8_t ima_encode(int32_t sample, int32_t* current, int32_t* current_clamp, int8_t* step_index) {
    int32_t c = *current;
    int32_t cc = *current_clamp;
    int32_t si = *step_index;
    uint8_t value = 0;
    int16_t step = ima_step_table[si];

    int32_t delta = sample - c;

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

    if ((value & 8) == 8) {
        cc -= diff;
        c = cc;
        if (cc < -0x8000)
            cc = -0x8000;
    }
    else {
        cc += diff;
        c = cc;
        if (cc > 0x7FFF)
            cc = 0x7FFF;
    }

    si += ima_index_table[value & 0x07];
    if (si < 0)
        si = 0;
    if (si > 88)
        si = 88;
    *current = c;
    *current_clamp = cc;
    *step_index = si;
    return value;
}
