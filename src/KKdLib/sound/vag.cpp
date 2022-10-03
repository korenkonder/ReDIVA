/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "vag.hpp"
#include "wav.hpp"
#include "../io/file_stream.hpp"
#include "../io/path.hpp"
#include "../str_utils.hpp"
#include <immintrin.h>

static const int8_t shift_factor_table[] = {
    0, 1, 2, 3, 4, 5, 6, 7, -8, -7, -6, -5, -4, -3, -2, -1
};

static const int16_t hevag_table[128][4] = {
    {      0,     0,     0,     0 },
    {   7680,     0,     0,     0 },
    {  14720, -6656,     0,     0 },
    {  12544, -7040,     0,     0 },
    {  15616, -7680,     0,     0 },
    {  14731, -7059,     0,     0 },
    {  14507, -7366,     0,     0 },
    {  13920, -7522,     0,     0 },
    {  13133, -7680,     0,     0 },
    {  12028, -7680,     0,     0 },
    {  10764, -7680,     0,     0 },
    {   9359, -7680,     0,     0 },
    {   7832, -7680,     0,     0 },
    {   6201, -7680,     0,     0 },
    {   4488, -7680,     0,     0 },
    {   2717, -7680,     0,     0 },
    {    910, -7680,     0,     0 },
    {   -910, -7680,     0,     0 },
    {  -2717, -7680,     0,     0 },
    {  -4488, -7680,     0,     0 },
    {  -6201, -7680,     0,     0 },
    {  -7832, -7680,     0,     0 },
    {  -9359, -7680,     0,     0 },
    { -10764, -7680,     0,     0 },
    { -12028, -7680,     0,     0 },
    { -13133, -7680,     0,     0 },
    { -13920, -7522,     0,     0 },
    { -14507, -7366,     0,     0 },
    { -14731, -7059,     0,     0 },
    {   5376, -9216,  3328, -3072 },
    {  -6400, -7168, -3328, -2304 },
    { -10496, -7424, -3584, -1024 },
    {   -167, -2722,  -494,  -541 },
    {  -7430, -2221, -2298,   424 },
    {  -8001, -3166, -2814,   289 },
    {   6018, -4750,  2649, -1298 },
    {   3798, -6946,  3875, -1216 },
    {  -8237, -2596, -2071,   227 },
    {   9199,  1982, -1382, -2316 },
    {  13021, -3044, -3792,  1267 },
    {  13112, -4487, -2250,  1665 },
    {  -1668, -3744, -6456,   840 },
    {   7819, -4328,  2111,  -506 },
    {   9571, -1336,  -757,   487 },
    {  10032, -2562,   300,   199 },
    {  -4745, -4122, -5486, -1493 },
    {  -5896,  2378, -4787, -6947 },
    {  -1193, -9117, -1237, -3114 },
    {   2783, -7108, -1575, -1447 },
    {  -7334, -2062, -2212,   446 },
    {   6127, -2577,  -315,   -18 },
    {   9457, -1858,   102,   258 },
    {   7876, -4483,  2126,  -538 },
    {  -7172, -1795, -2069,   482 },
    {  -7358, -2102, -2233,   440 },
    {  -9170, -3509, -2674,  -391 },
    {  -2638, -2647, -1929, -1637 },
    {   1873,  9183,  1860, -5746 },
    {   9214,  1859, -1124, -2427 },
    {  13204, -3012, -4139,  1370 },
    {  12437, -4792,  -256,   622 },
    {  -2653, -1144, -3182, -6878 },
    {   9331, -1048,  -828,   507 },
    {   1642,  -620,  -946, -4229 },
    {   4246, -7585,  -533, -2259 },
    {  -8988, -3891, -2807,    44 },
    {  -2562, -2735, -1730, -1899 },
    {   3182,  -483,  -714, -1421 },
    {   7937, -3844,  2821, -1019 },
    {  10069, -2609,   314,   195 },
    {   8400, -3297,  1551,  -155 },
    {  -8529, -2775, -2432,  -336 },
    {   9477, -1882,   108,   256 },
    {     75, -2241,  -298, -6937 },
    {  -9143, -4160, -2963,     5 },
    {  -7270, -1958, -2156,   460 },
    {  -2740,  3745,  5936, -1089 },
    {   8993,  1948,  -683, -2704 },
    {  13101, -2835, -3854,  1055 },
    {   9543, -1961,   130,   250 },
    {   5272, -4270,  3124, -3157 },
    {  -7696, -3383, -2907,  -456 },
    {   7309,  2523,   434, -2461 },
    {  10275, -2867,   391,   172 },
    {  10940, -3721,   665,    97 },
    {     24,  -310, -1262,   320 },
    {  -8122, -2411, -2311,  -271 },
    {  -8511, -3067, -2337,   163 },
    {    326, -3846,   419,  -933 },
    {   8895,  2194,  -541, -2880 },
    {  12073, -1876, -2017,  -601 },
    {   8729, -3423,  1674,  -169 },
    {  12950, -3847, -3007,  1946 },
    {  10038, -2570,   302,   198 },
    {   9385, -2757,  1008,    41 },
    {  -4720, -5006, -2852, -1161 },
    {   7869, -4326,  2135,  -501 },
    {   2450, -8597,  1299, -2780 },
    {  10192, -2763,   360,   181 },
    {  11313, -4213,   833,    53 },
    {  10154, -2716,   345,   185 },
    {   9638, -1417,  -737,   482 },
    {   3854, -4554,  2843, -3397 },
    {   6699, -5659,  2249, -1074 },
    {  11082, -3908,   728,    80 },
    {  -1026, -9810,  -805, -3462 },
    {  10396, -3746,  1367,   -96 },
    {  10287,   988, -1915, -1437 },
    {   7953,  3878,  -764, -3263 },
    {  12689, -3375, -3354,  2079 },
    {   6641,  3166,   231, -2089 },
    {  -2348, -7354, -1944, -4122 },
    {   9290, -4039,  1885,  -246 },
    {   4633, -6403,  1748, -1619 },
    {  11247, -4125,   802,    61 },
    {   9807, -2284,   219,   222 },
    {   9736, -1536,  -706,   473 },
    {   8440, -3436,  1562,  -176 },
    {   9307, -1021,  -835,   509 },
    {   1698, -9025,   688, -3037 },
    {  10214, -2791,   368,   179 },
    {   8390,  3248,  -758, -2989 },
    {   7201,  3316,    46, -2614 },
    {    -88, -7809,  -538, -4571 },
    {   6193, -5189,  2760, -1245 },
    {  12325, -1290, -3284,   253 },
    {  13064, -4075, -2824,  1877 },
    {   5333,  2999,   775, -1132 },
};

