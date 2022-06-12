/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "wav.hpp"
#include "../io/stream.hpp"

wav* wav_init() {
    wav* w = force_malloc_s(wav, 1);
    return w;
}

void wav_read(wav* w, const char* path, float_t** data, size_t* samples) {
    wchar_t* path_buf = utf8_to_utf16(path);
    wav_read(w, path_buf, data, samples);
    free(path_buf);
}

void wav_read(wav* w, const wchar_t* path, float_t** data, size_t* samples) {
    *data = 0;
    *samples = 0;
    memset(w, 0, sizeof(wav));

    stream s;
    s.open(path, L"rb");
    if (s.io.stream) {
        wav w_t;
        if (s.read_uint32_t() != 0x46464952)
            return;
        s.read_uint32_t();
        if (s.read_uint32_t() != 0x45564157 || s.read_uint32_t() != 0x20746D66)
            return;
        size_t offset = s.read_uint32_t();
        w_t.format = s.read_uint16_t();
        if (w_t.format != 0x01 && w_t.format != 0x03 && w_t.format != 0xFFFE)
            return;

        w_t.channels = s.read_uint16_t();
        w_t.sample_rate = s.read_uint32_t();
        s.read_uint32_t();
        s.read_uint16_t();
        w_t.bytes = s.read_uint16_t();
        if (w_t.bytes % 8 || !(w_t.bytes / 8))
            return;
        if (w_t.format == 0xFFFE) {
            s.read_uint32_t();
            w_t.channel_mask = s.read_uint32_t();
            w_t.format = s.read_uint16_t();
        }

        w_t.bytes >>= 3;
        if (w_t.bytes < 1 || (w_t.bytes > 4 && w_t.bytes != 8))
            return;
        else if (w_t.bytes > 0 && w_t.bytes < 4 && w_t.format == 3)
            return;
        else if (w_t.bytes == 8 && w_t.format == 1)
            return;

        s.set_position(offset + 0x14, SEEK_SET);
        if (s.read_uint32_t() != 0x61746164)
            return;
        w_t.size = s.read_uint32_t();
        w_t.header_size = (int32_t)s.get_position();
        w_t.is_supported = true;
        *w = w_t;

        *samples = w->size / w->channels / w->bytes;
        *data = force_malloc_s(float_t, w->size / w->bytes);

        size_t t_s = *samples;
        float_t* t_d = *data;

        size_t ch = w->channels;
        if (w->bytes == 0x01 && w->format == 0x01)
            for (size_t i = 0; i < t_s; i++)
                for (size_t c = 0; c < ch; c++) {
                    double_t sample = s.read_int8_t();
                    sample = div_min_max(sample, -128.0, 127.0);
                    t_d[i * ch + c] = (float_t)sample;
                }
        else if (w->bytes == 0x02 && (w->format == 0x01 || w->format == 0xFFFE))
            for (size_t i = 0; i < t_s; i++)
                for (size_t c = 0; c < ch; c++) {
                    double_t sample = s.read_int16_t();
                    sample = div_min_max(sample, -32768.0, 32767.0);
                    t_d[i * ch + c] = (float_t)sample;
                }
        else if (w->bytes == 0x03 && w->format == 0x01)
            for (size_t i = 0; i < t_s; i++)
                for (size_t c = 0; c < ch; c++) {
                    int32_t samp;
                    s.read(&samp, 3);
                    double_t sample = (samp << 8) >> 8;
                    sample = div_min_max(sample, -8388608.0, 8388607.0);
                    t_d[i * ch + c] = (float_t)sample;
                }
        else if (w->bytes == 0x04 && w->format == 0x01)
            for (size_t i = 0; i < t_s; i++)
                for (size_t c = 0; c < ch; c++) {
                    double_t sample = s.read_int32_t();
                    sample = div_min_max(sample, -2147483648.0, 2147483647.0);
                    t_d[i * ch + c] = (float_t)sample;
                }
        else if (w->bytes == 0x04 && (w->format == 0x03 || w->format == 0xFFFE))
            for (size_t i = 0; i < t_s; i++)
                for (size_t c = 0; c < ch; c++) {
                    float_t sample = s.read_float_t();
                    t_d[i * ch + c] = sample;
                }
        else if (w->bytes == 0x08 && (w->format == 0x03 || w->format == 0xFFFE))
            for (size_t i = 0; i < t_s; i++)
                for (size_t c = 0; c < ch; c++) {
                    double_t sample = s.read_double_t();
                    t_d[i * ch + c] = (float_t)sample;
                }
    }
}

