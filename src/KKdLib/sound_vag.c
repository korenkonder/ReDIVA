/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "sound_vag.h"
#include "sound_wav.h"
#include "io_path.h"
#include "io_stream.h"

static const int8_t shift_factor_table[] = {
    0, 1, 2, 3, 4, 5, 6, 7, -8, -7, -6, -5, -4, -3, -2, -1
};

static const int16_t hevag1_table[] = {
         0,   7680,  14720,  12544,  15616,  14731,  14507,  13920,
     13133,  12028,  10764,   9359,   7832,   6201,   4488,   2717,
       910,   -910,  -2717,  -4488,  -6201,  -7832,  -9359, -10764,
    -12028, -13133, -13920, -14507, -14731,   5376,  -6400, -10496,
      -167,  -7430,  -8001,   6018,   3798,  -8237,   9199,  13021,
     13112,  -1668,   7819,   9571,  10032,  -4745,  -5896,  -1193,
      2783,  -7334,   6127,   9457,   7876,  -7172,  -7358,  -9170,
     -2638,   1873,   9214,  13204,  12437,  -2653,   9331,   1642,
      4246,  -8988,  -2562,   3182,   7937,  10069,   8400,  -8529,
      9477,     75,  -9143,  -7270,  -2740,   8993,  13101,   9543,
      5272,  -7696,   7309,  10275,  10940,     24,  -8122,  -8511,
       326,   8895,  12073,   8729,  12950,  10038,   9385,  -4720,
      7869,   2450,  10192,  11313,  10154,   9638,   3854,   6699,
     11082,  -1026,  10396,  10287,   7953,  12689,   6641,  -2348,
      9290,   4633,  11247,   9807,   9736,   8440,   9307,   1698,
     10214,   8390,   7201,    -88,   6193,  12325,  13064,   5333,
};

static const int16_t hevag2_table[] = {
        0,     0, -6656, -7040, -7680, -7059, -7366, -7522,
    -7680, -7680, -7680, -7680, -7680, -7680, -7680, -7680,
    -7680, -7680, -7680, -7680, -7680, -7680, -7680, -7680,
    -7680, -7680, -7522, -7366, -7059, -9216, -7168, -7424,
    -2722, -2221, -3166, -4750, -6946, -2596,  1982, -3044,
    -4487, -3744, -4328, -1336, -2562, -4122,  2378, -9117,
    -7108, -2062, -2577, -1858, -4483, -1795, -2102, -3509,
    -2647,  9183,  1859, -3012, -4792, -1144, -1048,  -620,
    -7585, -3891, -2735,  -483, -3844, -2609, -3297, -2775,
    -1882, -2241, -4160, -1958,  3745,  1948, -2835, -1961,
    -4270, -3383,  2523, -2867, -3721,  -310, -2411, -3067,
    -3846,  2194, -1876, -3423, -3847, -2570, -2757, -5006,
    -4326, -8597, -2763, -4213, -2716, -1417, -4554, -5659,
    -3908, -9810, -3746,   988,  3878, -3375,  3166, -7354,
    -4039, -6403, -4125, -2284, -1536, -3436, -1021, -9025,
    -2791,  3248,  3316, -7809, -5189, -1290, -4075,  2999,
};

static const int16_t hevag3_table[] = {
        0,     0,     0,     0,     0,     0,     0,     0,
        0,     0,     0,     0,     0,     0,     0,     0,
        0,     0,     0,     0,     0,     0,     0,     0,
        0,     0,     0,     0,     0,  3328, -3328, -3584,
     -494, -2298, -2814,  2649,  3875, -2071, -1382, -3792,
    -2250, -6456,  2111,  -757,   300, -5486, -4787, -1237,
    -1575, -2212,  -315,   102,  2126, -2069, -2233, -2674,
    -1929,  1860, -1124, -4139,  -256, -3182,  -828,  -946,
     -533, -2807, -1730,  -714,  2821,   314,  1551, -2432,
      108,  -298, -2963, -2156,  5936,  -683, -3854,   130,
     3124, -2907,   434,   391,   665, -1262, -2311, -2337,
      419,  -541, -2017,  1674, -3007,   302,  1008, -2852,
     2135,  1299,   360,   833,   345,  -737,  2843,  2249,
      728,  -805,  1367, -1915,  -764, -3354,   231, -1944,
     1885,  1748,   802,   219,  -706,  1562,  -835,   688,
      368,  -758,    46,  -538,  2760, -3284, -2824,   775,
};

