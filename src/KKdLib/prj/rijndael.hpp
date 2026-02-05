/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../default.hpp"

namespace prj {
    static constexpr size_t Rijndael_Nb = 4;

    static constexpr uint32_t Rijndael_Nk128 = 4;
    static constexpr uint32_t Rijndael_Nr128 = 10;
    static constexpr uint32_t Rijndael_Nk192 = 6;
    static constexpr uint32_t Rijndael_Nr192 = 12;
    static constexpr uint32_t Rijndael_Nk256 = 8;
    static constexpr uint32_t Rijndael_Nr256 = 14;

    static constexpr size_t Rijndael_Nlen = Rijndael_Nb * sizeof(uint32_t);
}

struct alignas(prj::Rijndael_Nlen) PrjRijKeyParam {
    uint32_t num_key;
    uint32_t num_round;
    uint32_t num_block;
    uint32_t pad0;
    uint32_t enc_key[(prj::Rijndael_Nr256 + 1ULL) * prj::Rijndael_Nb];
    uint32_t dec_key[(prj::Rijndael_Nr256 + 1ULL) * prj::Rijndael_Nb];
    uint32_t pad1[4];
};

static_assert(offsetof(PrjRijKeyParam, enc_key) % prj::Rijndael_Nlen == 0, "\"PrjRijKeyParam\" struct should have \"enc_key\" field aligned to prj::Rijndael_Nlen");
static_assert(offsetof(PrjRijKeyParam, dec_key) % prj::Rijndael_Nlen == 0, "\"PrjRijKeyParam\" struct should have \"dec_key\" field aligned to prj::Rijndael_Nlen");

namespace prj {
    class Rijndael {
        PrjRijKeyParam M_kp;

    public:
        Rijndael();
        Rijndael(size_t nb, size_t nk, const void* key);

        void encrypt16(const void* in_buff16, void* out_buff16);
        void encrypt16(void* buff16);
        void decrypt16(const void* in_buff16, void* out_buff16);
        void decrypt16(void* buff16);

        void test_encrypt_decrypt(const void* plain);
    };
}

extern void prj_rij_gkey(PrjRijKeyParam* kp, size_t nb, size_t nk, const void* key);
extern void prj_rij_decrypt(PrjRijKeyParam* kp, const void* in_buff16, void* out_buff16);
extern void prj_rij_encrypt(PrjRijKeyParam* kp, const void* in_buff16, void* out_buff16);
extern void prj_rij_test_encrypt_decrypt(PrjRijKeyParam* kp, const void* plain);

namespace prj {
    inline Rijndael::Rijndael() : M_kp() { }

    inline Rijndael::Rijndael(size_t nb, size_t nk, const void* key) : M_kp() {
        prj_rij_gkey(&M_kp, nb, nk, key);
    }

    inline void Rijndael::encrypt16(const void* in_buff16, void* out_buff16) {
        prj_rij_encrypt(&M_kp, in_buff16, out_buff16);
    }

    inline void Rijndael::encrypt16(void* buff16) {
        prj_rij_encrypt(&M_kp, buff16, buff16);
    }

    inline void Rijndael::decrypt16(const void* in_buff16, void* out_buff16) {
        prj_rij_decrypt(&M_kp, in_buff16, out_buff16);
    }

    inline void Rijndael::decrypt16(void* buff16) {
        prj_rij_decrypt(&M_kp, buff16, buff16);
    }

    inline void Rijndael::test_encrypt_decrypt(const void* plain) {
        prj_rij_test_encrypt_decrypt(&M_kp, plain);
    }
}
