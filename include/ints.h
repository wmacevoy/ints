#pragma once

#include <iostream>
#include <array>
#include <stdint.h>
#include <algorithm>
#include <arpa/inet.h>

template <typename uint_type, uint32_t size, bool is_signed = true, bool is_little = (htonl(1) != 1)>
struct ints : std::array<uint_type, size>
{
    static const uint8_t uint_bits;
    static const uint_type uint_zeros;
    static const uint_type uint_ones;
    static const uint_type uint_sign_mask;
    static const uint_type uint_max;
    static const uint_type uint_min;

    static uint8_t is_zero(const uint_type &a)
    {
        return a == uint_zeros;
    }

    static uint8_t is_negative(const uint_type &a)
    {
        return (a & uint_sign_mask) != 0;
    }

    static uint8_t is_positive(const uint_type &a)
    {
        return uint_type(a + uint_type(-1)) < uint_max;
    }

    static int8_t sign(const uint_type &a)
    {
        uint8_t z = is_zero(a);
        uint8_t n = is_negative(a);
        uint8_t p = (!z) & (!n);

        return -1 * n + 1 * p;
    }

    static uint8_t add_with_carry(uint_type &a, const uint_type &b, uint8_t c)
    {
        a += c;
        c = c & (a == 0);
        uint8_t na = (a & (uint_type(1) << (sizeof(uint_type) * 8 - 1))) != 0;
        uint8_t nb = (b & (uint_type(1) << (sizeof(uint_type) * 8 - 1))) != 0;
        a += b;
        uint8_t ns = (a & (uint_type(1) << (sizeof(uint_type) * 8 - 1))) != 0;
        return c | (na & nb) | ((na ^ nb) & (~ns));
    }

    static uint_type fshift(const uint_type &a, int32_t pow2)
    {
        if (pow2 >= 0)
        {
            if (pow2 < uint_bits)
            {
                return a << pow2;
            }
            else
            {
                return uint_type(0);
            }
        }
        else
        {
            if (pow2 > -uint_bits)
            {
                return a >> -pow2;
            }
            else
            {
                return uint_type(0);
            }
        }
    }

    static void shift(uint_type &hi, uint_type &lo, int32_t pow2)
    {
        if (pow2 >= 0)
        {
            hi = fshift(hi, pow2) | fshift(lo, pow2 - uint_bits);
            lo = fshift(lo, pow2);
        }
        else
        {
            lo = fshift(lo, pow2) | fshift(hi, pow2 + uint_bits);
            hi = fshift(hi, pow2);
        }
    }

    static void uumultiply(uint_type &hi, uint_type &lo, const uint_type &a, const uint_type &b)
    {
        for (uint32_t i = 0; i < sizeof(uint_type); i += 32)
        {
            for (uint32_t j = 0; j < sizeof(uint_type); j += 32)
            {
                uint32_t k = (i + j);
                uint64_t p64 = uint32_t(a >> i) * uint32_t(b >> j);
                uint_type plo(p64);
                uint_type phi(p64 >> uint_bits);
                shift(phi, plo, k);
                hi += phi + add_with_carry(lo, plo, 0);
            }
        }
    }

    static void complement(const std::array<uint_type, size> &a)
    {
        if (is_little)
        {
            for (int32_t i = 0; i < size; ++i)
            {
                a[i] = ~a[i];
            }
        }
        else
        {
            for (int32_t i = size - 1; i >= 0; --i)
            {
                a[i] = ~a[i];
            }
        }
    }

    static uint8_t is_zero(const std::array<uint_type, size> &a)
    {
        if (is_little)
        {
            uint_type z(0);
            for (int32_t i = 0; i < size; ++i)
            {
                z |= a[i];
            }
            return z == 0;
        }
        else
        {
            uint_type z(0);
            for (int32_t i = size - 1; i >= 0; --i)
            {
                z |= a[i];
            }
            return z == 0;
        }
    }

    static uint8_t is_negative(const std::array<uint_type, size> &a)
    {
        if (is_little)
        {
            return (a[size - 1] & uint_sign_mask) != 0;
        }
        else
        {
            return (a[0] & uint_sign_mask) != 0;
        }
    }

    static int8_t sign(const std::array<uint_type, size> &a)
    {
        uint8_t z = is_zero(a);
        uint8_t n = is_negative(a);
        uint8_t p = (!z) & (!n);

        return -1 * n + 1 * p;
    }