#define BLOCK_SIZE 28

static bool vag_read_inner(vag* v, stream& s, float_t*& data, size_t& num_blocks, uint8_t*& flags);
static void vag_write_inner(vag* v, stream& s, const float_t* data, const uint8_t* flags, vag_option option);
static void vag_read_wav_straight(vag* v, const wchar_t* path, float_t*& data, size_t& samples);
static void vag_read_wav(vag* v, const wchar_t* path, float_t*& data, size_t& samples, uint8_t*& flags);
static void vag_write_wav_straight(vag* v, const wchar_t* path, const float_t* data, size_t num_blocks, const uint8_t* flags);
static void vag_write_wav(vag* v, const wchar_t* path, const float_t* data, size_t num_blocks, const uint8_t* flags);
static void calculate_4_bits_vag(int32_t* data, int8_t* four_bit, int32_t* coef_index,
    int32_t* shift_factor, int32_t* v, int32_t* tv, int32_t coef_index_count);
static void calculate_4_bits_hevag(int32_t* data, int8_t* four_bit, int32_t* coef_index,
    int32_t* shift_factor, int32_t* v, int32_t* tv, int32_t coef_index_count);
static void calculate_4_bits_vag_inner(int32_t* data, int8_t* four_bit, int32_t coef_index,
    int32_t* shift_factor, int32_t* v, int32_t* tv, int32_t* os, int32_t* ots);
static void calculate_4_bits_hevag_inner(int32_t* data, int8_t* four_bit, int32_t coef_index,
    int32_t* shift_factor, int32_t* v, int32_t* tv, int32_t* os, int32_t* ots);

vag::vag() : version(), sample_rate(), channels(), size() {

}

vag::~vag() {

}

