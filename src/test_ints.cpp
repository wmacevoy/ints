#include <iostream>
#include <tgmath.h>
#include "gtest/gtest.h"

#include "ints.h"

#define STR(x) (([&]() { std::ostringstream oss; oss << x; return oss.str(); })())

#define ATNV(n, v) << " && (" << #n << "==" << (v) << ")"
#define AT(v) ATNV(v, v)
#define ATI(v) ATNV(v, int32_t(v))
#define ATU(v) ATNV(v, int32_t(v))
#define ATB(v) << " && (" << ((v) ? "" : "! ") << #v ")"

#define BP(ats) SCOPED_TRACE(STR(" if (true" ats << ") bp(" << __LINE__ << ");"))

TEST(Ints, Constants8)
{
  typedef uint8_t uint_type;
  typedef int8_t int_type;

  ASSERT_EQ((ints<uint_type, 1, 1, 1>::uint_sign_mask), 0x80);
  ASSERT_EQ((ints<uint_type, 1, 0, 1>::uint_sign_mask), 0x00);
  ASSERT_EQ((ints<uint_type, 1, 1, 1>::uint_zeros), 0x00);
  ASSERT_EQ((ints<uint_type, 1, 0, 1>::uint_zeros), 0x00);
  ASSERT_EQ((ints<uint_type, 1, 1, 1>::uint_ones), 0xFF);
  ASSERT_EQ((ints<uint_type, 1, 0, 1>::uint_ones), 0xFF);
  ASSERT_EQ((ints<uint_type, 1, 1, 1>::uint_min), 0x80);
  ASSERT_EQ((ints<uint_type, 1, 0, 1>::uint_min), 0x00);
  ASSERT_EQ((ints<uint_type, 1, 1, 1>::uint_max), 0x7F);
  ASSERT_EQ((ints<uint_type, 1, 0, 1>::uint_max), 0xFF);
}

TEST(Ints, Constants16)
{
  typedef uint16_t uint_type;
  typedef int16_t int_type;

  ASSERT_EQ((ints<uint_type, 1, 1, 1>::uint_sign_mask), 0x8000);
  ASSERT_EQ((ints<uint_type, 1, 0, 1>::uint_sign_mask), 0x0000);
  ASSERT_EQ((ints<uint_type, 1, 1, 1>::uint_zeros), 0x0000);
  ASSERT_EQ((ints<uint_type, 1, 0, 1>::uint_zeros), 0x0000);
  ASSERT_EQ((ints<uint_type, 1, 1, 1>::uint_ones), 0xFFFF);
  ASSERT_EQ((ints<uint_type, 1, 0, 1>::uint_ones), 0xFFFF);
  ASSERT_EQ((ints<uint_type, 1, 1, 1>::uint_min), 0x8000);
  ASSERT_EQ((ints<uint_type, 1, 0, 1>::uint_min), 0x0000);
  ASSERT_EQ((ints<uint_type, 1, 1, 1>::uint_max), 0x7FFF);
  ASSERT_EQ((ints<uint_type, 1, 0, 1>::uint_max), 0xFFFF);
}

TEST(Ints, Flags8)
{
  typedef uint8_t uint_type;
  typedef int8_t int_type;

  for (int x = std::numeric_limits<int_type>::min(); x <= std::numeric_limits<int_type>::max(); ++x)
  {
    BP(ATI(x));

    int_type i(x);
    uint_type u(x);

    ASSERT_EQ((ints<uint_type, 1, 1, 1>::is_negative(i)), uint8_t(i < 0)) << "i=" << int32_t(i);
    ASSERT_EQ((ints<uint_type, 1, 0, 1>::is_negative(u)), uint8_t(u < 0)) << "u=" << uint32_t(u);

    ASSERT_EQ((ints<uint_type, 1, 1, 1>::is_zero(i)), uint8_t(i == 0)) << "i=" << int32_t(i);
    ASSERT_EQ((ints<uint_type, 1, 0, 1>::is_zero(u)), uint8_t(u == 0)) << "u=" << uint32_t(u);

    ASSERT_EQ((ints<uint_type, 1, 1, 1>::is_positive(i)), uint8_t(i > 0)) << "i=" << int32_t(i);
    ASSERT_EQ((ints<uint_type, 1, 0, 1>::is_positive(u)), uint8_t(u > 0)) << "u=" << uint32_t(u);

    EXPECT_EQ((ints<uint_type, 1, 1, 1>::sign(i)), int8_t(i < 0 ? -1 : (i == 0 ? 0 : 1))) << "i=" << int32_t(i);
    ASSERT_EQ((ints<uint_type, 1, 0, 1>::sign(u)), int8_t(u == 0 ? 0 : 1)) << "u=" << uint32_t(u);
  }
}

