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
#endif
