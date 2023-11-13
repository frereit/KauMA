#include <bitset>
#include <cassert>
#include <cstdint>
#include <ranges>

#include "f128_gcm/polynomial.hpp"

F128_GCM::Polynomial F128_GCM::Polynomial::from_gcm_bytes(
    const std::vector<std::uint8_t> &gcm_bytes) {
  assert((gcm_bytes.size() == 16) &&
         "Must supply exactly 16 bytes to construct a polynomial!");

  auto factors = std::bitset<128>(0);
  std::size_t out = 0;
  for (const std::uint8_t &byte : gcm_bytes) {
    for (const std::size_t &j :
         std::views::iota(0u, 8u) | std::views::reverse) {
      factors[out++] = (byte >> j) & 1;
    }
  }

  return F128_GCM::Polynomial(factors);
}

std::vector<std::uint8_t> F128_GCM::Polynomial::to_gcm_bytes() {
  auto gcm_bytes = std::vector<std::uint8_t>(16);
  for (const std::size_t &i : std::views::iota(0u, gcm_bytes.size())) {
    for (const std::size_t &j : std::views::iota(0u, 8u)) {
      gcm_bytes[i] |=
          static_cast<std::uint8_t>(this->m_polynomial[8 * i + 7 - j]) << j;
    }
  }
  return gcm_bytes;
}

F128_GCM::Polynomial F128_GCM::Polynomial::from_exponents(
    const std::vector<std::uint8_t> &exponents) {
  auto factors = std::bitset<128>(0);
  for (const std::uint8_t &exponent : exponents) {
    assert((exponent < factors.size()) && "Exponent must be less than 128");
    factors[exponent] = 1;
  }
  return F128_GCM::Polynomial(factors);
}

std::vector<std::uint8_t> F128_GCM::Polynomial::to_exponents() {
  auto exponents = std::vector<std::uint8_t>();
  for (const std::size_t &i : std::views::iota(0u, this->m_polynomial.size())) {
    if (this->m_polynomial[i]) {
      exponents.push_back(i);
    }
  }
  return exponents;
}

F128_GCM::Polynomial &F128_GCM::Polynomial::operator+=(const Polynomial &rhs) {
  this->m_polynomial ^= rhs.m_polynomial;
  return *this;
}

F128_GCM::Polynomial &F128_GCM::Polynomial::operator*=(const Polynomial &rhs) {
  auto mul = std::bitset<128>(0);
  std::bitset<128> a = this->m_polynomial;
  std::bitset<128> b = rhs.m_polynomial;
  while (a.any() && b.any()) {
    if (b[0]) {
      mul ^= a;
    }

    bool reduce = a[127];
    a <<= 1;
    if (reduce) {
      a ^= F128_GCM::REDUCTION_POLYNOMIAL;
    }
    b >>= 1;
  }
  this->m_polynomial = mul;
  return *this;
}

#ifdef TEST
#include <cstdint>
#include <iostream>
#include <vector>

#include "doctest.h"

TEST_CASE("polynomial gcm bytes conversions") {
  std::vector<std::uint8_t> gcm_bytes = {0xf0, 0xf0, 0xf0, 0xf0, 0x0f, 0x0f,
                                         0x0f, 0x0f, 0xf0, 0xf0, 0xf0, 0xf0,
                                         0x0f, 0x0f, 0x0f, 0x0f};
  F128_GCM::Polynomial actual = F128_GCM::Polynomial::from_gcm_bytes(gcm_bytes);
  F128_GCM::Polynomial expected = F128_GCM::Polynomial(std::bitset<128>(
      "111100001111000011110000111100000000111100001111000011110000111111110000"
      "11110000111100001111000000001111000011110000111100001111"));
  CHECK(actual == expected);

  // in reverse should work as well
  CHECK(expected.to_gcm_bytes() == gcm_bytes);
}

TEST_CASE("polynomial factor conversions") {
  std::vector<std::uint8_t> exponents = {0, 1, 2, 3, 125, 126, 127};
  F128_GCM::Polynomial actual = F128_GCM::Polynomial::from_exponents(exponents);
  F128_GCM::Polynomial expected = F128_GCM::Polynomial(std::bitset<128>(
      "111000000000000000000000000000000000000000000000000000000000000000000000"
      "00000000000000000000000000000000000000000000000000001111"));
  CHECK(actual == expected);
  CHECK(expected.to_exponents() == exponents);
}

TEST_CASE("polynomial addition") {
  F128_GCM::Polynomial a = F128_GCM::Polynomial::from_exponents(
      {0, 1, 2, 3, 10, 11, 12, 13, 125, 126, 127});
  F128_GCM::Polynomial b =
      F128_GCM::Polynomial::from_exponents({1, 2, 4, 11, 12, 13, 32, 127});
  F128_GCM::Polynomial expected =
      F128_GCM::Polynomial::from_exponents({0, 3, 4, 10, 32, 125, 126});

  CHECK(a + b == expected);
}

TEST_CASE("polynomial multiplication") {
  F128_GCM::Polynomial a = F128_GCM::Polynomial(std::bitset<128>(
      "011001010111010110001110001101011100000111010101011011000010111001110001"
      "00111100111101010010100010101101001110110101110110111111"));
  F128_GCM::Polynomial alpha = F128_GCM::Polynomial(std::bitset<128>(
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "00000000000000000000000000000000000000000000000000000010"));
  F128_GCM::Polynomial a_times_alpha = F128_GCM::Polynomial(std::bitset<128>(
      "110010101110101100011100011010111000001110101010110110000101110011100010"
      "01111001111010100101000101011010011101101011101101111110"));
  F128_GCM::Polynomial a_times_alpha_squared =
      F128_GCM::Polynomial(std::bitset<128>(
          "10010101110101100011100011010111000001110101010110110000101110011100"
          "010011110011110101001010001010110100111011010111011001111011"));

  CHECK(a * alpha == a_times_alpha);
  CHECK(a * alpha * alpha == a_times_alpha_squared);

  F128_GCM::Polynomial b = F128_GCM::Polynomial(std::bitset<128>(
      "010010100011001101011011110010011100011111111000101010000011001000110111"
      "10000110100100111101010111010011111011000001111011110111"));
  F128_GCM::Polynomial a_times_b = F128_GCM::Polynomial(std::bitset<128>(
      "111000101000001111011001001011110000110111011010111001010110110111101011"
      "01111001110001110011111111101101001001111100101101000111"));

  CHECK(a * b == a_times_b);
}
#endif
