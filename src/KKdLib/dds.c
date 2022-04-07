/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "dds.h"
#include "io/stream.h"
#include "str_utils.h"

#pragma pack(push, 1)

const uint32_t DDS_MAGIC = 'DDS ';

typedef struct DDS_PIXELFORMAT {
    uint32_t size;
    uint32_t flags;
    uint32_t fourCC;
    uint32_t RGBBitCount;
    uint32_t RBitMask;
    uint32_t GBitMask;
    uint32_t BBitMask;
    uint32_t ABitMask;
} DDS_PIXELFORMAT;

#define DDSD_CAPS           0x00000001
#define DDSD_HEIGHT         0x00000002
#define DDSD_WIDTH          0x00000004
#define DDSD_PITCH          0x00000008
#define DDSD_PIXELFORMAT    0x00001000
#define DDSD_MIPMAPCOUNT    0x00020000
#define DDSD_LINEARSIZE     0x00080000
#define DDSD_DEPTH          0x00800000

#define DDS_FOURCC      0x00000004  // DDPF_FOURCC
#define DDS_RGB         0x00000040  // DDPF_RGB
#define DDS_RGBA        0x00000041  // DDPF_RGB | DDPF_ALPHAPIXELS
#define DDS_LUMINANCE   0x00020000  // DDPF_LUMINANCE
#define DDS_LUMINANCEA  0x00020001  // DDPF_LUMINANCE | DDPF_ALPHAPIXELS
#define DDS_ALPHAPIXELS 0x00000001  // DDPF_ALPHAPIXELS
#define DDS_ALPHA       0x00000002  // DDPF_ALPHA

#define DDSGLOBALCONST extern const
#define DDSCONST extern const

#define DDS_HEADER_FLAGS_TEXTURE        0x00001007  // DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT
#define DDS_HEADER_FLAGS_MIPMAP         0x00020000  // DDSD_MIPMAPCOUNT
#define DDS_HEADER_FLAGS_VOLUME         0x00800000  // DDSD_DEPTH
#define DDS_HEADER_FLAGS_PITCH          0x00000008  // DDSD_PITCH
#define DDS_HEADER_FLAGS_LINEARSIZE     0x00080000  // DDSD_LINEARSIZE

#define DDS_HEIGHT 0x00000002 // DDSD_HEIGHT
#define DDS_WIDTH  0x00000004 // DDSD_WIDTH

#define DDS_SURFACE_FLAGS_TEXTURE 0x00001000 // DDSCAPS_TEXTURE
#define DDS_SURFACE_FLAGS_MIPMAP  0x00400008 // DDSCAPS_COMPLEX | DDSCAPS_MIPMAP
#define DDS_SURFACE_FLAGS_cube_map 0x00000008 // DDSCAPS_COMPLEX

#define DDS_cube_map_ALLFACES ( 0x00000400 | 0x00000800 |\
                               0x00001000 | 0x00002000 |\
                               0x00004000 | 0x00008000 | 0x00000200 )

#define DDS_cube_map 0x00000200 // DDSCAPS2_cube_map

#define DDS_FLAGS_VOLUME 0x00200000 // DDSCAPS2_VOLUME

typedef struct DDS_HEADER {
    uint32_t        size;
    uint32_t        flags;
    uint32_t        height;
    uint32_t        width;
    uint32_t        pitchOrLinearSize;
    uint32_t        depth; // only if DDS_HEADER_FLAGS_VOLUME is set in flags
    uint32_t        mipMapCount;
    uint32_t        reserved1[11];
    DDS_PIXELFORMAT ddspf;
    uint32_t        caps;
    uint32_t        caps2;
    uint32_t        caps3;
    uint32_t        caps4;
    uint32_t        reserved2;
} DDS_HEADER;

#pragma pack(pop)

DDSCONST DDS_PIXELFORMAT DDSPF_DXT1 =
{ sizeof(DDS_PIXELFORMAT), DDS_FOURCC, '1TXD', 0, 0, 0, 0, 0 };

DDSCONST DDS_PIXELFORMAT DDSPF_DXT3 =
{ sizeof(DDS_PIXELFORMAT), DDS_FOURCC, '3TXD', 0, 0, 0, 0, 0 };

DDSCONST DDS_PIXELFORMAT DDSPF_DXT5 =
{ sizeof(DDS_PIXELFORMAT), DDS_FOURCC, '5TXD', 0, 0, 0, 0, 0 };

