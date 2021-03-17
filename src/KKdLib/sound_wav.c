/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "sound_wav.h"
#include "io_stream.h"

wav* wav_init() {
    wav* w = force_malloc(sizeof(wav));
    return w;
}

void wav_dispose(wav* f) {
    free(f);
}

void wav_read(wav* w, char* path, float_t** data, size_t* samples) {
    wchar_t* path_buf = char_string_to_wchar_t_string(path);
    wav_wread(w, path_buf, data, samples);
    free(path_buf);
}

void wav_wread(wav* w, wchar_t* path, float_t** data, size_t* samples) {
    *data = 0;
    *samples = 0;
    memset(w, 0, sizeof(wav));

    stream* s = io_wopen(path, L"rb");
    if (s->io) {
        wav w_t;
        if (io_read_uint32_t(s) != 0x46464952)
            goto End;
        io_read_uint32_t(s);
        if (io_read_uint32_t(s) != 0x45564157 || io_read_uint32_t(s) != 0x20746D66)
            goto End;
        size_t offset = io_read_uint32_t(s);
        w_t.format = io_read_uint16_t(s);
        if (w_t.format != 0x01 && w_t.format != 0x03 && w_t.format != 0xFFFE)
            goto End;

        w_t.channels = io_read_uint16_t(s);
        w_t.sample_rate = io_read_uint32_t(s);
        io_read_uint32_t(s);
        io_read_uint16_t(s);
        w_t.bytes = io_read_uint16_t(s);
        if (w_t.bytes % 8 || !(w_t.bytes / 8))
            goto End;
        if (w_t.format == 0xFFFE) {
            io_read_uint32_t(s);
            w_t.channel_mask = io_read_uint32_t(s);
            w_t.format = io_read_uint16_t(s);
        }

        w_t.bytes >>= 3;
        if (w_t.bytes < 1 || (w_t.bytes > 4 && w_t.bytes != 8))
            goto End;
        else if (w_t.bytes > 0 && w_t.bytes < 4 && w_t.format == 3)
            goto End;
        else if (w_t.bytes == 8 && w_t.format == 1)
            goto End;

        io_set_position(s, offset + 0x14, IO_SEEK_SET);
        if (io_read_uint32_t(s) != 0x61746164)
            goto End;
        w_t.size = io_read_uint32_t(s);
        w_t.header_size = (int32_t)io_get_position(s);
        w_t.is_supported = true;
        *w = w_t;

        *samples = w->size / w->channels / w->bytes;
        *data = force_malloc_s(sizeof(float_t), w->size / w->bytes);

        size_t t_s = *samples;
        float_t* t_d = *data;

        size_t ch = w->channels;
        if (w->bytes == 0x01 && w->format == 0x01)
            for (size_t i = 0; i < t_s; i++)
                for (size_t c = 0; c < ch; c++) {
                    double_t sample = io_read_int8_t(s);
                    sample /= 128.0;
                    t_d[i * ch + c] = (float_t)sample;
                }
        else if (w->bytes == 0x02 && (w->format == 0x01 || w->format == 0xFFFE))
            for (size_t i = 0; i < t_s; i++)
                for (size_t c = 0; c < ch; c++) {
                    double_t sample = io_read_int16_t(s);
                    sample /= 32768.0;
                    t_d[i * ch + c] = (float_t)sample;
                }
        else if (w->bytes == 0x03 && w->format == 0x01)
            for (size_t i = 0; i < t_s; i++)
                for (size_t c = 0; c < ch; c++) {
                    uint16_t samp = (uint16_t)io_read_uint16_t(s);
                    double_t sample = ((int32_t)io_read_int8_t(s) << 16) | samp;
                    sample /= 8388608.0;
                    t_d[i * ch + c] = (float_t)sample;
                }
        else if (w->bytes == 0x04 && w->format == 0x01)
            for (size_t i = 0; i < t_s; i++)
                for (size_t c = 0; c < ch; c++) {
                    double_t sample = io_read_int32_t(s);
                    sample /= 2147483648.0;
                    t_d[i * ch + c] = (float_t)sample;
                }
        else if (w->bytes == 0x04 && (w->format == 0x03 || w->format == 0xFFFE))
            for (size_t i = 0; i < t_s; i++)
                for (size_t c = 0; c < ch; c++) {
                    float_t sample = io_read_float_t(s);
                    t_d[i * ch + c] = sample;
                }
        else if (w->bytes == 0x08 && (w->format == 0x03 || w->format == 0xFFFE))
            for (size_t i = 0; i < t_s; i++)
                for (size_t c = 0; c < ch; c++) {
                    double_t sample = io_read_double_t(s);
                    t_d[i * ch + c] = (float_t)sample;
                }
    }
End:
    io_dispose(s);
}

