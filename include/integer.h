#pragma once

#include <stdint.h>
#include <algorithm>

template <typename uint_type, uint32_t size, bool is_little = true>
struct integer : std::array<uint_type,size> {
    static const uint8_t uint_bits = 8*sizeof(uint_type);
    static const uint_type uint_zeros = uint_type(0);
    static const uint_type uint_ones = uint_type(~uint_type(0));
    static const uint_type uint_sign_mask = uint_type((uint_type(1) << (8*sizeof(uint_type)-1));
    static const uint_type uint_max = uint_sign_mask - 1;
    static const uint_type uint_min = uint_sign_mask;

    static uint8_t is_zero(const uint_type &a) {
        return a == uint_zeros;
    }

    static uint8_t is_negative(const uint_type &a) {
        return (a & uint_sign_mask) != 0;
    }

    static uint8_t is_positive(const uint_type &a) {
        return (a-1) < (uint_max-1);
    }

    static uint8_t add_with_carry(uint_type &a, const uint_type &b, uint8_t c) {
        uint8_t na = is_negative(a);
        uint8_t nb = is_negative(b);
        a += b + c;
        bool ns = is_negative(a);
        return (c & (b==uint_ones)) | ((ns^na) & ~(na ^ nb));
    }

    static void shift_left(uint_type &hi, uint_type &lo, uint8_t bits) {
        hi = (hi << bits) | (lo>>(uint_bits-bits));
        lo = (lo << bits);
    }

    static void shift_right(uint_type &hi, uint_type &lo, uint8_t bits) {
        lo = (lo >> bits) | (hi<<(uint_bits-bits));
        hi = (hi >> bits);
    }

    static void multiply(uint_type &hi, uint_type &lo, const type &a, const type &b) {
        for (uint32_t i=0; i<sizeof(UINT); i += 32) {
            for (uint32_t j=0; j<sizeof(UINT); j += 32) {
                uint32_t k = i + j;
                uint64 p64 = uint64_t(a >> i)*uint64_t(b >> j);
                uint_type plo(p64);
                uint_type phi(p64 >> uint_bits);
                shift_left(phi,plo,k % BITS);
                hi += phi + add_with_carry(lo,plo,0);
            }
        }
    }

    static void compliment(const std::array<uint_type,size> &a) {
        if (is_little) {
            for (int32_t i = 0; i<size; ++i) {
                a[i]=~a[i];
            }
        } else {
            for (int32_t i = size-1; i >= 0; --i) {
                a[i]=~a[i];
            }
        }
    }

    static uint8_t is_zero(const std::array<uint_type,size> &a) {
        if (is_little) {
            uint_type z(0);
            for (int32_t i = 0; i<size; ++i) {
                z |= a[i];
            }
            return z == 0;
        } else {
            uint_type z(0);
            for (int32_t i = size-1; i >= 0; --i) {
                z |= a[i];
            }
            return z == 0;
        }
    }

    static uint8_t is_negative(const std::array<uint_type,size> &a) {
        if (is_little) {
            return (a[size-1] & uint_sign_mask) != 0;
        } else {
            return (a[0] & uint_sign_mask) != 0;
        }
    }

    static int8_t sign(const std::array<uint_type,size> &a) {
        uint8_t z = is_zero(a);
        uint8_t n = is_negative(a);
        uint8_t p = (!z) & (!n);

        return -1*n+1*p;
    }

    static int8_t compare(const std::array<uint_type,size> &a, const std::array<uint_type,size> &b) {
        std::array<uint_type,size> tmp(b);
        compliment(tmp);
        add_with_carry(a,tmp,1);
        return sign(tmp);
    }

    static uint8_t add_with_carry(std::array<uint_type,size> &a, const std::array<uint_type,size> &b, uint8_t c) {
        if (is_little) {
            for (int32_t i = 0; i<size; ++i) {
                c = add_with_carry(a[i],b[i],c);
            }
            return c;
        } else {
            for (int32_t i = size-1; i >= 0; --i) {
                c = add_with_carry(a[i],b[i],c);
            }
            return c;
        }
    }

    template <uint32_t ans_size>
    static void multiply(std::array<uint_type,ans_size> &ans, 
            const std::array<uint_type,size> &a, 
            const std::array<uint_type,size> &b) {
        uint32_t over[2*size]={0};
        if (is_little) {
            int32_t ni=std::min(size,ans_size);
            for (int32_t i=0; i<ni; ++i) 
            {
                int32_t nj=std::min(size,ans_size-i);
                for (int32_t j=0; j<nj; ++j) {
                    int32_t k = i + j;
                    uint_type phi(0),plo(0);
                    multiply(phi,plo,a[i],b[j]);
                    over[k] += add_with_carry(ans[k],plo,0);
                    over[k+1] += add_with_carry(ans[k+1],phi,0);
                }
            }
            uint8_t c = 0;
            for (int32_t i=0; i<2*size; ++i) {
                c = add_with_carry(ans[i],uint_type(over[i]),c);
                over[i]=0;
            }
        } else {
            int32_t ni=std::min(size,ans_size);
            for (int32_t i=ni-1; i>=0 --i) {
                int32_t nj=std::min(size,ans_size-i);
                for (int32_t j=nj-1; j>=0; --j) {
                    int32_t k = i + j;
                    uint_type phi(0),plo(0);
                    multiply(phi,plo,a[i],b[j]);
                    over[k+1] += add_with_carry(ans[k+1],plo,0);
                    over[k] += add_with_carry(ans[k],phi,0);
                }
            }
            uint8_t c = 0;
            for (int32_t i=2*size-1; i>=0; --i) {
                c = add_with_carry(ans[i],uint_type(over[i]),c);
                over[i]=0;
            }
        }
    }
};