TEST(Ints, AddWithCarry1)
{
  typedef uint8_t uint_type;
  typedef int8_t int_type;
  for (int x = std::numeric_limits<int_type>::min(); x <= std::numeric_limits<int_type>::max(); ++x)
  {
    for (int y = std::numeric_limits<int_type>::min(); y <= std::numeric_limits<int_type>::max(); ++y)
    {
      for (bool c : {false, true})
      {
        for (int s : {false, true})
        {
          BP(ATI(x) ATI(y) ATB(c) ATB(s));
          if (s)
          {
            int_type s(x);
            int_type a(x);
            int_type b(y);
            int32_t sum = int32_t(int_type(x)) + int32_t(int_type(y)) + int32_t(int_type(c));
            uint32_t usum = uint32_t(uint_type(x)) + uint32_t(uint_type(y)) + uint32_t(uint_type(c));
            uint8_t c0 = (usum >> (8 * sizeof(int_type))) & 1;
            uint8_t c1 = ints<uint_type, 1, 1, 1>::add_with_carry((uint_type &)s, (const uint_type &)b, uint8_t(c));
            ASSERT_EQ(int_type(sum), s);
            ASSERT_EQ(c0, c1);
          }
          else
          {
            uint_type s(x);
            uint_type a(x);
            uint_type b(y);
            int32_t sum = int32_t(int_type(x)) + int32_t(int_type(y)) + int32_t(int_type(c));
            uint32_t usum = uint32_t(uint_type(x)) + uint32_t(uint_type(y)) + uint32_t(uint_type(c));
            uint8_t c0 = (usum >> (8 * sizeof(int_type))) & 1;
            uint8_t c1 = ints<uint_type, 1, 0, 1>::add_with_carry(s, b, uint8_t(c));
            ASSERT_EQ(uint_type(sum), s);
            ASSERT_EQ(c0, c1);
          }
        }
      }
    }
  }
}

void bp(int line)
{
  std::cout << "bp(" << line << ")" << std::endl;
};

TEST(Ints, FShift)
{
  typedef uint8_t uint_type;
  typedef int8_t int_type;

  for (int x = std::numeric_limits<int_type>::min(); x <= std::numeric_limits<int_type>::max(); ++x)
  {
    int_type i(x);
    uint_type u(x);

    for (int pow2 = -1000; pow2 <= 1000; ++pow2)
    {
      BP(ATI(x) ATI(pow2));

      int_type si = ints<uint_type, 1, 1, 1>::fshift(i, pow2);
      uint_type su = ints<uint_type, 1, 1, 1>::fshift(u, pow2);

      ASSERT_EQ(uint_type(si), su);

      if (pow2 < 0)
      {
        if (pow2 > -8 * sizeof(uint_type))
        {
          ASSERT_EQ(uint_type(u >> -pow2), su);
        }
        else
        {
          ASSERT_EQ(0, su);
        }
      }
      else
      {
        if (pow2 < 8 * sizeof(uint_type))
        {
          ASSERT_EQ(uint_type(u << pow2), su);
        }
        else
        {
          ASSERT_EQ(0, su);
        }
      }
    }
  }
}

TEST(Ints, Shift2)
{
  typedef uint8_t uint_type;
  typedef int8_t int_type;
  typedef uint16_t uint2_type;

  for (int x = std::numeric_limits<int_type>::min(); x <= std::numeric_limits<int_type>::max(); ++x)
  {
    for (int y = std::numeric_limits<int_type>::min(); y <= std::numeric_limits<int_type>::max(); ++y)
    {
      for (int pow2 = -20; pow2 <= 20; ++pow2)
      {
        uint_type hi(x);
        uint_type lo(y);
        uint2_type u(hi << 8 * sizeof(uint_type) | lo);
        ints<uint_type, 1, 1, 1>::shift(hi, lo, pow2);
        uint2_type v(hi << 8 * sizeof(uint_type) | lo);

        if (pow2 < 0)
        {
          if (pow2 > -8 * sizeof(uint2_type))
          {
            ASSERT_EQ(uint2_type(u >> -pow2), v) << " u=" << u << " pow2=" << pow2;
          }
          else
          {
            ASSERT_EQ(0, v) << " u=" << u << " pow2=" << pow2;
          }
        }
        else
        {
          if (pow2 < 8 * sizeof(uint2_type))
          {
            ASSERT_EQ(uint2_type(u << pow2), v) << " u=" << u << " pow2=" << pow2;
          }
          else
          {
            ASSERT_EQ(0, v) << " u=" << u << " pow2=" << pow2;
          }
        }
      }
    }
  }
}

