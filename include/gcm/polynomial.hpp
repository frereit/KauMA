#pragma once
#include <bitset>
#include <cassert>
#include <set>
#include <stdexcept>
#include <vector>

namespace GCM {

const std::bitset<128> REDUCTION_POLYNOMIAL(
    "00000000000000000000000000000000000000000000000000000000000000000000000000"
    "000000000000000000000000000000000000000000000010000111");

// FIXME: This could instead be a template<std::size_t N> Galois::Polynomial to
// support arbitrary sized fields with an arbitrary reduction polynomial.
// However, it is much easier to always use the GCM specific polynomial for now.
class Polynomial {
public:
  /// @brief construct a new Polynomial over F_(2^128) from a list of factors
  /// @param polynomial \p polynomial [i] is the factor of x^i
  Polynomial(const std::bitset<128> &polynomial) : m_polynomial(polynomial) {}

  // FIXME: Should the below static functions be constructor overloads instead?
  // Pro: These are, factually, constructors.
  // Con: The overload makes it very ambigious which is which, since both
  // from_gcm_polynomial and from_exponents take a container of std::uint8_t.

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
  std::vector<std::uint8_t> to_exponents();

  template <std::size_t l> Polynomial pow(std::bitset<l> exponents) const {
    if (exponents.size() == 0) {
      return Polynomial(1);
    }

    assert(exponents.test(exponents.size() - 1) &&
           "Exponent must be binary number with no leading zeros");
    Polynomial out = *this;
    for (const std::size_t &i :
         std::views::iota(0u, exponents.size() - 1) | std::views::reverse) {
      out *= out;
      if (exponents.test(i)) {
        out *= *this;
      }
    }
    return out;
  }

  Polynomial modular_inverse() const {
    return this->pow(std::bitset<128>(
        "1111111111111111111111111111111111111111111111111111111111111111111111"
        "11"
        "11111111111111111111111111111111111111111111111111111110"));
  }

  Polynomial &operator+=(const Polynomial &rhs);
  Polynomial &operator*=(const Polynomial &rhs);
  Polynomial &operator/=(const Polynomial &rhs);

  friend inline bool operator==(const Polynomial &lhs, const Polynomial &rhs) {
    return lhs.m_polynomial == rhs.m_polynomial;
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

private:
  std::bitset<128> m_polynomial;
};
} // namespace GCM