static const int16_t hevag4_table[] = {
        0,     0,     0,     0,     0,     0,     0,     0,
        0,     0,     0,     0,     0,     0,     0,     0,
        0,     0,     0,     0,     0,     0,     0,     0,
        0,     0,     0,     0,     0, -3072, -2304, -1024,
     -541,   424,   289, -1298, -1216,   227, -2316,  1267,
     1665,   840,  -506,   487,   199, -1493, -6947, -3114,
    -1447,   446,   -18,   258,  -538,   482,   440,  -391,
    -1637, -5746, -2427,  1370,   622, -6878,   507, -4229,
    -2259,    44, -1899, -1421, -1019,   195,  -155,  -336,
      256, -6937,     5,   460, -1089, -2704,  1055,   250,
    -3157,  -456, -2461,   172,    97,   320,  -271,   163,
     -933, -2880,  -601,  -169,  1946,   198,    41, -1161,
     -501, -2780,   181,    53,   185,   482, -3397, -1074,
       80, -3462,   -96, -1437, -3263,  2079, -2089, -4122,
     -246, -1619,    61,   222,   473,  -176,   509, -3037,
      179, -2989, -2614, -4571, -1245,   253,  1877, -1132,
};

#define BLOCK_SIZE 28

static void vag_read_wav_straight(vag* v, wchar_t* path, float_t** data, size_t* samples);
static void vag_read_wav(vag* v, wchar_t* path, float_t** data, size_t* samples, uint8_t** flags);
static void vag_write_wav_straight(vag* v, wchar_t* path, float_t* data, size_t num_blocks, uint8_t* flags);
static void vag_write_wav(vag* v, wchar_t* path, float_t* data, size_t num_blocks, uint8_t* flags);
static void calculate_4_bits_vag(int32_t* data, int8_t* four_bit, int32_t* coef_index,
    int32_t* shift_factor, int32_t* v, int32_t* tv, int32_t coef_index_count);
static void calculate_4_bits_hevag(int32_t* data, int8_t* four_bit, int32_t* coef_index,
    int32_t* shift_factor, int32_t* v, int32_t* tv, int32_t coef_index_count);
static void calculate_4_bits_vag_inner(int32_t* data, int8_t* four_bit, int32_t coef_index,
    int32_t* shift_factor, int32_t* v, int32_t* tv, int32_t* os, int32_t* ots);
static void calculate_4_bits_hevag_inner(int32_t* data, int8_t* four_bit, int32_t coef_index,
    int32_t* shift_factor, int32_t* v, int32_t* tv, int32_t* os, int32_t* ots);

vag* vag_init() {
    vag* v = force_malloc(sizeof(vag));
    return v;
}

void vag_dispose(vag* v) {
    free(v);
}

void vag_read(vag* v, char* path) {
    wchar_t* path_buf = char_string_to_wchar_t_string(path);
    vag_wread(v, path_buf);
    free(path_buf);
}

