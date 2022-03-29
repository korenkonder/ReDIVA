#include "unedat.h"
#include "../KKdLib/aes.h"
#include "lz.h"
#include <math.h>

void aescbc128_decrypt(uint8_t* key, uint8_t* iv, uint8_t* in, uint8_t* out, int len) {
    struct aes_ctx ctx;
    aes_init_ctx_iv(&ctx, key, iv);
    memcpy(out, in, len);
    aes_cbc_decrypt_buffer(&ctx, out, len);

    // Reset the IV.
    memset(iv, 0, 0x10);
}

void aescbc128_encrypt(uint8_t* key, uint8_t* iv, uint8_t* in, uint8_t* out, int len) {
    struct aes_ctx ctx;
    aes_init_ctx_iv(&ctx, key, iv);
    memcpy(out, in, len);
    aes_cbc_encrypt_buffer(&ctx, out, len);

    // Reset the IV.
    memset(iv, 0, 0x10);
}

void aesecb128_encrypt(uint8_t* key, uint8_t* in, uint8_t* out) {
    struct aes_ctx ctx;
    aes_init_ctx(&ctx, key);
    memcpy(out, in, 0x10);
    aes_ecb_encrypt(&ctx, out);
}

void rap_to_rif(uint8_t* rap, uint8_t* rif) {
    int i;
    int round;
    struct aes_ctx ctx;

    uint8_t key[0x10];
    uint8_t iv[0x10];
    memcpy(key, rap, 0x10);
    memset(iv, 0, 0x10);

    // Initial decrypt.
    aes_init_ctx_iv(&ctx, RAP_KEY, iv);
    aes_cbc_decrypt_buffer(&ctx, key, 0x10);

    // rap2rifkey round.
    for (round = 0; round < 5; ++round) {
        for (i = 0; i < 16; ++i) {
            int p = RAP_PBOX[i];
            key[p] ^= RAP_E1[p];
        }

        for (i = 15; i > 0; --i) {
            int p = RAP_PBOX[i];
            int pp = RAP_PBOX[i - 1];
            key[p] ^= key[pp];
        }

        uint8_t o = 0;
        for (i = 0; i < 16; ++i) {
            int p = RAP_PBOX[i];
            uint8_t kc = key[p] - o;
            uint8_t ec2 = RAP_E2[p];
            if (!o || kc != 0xFF)
                o = kc < ec2 ? 1 : 0;
            key[p] = kc - ec2;
        }
    }

    memcpy(rif, key, 0x10);
}

void generate_key(int crypto_mode, int version, uint8_t* key_final, uint8_t* iv_final, uint8_t* key, uint8_t* iv) {
    int mode = crypto_mode & 0xF0000000;
    switch (mode) {
    case 0x10000000:
        // Encrypted ERK.
        // Decrypt the key with EDAT_KEY + EDAT_IV and copy the original IV.
        aescbc128_decrypt(version ? EDAT_KEY_1 : EDAT_KEY_0, EDAT_IV, key, key_final, 0x10);
        memcpy(iv_final, iv, 0x10);
        break;
    case 0x20000000:
        // Default ERK.
        // Use EDAT_KEY and EDAT_IV.
        memcpy(key_final, version ? EDAT_KEY_1 : EDAT_KEY_0, 0x10);
        memcpy(iv_final, EDAT_IV, 0x10);
        break;
    case 0x00000000:
        // Unencrypted ERK.
        // Use the original key and iv.
        memcpy(key_final, key, 0x10);
        memcpy(iv_final, iv, 0x10);
        break;
    };
}

void generate_hash(int hash_mode, int version, uint8_t* hash_final, uint8_t* hash) {
    int mode = hash_mode & 0xF0000000;
    switch (mode) {
    case 0x10000000:
        // Encrypted HASH.
        // Decrypt the hash with EDAT_KEY + EDAT_IV.
        aescbc128_decrypt(version ? EDAT_KEY_1 : EDAT_KEY_0, EDAT_IV, hash, hash_final, 0x10);
        break;
    case 0x20000000:
        // Default HASH.
        // Use EDAT_HASH.
        memcpy(hash_final, version ? EDAT_HASH_1 : EDAT_HASH_0, 0x10);
        break;
    case 0x00000000:
        // Unencrypted ERK.
        // Use the original hash.
        memcpy(hash_final, hash, 0x10);
        break;
    };
}

