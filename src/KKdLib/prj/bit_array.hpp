/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../default.hpp"

namespace prj {
    template <size_t n>
    class BitArrayEmbCore {
    public:
        enum {
            bits_per_block = sizeof(uint32_t) * 8,
        };

        static constexpr size_t size = n;
        static constexpr size_t num_blocks = (n + bits_per_block - 1) / bits_per_block;

        uint32_t M_pool[num_blocks];
    };

    template <size_t n>
    class BitArrayExtCore {
    public:
        enum {
            bits_per_block = sizeof(uint32_t) * 8,
        };

        static constexpr size_t size = n;
        static constexpr size_t num_blocks = (n + bits_per_block - 1) / bits_per_block;

        uint32_t* M_pool;
    };

    template <class T>
    class BitArrayUtil {
    public:
        enum {
            bits_per_block = sizeof(uint32_t) * 8,
        };
    };

    template <class T>
    class BitArrayBase {
    private:
        T M_core;

    public:
        BitArrayBase(size_t count, uint32_t* data) : M_core() {
            init(count, data);
        }

        BitArrayBase(uint32_t* data) : M_core() {
            init(data);
        }

        BitArrayBase() : M_core() {

        }

        void init(uint32_t* data) {
            BitArrayUtil<T>::init(M_core, size(), num_blocks(), data);
        }

        void init(size_t size, uint32_t* data) {
            BitArrayUtil<T>::init(M_core, size, num_blocks(), data);
            M_reset_unused_bit();
        }

    private:
        uint32_t* M_data() {
            return M_core.M_pool;
        }
        
        const uint32_t* M_data() const {
            return M_core.M_pool;
        }

    public:
        constexpr size_t size() const {
            return T::size;
        }

        constexpr size_t max_size() const {
            return num_blocks() * T::bits_per_block;
        }

        constexpr size_t num_blocks() const {
            return T::num_blocks;
        }

        size_t count() const {
            size_t sum = 0;
            for (size_t i = 0; i < num_blocks(); i++)
                sum += S_mask_count(M_data()[i]);
            return sum;
        }

    private:
        size_t M_find0(size_t);
        size_t M_find1(size_t);

    public:
        bool operator[](size_t pos) const {
            return test(pos);
        }

        bool test(size_t pos) const {
            return ((M_data()[pos / T::bits_per_block] >> (pos % T::bits_per_block)) & 0x01) != 0x00;
        }

        BitArrayBase& set(size_t pos) {
            M_data()[pos / T::bits_per_block] |= 1u << (pos % T::bits_per_block);
            return *this;
        }

        BitArrayBase& set() {
            for (size_t i = 0, j = 0; i < num_blocks(); i++)
                M_data()[i] = 0xFFFFFFFFu;
            M_reset_unused_bit();
            return *this;
        }

        BitArrayBase& reset(size_t pos) {
            M_data()[pos / T::bits_per_block] &= ~(1u << (pos % T::bits_per_block));
            return *this;
        }

        BitArrayBase& reset() {
            for (size_t i = 0; i < num_blocks(); i++)
                M_data()[i] = 0;
            return *this;
        }

        BitArrayBase& flip(size_t pos) {
            set(pos, !test(pos));
            return *this;
        }

        BitArrayBase& flip() {
            for (size_t i = 0; i < num_blocks(); i++)
                M_data()[i] ^= 0xFFFFFFFFu;
            M_reset_unused_bit();
            return *this;
        }

    private:
        void M_reset_unused_bit() {
            M_data()[num_blocks() - 1] &= ~(0xFFFFFFFFu << (32 - (max_size() - size())));
        }

        void M_block_dump();
        void M_dump();
        void M_dump0();
        void M_dump1();
        size_t word(size_t);
        size_t offset(size_t);

        size_t S_mask2bit(uint32_t);

        size_t S_mask_count(uint32_t x) const {
            x = (x & 0x55555555) + ((x >>  1) & 0x55555555);
            x = (x & 0x33333333) + ((x >>  2) & 0x33333333);
            x = (x & 0x0F0F0F0F) + ((x >>  4) & 0x0F0F0F0F);
            x = (x & 0x00FF00FF) + ((x >>  8) & 0x00FF00FF);
            x = (x & 0x0000FFFF) + ((x >> 16) & 0x0000FFFF);
            return x;
        }

    public:
        size_t calc_num_blocks(size_t n) const {
            return (n + T::bits_per_block - 1) / T::bits_per_block;
        }

        bool operator==(const BitArrayBase<T>& right) const {
            for (size_t i = 0; i < num_blocks(); i++)
                if (M_data()[i] != right.M_data()[i])
                    return false;
            return true;
        }
        
        void operator&=(const BitArrayBase<T>& right) {
            for (size_t i = 0; i < num_blocks(); i++)
                M_data()[i] &= right.M_data()[i];
        }

        void operator|=(const BitArrayBase<T>& right) {
            for (size_t i = 0; i < num_blocks(); i++)
                M_data()[i] |= right.M_data()[i];
        }
    };

    template <size_t n>
    struct BitArrayUtil<BitArrayEmbCore<n> > {
        static void init(BitArrayEmbCore<n>& core, size_t size, size_t num_blocks, uint32_t* data) {
            for (size_t i = 0; i < num_blocks; ++i)
                core.M_pool[i] = data[i];
        }
    };

    template <size_t n>
    struct BitArrayUtil<BitArrayExtCore<n> > {
        static void init(BitArrayExtCore<n>& core, size_t size, size_t num_blocks, uint32_t* data) {
            core.M_pool = data;
        }
    };

    template <size_t n>
    class BitArray : public BitArrayBase<BitArrayEmbCore<n>> {
    public:
        BitArray(size_t count, const uint32_t* data) : BitArrayBase<BitArrayEmbCore<n>>(count, (uint32_t*)data) {}
        BitArray(const uint32_t* data) :  BitArrayBase<BitArrayEmbCore<n>>((uint32_t*)data) {}
        BitArray() : BitArrayBase<BitArrayEmbCore<n>>() {}
    };

    template <size_t n>
    class BitArrayExt : public BitArrayBase<BitArrayExtCore<n>> {
    public:
        BitArrayExt(size_t count, uint32_t* data) : BitArrayBase<BitArrayExtCore<n>>(count, data) {}
        BitArrayExt(uint32_t* data) : BitArrayBase<BitArrayExtCore<n>>(data) {}
        BitArrayExt() : BitArrayBase<BitArrayExtCore<n>>() {};
    };
}