TEST(Ints, UUMultiply)
{
  typedef uint8_t uint_type;
  typedef int8_t int_type;
  typedef uint16_t uint2_type;
  typedef int16_t int2_type;

  for (int x = std::numeric_limits<int_type>::min(); x <= std::numeric_limits<int_type>::max(); ++x)
  {
    for (int y = std::numeric_limits<int_type>::min(); y <= std::numeric_limits<int_type>::max(); ++y)
    {
      uint_type hi(0);
      uint_type lo(0);

      uint_type a(x);
      uint_type b(y);

      ints<uint_type, 1, false, true>::uumultiply(hi, lo, a, b);

      uint2_type u(hi << 8 * sizeof(uint_type) | lo);

      uint2_type ab = uint2_type(a) * uint2_type(b);

      ASSERT_EQ(ab, u)
          << " if (a==" << uint32_t(a) << " && b==" << uint32_t(b) << ") bp(__LINE__);";
    }
  }
}

template <typename uint_type>
struct wrap_base
{
  const uint32_t size;
  const bool is_signed;
  const bool is_little;

  struct ptr : std::shared_ptr<wrap_base>
  {
    ptr(wrap_base *p) : std::shared_ptr<wrap_base>(p) {}
  };

  wrap_base(uint32_t _size, bool _is_signed, bool _is_little)
      : size(_size), is_signed(_is_signed), is_little(_is_little)
  {
  }
  virtual uint_type get(uint32_t i) const = 0;
  virtual void set(uint32_t i, uint_type vi) = 0;
  virtual uint8_t add_with_carry(const ptr b, uint8_t c) = 0;
  virtual void print(std::ostream &out) const=0;
  virtual ~wrap_base() {}

  friend std::ostream& operator<<(std::ostream& out, const wrap_base &wb) {
    wb.print(out);
    return out;
  }
  
};

template <typename uint_type, uint32_t size, bool is_signed, bool is_little>
struct wrap : wrap_base<uint_type>
{
  typedef ints<uint_type, size, is_signed, is_little> type;
  type value;
  wrap() : wrap_base<uint_type>(size, is_signed, is_little)
  {
    for (int i = 0; i < size; ++i)
      value[i] = 0;
  }

  virtual uint_type get(uint32_t i) const override { return value.at(i); };
  virtual void set(uint32_t i, uint_type vi) override { value.at(i) = vi; };
  virtual uint8_t add_with_carry(const typename wrap_base<uint_type>::ptr b, uint8_t c) override
  {
    const type &bvalue = dynamic_cast<const wrap &>(*b).value;
    return type::add_with_carry(value, bvalue, c);
  }
  virtual void print(std::ostream &out) const {
    out << value;
  }
};

template <typename uint_type>
typename wrap_base<uint_type>::ptr wrapper(int size, bool is_signed, bool is_little)
{
  switch (size)
  {
  case 1:
    if (is_signed)
    {
      if (is_little)
      {
        return new wrap<uint_type, 1, true, true>();
      }
      else
      {
        return new wrap<uint_type, 1, true, false>();
      }
    }
    else
    {
      if (is_little)
      {
        return new wrap<uint_type, 1, false, true>();
      }
      else
      {
        return new wrap<uint_type, 1, false, false>();
      }
    }
  case 2:
    if (is_signed)
    {
      if (is_little)
      {
        return new wrap<uint_type, 2, true, true>();
      }
      else
      {
        return new wrap<uint_type, 2, true, false>();
      }
    }
    else
    {
      if (is_little)
      {
        return new wrap<uint_type, 2, false, true>();
      }
      else
      {
        return new wrap<uint_type, 2, false, false>();
      }
    }
  }
  throw std::range_error("unsuported");
}