void vag::read(const char* path) {
    char* path_vag = str_utils_add(path, ".vag");
    file_stream s;
    s.open(path_vag, "rb");
    if (s.check_not_null()) {
        float_t* data;
        size_t num_blocks;
        uint8_t* flags;
        if (vag_read_inner(this, s, data, num_blocks, flags)) {
            wchar_t* path_temp = utf8_to_utf16(path);
            vag_write_wav(this, path_temp, data, num_blocks, flags);
            free_def(path_temp);
            free_def(flags);
            free_def(data);
        }
    }
    free_def(path_vag);
}

void vag::read(const wchar_t* path) {
    wchar_t* path_vag = str_utils_add(path, L".vag");
    file_stream s;
    s.open(path_vag, L"rb");
    if (s.check_not_null()) {
        float_t* data;
        size_t num_blocks;
        uint8_t* flags;
        if (vag_read_inner(this, s, data, num_blocks, flags)) {
            vag_write_wav(this, path, data, num_blocks, flags);
            free_def(flags);
            free_def(data);
        }
    }
    free_def(path_vag);
}

void vag::write(const char* path, vag_option option) {
    float_t* data;
    size_t num_samples;
    uint8_t* flags;

    wchar_t* path_temp = utf8_to_utf16(path);
    vag_read_wav(this, path_temp, data, num_samples, flags);
    free_def(path_temp);
    if (!data || !num_samples)
        return;

    size = align_val_divide(num_samples, BLOCK_SIZE, BLOCK_SIZE);

    char* path_vag = str_utils_add(path, ".vag");
    file_stream s;
    s.open(path_vag, "wb");
    if (s.check_not_null())
        vag_write_inner(this, s, data, flags, option);
    free_def(path_vag);
    free_def(flags);
    free_def(data);
}

void vag::write(const wchar_t* path, vag_option option) {
    float_t* data;
    size_t num_samples;
    uint8_t* flags;

    vag_read_wav(this, path, data, num_samples, flags);
    if (!data || !num_samples)
        return;

    size = align_val_divide(num_samples, BLOCK_SIZE, BLOCK_SIZE);

    wchar_t* path_vag = str_utils_add(path, L".vag");
    file_stream s;
    s.open(path_vag, L"wb");
    if (s.check_not_null())
        vag_write_inner(this, s, data, flags, option);
    free_def(path_vag);
    free_def(flags);
    free_def(data);
}

static bool vag_read_inner(vag* v, stream& s, float_t*& data, size_t& num_blocks, uint8_t*& flags) {
    data = 0;
    num_blocks = 0;
    flags = 0;

    uint32_t signature = s.read_uint32_t_reverse_endianness(true);
    if (signature != 'VAGp')
        return false;

    v->version = s.read_uint32_t_reverse_endianness(true);
    s.read_uint32_t();
    v->size = s.read_uint32_t_reverse_endianness(true);
    v->sample_rate = s.read_uint32_t_reverse_endianness(true);
    s.read_uint32_t();
    s.read_uint32_t();
    s.read_uint16_t();
    v->channels = s.read_uint16_t();
    if (v->channels < 2)
        v->channels = 1;
    s.read_uint64_t();
    s.read_uint64_t();

    bool is_hevag = v->version == 0x00020001 || v->version == 0x00030000;
    if (!is_hevag)
        v->channels = 1;

    size_t ch = v->channels;
    size_t vag_block_size = BLOCK_SIZE * ch;

    num_blocks = (v->size / v->channels) >> 4;
    int32_t* samp = force_malloc_s(int32_t, ch * 4);
    data = force_malloc_s(float_t, num_blocks * vag_block_size);
    flags = force_malloc_s(uint8_t, num_blocks);

    uint8_t nibble[BLOCK_SIZE];
    float_t* temp_data = data;
    int16_t hevag1, hevag2, hevag3, hevag4;
    int32_t s1, s2, s3, s4, sample;
    size_t c, i, i1, i2, j;
    uint8_t coef_index, d, shift_factor;
    for (i1 = 0; i1 < num_blocks; i1++, temp_data += vag_block_size)
        for (c = 0; c < ch; c++) {
            d = s.read_uint8_t();
            coef_index = (d & 0xF0) >> 4;
            shift_factor = d & 0x0F;
            d = s.read_uint8_t();
            coef_index = (d & 0x70) | coef_index;
            flags[i1] = d & 0x0F;

            if (coef_index > 127)
                coef_index = 0;

            for (i = 0, i2 = 1, j = BLOCK_SIZE; j; i += 2, i2 += 2, j -= 2) {
                d = s.read_uint8_t();
                nibble[i] = d & 0x0F;
                nibble[i2] = (d & 0xF0) >> 4;
            }

            hevag1 = hevag_table[coef_index][0];
            hevag2 = hevag_table[coef_index][1];
            hevag3 = hevag_table[coef_index][2];
            hevag4 = hevag_table[coef_index][3];
            s1 = samp[c * 4 + 0];
            s2 = samp[c * 4 + 1];
            s3 = samp[c * 4 + 2];
            s4 = samp[c * 4 + 3];
            shift_factor = 20 - shift_factor;
            for (i = 0, j = BLOCK_SIZE; j; i++, j--) {
                sample = shift_factor_table[nibble[i]] << shift_factor;
                sample += (s1 * hevag1 + s2 * hevag2 + s3 * hevag3 + s4 * hevag4) >> 5;
                s4 = s3;
                s3 = s2;
                s2 = s1;
                s1 = sample >> 8;
                temp_data[i * ch + c] = (float_t)((double_t)sample / 8388608.0);
            }

            samp[c * 4 + 0] = s1;
            samp[c * 4 + 1] = s2;
            samp[c * 4 + 2] = s3;
            samp[c * 4 + 3] = s4;
        }
    free_def(samp);
    return true;
}

