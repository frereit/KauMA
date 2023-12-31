#include <algorithm>
#include <bitset>
#include <cassert>
#include <csignal>
#include <cstdint>
#include <cstdlib>
#include <emmintrin.h>
#include <iterator>
#include <ranges>
#include <smmintrin.h>
#include <wmmintrin.h>

#include "gcm/polynomial.hpp"
#include "sse.h"
#include <random>

#include <iomanip>
#include <iostream>

GCM::Polynomial
GCM::Polynomial::from_gcm_bytes(const std::vector<std::uint8_t> &gcm_bytes) {
  assert((gcm_bytes.size() == 16) &&
         "Must supply exactly 16 bytes to construct a polynomial!");
  __m128i x = _mm_setr_epi8(
      gcm_bytes.at(15), gcm_bytes.at(14), gcm_bytes.at(13), gcm_bytes.at(12),
      gcm_bytes.at(11), gcm_bytes.at(10), gcm_bytes.at(9), gcm_bytes.at(8),
      gcm_bytes.at(7), gcm_bytes.at(6), gcm_bytes.at(5), gcm_bytes.at(4),
      gcm_bytes.at(3), gcm_bytes.at(2), gcm_bytes.at(1), gcm_bytes.at(0));
  // std::raise(SIGTRAP);
  return GCM::Polynomial(x);
}

std::vector<std::uint8_t> GCM::Polynomial::to_gcm_bytes() {
  alignas(16) std::uint8_t bytes[16];
  _mm_store_si128((__m128i *)bytes, this->m_polynomial);
  std::reverse_iterator<const std::uint8_t *> first(&bytes[16]);
  std::reverse_iterator<const std::uint8_t *> last(&bytes[0]);
  auto gcm_bytes = std::vector<std::uint8_t>(first, last);
  return gcm_bytes;
}

GCM::Polynomial
GCM::Polynomial::from_exponents(const std::vector<std::uint8_t> &exponents) {
  __m128i x = _mm_setzero_si128();
  for (const std::uint8_t &exponent : exponents) {
    assert((exponent < 128) && "Exponent must be less than 128");
    x = _mm_or_si128(x, SSE::from_index(127 - exponent));
  }
  // std::raise(SIGTRAP);
  return GCM::Polynomial(x);
}

std::vector<std::uint8_t> GCM::Polynomial::to_exponents() const {
  auto exponents = std::vector<std::uint8_t>();
  for (const std::size_t &exponent : std::views::iota(0u, 128u)) {
    if (SSE::test(this->m_polynomial, 127 - exponent)) {
      exponents.push_back(exponent);
    }
  }
  return exponents;
}

GCM::Polynomial &GCM::Polynomial::operator+=(const Polynomial &rhs) {
  this->m_polynomial = _mm_xor_si128(this->m_polynomial, rhs.m_polynomial);
  return *this;
}

GCM::Polynomial &GCM::Polynomial::operator*=(const Polynomial &rhs) {
  __m128i a = this->m_polynomial;
  __m128i b = rhs.m_polynomial;

  // Algorithm from
  // https://www.intel.com/content/dam/develop/external/us/en/documents/clmul-wp-rev-2-02-2014-04-20.pdf

  // It uses 4 clmul operations to compute the 256-bit product (by textbook long
  // multiplication) and then reduces the resulting polynomial using shifting
  // and XOR tricks.
  __m128i tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, tmp8, tmp9;
  tmp3 = _mm_clmulepi64_si128(a, b, 0x00);
  tmp4 = _mm_clmulepi64_si128(a, b, 0x10);
  tmp5 = _mm_clmulepi64_si128(a, b, 0x01);
  tmp6 = _mm_clmulepi64_si128(a, b, 0x11);
  tmp4 = _mm_xor_si128(tmp4, tmp5);
  tmp5 = _mm_slli_si128(tmp4, 8);
  tmp4 = _mm_srli_si128(tmp4, 8);
  tmp3 = _mm_xor_si128(tmp3, tmp5);
  tmp6 = _mm_xor_si128(tmp6, tmp4);
  tmp7 = _mm_srli_epi32(tmp3, 31);
  tmp8 = _mm_srli_epi32(tmp6, 31);
  tmp3 = _mm_slli_epi32(tmp3, 1);
  tmp6 = _mm_slli_epi32(tmp6, 1);
  tmp9 = _mm_srli_si128(tmp7, 12);
  tmp8 = _mm_slli_si128(tmp8, 4);
  tmp7 = _mm_slli_si128(tmp7, 4);
  tmp3 = _mm_or_si128(tmp3, tmp7);
  tmp6 = _mm_or_si128(tmp6, tmp8);
  tmp6 = _mm_or_si128(tmp6, tmp9);
  tmp7 = _mm_slli_epi32(tmp3, 31);
  tmp8 = _mm_slli_epi32(tmp3, 30);
  tmp9 = _mm_slli_epi32(tmp3, 25);
  tmp7 = _mm_xor_si128(tmp7, tmp8);
  tmp7 = _mm_xor_si128(tmp7, tmp9);
  tmp8 = _mm_srli_si128(tmp7, 4);
  tmp7 = _mm_slli_si128(tmp7, 12);
  tmp3 = _mm_xor_si128(tmp3, tmp7);
  tmp2 = _mm_srli_epi32(tmp3, 1);
  tmp4 = _mm_srli_epi32(tmp3, 2);
  tmp5 = _mm_srli_epi32(tmp3, 7);
  tmp2 = _mm_xor_si128(tmp2, tmp4);
  tmp2 = _mm_xor_si128(tmp2, tmp5);
  tmp2 = _mm_xor_si128(tmp2, tmp8);
  tmp3 = _mm_xor_si128(tmp3, tmp2);
  tmp6 = _mm_xor_si128(tmp6, tmp3);

  this->m_polynomial = tmp6;
  return *this;
}