bool decrypt(int crypto_mode, int version, uint8_t* in, uint8_t* out, int length, uint8_t* key, uint8_t* iv) {
    // Setup buffers for key, iv and hash.
    uint8_t key_final[0x10];
    uint8_t iv_final[0x10];

    memset(key_final, 0, 0x10);
    memset(iv_final, 0, 0x10);

    // Generate crypto key and hash.
    generate_key(crypto_mode, version, key_final, iv_final, key, iv);

    if ((crypto_mode & 0xFF) == 0x01)  // No algorithm.
        memcpy(out, in, length);
    else if ((crypto_mode & 0xFF) == 0x02)  // AES128-CBC
        aescbc128_decrypt(key_final, iv_final, in, out, length);
    else
        return false;
    return true;
}

// EDAT/SDAT functions.
void get_block_key(int block, NPD_HEADER* npd, uint8_t* dest_key) {
    uint8_t empty_key[0x10];
    memset(empty_key, 0, 0x10);
    uint8_t* src_key = (npd->version <= 1) ? empty_key : npd->dev_hash;
    memcpy(dest_key, src_key, 0xC);
    store_reverse_endianness_int32_t(block, &dest_key[0xC]);
}

// for out data, allocate a buffer the size of 'edat->block_size'
// Also, set 'in file' to the beginning of the encrypted data, which may be offset
// if inside another file, but normally just reset to beginning of file
// returns number of bytes written, -1 for error
int decrypt_block(stream* in, uint8_t* out, EDAT_HEADER* edat, NPD_HEADER* npd,
    uint8_t* crypt_key, uint32_t block_num, uint32_t total_blocks, int64_t size_left) {
    // Get metadata info and setup buffers.
    const int64_t metadata_section_size = edat->flags & (EDAT_COMPRESSED_FLAG | EDAT_FLAG_0x20) ? 0x20 : 0x10;
    const int metadata_offset = 0x100;

    uint8_t key_result[0x10] = { 0 };

    uint64_t offset = 0;
    uint64_t metadata_sec_offset = 0;
    int32_t length = 0;
    int32_t compression_end = 0;
    uint8_t empty_iv[0x10];
    memset(empty_iv, 0, 0x10);

    size_t file_offset = io_get_position(in);

    // Decrypt the metadata.
    if (edat->flags & EDAT_COMPRESSED_FLAG) {
        metadata_sec_offset = metadata_offset + block_num * metadata_section_size;

        io_set_position(in, file_offset + metadata_sec_offset, SEEK_SET);

        uint8_t metadata[0x20];
        memset(metadata, 0, 0x20);
        io_read(in, metadata,  0x20);

        // If the data is compressed, decrypt the metadata.
        // NOTE: For NPD version 1 the metadata is not encrypted.
        if (npd->version <= 1) {
            offset = load_reverse_endianness_uint64_t(&metadata[0x10]);
            length = load_reverse_endianness_int32_t(&metadata[0x18]);
            compression_end = load_reverse_endianness_int32_t(&metadata[0x1C]);
        }
        else {
            uint8_t dec[0x10];
            dec[0x00] = metadata[0xC] ^ metadata[0x8] ^ metadata[0x10];
            dec[0x01] = metadata[0xD] ^ metadata[0x9] ^ metadata[0x11];
            dec[0x02] = metadata[0xE] ^ metadata[0xA] ^ metadata[0x12];
            dec[0x03] = metadata[0xF] ^ metadata[0xB] ^ metadata[0x13];
            dec[0x04] = metadata[0x4] ^ metadata[0x8] ^ metadata[0x14];
            dec[0x05] = metadata[0x5] ^ metadata[0x9] ^ metadata[0x15];
            dec[0x06] = metadata[0x6] ^ metadata[0xA] ^ metadata[0x16];
            dec[0x07] = metadata[0x7] ^ metadata[0xB] ^ metadata[0x17];
            dec[0x08] = metadata[0xC] ^ metadata[0x0] ^ metadata[0x18];
            dec[0x09] = metadata[0xD] ^ metadata[0x1] ^ metadata[0x19];
            dec[0x0A] = metadata[0xE] ^ metadata[0x2] ^ metadata[0x1A];
            dec[0x0B] = metadata[0xF] ^ metadata[0x3] ^ metadata[0x1B];
            dec[0x0C] = metadata[0x4] ^ metadata[0x0] ^ metadata[0x1C];
            dec[0x0D] = metadata[0x5] ^ metadata[0x1] ^ metadata[0x1D];
            dec[0x0E] = metadata[0x6] ^ metadata[0x2] ^ metadata[0x1E];
            dec[0x0F] = metadata[0x7] ^ metadata[0x3] ^ metadata[0x1F];

            offset = load_reverse_endianness_uint64_t(&dec[0x00]);
            length = load_reverse_endianness_int32_t(&dec[0x08]);
            compression_end = load_reverse_endianness_int32_t(&dec[0x0C]);
        }
    }
    else if (edat->flags & EDAT_FLAG_0x20) {
        // If FLAG 0x20, the metadata precedes each data block.
        metadata_sec_offset = metadata_offset + block_num * (metadata_section_size + edat->block_size);

        io_set_position(in, file_offset + metadata_sec_offset, SEEK_SET);

        uint8_t metadata[0x20];
        memset(metadata, 0, 0x20);
        io_read(in, metadata, 0x20);

        offset = metadata_sec_offset + 0x20;
        length = edat->block_size;

        if ((block_num == (total_blocks - 1)) && (edat->file_size % edat->block_size))
            length = (int32_t)(edat->file_size % edat->block_size);
    }
    else {
        metadata_sec_offset = metadata_offset + (uint64_t)block_num * metadata_section_size;
        io_set_position(in, file_offset + metadata_sec_offset, SEEK_SET);
        offset = metadata_offset + (uint64_t)block_num * edat->block_size + total_blocks * metadata_section_size;
        length = edat->block_size;

        if ((block_num == (total_blocks - 1)) && (edat->file_size % edat->block_size))
            length = (int32_t)(edat->file_size % edat->block_size);
    }

    // Locate the real data.
    const int pad_length = length;
    length = (pad_length + 0xF) & 0xFFFFFFF0;

    // Setup buffers for decryption and read the data.
    memset(key_result, 0, 0x10);

    uint8_t* enc_data = force_malloc_s(uint8_t, length);
    uint8_t* dec_data = force_malloc_s(uint8_t, length);
    io_set_position(in, file_offset + offset, 0);
    io_read(in, enc_data, length);

    // Generate a key for the current block.
    uint8_t b_key[16];
    get_block_key(block_num, npd, b_key);

    // Encrypt the block key with the crypto key.
    aesecb128_encrypt(crypt_key, b_key, key_result);

    // Setup the crypto and hashing mode based on the extra flags.
    int crypto_mode = ((edat->flags & EDAT_FLAG_0x02) == 0) ? 0x2 : 0x1;

    if (edat->flags & EDAT_ENCRYPTED_KEY_FLAG)
        crypto_mode |= 0x10000000;

    if (edat->flags & EDAT_DEBUG_DATA_FLAG) {
        // Reset the flags.
        crypto_mode |= 0x01000000;
        // Simply copy the data without the header or the footer.
        memcpy(dec_data, enc_data, length);
    }
    else {
        // IV is null if NPD version is 1 or 0.
        uint8_t* iv = (npd->version <= 1) ? empty_iv : npd->digest;
        // Call main crypto routine on this data block.
        if (!decrypt(crypto_mode, (npd->version == 4), enc_data, dec_data, length, key_result, iv))
            return -1;
    }

    // Apply additional de-compression if needed and write the decrypted data.
    if ((edat->flags & EDAT_COMPRESSED_FLAG) && compression_end) {
        int res = decompress(out, dec_data, edat->block_size);
        size_left -= res;

        if (size_left == 0 && res < 0)
            return -1;
        return res;
    }

    memcpy(out, dec_data, pad_length);
    return pad_length;
}

