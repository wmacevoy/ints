#include <iostream>
#include <tgmath.h>
#include "gtest/gtest.h"

#include "ints.h"

TEST(Ints,Constants8) {
    typedef uint8_t uint_type;
    typedef int8_t  int_type;

    ASSERT_EQ((ints<uint_type,1,1,1>::uint_sign_mask),0x80);
    ASSERT_EQ((ints<uint_type,1,0,1>::uint_sign_mask),0x00);
    ASSERT_EQ((ints<uint_type,1,1,1>::uint_zeros),0x00);
    ASSERT_EQ((ints<uint_type,1,0,1>::uint_zeros),0x00);
    ASSERT_EQ((ints<uint_type,1,1,1>::uint_ones),0xFF);
    ASSERT_EQ((ints<uint_type,1,0,1>::uint_ones),0xFF);
    ASSERT_EQ((ints<uint_type,1,1,1>::uint_min),0x80);
    ASSERT_EQ((ints<uint_type,1,0,1>::uint_min),0x00);
    ASSERT_EQ((ints<uint_type,1,1,1>::uint_max),0x7F);
    ASSERT_EQ((ints<uint_type,1,0,1>::uint_max),0xFF);
}

TEST(Ints,Constants16) {
    typedef uint16_t uint_type;
    typedef int16_t  int_type;

    ASSERT_EQ((ints<uint_type,1,1,1>::uint_sign_mask),0x8000);
    ASSERT_EQ((ints<uint_type,1,0,1>::uint_sign_mask),0x0000);
    ASSERT_EQ((ints<uint_type,1,1,1>::uint_zeros),0x0000);
    ASSERT_EQ((ints<uint_type,1,0,1>::uint_zeros),0x0000);
    ASSERT_EQ((ints<uint_type,1,1,1>::uint_ones),0xFFFF);
    ASSERT_EQ((ints<uint_type,1,0,1>::uint_ones),0xFFFF);
    ASSERT_EQ((ints<uint_type,1,1,1>::uint_min),0x8000);
    ASSERT_EQ((ints<uint_type,1,0,1>::uint_min),0x0000);
    ASSERT_EQ((ints<uint_type,1,1,1>::uint_max),0x7FFF);
    ASSERT_EQ((ints<uint_type,1,0,1>::uint_max),0xFFFF);
}

TEST(Ints,Flags8) {
    typedef uint8_t uint_type;
    typedef int8_t  int_type;

    uint32_t count = 0;

    for (int x=std::numeric_limits<int_type>::min(); x<=std::numeric_limits<int_type>::max(); ++x) {
        ++count;
        int_type i(x);
        uint_type u(x);

        ASSERT_EQ((ints<uint_type,1,1,1>::is_negative(i)),uint8_t(i<0)) << "i=" << int32_t(i);
        ASSERT_EQ((ints<uint_type,1,0,1>::is_negative(u)),uint8_t(u<0)) << "u=" << uint32_t(u);

        ASSERT_EQ((ints<uint_type,1,1,1>::is_zero(i)),uint8_t(i==0)) << "i=" << int32_t(i);
        ASSERT_EQ((ints<uint_type,1,0,1>::is_zero(u)),uint8_t(u==0)) << "u=" << uint32_t(u);

        ASSERT_EQ((ints<uint_type,1,1,1>::is_positive(i)),uint8_t(i>0)) << "i=" << int32_t(i);
        ASSERT_EQ((ints<uint_type,1,0,1>::is_positive(u)),uint8_t(u>0)) << "u=" << uint32_t(u);

        EXPECT_EQ((ints<uint_type,1,1,1>::sign(i)),int8_t(i<0?-1:(i==0?0:1))) << "i=" << int32_t(i);
        ASSERT_EQ((ints<uint_type,1,0,1>::sign(u)),int8_t(u==0?0:1)) << "u=" << uint32_t(u);
    }

    ASSERT_EQ(count,exp2(8*sizeof(uint_type)));
}

TEST(Ints,AddWithCarry) {
    typedef uint8_t uint_type;
    typedef int8_t  int_type;
    for (int x=std::numeric_limits<int_type>::min(); x<=std::numeric_limits<int_type>::max(); ++x) {
        for (int y=std::numeric_limits<int_type>::min(); y<=std::numeric_limits<int_type>::max(); ++y) {
            for (int c=0; c<2; ++c) {
                for (int s=0; s<2; ++s) {
                    if (s) {
                        int_type s(x);
                        int_type a(x);
                        int_type b(y);
                        int32_t sum = int32_t(int_type(x))+int32_t(int_type(y))+int32_t(int_type(c));
                        uint32_t usum = uint32_t(uint_type(x))+uint32_t(uint_type(y))+uint32_t(uint_type(c));
                        uint8_t c0 = (usum >> (8*sizeof(int_type))) & 1;
                        uint8_t c1 = ints<uint_type,1,1,1>::add_with_carry((uint_type&)s,(const uint_type&)b,uint8_t(c));
                        ASSERT_EQ(int_type(sum),s) << "s=" << int32_t(s) << " a=" << int32_t(a) << " b=" << int32_t(b) << " c=" << int32_t(c);
                        ASSERT_EQ(c0,c1) << "s=" << int32_t(s) << " a=" << int32_t(a) << " b=" << int32_t(b) << " c=" << int32_t(c);
                    } else {
                        uint_type s(x);
                        uint_type a(x);
                        uint_type b(y);
                        int32_t sum = int32_t(int_type(x))+int32_t(int_type(y))+int32_t(int_type(c));
                        uint32_t usum = uint32_t(uint_type(x))+uint32_t(uint_type(y))+uint32_t(uint_type(c));
                        uint8_t c0 = (usum >> (8*sizeof(int_type))) & 1;
                        uint8_t c1 = ints<uint_type,1,0,1>::add_with_carry(s,b,uint8_t(c));
                        ASSERT_EQ(uint_type(sum),s) << "s=" << uint32_t(s) << " a=" << uint32_t(a) << " b=" << uint32_t(b) << " c=" << uint32_t(c);
                        ASSERT_EQ(c0,c1) << "s=" << uint32_t(s) << " a=" << uint32_t(a) << " b=" << uint32_t(b) << " c=" << uint32_t(c);
                    }
                }
            }
        }
    }
}

void bp() {};

TEST(Ints,Shift1) {
    typedef uint8_t uint_type;
    typedef int8_t  int_type;

    for (int x=std::numeric_limits<int_type>::min(); x<=std::numeric_limits<int_type>::max(); ++x) {
      int_type i(x);
      uint_type u(x);

      for (int pow2 = -1000; pow2 <= 1000; ++pow2) {
	if (u == 256 && pow2 == 1) bp();

	int_type si=ints<uint_type,1,1,1>::shift(i,pow2);
	uint_type su=ints<uint_type,1,1,1>::shift(u,pow2);

	ASSERT_EQ(uint_type(si),su);

	if (pow2 < 0) {
	  if (pow2 > -8*sizeof(uint_type)) {
	    ASSERT_EQ(u >> -pow2,su) << " u=" << u << " pow2=" << pow2;
	  } else {
	    ASSERT_EQ(0,su) << " u=" << u << " pow2=" << pow2;
	  }
	} else {
	  if (pow2 < 8*sizeof(uint_type)) {
	    ASSERT_EQ(u << pow2,su) << " u=" << u << " pow2=" << pow2;
	  } else {
	    ASSERT_EQ(0,su) << " u=" << u << " pow2=" << pow2;
	  }
	}
      }
    }
}
	
	

						   