GCM::Polynomial &GCM::Polynomial::operator/=(const Polynomial &rhs) {
  *this *= rhs.modular_inverse();
  return *this;
}

GCM::Polynomial GCM::Polynomial::pow(__m128i exponent) const {
  __m128i lowest_bit = _mm_setr_epi32(0x1, 0, 0, 0);
  Polynomial out = Polynomial::one();
  Polynomial base = *this;
  while (!_mm_test_all_zeros(exponent, exponent)) {
    if (!_mm_testz_si128(exponent, lowest_bit)) {
      out *= base;
    }
    __m128i shifted = _mm_srli_epi32(exponent, 1);
    __m128i carry = _mm_slli_epi32(exponent, 31);
    carry = _mm_srli_si128(carry, 4);
    exponent = _mm_or_si128(shifted, carry);
    base *= base;
  }
  return out;
}

GCM::Polynomial GCM::Polynomial::random() {
  static std::random_device rd;
  static std::mt19937_64 gen(rd());
  static std::uniform_int_distribution<std::uint64_t> dis;
  __m128i x = _mm_setr_epi64(_mm_set_pi64x(dis(gen)), _mm_set_pi64x(dis(gen)));
  return GCM::Polynomial(x);
}

#ifdef TEST
#include "doctest.h"

TEST_CASE("polynomial gcm bytes conversions") {
  std::vector<std::uint8_t> gcm_bytes = {0xf0, 0xf0, 0xf0, 0xf0, 0x0f, 0x0f,
                                         0x0f, 0x0f, 0xf0, 0xf0, 0xf0, 0xf0,
                                         0x0f, 0x0f, 0x0f, 0x0f};
  GCM::Polynomial actual = GCM::Polynomial::from_gcm_bytes(gcm_bytes);
  GCM::Polynomial expected =
      GCM::Polynomial(0xf0f0f0f00f0f0f0f, 0xf0f0f0f00f0f0f0f);
  CHECK(actual == expected);

  // in reverse should work as well
  CHECK(expected.to_gcm_bytes() == gcm_bytes);
}

TEST_CASE("polynomial factor conversions") {
  std::vector<std::uint8_t> exponents = {0, 1, 2, 3, 125, 126, 127};
  GCM::Polynomial actual = GCM::Polynomial::from_exponents(exponents);
  GCM::Polynomial expected = GCM::Polynomial(0xf000000000000000, 0x7);
  CHECK(actual == expected);
  CHECK(expected.to_exponents() == exponents);
}

TEST_CASE("polynomial addition") {
  GCM::Polynomial a = GCM::Polynomial::from_exponents(
      {0, 1, 2, 3, 10, 11, 12, 13, 125, 126, 127});
  GCM::Polynomial b =
      GCM::Polynomial::from_exponents({1, 2, 4, 11, 12, 13, 32, 127});
  GCM::Polynomial expected =
      GCM::Polynomial::from_exponents({0, 3, 4, 10, 32, 125, 126});

  CHECK(a + b == expected);
}

TEST_CASE("polynomial multiplication") {
  GCM::Polynomial a = GCM::Polynomial(0xfdbadcb514af3c8e, 0x7436ab83ac71aea6);
  GCM::Polynomial alpha = GCM::Polynomial::from_exponents({1});

  GCM::Polynomial a_times_alpha =
      GCM::Polynomial(0x7edd6e5a8a579e47, 0x3a1b55c1d638d753);
  GCM::Polynomial a_times_alpha_squared =
      GCM::Polynomial(0xde6eb72d452bcf23, 0x9d0daae0eb1c6ba9);

  CHECK(a * alpha == a_times_alpha);
  CHECK(a * alpha * alpha == a_times_alpha_squared);

  GCM::Polynomial b = GCM::Polynomial(0xef7837cbabc961ec, 0x4c151fe393dacc52);
  GCM::Polynomial a_times_b =
      GCM::Polynomial(0xe2d3e4b7fce39ed7, 0xb6a75bb0f49bc147);

  CHECK(a * b == a_times_b);
}

TEST_CASE("polynomial inverse") {
  GCM::Polynomial a = GCM::Polynomial(0xfdbadcb514af3c8e, 0x7436ab83ac71aea6);
  GCM::Polynomial a_inv =
      GCM::Polynomial(0x2eca9f04beb1572f, 0x52e0c5e279ba7d7c);

  CHECK(a.modular_inverse() == a_inv);
  CHECK(a * a_inv == GCM::Polynomial::one());
}

TEST_CASE("polynomial division") {
  GCM::Polynomial a = GCM::Polynomial(0xfdbadcb514af3c8e, 0x7436ab83ac71aea6);
  GCM::Polynomial b = GCM::Polynomial(0xef7837cbabc961ec, 0x4c151fe393dacc52);

  GCM::Polynomial a_div_b =
      GCM::Polynomial(0x76fbadd3f9bf76bf, 0x70ead1484745c982);
  CHECK(a / b == a_div_b);
  CHECK(b * a_div_b == a);
}

#endif