void vag_wread(vag* v, wchar_t* path) {
    wchar_t* path_vag = path_wadd_extension(path, L".vag");
    stream* s = io_wopen(path_vag, L"rb");
    if (s->io.stream) {
        uint32_t signature = io_read_uint32_t(s);
        if (signature != 0x70474156)
            goto End;

        v->version = io_read_uint32_t_reverse_endianess(s, true);
        io_read_uint32_t(s);
        v->size = io_read_uint32_t_reverse_endianess(s, true);
        v->sample_rate = io_read_uint32_t_reverse_endianess(s, true);
        io_read_uint32_t(s);
        io_read_uint32_t(s);
        io_read_uint16_t(s);
        v->channels = io_read_uint16_t(s);
        if (v->channels < 2)
            v->channels = 1;
        io_read_uint64_t(s);
        io_read_uint64_t(s);

        bool hevag = v->version == 0x00020001 || v->version == 0x00030000;
        if (!hevag)
            v->channels = 1;

        size_t ch = v->channels;
        size_t vag_block_size = BLOCK_SIZE * ch;

        size_t num_blocks = (v->size / v->channels) >> 4;
        int32_t* samp = force_malloc_s(sizeof(int32_t), ch * 4);
        float_t* data = force_malloc_s(sizeof(float_t), num_blocks * vag_block_size);
        uint8_t* flags = force_malloc(num_blocks);

        uint8_t nibble[BLOCK_SIZE];
        float_t* temp_data = data;
        int16_t hevag1, hevag2, hevag3, hevag4;
        int32_t s1, s2, s3, s4, sample;
        size_t c, i, i1, i2;
        uint8_t coef_index, d, shift_factor;
        for (i1 = 0; i1 < num_blocks; i1++, temp_data += vag_block_size)
            for (c = 0; c < ch; c++) {
                d = io_read_uint8_t(s);
                coef_index = (d & 0xF0) >> 4;
                shift_factor = d & 0x0F;
                d = io_read_uint8_t(s);
                coef_index = (d & 0xF0) | coef_index;
                flags[i1] = d & 0x0F;

                if (coef_index > 127)
                    coef_index = 0;

                for (i = 0, i2 = 1; i < BLOCK_SIZE; i += 2, i2 += 2) {
                    d = io_read_uint8_t(s);
                    nibble[i] = d & 0x0F;
                    nibble[i2] = (d & 0xF0) >> 4;
                }

                hevag1 = hevag1_table[coef_index];
                hevag2 = hevag2_table[coef_index];
                hevag3 = hevag3_table[coef_index];
                hevag4 = hevag4_table[coef_index];
                s1 = samp[c * 4 + 0];
                s2 = samp[c * 4 + 1];
                s3 = samp[c * 4 + 2];
                s4 = samp[c * 4 + 3];
                shift_factor = 20 - shift_factor;
                for (i = 0; i < BLOCK_SIZE; i++) {
                    sample = shift_factor_table[nibble[i]] << shift_factor;
                    sample += (s1 * hevag1 + s2 * hevag2 + s3 * hevag3 + s4 * hevag4) >> 5;
                    s4 = s3;
                    s3 = s2;
                    s2 = s1;
                    s1 = sample >> 8;
                    temp_data[i * ch + c] = (float_t)(sample / 8388608.0);
                }
                samp[c * 4 + 0] = s1;
                samp[c * 4 + 1] = s2;
                samp[c * 4 + 2] = s3;
                samp[c * 4 + 3] = s4;
            }
        free(samp);

        vag_write_wav(v, path, data, num_blocks, flags);
        free(flags);
        free(data);
    }
End:
    io_dispose(s);
    free(path_vag);
}

void vag_write(vag* v, char* path, vag_option option) {
    wchar_t* path_buf = char_string_to_wchar_t_string(path);
    vag_wwrite(v, path_buf, option);
    free(path_buf);
}

