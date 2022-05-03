#pragma once

template <uint32_t LEN, typename UINT=uint64_t, bool LITTLE = true>
struct Integer : std::array<LENGTH64,UINT> {
    struct Flags {
        uint8_t zero : 1;
        uint8_t negative : 1;
        uint8_t carry : 1;
        uint8_t overflow : 1;
    };

    static const UINT ZEROS = UINT(0);
    static const UINT ONES = UINT(~UINT(0));
    static const UINT SIGN_MASK = UINT(UINT(1) << (8*sizeof(UINT)-1));
    static const UINT MIN = SIGN_MASK;
    static const UINT MAX = SIGN_MASK-1;

    static bool ZERO(const UINT &a) {
        return a == ZEROS;
    }

    static bool NEGATIVE(const UINT &a) {
        return (a & SIGN_MASK) != 0;
    }

    static bool POSITIVE(const UINT &a) {
        return (0 < a && a < SIGN_MASK);
    }

    static bool ADC(UINT &a, const UINT &b, bool c) {
        if (c && (b == ONES)) return true;
        bool na = NEGATIVE(a);
        bool nb = NEGATIVE(b);
        a += b + c;
        bool ns = NEGATIVE(a);
        return (ns != na) && (na == nb);
    }

    static bool MUL(UINT &ahi, UINT &alo, const UINT &a, const UINT &b) {

    }

    static bool SBC(UINT &a, const UINT &b, bool c) {
        bool na = NEGATIVE(a);
        bool nb = NEGATIVE(b);
        a -= b + c;
        bool ns = NEGATIVE(a);
        return (na ^ nb) & ~(ns ^ na);
    }






        0 1 1 
        0 1 0

        0 0 
    }




    void inc() {
        if (LITTLE) {
            for (uint32_t i = 0; i<LENGTH; ++i) {
                if ((++(*this)[i]) != 0) break;
            }
        } else {
            for (uint32_t i = LENGTH-1; i >= 0; --i) {
                if ((++(*this)[i]) != 0) break;
            }
        }
    }
    void adc(const Integer &op) {
        if (LITTLE) {
            for (uint32_t i = 0; i<LENGTH; ++i) {
                flags.carry = 
                if ((++(*this)[i]) != 0) break;
            }
        } else {
            for (uint32_t i = LENGTH-1; i >= 0; --i) {
                if ((++(*this)[i]) != 0) break;
            }
        }
    }
    void dec() {
        for (uint32_t i = LENGTH-1; i >= 0; --i) {

        }
    }
    void adc(const IntgerBE &op) {

    }
}

template <uint32_t BITS>
struct Integer : std::bitset<BITS> {
    struct {
        //  00000
        //    1
        //   -1
        //  -2
        //      1
        //       2
        //  000   0 zero
        //  001  +1 pos
        //  010  +2 pos overflow
        //  111  -1 neg
        //  110  -2 neg overflow
        //  101  (-3 unused)
        uint8_t not_positive : 1;
        uint8_t negative : 1;
        uint8_t overflow_xor_negative : 1;
    }
    int8_t flags; // 0 for 0 no carry 

    Integer() { }
    Integer(int64_t x) { 
        int m = BITS < 64 ? BITS : 64; 
        for (int i=m-1; i>=0; --i) set((x >> i)&1);
    }
    int8_t adc(const Integer &op, int8_t flags) {
        bool c = (flags>>6)^(flags>>7);
        for (int i=BITS-1; i>=0; --i) {
            uint8_t sum = test(i)+op.test(i)+c;
            set(i,sum&1);
            c = (sum>>1);
        }

        carry = c;
    }

    void add(const Integer &op) {
        bool c=0;
        adc(op,c);
    }

    Integer& compliment() {
        for (int i=BITS-1; i>=0; --i) {
            set(i,!test(i));
        }
    }

    Integer& negate() {
        bool c = 1;
        for (int i=BITS-1; i>=0; --i) {
            uint8_t sum = !test(i)+c;
            set(i,sum&1);
            c = (sum>>1);
        }
    }

    void shift(int bits) {
        for (int i=BITS-1; i>=0; --i) {
            set(i+bits,test(i));
        }
    }

    void mul(const Integer &a, const Integer &b) {
        zero();
        Integer tmp(b);
        for (int i=BITS-1; i>=0; --i) {
            if (a.test(i)) {
                add(tmp);
            }
            tmp.shift(1);
        }
    }

    Integer& operator+=(const Integer& op) {
        bool c = 0;
        bool o = test(0)^op.test(0);
        for (int i=BITS-1; i>=0; --i) {
            uint8_t sum = test(i)+op(i)+c;
            set(i,sum&1);
            c = (sum>>1);
        }
        if (c & !test(0)) {
            for (int i=BITS-2; i>=0; --i) {
                set(i,1);
            }
        } else if (!c & test(0)) {

        }
        if (c ^ test(0)) {

        }
        if (o & (s^c)) {
            if 
        }

    }


}