DDSCONST DDS_PIXELFORMAT DDSPF_ATI1 =
{ sizeof(DDS_PIXELFORMAT), DDS_FOURCC, '1ITA', 0, 0, 0, 0, 0 };

DDSCONST DDS_PIXELFORMAT DDSPF_ATI2 =
{ sizeof(DDS_PIXELFORMAT), DDS_FOURCC, '2ITA', 0, 0, 0, 0, 0 };

DDSCONST DDS_PIXELFORMAT DDSPF_BC4_UNORM =
{ sizeof(DDS_PIXELFORMAT), DDS_FOURCC, 'U4CB', 0, 0, 0, 0, 0 };

DDSCONST DDS_PIXELFORMAT DDSPF_BC5_UNORM =
{ sizeof(DDS_PIXELFORMAT), DDS_FOURCC, 'U5CB', 0, 0, 0, 0, 0 };

DDSCONST DDS_PIXELFORMAT DDSPF_A8R8G8B8 =
{ sizeof(DDS_PIXELFORMAT), DDS_RGBA, 0, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000 };

DDSCONST DDS_PIXELFORMAT DDSPF_A8B8G8R8 =
{ sizeof(DDS_PIXELFORMAT), DDS_RGBA, 0, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000 };

DDSCONST DDS_PIXELFORMAT DDSPF_R5G6B5 =
{ sizeof(DDS_PIXELFORMAT), DDS_RGB, 0, 16, 0xf800, 0x07e0, 0x001f, 0 };

DDSCONST DDS_PIXELFORMAT DDSPF_B5G6R5 =
{ sizeof(DDS_PIXELFORMAT), DDS_RGB, 0, 16, 0x001f, 0x07e0, 0xf800, 0 };

DDSCONST DDS_PIXELFORMAT DDSPF_A1R5G5B5 =
{ sizeof(DDS_PIXELFORMAT), DDS_RGBA, 0, 16, 0x7c00, 0x03e0, 0x001f, 0x8000 };

DDSCONST DDS_PIXELFORMAT DDSPF_A1B5G5R5 =
{ sizeof(DDS_PIXELFORMAT), DDS_RGBA, 0, 16, 0x001f, 0x03e0, 0x7c00, 0x8000 };

DDSCONST DDS_PIXELFORMAT DDSPF_A4R4G4B4 =
{ sizeof(DDS_PIXELFORMAT), DDS_RGBA, 0, 16, 0x0f00, 0x00f0, 0x000f, 0xf000 };

DDSCONST DDS_PIXELFORMAT DDSPF_A4B4G4R4 =
{ sizeof(DDS_PIXELFORMAT), DDS_RGBA, 0, 16, 0x000f, 0x00f0, 0x0f00, 0xf000 };

DDSCONST DDS_PIXELFORMAT DDSPF_R8G8B8 =
{ sizeof(DDS_PIXELFORMAT), DDS_RGB, 0, 24, 0xff0000, 0x00ff00, 0x0000ff, 0 };

DDSCONST DDS_PIXELFORMAT DDSPF_B8G8R8 =
{ sizeof(DDS_PIXELFORMAT), DDS_RGB, 0, 24, 0x0000ff, 0x00ff00, 0xff0000, 0 };

DDSCONST DDS_PIXELFORMAT DDSPF_L8 =
{ sizeof(DDS_PIXELFORMAT), DDS_LUMINANCE, 0, 8, 0xff, 0, 0, 0 };

DDSCONST DDS_PIXELFORMAT DDSPF_A8L8 =
{ sizeof(DDS_PIXELFORMAT), DDS_LUMINANCEA, 0, 16, 0x00ff, 0, 0, 0xff00 };

DDSCONST DDS_PIXELFORMAT DDSPF_L8A8 =
{ sizeof(DDS_PIXELFORMAT), DDS_LUMINANCEA, 0, 16, 0xff00, 0, 0, 0x00ff };

DDSCONST DDS_PIXELFORMAT DDSPF_A8 =
{ sizeof(DDS_PIXELFORMAT), DDS_ALPHA, 0, 8, 0, 0, 0, 0xff };

static void dds_reverse_rgb(txp_format format, ssize_t size, uint8_t* data);
static bool dds_check_is_dxt1a(ssize_t size, uint8_t* data);

dds* dds_init() {
    dds* d = force_malloc_s(dds, 1);
    return d;
}