static void vag_write_inner(vag* v, stream& s, const float_t* data, const uint8_t* flags, vag_option option) {
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
    s.write_uint32_t_reverse_endianness('VAGp', true);
    s.write_uint32_t_reverse_endianness(hevag ? 0x00020001 : 0x00000020, true);
    s.write_uint32_t(0);
    s.write_uint32_t_reverse_endianness((uint32_t)(((v->size + 1) * (hevag ? v->channels : 1)) << 4), true);
    s.write_uint32_t_reverse_endianness(v->sample_rate, true);
    s.write_uint32_t(0);
    s.write_uint32_t(0);
    s.write_uint16_t(0);
    s.write_uint16_t(hevag ? v->channels : 1);
    s.write_uint64_t(0);
    s.write_uint64_t(0);

    size_t c, i, i1, i2, j;
    int32_t ch = v->channels;
    int32_t* samp = force_malloc_s(int32_t, hevag ? v->channels * 8ULL : 4ULL);
    if (hevag && coef_index_count > 29) {
        uint8_t flag, sample;
        int32_t temp_data[BLOCK_SIZE];
        int8_t four_bit[BLOCK_SIZE];
        int32_t coef_index, shift_factor;
        size_t size = v->size;
        for (i1 = 0; i1 < size; i1++)
            for (c = 0; c < ch; c++) {
                for (i = 0, j = BLOCK_SIZE; j; i++, j--) {
                    float_t t_s = data[i1 * BLOCK_SIZE * ch + i * ch + c];
                    temp_data[i] = (int32_t)round(t_s * 8388608.0);
                }

                calculate_4_bits_hevag(temp_data, four_bit, &coef_index,
                    &shift_factor, samp + c * 8, samp + c * 8 + 4, coef_index_count);

                flag = flags ? (flags[i1] & 0x0F) : (i1 + 1 == size) ? 0x01 : 0;
                sample = ((coef_index & 0x0F) << 4) | (shift_factor & 0x0F);
                s.write_uint8_t(sample);
                sample = (coef_index & 0xF0) | flag;
                s.write_uint8_t(sample);
                for (i = 0, i2 = 1, j = BLOCK_SIZE; j; i += 2, i2 += 2, j -= 2) {
                    sample = ((four_bit[i2] & 0x0F) << 4) | (four_bit[i] & 0x0F);
                    s.write_uint8_t(sample);
                }
            }

        for (c = 0; c < ch; c++) {
            s.write_uint64_t(0x7777777777770700);
            s.write_uint64_t(0x7777777777777777);
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
                for (i = 0, j = BLOCK_SIZE; j; i++, j--) {
                    float_t t_s = data[i1 * BLOCK_SIZE * ch + i * ch + c];
                    temp_data[i] = (int32_t)round(t_s * 8388608.0);
                }

                calculate_4_bits_vag(temp_data, four_bit, &coef_index,
                    &shift_factor, samp + c * 4, samp + c * 4 + 2, coef_index_count);

                flag = flags ? (flags[i1] & 0x0F) : (i1 + 1 == size) ? 0x01 : 0;
                sample = ((coef_index & 0x0F) << 4) | (shift_factor & 0x0F);
                s.write_uint8_t(sample);
                sample = (coef_index & 0xF0) | flag;
                s.write_uint8_t(sample);
                for (i = 0, i2 = 1, j = BLOCK_SIZE; j; i += 2, i2 += 2, j -= 2) {
                    sample = ((four_bit[i2] & 0x0F) << 4) | (four_bit[i] & 0x0F);
                    s.write_uint8_t(sample);
                }
            }

        for (c = 0; c < ch; c++) {
            s.write_uint64_t(0x7777777777770700);
            s.write_uint64_t(0x7777777777777777);
        }
    }
    else {
        uint8_t flag, sample;
        int32_t temp_data[BLOCK_SIZE];
        int8_t four_bit[BLOCK_SIZE];
        int32_t coef_index, shift_factor;
        size_t size = v->size;
        for (i1 = 0; i1 < size; i1++) {
            for (i = 0, j = BLOCK_SIZE; j; i++, j--) {
                float_t t_s = 0.0f;
                for (c = 0; c < ch; c++)
                    t_s += data[i1 * BLOCK_SIZE * ch + i * ch + c];
                t_s /= ch;
                temp_data[i] = (int32_t)round(t_s * 8388608.0);
            }

            calculate_4_bits_vag(temp_data, four_bit, &coef_index,
                &shift_factor, samp, samp + 2, coef_index_count);

            flag = flags ? (flags[i1] & 0x0F) : (i1 + 1 == size) ? 0x01 : 0;
            sample = ((coef_index & 0x0F) << 4) | (shift_factor & 0x0F);
            s.write_uint8_t(sample);
            sample = (coef_index & 0xF0) | flag;
            s.write_uint8_t(sample);
            for (i = 0, i2 = 1, j = BLOCK_SIZE; j; i += 2, i2 += 2, j -= 2) {
                sample = ((four_bit[i2] & 0x0F) << 4) | (four_bit[i] & 0x0F);
                s.write_uint8_t(sample);
            }
        }
        s.write_uint64_t(0x7777777777770700);
        s.write_uint64_t(0x7777777777777777);
    }
    s.write_uint64_t(0x1010101010101010);
    s.write_uint64_t(0x1010101010101010);
    free_def(samp);
}