void vag_wwrite(vag* v, wchar_t* path, vag_option option) {
    size_t num_samples;
    float_t* data;
    uint8_t* flags;

    vag_read_wav(v, path, &data, &num_samples, &flags);
    if (!data || !num_samples)
        return;

    v->size = align_val_divide(num_samples, BLOCK_SIZE, BLOCK_SIZE);

    wchar_t* temp_path = path_wget_without_extension(path);
    wchar_t* path_vag = path_wadd_extension(temp_path, L".vag");
    stream* s = io_wopen(path_vag, L"wb");
    if (s->io.stream) {
        int32_t coef_index_count;
        switch (option) {
        case VAG_OPTION_HEVAG_FAST:
            coef_index_count = 16;
            break;
        case VAG_OPTION_HEVAG_MEDIUM:
            coef_index_count = 32;
            break;
        case VAG_OPTION_HEVAG_SLOW:
            coef_index_count = 64;
            break;
        case VAG_OPTION_HEVAG_SLOWEST:
            coef_index_count = 96;
            break;
        case VAG_OPTION_HEVAG_SLOWASHELL:
            coef_index_count = 128;
            break;
        default:
            coef_index_count = 5;
            break;
        }

        bool hevag = option != VAG_OPTION_VAG;
        io_write_uint32_t(s, 0x70474156);
        io_write_uint32_t_reverse_endianess(s, hevag ? 0x00020001 : 0x00000020, true);
        io_write_uint32_t(s, 0);
        io_write_uint32_t_reverse_endianess(s, (uint32_t)(((v->size + 1) * (hevag ? v->channels : 1)) << 4), true);
        io_write_uint32_t_reverse_endianess(s, v->sample_rate, true);
        io_write_uint32_t(s, 0);
        io_write_uint32_t(s, 0);
        io_write_uint16_t(s, 0);
        io_write_uint16_t(s, hevag ? v->channels : 1);
        io_write_uint64_t(s, 0);
        io_write_uint64_t(s, 0);

        size_t c, i, i1, i2;
        int32_t ch = v->channels;
        int32_t* samp = force_malloc_s(sizeof(int32_t), hevag ? v->channels * 8LL : 4LL);
        if (hevag && coef_index_count > 29) {
            uint8_t flag, sample;
            int32_t temp_data[BLOCK_SIZE];
            int8_t four_bit[BLOCK_SIZE];
            int32_t coef_index, shift_factor;
            size_t size = v->size;
            for (i1 = 0; i1 < size; i1++)
                for (c = 0; c < ch; c++) {
                    for (i = 0; i < BLOCK_SIZE; i++) {
                        float_t t_s = data[i1 * BLOCK_SIZE * ch + i * ch + c];
                        temp_data[i] = (int32_t)round(t_s * 8388608.0);
                    }

                    calculate_4_bits_hevag(temp_data, four_bit, &coef_index,
                        &shift_factor, samp + c * 8, samp + c * 8 + 4, coef_index_count);

                    flag = flags ? (flags[i1] & 0x0F) : (i1 + 1 == size) ? 0x01 : 0;
                    sample = ((coef_index & 0x0F) << 4) | (shift_factor & 0x0F);
                    io_write_char(s, sample);
                    sample = (coef_index & 0xF0) | flag;
                    io_write_char(s, sample);
                    for (i = 0, i2 = 1; i < BLOCK_SIZE; i += 2, i2 += 2) {
                        sample = ((four_bit[i2] & 0x0F) << 4) | (four_bit[i] & 0x0F);
                        io_write_char(s, sample);
                    }
                }
            for (c = 0; c < ch; c++) {
                io_write_uint64_t(s, 0x7777777777770700);
                io_write_uint64_t(s, 0x7777777777777777);
            }
        }
        else if (hevag) {
            uint8_t flag, sample;
            int32_t temp_data[BLOCK_SIZE];
            int8_t four_bit[BLOCK_SIZE];
            int32_t coef_index, shift_factor;
            size_t size = v->size;
            for (i1 = 0; i1 < size; i1++)
                for (c = 0; c < ch; c++) {
                    for (i = 0; i < BLOCK_SIZE; i++) {
                        float_t t_s = data[i1 * BLOCK_SIZE * ch + i * ch + c];
                        temp_data[i] = (int32_t)round(t_s * 8388608.0);
                    }

                    calculate_4_bits_vag(temp_data, four_bit, &coef_index,
                        &shift_factor, samp + c * 4, samp + c * 4 + 2, coef_index_count);

                    flag = flags ? (flags[i1] & 0x0F) : (i1 + 1 == size) ? 0x01 : 0;
                    sample = ((coef_index & 0x0F) << 4) | (shift_factor & 0x0F);
                    io_write_char(s, sample);
                    sample = (coef_index & 0xF0) | flag;
                    io_write_char(s, sample);
                    for (i = 0, i2 = 1; i < BLOCK_SIZE; i += 2, i2 += 2) {
                        sample = ((four_bit[i2] & 0x0F) << 4) | (four_bit[i] & 0x0F);
                        io_write_char(s, sample);
                    }
                }

            for (c = 0; c < ch; c++) {
                io_write_uint64_t(s, 0x7777777777770700);
                io_write_uint64_t(s, 0x7777777777777777);
            }
        }
        else {
            uint8_t flag, sample;
            int32_t temp_data[BLOCK_SIZE];
            int8_t four_bit[BLOCK_SIZE];
            int32_t coef_index, shift_factor;
            size_t size = v->size;
            for (i1 = 0; i1 < size; i1++) {

                for (i = 0; i < BLOCK_SIZE; i++) {
                    float_t t_s = 0.0f;
                    for (c = 0; c < ch; c++)
                        t_s += data[i1 * BLOCK_SIZE * ch + i * ch + c];
                    t_s /= ch;
                    temp_data[i] = (int32_t)round(t_s * 8388608.0);
                }

                calculate_4_bits_vag(temp_data, four_bit, &coef_index,
                    &shift_factor, samp, samp + 2, coef_index_count);

                for (i = 0, i2 = 0; i < BLOCK_SIZE; i++)
                    if (!four_bit[i])
                        i2++;

                flag = flags ? (flags[i1] & 0x0F) : (i1 + 1 == size) ? 0x01 : 0;
                sample = ((coef_index & 0x0F) << 4) | (shift_factor & 0x0F);
                io_write_char(s, sample);
                sample = (coef_index & 0xF0) | flag;
                io_write_char(s, sample);
                for (i = 0, i2 = 1; i < BLOCK_SIZE; i += 2, i2 += 2) {
                    sample = ((four_bit[i2] & 0x0F) << 4) | (four_bit[i] & 0x0F);
                    io_write_char(s, sample);
                }
            }
            io_write_uint64_t(s, 0x7777777777770700);
            io_write_uint64_t(s, 0x7777777777777777);
        }
        io_write_uint64_t(s, 0x1010101010101010);
        io_write_uint64_t(s, 0x1010101010101010);
        free(samp);
    }
    io_dispose(s);
    free(path_vag);
    free(temp_path);
    free(flags);
    free(data);
}

