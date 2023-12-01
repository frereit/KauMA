#pragma once
#include <cassert>
#include <cppcodec/base64_default_rfc4648.hpp>
#include <emmintrin.h>
#include <iostream>
#include <ranges>
#include <set>
#include <smmintrin.h>
#include <stdexcept>
#include <vector>

namespace GCM {

const __m128i REDUCTION_POLYNOMIAL = _mm_setr_epi64(
    _mm_set_pi64x(0), _mm_set_pi64x(1 << 7 | 1 << 2 | 1 << 1 | 1 << 0));

// FIXME: This could instead be a template<std::size_t N> Galois::Polynomial to
// support arbitrary sized fields with an arbitrary reduction polynomial.
// However, it is much easier to always use the GCM specific polynomial for now.
class Polynomial {
public:
  /// @brief construct a new Polynomial over F_(2^128) from a list of factors in
  /// GCM convention (i.e. reversed)
  /// @param polynomial \p polynomial [i] is the factor of x^i
  Polynomial(const __m128i polynomial) : m_polynomial(polynomial) {}

  /// @brief construct a new Polynomial over F_(2^128) from a list of factors in
  /// GCM convention (i.e. reversed)
  /// @param high factors 0 to 63
  /// @param low factors 64 to 127
  Polynomial(const std::uint64_t high, const std::uint64_t low)
      : m_polynomial(_mm_setr_epi64(_mm_set_pi64x(low), _mm_set_pi64x(high))) {}

  static Polynomial one() { return Polynomial(1llu << 63, 0); }

  static Polynomial zero() { return Polynomial(0, 0); }

  /// @brief construct a new Polynomial over F_(2^128) from a list of bytes,
  /// ordered as in specified in NIST Special Publication 800-38D, page 12.
  /// @param gcm_bytes the factors for the polynomial. \p gcm_bytes
  /// [0]'s MSB represents the factor of x^0, \p gcm_bytes[0]'s LSB
  /// represents the factor of x^7, and so on.
  /// @return the constructed Polynomial
  static Polynomial from_gcm_bytes(const std::vector<std::uint8_t> &gcm_bytes);

  /// @brief construct a new Polynomial over F_(2^128) from a set of exponents.
  /// @param exponents the polynomial will have a 1 as a factor at every index
  /// specified in this vector.
  /// @return the constructed Polynomial
  static Polynomial from_exponents(const std::vector<std::uint8_t> &exponents);

  // FIXME: These conversion functions return std::vector<std::uint8_t>.
  // This is mostly done for compatibility with the JSON parsing, but
  // breaks the abstraction that this code is just a "library" being used by the
  // glue code. Instead, `to_gcm_bytes` and `to_exponents` should return a
  // std::array<16, std::uint8_t> and std::set<std::uint8_t> respectively.

  /// @brief convert the Polynomial to a representation as specified in NIST
  /// Special Publication 800-38D, page 12.
  /// @return the factors for the polynomial. gcm_bytes[0]'s MSB represents the
  /// factor of x^0, gcm_bytes[0]'s LSB represents the factor of x^7, and so on.
  std::vector<std::uint8_t> to_gcm_bytes();

  /// @brief convert the Polynomial to the list of exponents.
  /// @return the polynomial has a 1 as a factor at every index
  /// specified in this vector.
  std::vector<std::uint8_t> to_exponents() const;

  Polynomial pow(__m128i exponent) const;

  Polynomial modular_inverse() const {
    return this->pow(
        _mm_setr_epi32(0xfffffffe, 0xffffffff, 0xffffffff, 0xffffffff));
  }

  /// @brief generate a random polynomial in GF_(2^128)
  /// @return a polynomial with each exponent appearing with roughly 50%
  /// probability.
  static Polynomial random();

  Polynomial &operator+=(const Polynomial &rhs);
  Polynomial &operator*=(const Polynomial &rhs);
  Polynomial &operator/=(const Polynomial &rhs);

  friend inline bool operator==(const Polynomial &lhs, const Polynomial &rhs) {
    __m128i diff = _mm_xor_si128(lhs.m_polynomial, rhs.m_polynomial);
    return _mm_test_all_zeros(diff, diff);
  }

  friend inline bool operator!=(const Polynomial &lhs, const Polynomial &rhs) {
    return !(lhs == rhs);
  }

  friend Polynomial operator+(Polynomial lhs, const Polynomial &rhs) {
    lhs += rhs;
    return lhs;
  }

  friend Polynomial operator-(Polynomial lhs, const Polynomial &rhs) {
    lhs += rhs;
    return lhs;
  }

  Polynomial &operator-=(const Polynomial &rhs) {
    *this += rhs;
    return *this;
  }

  friend Polynomial operator*(Polynomial lhs, const Polynomial &rhs) {
    lhs *= rhs;
    return lhs;
  }

  friend Polynomial operator/(Polynomial lhs, const Polynomial &rhs) {
    lhs /= rhs;
    return lhs;
  }

  friend std::ostream &operator<<(std::ostream &os, Polynomial &poly) {
    return os << cppcodec::base64_rfc4648::encode(poly.to_gcm_bytes());
  }

private:
  __m128i m_polynomial;
};
} // namespace GCM