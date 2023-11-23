#pragma once
#include <algorithm>
#include <botan/hex.h>
#include <cassert>
#include <nlohmann/json.hpp>
#include <ranges>
#include <string>
#include <tuple>
#include <vector>

#include "gcm/polynomial.hpp"

namespace GCM::CantorZassenhaus {
class Polynomial {
public:
  Polynomial(std::vector<GCM::Polynomial> coefficients)
      : m_coeffs(coefficients) {}

  std::size_t degree() const {
    if (this->m_coeffs.size() == 0)
      return 0;
    return this->m_coeffs.size() - 1;
  }

  Polynomial &operator+=(Polynomial rhs);

  Polynomial &operator-=(Polynomial rhs) {
    *this += rhs;
    return *this;
  }

  GCM::Polynomial &coefficient(std::size_t index) {
    return this->m_coeffs.at(index);
  }

  GCM::Polynomial const &coefficient(std::size_t index) const {
    return this->m_coeffs.at(index);
  }

  bool empty() const { return this->m_coeffs.size() == 0; }

  friend Polynomial operator+(Polynomial lhs, const Polynomial &rhs) {
    lhs += rhs;
    return lhs;
  }

  friend Polynomial operator-(Polynomial lhs, const Polynomial &rhs) {
    lhs -= rhs;
    return lhs;
  }

  Polynomial operator*(const Polynomial &rhs);

  Polynomial &operator*=(const Polynomial &rhs) {
    *this = *this * rhs;
    return *this;
  }

  friend bool operator==(Polynomial lhs, const Polynomial &rhs) {
    if (lhs.m_coeffs.size() != rhs.m_coeffs.size())
      return false;
    for (std::size_t i = 0; i < lhs.m_coeffs.size(); ++i) {
      if (lhs.coefficient(i) != rhs.coefficient(i)) {
        return false;
      }
    }
    return true;
  }

  /// @brief calling this function ensures that the highest order coefficient
  /// is 1.
  void ensure_monic();

  template <std::size_t l>
  Polynomial pow(std::bitset<l> exponents, Polynomial mod) const {
    if (exponents.size() == 0) {
      return Polynomial({GCM::Polynomial(1)});
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
      auto [_quotient, remainder] = out.divmod(mod);
      out = remainder;
    }
    return out;
  }

  Polynomial &operator<<=(std::size_t amount) {
    for (std::size_t i = 0; i < amount; ++i) {
      this->m_coeffs.insert(this->m_coeffs.begin(), GCM::Polynomial(0));
    }
    return *this;
  }

  friend Polynomial operator<<(Polynomial lhs, std::size_t amount) {
    lhs <<= amount;
    return lhs;
  }

  std::tuple<Polynomial, Polynomial> divmod(Polynomial divisor) const;

  /// @brief generate a Cantor Zassenhaus polynomial with random coefficient
  /// @param degree the degree of the resulting polynomial
  /// @return a random polynomial
  static Polynomial random(std::size_t degree);

  nlohmann::json to_json() {
    std::vector<std::string> coefficients;
    for (std::size_t i = 0; i < this->m_coeffs.size(); ++i) {
      coefficients.push_back(
          Botan::hex_encode(this->m_coeffs.at(i).to_gcm_bytes()));
    }
    return nlohmann::json(coefficients);
  }

private:
  std::vector<GCM::Polynomial> m_coeffs;
};
} // namespace GCM::CantorZassenhaus