void dds_read(dds* d, char* path) {
    if (!d || !path)
        return;

    wchar_t* path_buf = utf8_to_utf16(path);
    dds_wread(d, path_buf);
    free(path_buf);
}

void dds_wread(dds* d, wchar_t* path) {
    if (!d || !path)
        return;

    vector_old_ptr_void_free(&d->data, 0);
    memset(d, 0, sizeof(dds));
    wchar_t* path_dds = str_utils_add(path, L".dds");
    stream s;
    io_open(&s, path_dds, L"rb");
    if (s.io.stream) {
        if (io_read_uint32_t_reverse_endianness(&s, true) != DDS_MAGIC)
            goto End;

        DDS_HEADER dds_h;
        memset(&dds_h, 0, sizeof(DDS_HEADER));
        if (io_read(&s, &dds_h, sizeof(DDS_HEADER)) != sizeof(DDS_HEADER))
            goto End;

        if (!((dds_h.flags & DDSD_CAPS) && (dds_h.flags & DDSD_HEIGHT)
            && (dds_h.flags & DDSD_WIDTH) && (dds_h.flags & DDSD_PIXELFORMAT)))
            goto End;
        else if ((dds_h.flags & DDSD_PITCH) || (dds_h.flags & DDSD_DEPTH))
            goto End;
        else if (!(dds_h.caps & DDS_SURFACE_FLAGS_TEXTURE))
            goto End;
        else if ((dds_h.caps2 & DDS_cube_map) && (~dds_h.caps2 & DDS_cube_map_ALLFACES) || (dds_h.flags & DDS_FLAGS_VOLUME))
            goto End;

        bool reverse = false;
        if (!memcmp(&dds_h.ddspf, &DDSPF_A8, sizeof(DDS_PIXELFORMAT)))
            d->format = TXP_A8;
        else if (!memcmp(&dds_h.ddspf, &DDSPF_R8G8B8, sizeof(DDS_PIXELFORMAT))) {
            d->format = TXP_RGB8;
            reverse = true;
        }
        else if (!memcmp(&dds_h.ddspf, &DDSPF_B8G8R8, sizeof(DDS_PIXELFORMAT)))
            d->format = TXP_RGB8;
        else if (!memcmp(&dds_h.ddspf, &DDSPF_A8R8G8B8, sizeof(DDS_PIXELFORMAT))) {
            d->format = TXP_RGBA8;
            reverse = true;
        }
        else if (!memcmp(&dds_h.ddspf, &DDSPF_A8B8G8R8, sizeof(DDS_PIXELFORMAT)))
            d->format = TXP_RGBA8;
        else if (!memcmp(&dds_h.ddspf, &DDSPF_R5G6B5, sizeof(DDS_PIXELFORMAT))) {
            d->format = TXP_RGB5;
            reverse = true;
        }
        else if (!memcmp(&dds_h.ddspf, &DDSPF_B5G6R5, sizeof(DDS_PIXELFORMAT)))
            d->format = TXP_RGB5;
        else if (!memcmp(&dds_h.ddspf, &DDSPF_A1R5G5B5, sizeof(DDS_PIXELFORMAT))) {
            d->format = TXP_RGB5A1;
            reverse = true;
        }
        else if (!memcmp(&dds_h.ddspf, &DDSPF_A1B5G5R5, sizeof(DDS_PIXELFORMAT)))
            d->format = TXP_A8;
        else if (!memcmp(&dds_h.ddspf, &DDSPF_A4R4G4B4, sizeof(DDS_PIXELFORMAT))) {
            d->format = TXP_RGBA4;
            reverse = true;
        }
        else if (!memcmp(&dds_h.ddspf, &DDSPF_A4B4G4R4, sizeof(DDS_PIXELFORMAT)))
            d->format = TXP_RGBA4;
        else if (!memcmp(&dds_h.ddspf, &DDSPF_DXT1, sizeof(uint32_t) * 3))
            d->format = TXP_DXT1;
        else if (!memcmp(&dds_h.ddspf, &DDSPF_DXT3, sizeof(uint32_t) * 3))
            d->format = TXP_DXT3;
        else if (!memcmp(&dds_h.ddspf, &DDSPF_DXT5, sizeof(uint32_t) * 3))
            d->format = TXP_DXT5;
        else if (!memcmp(&dds_h.ddspf, &DDSPF_ATI1, sizeof(uint32_t) * 3))
            d->format = TXP_ATI1;
        else if (!memcmp(&dds_h.ddspf, &DDSPF_BC4_UNORM, sizeof(uint32_t) * 3))
            d->format = TXP_ATI1;
        else if (!memcmp(&dds_h.ddspf, &DDSPF_ATI2, sizeof(uint32_t) * 3))
            d->format = TXP_ATI2;
        else if (!memcmp(&dds_h.ddspf, &DDSPF_BC5_UNORM, sizeof(uint32_t) * 3))
            d->format = TXP_ATI2;
        else if (!memcmp(&dds_h.ddspf, &DDSPF_L8, sizeof(DDS_PIXELFORMAT)))
            d->format = TXP_L8;
        else if (!memcmp(&dds_h.ddspf, &DDSPF_L8A8, sizeof(DDS_PIXELFORMAT))) {
            d->format = TXP_L8A8;
            reverse = true;
        }
        else if (!memcmp(&dds_h.ddspf, &DDSPF_A8L8, sizeof(DDS_PIXELFORMAT)))
            d->format = TXP_L8A8;
        else
            goto End;

        d->width = dds_h.width;
        d->height = dds_h.height;
        d->mipmaps_count = dds_h.flags & DDSD_MIPMAPCOUNT ? dds_h.mipMapCount : 1;
        d->has_cube_map = dds_h.caps2 & DDS_cube_map ? true : false;
        d->data = vector_old_ptr_empty(void);
        vector_old_ptr_void_reserve(&d->data, d->has_cube_map ? d->mipmaps_count * 6ULL : d->mipmaps_count);

        do
            for (uint32_t i = 0; i < d->mipmaps_count; i++) {
                uint32_t width = max(d->width >> i, 1);
                uint32_t height = max(d->height >> i, 1);
                uint32_t size = txp::get_size(d->format,
                    max(d->width >> i, 1), max(d->height >> i, 1));
                void* data = force_malloc(size);
                io_read(&s, data, size);
                if (reverse)
                    dds_reverse_rgb(d->format, size, (uint8_t*)data);
                else if (d->format == TXP_DXT1 && dds_check_is_dxt1a(size, (uint8_t*)data))
                    d->format = TXP_DXT1a;
                vector_old_ptr_void_push_back(&d->data, &data);
            }
        while (d->has_cube_map && vector_old_length(d->data) / d->mipmaps_count < 6);
    }
End:
    io_free(&s);
    free(path_dds);
}