void wav_write(wav* w, const char* path, float_t* data, size_t samples) {
    wchar_t* path_buf = utf8_to_utf16(path);
    wav_write(w, path_buf, data, samples);
    free(path_buf);
}

void wav_write(wav* w, const wchar_t* path, float_t* data, size_t samples) {
    stream s;
    s.open(path, L"wb");
    if (s.io.stream && data) {
        w->size = (int32_t)(samples * w->channels * w->bytes);

        if (w->bytes == 0x01 && w->format == 0x01);
        else if (w->bytes == 0x02 && w->format == 0x01);
        else if (w->bytes == 0x04 && w->format == 0x01);
        else if (w->bytes == 0x04 && (w->format == 0x03 || w->format == 0xFFFE));
        else if (w->bytes == 0x08 && (w->format == 0x03 || w->format == 0xFFFE));
        else
            w->size = 0;

        s.write_uint32_t(0x46464952);
        s.write_uint32_t(w->size + (w->format != 0xFFFE ? 0x24 : 0x3C));
        s.write_uint32_t(0x45564157);
        s.write_uint32_t(0x20746D66);
        s.write_uint32_t(w->format != 0xFFFE ? 0x10 : 0x28);
        s.write_uint16_t(w->format);
        s.write_uint16_t(w->channels);
        s.write_uint32_t(w->sample_rate);
        s.write_uint32_t(w->sample_rate * w->channels * w->bytes);
        s.write_uint16_t((uint16_t)(w->channels * w->bytes));
        s.write_uint16_t((uint16_t)(w->bytes << 3));
        if (w->format == 0xFFFE) {
            s.write_uint16_t(0x16);
            s.write_uint16_t((uint16_t)(w->bytes << 3));
            s.write_uint32_t(w->channel_mask);
            s.write_uint32_t(w->bytes == 2 ? 0x01 : 0x03);
            s.write_uint32_t(0x00100000);
            s.write_uint32_t(0xAA000080);
            s.write_uint32_t(0x719B3800);
        }
        s.write_uint32_t(0x61746164);
        s.write_uint32_t(w->size);

        size_t ch = w->channels;
        if (w->bytes == 0x01 && w->format == 0x01)
            for (size_t i = 0; i < samples; i++)
                for (size_t c = 0; c < ch; c++) {
                    double_t sample = data[i * ch + c];
                    sample = clamp(sample, -1.0, 1.0);
                    sample = mult_min_max(sample, -128.0, 127.0);
                    s.write_int8_t((int8_t)round(sample));
                }
        else if (w->bytes == 0x02 && (w->format == 0x01 || w->format == 0xFFFE))
            for (size_t i = 0; i < samples; i++)
                for (size_t c = 0; c < ch; c++) {
                    double_t sample = data[i * ch + c];
                    sample = clamp(sample, -1.0, 1.0);
                    sample = mult_min_max(sample, -32768.0, 32767.0);
                    s.write_int16_t((int16_t)round(sample));
                }
        else if (w->bytes == 0x03 && w->format == 0x01)
            for (size_t i = 0; i < samples; i++)
                for (size_t c = 0; c < ch; c++) {
                    double_t sample = data[i * ch + c];
                    sample = clamp(sample, -1.0, 1.0);
                    sample = mult_min_max(sample, -8388608.0, 8388607.0);
                    int32_t samp = (int32_t)round(sample);
                    s.write(&samp, 3);
                }
        else if (w->bytes == 0x04 && w->format == 0x01)
            for (size_t i = 0; i < samples; i++)
                for (size_t c = 0; c < ch; c++) {
                    double_t sample = data[i * ch + c];
                    sample = clamp(sample, -1.0, 1.0);
                    sample = mult_min_max(sample, -2147483648.0, 2147483647.0);
                    s.write_int32_t((int32_t)round(sample));
                }
        else if (w->bytes == 0x04 && (w->format == 0x03 || w->format == 0xFFFE))
            for (size_t i = 0; i < samples; i++)
                for (size_t c = 0; c < ch; c++) {
                    float_t sample = data[i * ch + c];
                    s.write_float_t(sample);
                }
        else if (w->bytes == 0x08 && (w->format == 0x03 || w->format == 0xFFFE))
            for (size_t i = 0; i < samples; i++)
                for (size_t c = 0; c < ch; c++) {
                    double_t sample = data[i * ch + c];
                    s.write_double_t(sample);
                }
    }
}

void wav_dispose(wav* f) {
    free(f);
}