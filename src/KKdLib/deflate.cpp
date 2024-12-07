/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "deflate.hpp"
#include <libdeflate.h>

namespace deflate {
    static void* deflate_malloc(void* data, size_t size) {
        return malloc(size);
    }

    static void deflate_free(void* data, void* ptr) {
        free(ptr);
    }

    const allocator default_allocator = {
        0,
        deflate_malloc,
        deflate_free,
    };

    static int32_t compress_static(struct libdeflate_compressor* c, const void* src, size_t src_length,
        void*& dst, size_t& dst_length, int32_t compression_level, mode mode, const allocator& allocator);
    static int32_t decompress_static(struct libdeflate_decompressor* d, const void* src, size_t src_length,
        void*& dst, size_t& dst_length, mode mode, const allocator& allocator);

    int32_t compress(const void* src, size_t src_length, void*& dst, size_t& dst_length,
        mode mode, int32_t compression_level, const allocator& allocator) {
        if (!src_length)
            return -1;
        else if (!src)
            return -2;
        else if (mode < MODE_DEFLATE || mode > MODE_ZLIB)
            return -3;
        else if (compression_level < 0 || compression_level > 12)
            return -4;

        struct libdeflate_compressor* c = libdeflate_alloc_compressor(compression_level);
        if (!c)
            return -5;

        int32_t result = compress_static(c, src, src_length,
            dst, dst_length, compression_level, mode, allocator);
        libdeflate_free_compressor(c);
        return result >= 0 ? result : result - 0x10;
    }

    int32_t compress_gzip(const void* src, size_t src_length, void*& dst, size_t& dst_length,
        int32_t compression_level, const char* file_name, const allocator& allocator) {
        if (!src_length)
            return -1;
        else if (!src)
            return -2;
        else if (compression_level < 0 || compression_level > 12)
            return -4;

        struct libdeflate_compressor* c = libdeflate_alloc_compressor(compression_level);
        if (!c)
            return -5;

        int32_t result = compress_static(c, src, src_length,
            dst, dst_length, compression_level, MODE_GZIP, allocator);
        libdeflate_free_compressor(c);
        if (result < 0)
            return result - 0x10;

        size_t file_name_length = utf8_length(file_name);
        void* temp = allocator.malloc_callback(allocator.data, dst_length + file_name_length + 1);
        size_t t = (size_t)temp;
        size_t d = (size_t)dst;
        memcpy((void*)t, (void*)d, 0x0A);
        memcpy((void*)(t + 0x0A), file_name, file_name_length + 1);
        memcpy((void*)(t + 0x0A + file_name_length + 1), (void*)(d + 0x0A), dst_length - 0x0A);
        ((uint8_t*)t)[0x03] |= 0x08;
        if (dst) {
            allocator.free_callback(allocator.data, dst);
            dst = 0;
        }
        dst = temp;
        dst_length += file_name_length + 1;
        return result;
    }

    int32_t decompress(const void* src, size_t src_length, void*& dst, size_t& dst_length,
        mode mode, const allocator& allocator) {
        if (!src_length)
            return -1;
        else if (!src)
            return -2;
        else if (mode < MODE_DEFLATE || mode > MODE_ZLIB)
            return -3;

        if (!dst_length)
            dst_length = 1;
        struct libdeflate_decompressor* d = libdeflate_alloc_decompressor();
        if (!d)
            return -4;

        int32_t result = decompress_static(d, src, src_length, dst, dst_length, mode, allocator);
        libdeflate_free_decompressor(d);
        return result >= 0 ? result : result - 0x10;
    }

    static int32_t compress_static(struct libdeflate_compressor* c, const void* src, size_t src_length,
        void*& dst, size_t& dst_length, int32_t compression_level, mode mode, const allocator& allocator) {
        size_t dst_max_length;
        switch (mode) {
        case MODE_GZIP:
            dst_max_length = libdeflate_gzip_compress_bound(c, src_length);
            break;
        case MODE_ZLIB:
            dst_max_length = libdeflate_zlib_compress_bound(c, src_length);
            break;
        default:
            dst_max_length = libdeflate_deflate_compress_bound(c, src_length);
            break;
        }

        dst = allocator.malloc_callback(allocator.data, dst_max_length);
        if (!dst)
            return -1;

        size_t dst_act_length;
        switch (mode) {
        case MODE_GZIP:
            dst_act_length = libdeflate_gzip_compress(c, src, src_length, dst, dst_max_length);
            break;
        case MODE_ZLIB:
            dst_act_length = libdeflate_zlib_compress(c, src, src_length, dst, dst_max_length);
            break;
        default:
            dst_act_length = libdeflate_deflate_compress(c, src, src_length, dst, dst_max_length);
            break;
        }

        if (dst_act_length == dst_max_length) {
            dst_length = dst_act_length;
            return 0;
        }

        void* temp = allocator.malloc_callback(allocator.data, dst_act_length);
        if (!temp) {
            if (dst) {
                allocator.free_callback(allocator.data, dst);
                dst = 0;
            }
            return -2;
        }

        memcpy(temp, dst, dst_act_length);
        if (dst) {
            allocator.free_callback(allocator.data, dst);
            dst = 0;
        }
        dst = temp;
        dst_length = dst_act_length;
        return 0;
    }

    static int32_t decompress_static(struct libdeflate_decompressor* d, const void* src, size_t src_length,
        void*& dst, size_t& dst_length, mode mode, const allocator& allocator) {
        dst = allocator.malloc_callback(allocator.data, dst_length);
        if (!dst)
            return -1;

        size_t dst_act_length = 0;
        enum libdeflate_result result;
        switch (mode) {
        case MODE_GZIP:
            result = libdeflate_gzip_decompress(d, src, src_length, dst, dst_length, &dst_act_length);
            break;
        case MODE_ZLIB:
            result = libdeflate_zlib_decompress(d, src, src_length, dst, dst_length, &dst_act_length);
            break;
        default:
            result = libdeflate_deflate_decompress(d, src, src_length, dst, dst_length, &dst_act_length);
            break;
        }

        switch (result) {
        case LIBDEFLATE_BAD_DATA:
            if (dst) {
                allocator.free_callback(allocator.data, dst);
                dst = 0;
            }
            return -2;
        case LIBDEFLATE_INSUFFICIENT_SPACE:
            if (dst) {
                allocator.free_callback(allocator.data, dst);
                dst = 0;
            }
            dst_length *= 2;
            return decompress_static(d, src, src_length, dst, dst_length, mode, allocator);
        default:
            if (dst_act_length >= dst_length)
                break;

            void* temp = allocator.malloc_callback(allocator.data, dst_act_length);
            if (!temp) {
                if (dst) {
                    allocator.free_callback(allocator.data, dst);
                    dst = 0;
                }
                return -3;
            }

            memcpy(temp, dst, dst_act_length);
            if (dst) {
                allocator.free_callback(allocator.data, dst);
                dst = 0;
            }
            dst = temp;
            dst_length = dst_act_length;
            break;
        }
        return 0;
    }
}