    static int8_t compare(const std::array<uint_type, size> &a, const std::array<uint_type, size> &b)
    {
        std::array<uint_type, size> tmp(b);
        compliment(tmp);
        add_with_carry(a, tmp, 1);
        return sign(tmp);
    }

    static uint8_t add_with_carry(std::array<uint_type, size> &a, const std::array<uint_type, size> &b, uint8_t c)
    {
        if (is_little)
        {
            for (int32_t i = 0; i < size; ++i)
            {
                c = add_with_carry(a[i], b[i], c);
            }
            return c;
        }
        else
        {
            for (int32_t i = size - 1; i >= 0; --i)
            {
                c = add_with_carry(a[i], b[i], c);
            }
            return c;
        }
    }

    template <uint32_t ans_size>
    static void multiply(std::array<uint_type, ans_size> &ans,
                         const std::array<uint_type, size> &a,
                         const std::array<uint_type, size> &b)
    {
        uint32_t over[2 * size] = {0};
        if (is_little)
        {
            int32_t ni = std::min(size, ans_size);
            for (int32_t i = 0; i < ni; ++i)
            {
                int32_t nj = std::min(size, ans_size - i);
                for (int32_t j = 0; j < nj; ++j)
                {
                    int32_t k = i + j;
                    uint_type phi(0), plo(0);
                    multiply(phi, plo, a[i], b[j]);
                    over[k] += add_with_carry(ans[k], plo, 0);
                    over[k + 1] += add_with_carry(ans[k + 1], phi, 0);
                }
            }
            uint8_t c = 0;
            for (int32_t i = 0; i < 2 * size; ++i)
            {
                c = add_with_carry(ans[i], uint_type(over[i]), c);
                over[i] = 0;
            }
        }
        else
        {
            int32_t ni = std::min(size, ans_size);
            for (int32_t i = ni - 1; i >= 0; --i)
            {
                int32_t nj = std::min(size, ans_size - i);
                for (int32_t j = nj - 1; j >= 0; --j)
                {
                    int32_t k = i + j;
                    uint_type phi(0), plo(0);
                    multiply(phi, plo, a[i], b[j]);
                    over[k + 1] += add_with_carry(ans[k + 1], plo, 0);
                    over[k] += add_with_carry(ans[k], phi, 0);
                }
            }
            uint8_t c = 0;
            for (int32_t i = 2 * size - 1; i >= 0; --i)
            {
                c = add_with_carry(ans[i], uint_type(over[i]), c);
                over[i] = 0;
            }
        }
    }

    friend std::ostream& operator<<(std::ostream& out, const ints &x) {
      out << (is_signed ? "i" : "u") << 8*sizeof(uint_type) << "x" << size << (is_little ? "_le" : "_be") << "{";
      for (uint32_t i = 0; i < size; ++i) {
	if (i != 0) out << ",";
	out << uint64_t(x.at(i));
      }
      out << "}";
      return out;
    }
};


template <typename uint_type, uint32_t size, bool is_signed, bool is_little>
const uint8_t ints<uint_type, size, is_signed, is_little>::uint_bits(8 * sizeof(uint_type));

template <typename uint_type, uint32_t size, bool is_signed, bool is_little>
const uint_type ints<uint_type, size, is_signed, is_little>::uint_zeros(uint_type(0));

template <typename uint_type, uint32_t size, bool is_signed, bool is_little>
const uint_type ints<uint_type, size, is_signed, is_little>::uint_ones(uint_type(~uint_type(0)));

template <typename uint_type, uint32_t size, bool is_signed, bool is_little>
const uint_type ints<uint_type, size, is_signed, is_little>::uint_sign_mask(is_signed ? (uint_type(1) << (sizeof(uint_type) * 8 - 1)) : 0);

template <typename uint_type, uint32_t size, bool is_signed, bool is_little>
const uint_type ints<uint_type, size, is_signed, is_little>::uint_max(is_signed ? ((uint_type(1) << (sizeof(uint_type) * 8 - 1)) - 1) : (~uint_type(0)));

template <typename uint_type, uint32_t size, bool is_signed, bool is_little>
const uint_type ints<uint_type, size, is_signed, is_little>::uint_min(is_signed ? ((uint_type(1) << (sizeof(uint_type) * 8 - 1))) : 0);