static void vag_read_wav_straight(vag* v, wchar_t* path, float_t** data, size_t* samples) {
    *data = 0;
    *samples = 0;
    wchar_t* path_wav = path_wadd_extension(path, L".wav");
    wav* w = wav_init();
    wav_wread(w, path_wav, data, samples);
    v->channels = w->channels;
    v->sample_rate = w->sample_rate;
    wav_dispose(w);
    free(path_wav);
}

static void vag_read_wav(vag* v, wchar_t* path, float_t** data, size_t* samples, uint8_t** flags) {
    *data = 0;
    *samples = 0;
    *flags = 0;
    if (!path_wcheck_ends_with(path, L".0")) {
        vag_read_wav_straight(v, path, data, samples);
        return;
    }

    wchar_t temp[MAX_PATH];
    size_t count = 0;
    wchar_t* temp_path = path_wget_without_extension(path);
    vector_bool loop;
    bool n, l;
    bool add_loop = false;

    loop = (vector_bool){ 0, 0, 0 };
    while (true) {
        swprintf_s(temp, MAX_PATH, L"%ls.%llu.wav", temp_path, count);
        n = path_wcheck_file_exists(temp);
        l = false;
        if (!n) {
            swprintf_s(temp, MAX_PATH, L"%ls.%llu.loop.wav", temp_path, count);
            l = path_wcheck_file_exists(temp);
        }
        if (!n && !l)
            break;

        add_loop = !n;

        vector_bool_push_back(&loop, &l);
        count++;
    }

    if (!count) {
        vector_bool_free(&loop);
        free(temp_path);
        return;
    }

    count += add_loop;
    float_t** wav_data = force_malloc_s(sizeof(float_t*), count);
    size_t* wav_samples = force_malloc_s(sizeof(size_t), count);

    wav* w = wav_init();
    for (size_t i = 0; i < count; i++) {
        if (add_loop && i + 1 == count)
            continue;

        swprintf_s(temp, MAX_PATH, loop.begin[i] ? L"%ls.%llu.loop.wav" : L"%ls.%llu.wav", temp_path, i);
        wav_wread(w, temp, &wav_data[i], &wav_samples[i]);
        if (i == 0) {
            v->channels = w->channels;
            v->sample_rate = w->sample_rate;
        }
        else if (v->channels != w->channels || v->sample_rate != w->sample_rate) {
            wav_dispose(w);

            for (size_t i1 = 0; i1 <= i; i1++)
                free(wav_data[i1]);

            free(wav_samples);
            free(wav_data);
            vector_bool_free(&loop);
            free(temp_path);
            return;
        }
    }
    wav_dispose(w);

    *samples = 0;
    for (size_t i = 0; i < count; i++)
        if (add_loop && i + 1 == count)
            *samples += BLOCK_SIZE;
        else
            *samples += align_val(wav_samples[i], BLOCK_SIZE);

    l = false;
    size_t ch = v->channels;
    *data = force_malloc_s(sizeof(float_t), *samples * ch);
    float_t* data_temp = *data;
    for (size_t i = 0; i < count; i++) {
        size_t i3;
        if (add_loop && i + 1 == count)
            i3 = BLOCK_SIZE;
        else {
            size_t size = wav_samples[i] * ch;
            memcpy(data_temp, wav_data[i], sizeof(float_t) * size);
            data_temp += size;
            i3 = wav_samples[i] - align_val(wav_samples[i], BLOCK_SIZE);
        }

        for (size_t i2 = 0; i2 < i3; i2++)
            for (size_t c = 0; c < ch; c++)
                *data_temp++ = 0.0f;
    }

    l = false;
    *flags = force_malloc(*samples / BLOCK_SIZE);
    uint8_t* f = *flags;
    for (size_t i = 0; i < count; i++) {
        size_t num_blocks;
        if (add_loop && i + 1 == count)
            num_blocks = 1;
        else
            num_blocks = align_val_divide(wav_samples[i], BLOCK_SIZE, BLOCK_SIZE);
        if (loop.begin[i]) {
            *f++ = 6;
            for (size_t i1 = 1; i1 < num_blocks; i1++)
                *f++ = 2;
            l = true;
        }
        else {
            if (l) {
                *f++ = 3;
                l = false;
            }
            else if (num_blocks == 1 && i + 1 == count)
                *f++ = 1;
            else
                *f++ = 0;
            for (size_t i1 = 1; i1 < num_blocks - 1; i1++)
                *f++ = 0;

            if (num_blocks > 1)
                if (i + 1 == count)
                    *f++ = 1;
                else
                    *f++ = 0;
        }
    }

    for (size_t i = 0; i < count; i++)
        free(wav_data[i]);

    free(wav_samples);
    free(wav_data);
    vector_bool_free(&loop);
    free(temp_path);
}

