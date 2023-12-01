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

GCM::Polynomial GCM::Polynomial::random() {
  static std::random_device rd;
  static std::mt19937_64 gen(rd());
  static std::uniform_int_distribution<std::uint64_t> dis;
  __m128i x = _mm_setr_epi64(_mm_set_pi64x(dis(gen)), _mm_set_pi64x(dis(gen)));
  return GCM::Polynomial(x);
}