static void vag_read_wav_straight(vag* v, const wchar_t* path, float_t*& data, size_t& samples) {
    data = 0;
    samples = 0;
    wchar_t* path_wav = str_utils_add(path, L".wav");
    wav w;
    w.read(path_wav, data, samples);
    v->channels = w.channels;
    v->sample_rate = w.sample_rate;
    free_def(path_wav);
}

static void vag_read_wav(vag* v, const wchar_t* path, float_t*& data, size_t& samples, uint8_t*& flags) {
    data = 0;
    samples = 0;
    flags = 0;

    if (!str_utils_check_ends_with(path, L".0")) {
        vag_read_wav_straight(v, path, data, samples);
        return;
    }

    wchar_t temp[MAX_PATH];
    size_t count = 0;
    wchar_t* temp_path = str_utils_get_without_extension(path);
    std::vector<bool> loop;
    bool n, l;
    bool add_loop = false;

    while (true) {
        swprintf_s(temp, MAX_PATH, L"%ls.%llu.wav", temp_path, count);
        n = path_check_file_exists(temp);
        l = false;
        if (!n) {
            swprintf_s(temp, MAX_PATH, L"%ls.%llu.loop.wav", temp_path, count);
            l = path_check_file_exists(temp);
        }
        if (!n && !l)
            break;

        add_loop = !n;

        loop.push_back(l);
        count++;
    }

    if (!count) {
        free_def(temp_path);
        return;
    }

    count += add_loop;
    float_t** wav_data = force_malloc_s(float_t*, count);
    size_t* wav_samples = force_malloc_s(size_t, count);

    for (size_t i = 0; i < count; i++) {
        if (add_loop && i + 1 == count)
            continue;

        swprintf_s(temp, MAX_PATH, loop[i] ? L"%ls.%llu.loop.wav" : L"%ls.%llu.wav", temp_path, i);
        wav w;
        w.read(temp, wav_data[i], wav_samples[i]);
        if (i == 0) {
            v->channels = w.channels;
            v->sample_rate = w.sample_rate;
        }
        else if (v->channels != w.channels || v->sample_rate != w.sample_rate) {
            for (size_t i1 = 0; i1 <= i; i1++)
                free_def(wav_data[i1]);

            free_def(wav_samples);
            free_def(wav_data);
            free_def(temp_path);
            return;
        }
    }

    samples = 0;
    for (size_t i = 0; i < count; i++)
        if (add_loop && i + 1 == count)
            samples += BLOCK_SIZE;
        else
            samples += align_val(wav_samples[i], BLOCK_SIZE);

    l = false;
    size_t ch = v->channels;
    data = force_malloc_s(float_t, samples * ch);
    float_t* data_temp = data;
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
    flags = force_malloc_s(uint8_t, samples / BLOCK_SIZE);
    uint8_t* f = flags;
    for (size_t i = 0; i < count; i++) {
        size_t num_blocks;
        if (add_loop && i + 1 == count)
            num_blocks = 1;
        else
            num_blocks = align_val_divide(wav_samples[i], BLOCK_SIZE, BLOCK_SIZE);
        if (loop[i]) {
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
        free_def(wav_data[i]);

    free_def(wav_samples);
    free_def(wav_data);
    free_def(temp_path);
}

static void vag_write_wav_straight(vag* v, const wchar_t* path, const float_t* data, size_t num_blocks, const uint8_t* flags) {
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

    wchar_t* path_wav = str_utils_add(path, L".wav");
    wav w;
    w.bytes = 4;
    w.channels = v->channels;
    w.format = 0x03;
    w.sample_rate = v->sample_rate;
    w.write(path_wav, data, i * BLOCK_SIZE);
    free_def(path_wav);
}

static void vag_write_wav(vag* v, const wchar_t* path, const float_t* data, size_t num_blocks, const uint8_t* flags) {
    uint8_t flag = flags[0];
    if (flag == 5 || flag == 7)
        return;

    wchar_t temp[MAX_PATH];
    bool loop = false;
    size_t i = 0, i2 = 0;
    size_t ch = v->channels;
    size_t vag_block_size = BLOCK_SIZE * ch;
    const float_t* temp_data = data;
    const float_t* temp_data_last = data;
    for (size_t i1 = 0; i1 < num_blocks; i1++, temp_data += vag_block_size) {
        flag = flags[i1];
        if (flag == 5 || flag == 7)
            break;
        else if (flag == 3 || flag == 6) {
            if (i) {
                swprintf_s(temp, MAX_PATH, loop ? L"%ls.%llu.loop.wav" : L"%ls.%llu.wav", path, i2);
                wav w;
                w.bytes = 4;
                w.channels = v->channels;
                w.format = 0x03;
                w.sample_rate = v->sample_rate;
                w.write(temp, temp_data_last, i * BLOCK_SIZE);
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
        wav w;
        w.bytes = 4;
        w.channels = v->channels;
        w.format = 0x03;
        w.sample_rate = v->sample_rate;
        w.write(temp, temp_data_last, i * BLOCK_SIZE);
    }
}

static void calculate_4_bits_vag(int32_t* data, int8_t* four_bit, int32_t* coef_index,
    int32_t* shift_factor, int32_t* v, int32_t* tv, int32_t coef_index_count) {
    int32_t d0, e, g, i, j, k, l;
    int32_t err, min, s1, s2, c_f;

    c_f = 0;
    min = 0x7FFFFFFF;
    for (j = 0, l = coef_index_count; l; j++, l--) {
        calculate_4_bits_vag_inner(data, four_bit, j, shift_factor, v, tv, 0, 0);

        int32_t vag1 = hevag_table[j][0];
        int32_t vag2 = hevag_table[j][1];
        int32_t _shift_factor = *shift_factor;

        s1 = v[0];
        s2 = v[1];
        err = 0;
        for (i = 0, k = BLOCK_SIZE; k; i++, k--) {
            d0 = four_bit[i] << _shift_factor;

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
    *shift_factor = (int32_t)(20ULL - *shift_factor);
}

static void calculate_4_bits_vag_inner(int32_t* data, int8_t* four_bit, int32_t coef_index,
    int32_t* shift_factor, int32_t* v, int32_t* tv, int32_t* os, int32_t* ots) {
    int32_t d0, d1, e, g, i, j, s_f, t_s_f, m, shift_max;
    int32_t temp[BLOCK_SIZE];

    int32_t vag1 = hevag_table[coef_index][0];
    int32_t vag2 = hevag_table[coef_index][1];

    int32_t s1 = v[0];
    int32_t s2 = v[1];
    for (i = 0, j = BLOCK_SIZE, m = 0; j; i++, j--) {
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

    for (s_f = 12, shift_max = 0x400000; s_f; s_f--, shift_max >>= 1)
        if ((shift_max & (m + (shift_max >> 4))) == shift_max)
            break;

    t_s_f = (int32_t)(8ULL + s_f);
    s_f = (int32_t)(12ULL - s_f);

    int32_t ts1 = tv[0];
    int32_t ts2 = tv[1];
    for (i = 0, j = BLOCK_SIZE; j; i++, j--) {
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
    int32_t d0, e, g, i, j, k, l;
    int32_t err, min, s1, s2, s3, s4, c_f;

    c_f = 0;
    min = 0x7FFFFFFF;
    for (j = 0, l = coef_index_count; l; j++, l--) {
        calculate_4_bits_hevag_inner(data, four_bit, j, shift_factor, v, tv, 0, 0);

        int32_t hevag1 = hevag_table[j][0];
        int32_t hevag2 = hevag_table[j][1];
        int32_t hevag3 = hevag_table[j][2];
        int32_t hevag4 = hevag_table[j][3];
        s1 = v[0];
        s2 = v[1];
        s3 = v[2];
        s4 = v[3];
        int32_t _shift_factor = *shift_factor;
        err = 0;
        for (i = 0, k = BLOCK_SIZE; k; i++, k--) {
            d0 = four_bit[i] << _shift_factor;

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
    *shift_factor = (int32_t)(20ULL - *shift_factor);
}

static void calculate_4_bits_hevag_inner(int32_t* data, int8_t* four_bit, int32_t coef_index,
    int32_t* shift_factor, int32_t* v, int32_t* tv, int32_t* os, int32_t* ots) {
    int32_t d0, d1, e, g, i, j, s_f, t_s_f, m, shift_max;
    int32_t temp[BLOCK_SIZE];

    int32_t hevag1 = hevag_table[coef_index][0];
    int32_t hevag2 = hevag_table[coef_index][1];
    int32_t hevag3 = hevag_table[coef_index][2];
    int32_t hevag4 = hevag_table[coef_index][3];

    int32_t s1 = v[0];
    int32_t s2 = v[1];
    int32_t s3 = v[2];
    int32_t s4 = v[3];
    for (i = 0, j = BLOCK_SIZE, m = 0; j; i++, j--) {
        g = data[i];
        e = g - ((s1 * hevag1 + s2 * hevag2 + s3 * hevag3 + s4 * hevag4) >> 5);
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

    for (s_f = 12, shift_max = 0x400000; s_f; s_f--, shift_max >>= 1)
        if ((shift_max & (m + (shift_max >> 4))) == shift_max)
            break;

    t_s_f = (int32_t)(8ULL + s_f);
    s_f = (int32_t)(12ULL - s_f);

    int32_t ts1 = tv[0];
    int32_t ts2 = tv[1];
    int32_t ts3 = tv[2];
    int32_t ts4 = tv[3];

    for (i = 0, j = BLOCK_SIZE; j; i++, j--) {
        g = temp[i];
        e = g - ((ts1 * hevag1 + ts2 * hevag2 + ts3 * hevag3 + ts4 * hevag4) >> 5);

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