static void vag_write_wav_straight(vag* v, wchar_t* path, float_t* data, size_t num_blocks, uint8_t* flags) {
    size_t i = 0;
    for (i = 0; i < num_blocks; i++) {
        uint8_t flag = flags[i];
        if (flag == 1) {
            i++;
            break;
        }
        else if (flag == 5 || flag == 7)
            break;
    }

    wchar_t* path_wav = path_wadd_extension(path, L".wav");
    wav* w = wav_init();
    w->bytes = 4;
    w->channels = v->channels;
    w->format = 0x03;
    w->sample_rate = v->sample_rate;
    wav_wwrite(w, path_wav, data, i * BLOCK_SIZE);
    wav_dispose(w);
    free(path_wav);
}

static void vag_write_wav(vag* v, wchar_t* path, float_t* data, size_t num_blocks, uint8_t* flags) {
    uint8_t flag = flags[0];
    if (flag == 5 || flag == 7)
        return;

    wchar_t temp[MAX_PATH];
    wav* w = wav_init();
    w->bytes = 4;
    w->channels = v->channels;
    w->format = 0x03;
    w->sample_rate = v->sample_rate;
    bool loop = false;
    size_t i = 0, i2 = 0;
    size_t ch = v->channels;
    size_t vag_block_size = BLOCK_SIZE * ch;
    float_t* temp_data = data;
    float_t* temp_data_last = data;
    for (size_t i1 = 0; i1 < num_blocks; i1++, temp_data += vag_block_size) {
        flag = flags[i1];
        if (flag == 5 || flag == 7)
            break;
        else if (flag == 3 || flag == 6) {
            if (i) {
                swprintf_s(temp, MAX_PATH, loop ? L"%ls.%llu.loop.wav" : L"%ls.%llu.wav", path, i2);
                wav_wwrite(w, temp, temp_data_last, i * BLOCK_SIZE);
                i2++;
            }

            loop = flag == 6;
            i = 0;
            temp_data_last = temp_data;
        }

        i++;

        if (flag == 1)
            break;
    }

    if (i) {
        swprintf_s(temp, MAX_PATH, L"%ls.%llu.wav", path, i2);
        wav_wwrite(w, temp, temp_data_last, i * BLOCK_SIZE);
    }
    wav_dispose(w);
}