void wav_write(wav* w, char* path, float_t* data, size_t samples) {
    wchar_t* path_buf = char_string_to_wchar_t_string(path);
    wav_wwrite(w, path_buf, data, samples);
    free(path_buf);
}

void wav_wwrite(wav* w, wchar_t* path, float_t* data, size_t samples) {
    stream* s = io_wopen(path, L"wb");
    if (s->io && data) {
        w->size = (int32_t)(samples * w->channels * w->bytes);

        if (w->bytes == 0x01 && w->format == 0x01);
        else if (w->bytes == 0x02 && w->format == 0x01);
        else if (w->bytes == 0x04 && w->format == 0x01);
        else if (w->bytes == 0x04 && (w->format == 0x03 || w->format == 0xFFFE));
        else if (w->bytes == 0x08 && (w->format == 0x03 || w->format == 0xFFFE));
        else
            w->size = 0;

        io_write_uint32_t(s, 0x46464952);
        io_write_uint32_t(s, w->size + (w->format != 0xFFFE ? 0x24 : 0x3C));
        io_write_uint32_t(s, 0x45564157);
        io_write_uint32_t(s, 0x20746D66);
        io_write_uint32_t(s, w->format != 0xFFFE ? 0x10 : 0x28);
        io_write_uint16_t(s, w->format);
        io_write_uint16_t(s, w->channels);
        io_write_uint32_t(s, w->sample_rate);
        io_write_uint32_t(s, w->sample_rate * w->channels * w->bytes);
        io_write_uint16_t(s, (uint16_t)(w->channels * w->bytes));
        io_write_uint16_t(s, (uint16_t)(w->bytes << 3));
        if (w->format == 0xFFFE) {
            io_write_uint16_t(s, 0x16);
            io_write_uint16_t(s, (uint16_t)(w->bytes << 3));
            io_write_uint32_t(s, w->channel_mask);
            io_write_uint32_t(s, w->bytes == 2 ? 0x01 : 0x03);
            io_write_uint32_t(s, 0x00100000);
            io_write_uint32_t(s, 0xAA000080);
            io_write_uint32_t(s, 0x719B3800);
        }
        io_write_uint32_t(s, 0x61746164);
        io_write_uint32_t(s, w->size);

        size_t ch = w->channels;
        if (w->bytes == 0x01 && w->format == 0x01)
            for (size_t i = 0; i < samples; i++)
                for (size_t c = 0; c < ch; c++) {
                    double_t sample = data[i * ch + c] * 128.0;
                    sample = clamp(sample, -128.0, 127.0);
                    io_write_char(s, (int8_t)round(sample));
                }
        else if (w->bytes == 0x02 && (w->format == 0x01 || w->format == 0xFFFE))
            for (size_t i = 0; i < samples; i++)
                for (size_t c = 0; c < ch; c++) {
                    double_t sample = data[i * ch + c] * 32768.0;
                    sample = clamp(sample, -32768.0, 32767.0);
                    io_write_int16_t(s, (int16_t)round(sample));
                }
        else if (w->bytes == 0x03 && w->format == 0x01)
            for (size_t i = 0; i < samples; i++)
                for (size_t c = 0; c < ch; c++) {
                    double_t sample = data[i * ch + c] * 8388608.0;
                    sample = clamp(sample, -8388608.0, 8388607.0);
                    int32_t samp = (int32_t)round(sample);
                    io_write_uint16_t(s, (uint16_t)samp);
                    io_write_int8_t(s, (int8_t)(samp >> 16));
                }
        else if (w->bytes == 0x04 && w->format == 0x01)
            for (size_t i = 0; i < samples; i++)
                for (size_t c = 0; c < ch; c++) {
                    double_t sample = data[i * ch + c] * 2147483648.0;
                    sample = clamp(sample, -2147483648.0, 2147483647.0);
                    io_write_int32_t(s, (int32_t)round(sample));
                }
        else if (w->bytes == 0x04 && (w->format == 0x03 || w->format == 0xFFFE))
            for (size_t i = 0; i < samples; i++)
                for (size_t c = 0; c < ch; c++) {
                    float_t sample = data[i * ch + c];
                    io_write_float_t(s, sample);
                }
        else if (w->bytes == 0x08 && (w->format == 0x03 || w->format == 0xFFFE))
            for (size_t i = 0; i < samples; i++)
                for (size_t c = 0; c < ch; c++) {
                    double_t sample = data[i * ch + c];
                    io_write_double_t(s, sample);
                }
    }
    io_dispose(s);
}