#define U2(x,y) uint2_type(uint2_type(uint_type(x) << (8 * sizeof(uint_type))) | uint_type(y))
#define LO2(xy) uint_type(xy)
#define HI2(xy) uint_type(xy >> (8*sizeof(uint_type)))

TEST(Ints, Rep2)
{
  typedef uint8_t uint_type;
  typedef int8_t int_type;
  typedef uint16_t uint2_type;
  typedef int16_t int2_type;

  for (int x = std::numeric_limits<int_type>::min(); x <= std::numeric_limits<int_type>::max(); ++x)
  {
    for (int y = std::numeric_limits<int_type>::min(); y <= std::numeric_limits<int_type>::max(); ++y)
    {
      for (int is_signed = 0; is_signed < 2; ++is_signed)
	{
	  for (int is_little = 0; is_little < 2; ++is_little)
	    {
	      BP(ATI(x) ATI(y) ATB(is_signed) ATB(is_little));
	      uint32_t size = 2;

	      int hi = is_little ? 1 : 0;
	      int lo = is_little ? 0 : 1;

	      wrap_base<uint_type>::ptr wxy = wrapper<uint_type>(size, is_signed, is_little);
	      wxy->set(hi,x);
	      wxy->set(lo,y);
	      uint2_type xy = U2(x,y);

	      ASSERT_EQ(uint_type(wxy->get(lo)),LO2(xy));
	      ASSERT_EQ(uint_type(wxy->get(hi)),HI2(xy));
	    }
	}
    }
  }
}
					 
TEST(Ints, AddWithCarry2)
{
  typedef uint8_t uint_type;
  typedef int8_t int_type;
  typedef uint16_t uint2_type;
  typedef int16_t int2_type;

  for (int x : { -128,-127,-126, -65,-64,-63, -33,-32,-31, -17,-16,-15, -3,-2,-1,0,1,2,3, 15,16,17, 31,32,33, 63,64,65, 126,127 })
  {
    for (int y : { -128,-127,-126, -65,-64,-63, -33,-32,-31, -17,-16,-15, -3,-2,-1,0,1,2,3, 15,16,17, 31,32,33, 63,64,65, 126,127 })
    {
      for (int a : { -128,-127,-126, -65,-64,-63, -33,-32,-31, -17,-16,-15, -3,-2,-1,0,1,2,3, 15,16,17, 31,32,33, 63,64,65, 126,127 })
      {
        for (int b : { -128,-127,-126, -65,-64,-63, -33,-32,-31, -17,-16,-15, -3,-2,-1,0,1,2,3, 15,16,17, 31,32,33, 63,64,65, 126,127 })
        {
          for (int c = 0; c < 2; ++c)
          {
            for (int is_signed = 0; is_signed < 2; ++is_signed)
            {
              for (int is_little = 0; is_little < 2; ++is_little)
              {
                BP(ATI(x) ATI(y) ATI(a) ATI(b) ATI(c) ATB(is_signed) ATB(is_little));
                uint32_t size = 2;

                int hi = is_little ? 1 : 0;
                int lo = is_little ? 0 : 1;
		
                wrap_base<uint_type>::ptr wxy = wrapper<uint_type>(size, is_signed, is_little);
                wrap_base<uint_type>::ptr wab = wrapper<uint_type>(size, is_signed, is_little);
                wrap_base<uint_type>::ptr wsum = wrapper<uint_type>(size, is_signed, is_little);

                uint2_type xy = U2(x,y);
                uint2_type ab = U2(a,b);

                wxy->set(hi, x);
                wxy->set(lo, y);
                wab->set(hi, a);
                wab->set(lo, b);
                wsum->set(hi, x);
                wsum->set(lo, y);

                uint64_t ans = xy + ab + uint2_type(c);

		uint_type ans_lo = LO2(ans);
		uint_type ans_hi = HI2(ans);
		uint_type ans_c  = 1U & uint_type(ans >> (16 * sizeof(uint_type)));

                uint8_t d = wsum->add_with_carry(wab, c);

                ASSERT_EQ(wsum->get(lo), ans_lo);
                ASSERT_EQ(wsum->get(hi), ans_hi);
		ASSERT_EQ(d, ans_c);
              }
            }
          }
        }
      }
    }
  }
}