void dds_write(dds* d, char* path) {
    if (!d || !path || vector_old_length(d->data) < 1)
        return;

    wchar_t* path_buf = utf8_to_utf16(path);
    dds_wwrite(d, path_buf);
    free(path_buf);
}

void dds_wwrite(dds* d, wchar_t* path) {
    if (!d || !path || vector_old_length(d->data) < 1)
        return;

    wchar_t* path_dds = str_utils_add(path, L".dds");
    stream s;
    io_open(&s, path_dds, L"wb");
    if (s.io.stream) {
        DDS_HEADER dds_h;
        memset(&dds_h, 0, sizeof(DDS_HEADER));
        dds_h.size = sizeof(DDS_HEADER);
        dds_h.flags = DDS_HEADER_FLAGS_TEXTURE | DDSD_LINEARSIZE;
        if (d->mipmaps_count > 1)
            dds_h.flags |= DDSD_MIPMAPCOUNT;
        dds_h.height = d->height;
        dds_h.width = d->width;
        dds_h.pitchOrLinearSize = txp::get_size(d->format, d->width, d->height);
        dds_h.mipMapCount = d->mipmaps_count;
        dds_h.caps |= DDS_SURFACE_FLAGS_TEXTURE;
        if (d->has_cube_map)
            dds_h.caps |= DDS_SURFACE_FLAGS_cube_map;

        if (d->mipmaps_count > 1)
            dds_h.caps |= DDS_SURFACE_FLAGS_MIPMAP;

        if (d->has_cube_map)
            dds_h.caps2 |= DDS_cube_map_ALLFACES;

        switch (d->format) {
        case TXP_A8:
            dds_h.ddspf = DDSPF_A8;
            break;
        case TXP_RGB8:
            dds_h.ddspf = DDSPF_R8G8B8;
            break;
        case TXP_RGBA8:
            dds_h.ddspf = DDSPF_A8R8G8B8;
            break;
        case TXP_RGB5:
            dds_h.ddspf = DDSPF_R5G6B5;
            break;
        case TXP_RGB5A1:
            dds_h.ddspf = DDSPF_A1R5G5B5;
            break;
        case TXP_RGBA4:
            dds_h.ddspf = DDSPF_A4R4G4B4;
            break;
        case TXP_DXT1:
        case TXP_DXT1a:
            dds_h.ddspf = DDSPF_DXT1;
            break;
        case TXP_DXT3:
            dds_h.ddspf = DDSPF_DXT3;
            break;
        case TXP_DXT5:
            dds_h.ddspf = DDSPF_DXT5;
            break;
        case TXP_ATI1:
            dds_h.ddspf = DDSPF_ATI1;
            break;
        case TXP_ATI2:
            dds_h.ddspf = DDSPF_ATI2;
            break;
        case TXP_L8:
            dds_h.ddspf = DDSPF_L8;
            break;
        case TXP_L8A8:
            dds_h.ddspf = DDSPF_L8A8;
            break;
        }

        io_write_uint32_t_reverse_endianness(&s, DDS_MAGIC, true);
        io_write(&s, &dds_h, sizeof(DDS_HEADER));

        uint32_t index = 0;
        do
            for (uint32_t i = 0; i < d->mipmaps_count; i++) {
                uint32_t size = txp::get_size(d->format,
                    max(d->width >> i, 1), max(d->height >> i, 1));
                void* data = force_malloc(size);
                memcpy(data, d->data.begin[index], size);
                dds_reverse_rgb(d->format, size, (uint8_t*)data);
                io_write(&s, data, size);
                free(data);
                index++;
            }
        while (d->has_cube_map && index / d->mipmaps_count < 6);
    }
    io_free(&s);
    free(path_dds);
}