// EDAT/SDAT decryption.
// reset file to beginning of data before calling
int decrypt_data(stream* in, stream* out, EDAT_HEADER* edat, NPD_HEADER* npd, unsigned char* crypt_key) {
    const int total_blocks = (int)((edat->file_size + edat->block_size - 1) / edat->block_size);
    int64_t size_left = edat->file_size;
    uint8_t* data = force_malloc_s(uint8_t, edat->block_size);

    for (int i = 0; i < total_blocks; i++) {
        io_set_position(in, 0, 0);
        memset(data, 0, edat->block_size);
        int res = decrypt_block(in, data, edat, npd, crypt_key, i, total_blocks, size_left);
        if (res == -1)
            return 1;

        size_left -= res;
        io_write(out, data, res);
    }
    free(data);

    return 0;
}

void read_npd_edat_header(stream* input, NPD_HEADER* NPD, EDAT_HEADER* EDAT) {
    char npd_header[0x80];
    char edat_header[0x10];
    io_read(input, npd_header, sizeof(npd_header));
    io_read(input, edat_header, sizeof(edat_header));

    memcpy(&NPD->magic, npd_header, 0x04);
    NPD->version = load_reverse_endianness_int32_t(&npd_header[0x04]);
    NPD->license = load_reverse_endianness_int32_t(&npd_header[0x08]);
    NPD->type = load_reverse_endianness_int32_t(&npd_header[0x0C]);
    memcpy(&NPD->content_id, &npd_header[0x10], 0x30);
    memcpy(&NPD->digest, &npd_header[0x40], 0x10);
    memcpy(&NPD->title_hash, &npd_header[0x50], 0x10);
    memcpy(&NPD->dev_hash, &npd_header[0x60], 0x10);
    NPD->unk1 = load_reverse_endianness_int64_t(&npd_header[0x70]);
    NPD->unk2 = load_reverse_endianness_int64_t(&npd_header[0x78]);

    EDAT->flags = load_reverse_endianness_int32_t(&edat_header[0x00]);
    EDAT->block_size = load_reverse_endianness_int32_t(&edat_header[0x04]);
    EDAT->file_size = load_reverse_endianness_int64_t(&edat_header[0x08]);
}