static void calculate_4_bits_vag(int32_t* data, int8_t* four_bit, int32_t* coef_index,
    int32_t* shift_factor, int32_t* v, int32_t* tv, int32_t coef_index_count) {
    int32_t d0, e, g, i, j;
    int32_t err, min, s1, s2, c_f;

    c_f = 0;
    min = 0x7FFFFFFF;
    for (j = 0; j < coef_index_count; j++) {
        calculate_4_bits_vag_inner(data, four_bit, j, shift_factor, v, tv, 0, 0);

        int32_t vag1 = hevag1_table[j];
        int32_t vag2 = hevag2_table[j];

        s1 = v[0];
        s2 = v[1];
        err = 0;
        for (i = 0; i < BLOCK_SIZE; i++) {
            d0 = four_bit[i] << *shift_factor;

            g = ((s1 * vag1 + s2 * vag2) >> 5) + d0;
            s2 = s1;
            s1 = g >> 8;
            e = data[i] - g;
            g = e >> 31;
            err += (e + g) ^ g;
        }

        if (err < min) {
            c_f = j;
            min = err;
        }
    }
    *coef_index = c_f;

    calculate_4_bits_vag_inner(data, four_bit, c_f, shift_factor, v, tv, v, tv);
    *shift_factor = (int32_t)(20LL - *shift_factor);
}

static void calculate_4_bits_vag_inner(int32_t* data, int8_t* four_bit, int32_t coef_index,
    int32_t* shift_factor, int32_t* v, int32_t* tv, int32_t* os, int32_t* ots) {
    int32_t d0, d1, e, g, i, s_f, t_s_f, m, shift_max;
    int32_t temp[BLOCK_SIZE];

    int32_t vag1 = hevag1_table[coef_index];
    int32_t vag2 = hevag2_table[coef_index];

    int32_t s1 = v[0];
    int32_t s2 = v[1];
    for (i = 0, m = 0; i < BLOCK_SIZE; i++) {
        g = data[i];
        e = (s1 * vag1 + s2 * vag2) >> 5;
        e = g - e;
        s2 = s1;
        s1 = g >> 8;

        if (e > 0x77FFFF)
            e = 0x77FFFF;
        else if (e < -0x780000)
            e = -0x780000;

        temp[i] = e;
        g = e >> 31;
        e = (e + g) ^ g;

        if (e > m)
            m = e;
    }

    if (os) {
        os[0] = s1;
        os[1] = s2;
    }

    for (s_f = 12, shift_max = 0x400000; s_f > 0; s_f--, shift_max >>= 1)
        if ((shift_max & (m + (shift_max >> 4))) == shift_max)
            break;

    t_s_f = (int32_t)(8LL + s_f);
    s_f = (int32_t)(12LL - s_f);

    int32_t ts1 = tv[0];
    int32_t ts2 = tv[1];
    for (i = 0; i < BLOCK_SIZE; i++) {
        g = temp[i];
        e = (ts1 * vag1 + ts2 * vag2) >> 5;
        e = g - e;

        d1 = e << s_f;
        d0 = (d1 + 0x80000) >> 20;

        if (d0 > 7)
            d0 = 7;
        else if (d0 < -8)
            d0 = -8;

        four_bit[i] = d0;
        d0 <<= t_s_f;

        ts2 = ts1;
        ts1 = (d0 - e) >> 8;
    }
    *shift_factor = t_s_f;

    if (ots) {
        ots[0] = ts1;
        ots[1] = ts2;
    }
}