void dds_dispose(dds* d) {
    if (!d)
        return;

    vector_old_ptr_void_free(&d->data, 0);
    free(d);
}

static void dds_reverse_rgb(txp_format format, ssize_t size, uint8_t* data) {
    uint8_t l, r, g, b, a;
    switch (format) {
    case TXP_RGB8:
        while (size > 0) {
            b = data[0];
            g = data[1];
            r = data[2];
            data[0] = r;
            data[1] = g;
            data[2] = b;
            data += 3;
            size -= 3;
        }
        break;
    case TXP_RGBA8:
        while (size > 0) {
            b = data[0];
            g = data[1];
            r = data[2];
            a = data[3];
            data[0] = r;
            data[1] = g;
            data[2] = b;
            data[3] = a;
            data += 4;
            size -= 4;
        }
        break;
    case TXP_RGB5:
        while (size > 0) {
            b = (uint8_t)(*(uint16_t*)data) & 0x1F;
            g = (uint8_t)(*(uint16_t*)data >> 5) & 0x3F;
            r = (uint8_t)(*(uint16_t*)data >> 11) & 0x1F;
            *(uint16_t*)data = (b << 11) | (g << 5) | r;
            data += 2;
            size -= 2;
        }
        break;
    case TXP_RGB5A1:
        while (size > 0) {
            b = (uint8_t)(*(uint16_t*)data) & 0x1F;
            g = (uint8_t)(*(uint16_t*)data >> 5) & 0x1F;
            r = (uint8_t)(*(uint16_t*)data >> 10) & 0x1F;
            a = (uint8_t)(*(uint16_t*)data >> 15) & 0x01;
            *(uint16_t*)data = (a << 15) | (b << 10) | (g << 5) | r;
            data += 2;
            size -= 2;
        }
        break;
    case TXP_RGBA4:
        while (size > 0) {
            b = (uint8_t)(*(uint16_t*)data) & 0x0F;
            g = (uint8_t)(*(uint16_t*)data >> 4) & 0x0F;
            r = (uint8_t)(*(uint16_t*)data >> 8) & 0x0F;
            a = (uint8_t)(*(uint16_t*)data >> 12) & 0x0F;
            *(uint16_t*)data = (a << 12) | (b << 8) | (g << 4) | r;
            data += 2;
            size -= 2;
        }
        break;
    case TXP_L8A8:
        while (size > 0) {
            a = data[0];
            l = data[1];
            data[0] = l;
            data[1] = a;
            data += 2;
            size -= 2;
        }
        break;
    }
}

static bool dds_check_is_dxt1a(ssize_t size, uint8_t* data) {
    while (size > 0) {
        if (*(uint16_t*)data <= *(uint16_t*)(data + 2))
            return true;
        data += 8;
        size -= 8;
    }
    return false;
}