bool extract_all_data(stream* input, stream* output, uint8_t* devKlic, uint8_t* rifKey) {
    // Setup NPD and EDAT/SDAT structs.
    NPD_HEADER NPD;
    EDAT_HEADER EDAT;

    // Read in the NPD and EDAT/SDAT headers.
    read_npd_edat_header(input, &NPD, &EDAT);

    const char* npd_magic = "NPD";
    if (memcmp(&NPD.magic, npd_magic, 0x04))
        return 1;

    // Set decryption key.
    uint8_t key[16];

    // Check EDAT/SDAT flag.
    if ((EDAT.flags & SDAT_FLAG) == SDAT_FLAG)
        // Generate SDAT key.
        for (int i = 0; i < 16; i++)
            key[i] = NPD.dev_hash[i] ^ SDAT_KEY[i];
    else if (NPD.license == 0x3) // Type 3: Use supplied devklic.
        memcpy(key, devKlic, 0x10);
    else if (NPD.license == 0x2) // Type 2: Use key from RAP file (RIF key).
        memcpy(key, rifKey, 0x10);
    else if (NPD.license == 0x1) // Type 1: Use network activation.
        memcpy(key, rifKey, 0x10);

    io_set_position(input, 0, 0);
    if (decrypt_data(input, output, &EDAT, &NPD, key))
        return 1;

    return 0;
}

void GetEdatRifKeyFromRapFile(stream* rap_file, uint8_t* rifKey) {
    uint8_t rapKey[0x10];
    io_read(rap_file, rapKey, 0x10);
    rap_to_rif(rapKey, rifKey);
}

// Decrypts full file
void DecryptEDAT(stream* input, stream* output, int mode, const char* rap_file_name, uint8_t* custom_klic) {
    if (!input || !output || !rap_file_name)
        return;

    // Prepare the files.
    io_set_position(input, 0, 0);

    // Set keys (RIF and DEVKLIC).
    uint8_t rifKey[16];
    uint8_t devklic[16];

    // Select the EDAT key mode.
    switch (mode) {
    case 0:
        break;
    case 1:
        memcpy(devklic, NP_KLIC_FREE, 0x10);
        break;
    case 2:
        memcpy(devklic, NP_OMAC_KEY_2, 0x10);
        break;
    case 3:
        memcpy(devklic, NP_OMAC_KEY_3, 0x10);
        break;
    case 4:
        memcpy(devklic, NP_KLIC_KEY, 0x10);
        break;
    case 5:
        memcpy(devklic, NP_PSX_KEY, 0x10);
        break;
    case 6:
        memcpy(devklic, NP_PSP_KEY_1, 0x10);
        break;
    case 7:
        memcpy(devklic, NP_PSP_KEY_2, 0x10);
        break;
    case 8:
        if (custom_klic)
            memcpy(devklic, custom_klic, 0x10);
        break;
    }

    // Read the RAP file, if provided.
    if (*rap_file_name) {
        stream rap;
        io_open(&rap, rap_file_name, "rb");
        if (rap.io.stream)
            GetEdatRifKeyFromRapFile(&rap, rifKey);
        io_free(&rap);
    }

    extract_all_data(input, output, devklic, rifKey);
}
