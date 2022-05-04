#include "gtest/gtest.h"
#include "integer.h"

TEST(Ints,Flags) {
    typedef uint8_t uint_type;
    typedef int8_t  int_type;
    for (int x=std::numeric_limits<int_type>::min(); x<=std::numeric_limits<int_type>::max(); ++x) {
        int_type i(x);
        uint_type u(x);

        ASSERT_EQ((ints<uint_type,1,1,1>::is_negative(i)),uint8_t(i<0)) << "i=" << int32_t(i);
        ASSERT_EQ((ints<uint_type,1,0,1>::is_negative(u)),uint8_t(u<0)) << "u=" << uint32_t(u);

        ASSERT_EQ((ints<uint_type,1,1,1>::is_zero(i)),uint8_t(i==0)) << "i=" << int32_t(i);
        ASSERT_EQ((ints<uint_type,1,0,1>::is_zero(u)),uint8_t(u==0)) << "u=" << uint32_t(u);

        ASSERT_EQ((ints<uint_type,1,1,1>::is_positive(i)),uint8_t(i>0)) << "i=" << int32_t(i);
        ASSERT_EQ((ints<uint_type,1,0,1>::is_positive(u)),uint8_t(u>0)) << "u=" << uint32_t(u);

        ASSERT_EQ((ints<uint_type,1,1,1>::sign(i)),int8_t(i<0?-1:(i==0?0:1))) << "i=" << int32_t(i);
        ASSERT_EQ((ints<uint_type,1,0,1>::sign(u)),int8_t(u==0?0:1)) << "u=" << uint32_t(u);
    }
}