static void calculate_4_bits_hevag(int32_t* data, int8_t* four_bit, int32_t* coef_index,
    int32_t* shift_factor, int32_t* v, int32_t* tv, int32_t coef_index_count) {
    int32_t d0, e, g, i, j;
    int32_t err, min, s1, s2, s3, s4, c_f;

    c_f = 0;
    min = 0x7FFFFFFF;
    for (j = 0; j < coef_index_count; j++) {
        calculate_4_bits_hevag_inner(data, four_bit, j, shift_factor, v, tv, 0, 0);

        int32_t hevag1 = hevag1_table[j];
        int32_t hevag2 = hevag2_table[j];
        int32_t hevag3 = hevag3_table[j];
        int32_t hevag4 = hevag4_table[j];

        s1 = v[0];
        s2 = v[1];
        s3 = v[2];
        s4 = v[3];
        err = 0;
        for (i = 0; i < BLOCK_SIZE; i++) {
            d0 = four_bit[i] << *shift_factor;

            g = ((s1 * hevag1 + s2 * hevag2 + s3 * hevag3 + s4 * hevag4) >> 5) + d0;
            s4 = s3;
            s3 = s2;
            s2 = s1;
            s1 = g >> 8;
            e = data[i] - g;
            g = e >> 31;
            err += (e + g) ^ g;
        }

        if (err < min) {
            c_f = j;
            min = err;
        }
    }
    *coef_index = c_f;

    calculate_4_bits_hevag_inner(data, four_bit, c_f, shift_factor, v, tv, v, tv);
    *shift_factor = (int32_t)(20LL - *shift_factor);
}

static void calculate_4_bits_hevag_inner(int32_t* data, int8_t* four_bit, int32_t coef_index,
    int32_t* shift_factor, int32_t* v, int32_t* tv, int32_t* os, int32_t* ots) {
    int32_t d0, d1, e, g, i, s_f, t_s_f, m, shift_max;
    int32_t temp[BLOCK_SIZE];

    int32_t hevag1 = hevag1_table[coef_index];
    int32_t hevag2 = hevag2_table[coef_index];
    int32_t hevag3 = hevag3_table[coef_index];
    int32_t hevag4 = hevag4_table[coef_index];

    int32_t s1 = v[0];
    int32_t s2 = v[1];
    int32_t s3 = v[2];
    int32_t s4 = v[3];
    for (i = 0, m = 0; i < BLOCK_SIZE; i++) {
        g = data[i];
        e = (s1 * hevag1 + s2 * hevag2 + s3 * hevag3 + s4 * hevag4) >> 5;
        e = g - e;
        s4 = s3;
        s3 = s2;
        s2 = s1;
        s1 = g >> 8;

        if (e > 0x77FFFF)
            e = 0x77FFFF;
        else if (e < -0x780000)
            e = -0x780000;

        temp[i] = e;
        g = e >> 31;
        e = (e + g) ^ g;

        if (e > m)
            m = e;
    }

    if (os) {
        os[0] = s1;
        os[1] = s2;
        os[2] = s3;
        os[3] = s4;
    }

    for (s_f = 12, shift_max = 0x400000; s_f > 0; s_f--, shift_max >>= 1)
        if ((shift_max & (m + (shift_max >> 4))) == shift_max)
            break;

    t_s_f = (int32_t)(8LL + s_f);
    s_f = (int32_t)(12LL - s_f);

    int32_t ts1 = tv[0];
    int32_t ts2 = tv[1];
    int32_t ts3 = tv[2];
    int32_t ts4 = tv[3];
    for (i = 0; i < BLOCK_SIZE; i++) {
        g = temp[i];
        e = (ts1 * hevag1 + ts2 * hevag2 + ts3 * hevag3 + ts4 * hevag4) >> 5;
        e = g - e;

        d1 = e << s_f;
        d0 = (d1 + 0x80000) >> 20;

        if (d0 > 7)
            d0 = 7;
        else if (d0 < -8)
            d0 = -8;

        four_bit[i] = d0;
        d0 <<= t_s_f;

        ts4 = ts3;
        ts3 = ts2;
        ts2 = ts1;
        ts1 = (d0 - e) >> 8;
    }
    *shift_factor = t_s_f;

    if (ots) {
        ots[0] = ts1;
        ots[1] = ts2;
        ots[2] = ts3;
        ots[3] = ts4;
    }
}
