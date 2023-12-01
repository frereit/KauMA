#include <algorithm>
#include <cassert>
#include <cppcodec/base64_default_rfc4648.hpp>
#include <nlohmann/json.hpp>
#include <ranges>
#include <tuple>
#include <vector>

#include "gcm/cantor_zassenhaus/polynomial.hpp"
#include "gcm/polynomial.hpp"

GCM::CantorZassenhaus::Polynomial &
GCM::CantorZassenhaus::Polynomial::operator+=(
    GCM::CantorZassenhaus::Polynomial rhs) {
  if (this->m_coeffs.size() < rhs.m_coeffs.size()) {
    this->m_coeffs.resize(rhs.m_coeffs.size(), GCM::Polynomial::zero());
  } else if (rhs.m_coeffs.size() < this->m_coeffs.size()) {
    rhs.m_coeffs.resize(this->m_coeffs.size(), GCM::Polynomial::zero());
  }
  assert(rhs.m_coeffs.size() == this->m_coeffs.size() &&
         "Sizes of Polynomials must be equal after resizing.");

  for (std::size_t i = 0; i < rhs.m_coeffs.size(); ++i) {
    this->m_coeffs.at(i) += rhs.m_coeffs.at(i);
  }
  this->ensure_normalized();
  return *this;
}

GCM::CantorZassenhaus::Polynomial GCM::CantorZassenhaus::Polynomial::operator*(
    const GCM::CantorZassenhaus::Polynomial &rhs) {
  GCM::CantorZassenhaus::Polynomial out(std::vector<GCM::Polynomial>(
      this->degree() + rhs.degree() + 1, GCM::Polynomial::zero()));

  for (std::size_t i = 0; i < this->m_coeffs.size(); ++i) {
    for (std::size_t j = 0; j < rhs.m_coeffs.size(); ++j) {
      out.coefficient(i + j) += this->m_coeffs.at(i) * rhs.m_coeffs.at(j);
    }
  }

  out.ensure_normalized();
  return out;
}

std::tuple<GCM::CantorZassenhaus::Polynomial, GCM::CantorZassenhaus::Polynomial>
GCM::CantorZassenhaus::Polynomial::divmod(
    GCM::CantorZassenhaus::Polynomial divisor) const {
  if (this->degree() < divisor.degree()) {
    return {GCM::CantorZassenhaus::Polynomial({}), *this};
  }

  std::size_t out_degree = this->degree() - divisor.degree();
  GCM::CantorZassenhaus::Polynomial q(
      {out_degree + 1, GCM::Polynomial::zero()}),
      r = *this;
  while (r.degree() >= divisor.degree() && !r.empty()) {
    std::size_t degree = r.degree() - divisor.degree();
    q.coefficient(degree) =
        r.coefficient(r.degree()) / divisor.coefficient(divisor.degree());
    GCM::CantorZassenhaus::Polynomial factor{{q.coefficient(degree)}};
    factor <<= degree;
    r -= factor * divisor;
  }
  assert(q * divisor + r == *this);
  q.ensure_normalized();
  r.ensure_normalized();
  return {q, r};
}

GCM::CantorZassenhaus::Polynomial GCM::CantorZassenhaus::Polynomial::pow(
    __m128i exponent, GCM::CantorZassenhaus::Polynomial mod) const {
  __m128i lowest_bit = _mm_setr_epi32(0x1, 0, 0, 0);
  Polynomial out({GCM::Polynomial::one()});
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

    base %= mod;
    out %= mod;
  }
  return out;
}

void GCM::CantorZassenhaus::Polynomial::ensure_monic() {
  this->ensure_normalized();
  for (std::size_t i = 0; i < this->m_coeffs.size(); ++i) {
    this->m_coeffs.at(i) /= this->m_coeffs.back();
  }
}

void GCM::CantorZassenhaus::Polynomial::ensure_normalized() {
  while (this->m_coeffs.size() > 0 &&
         this->m_coeffs.back() == GCM::Polynomial::zero()) {
    this->m_coeffs.pop_back();
  }
}

GCM::CantorZassenhaus::Polynomial
GCM::CantorZassenhaus::Polynomial::random(std::size_t degree) {
  std::vector<GCM::Polynomial> rand;
  for (std::size_t i = 0; i <= degree; ++i) {
    rand.push_back(GCM::Polynomial::random());
  }
  return GCM::CantorZassenhaus::Polynomial(rand);
}

nlohmann::json GCM::CantorZassenhaus::Polynomial::to_json() {
  std::vector<std::string> coefficients;
  for (std::size_t i = 0; i < this->m_coeffs.size(); ++i) {
    coefficients.push_back(
        cppcodec::base64_rfc4648::encode(this->m_coeffs.at(i).to_gcm_bytes()));
  }
  return nlohmann::json(coefficients);
}

#ifdef TEST
#include "doctest.h"

#include <botan/hex.h>

TEST_CASE("Cantor-Zassenhaus polynomial arithmetic") {
  GCM::CantorZassenhaus::Polynomial product(
      {GCM::Polynomial(0x7a9c3400001a584b, 0xb29b0a03b7971984),
       GCM::Polynomial(0x1b81c000000000a9, 0xd95c170026d05960),
       GCM::Polynomial(0xf438000000000000, 0x00c45e91cfdc121e),
       GCM::Polynomial(0x0000000000000000, 0x00000000de6df8f8),
       GCM::Polynomial::one()});

  GCM::CantorZassenhaus::Polynomial factor_a(
      {GCM::Polynomial(0x0000000000000000, 0x0000000000c0ffee),
       GCM::Polynomial::one()});
  GCM::CantorZassenhaus::Polynomial factor_b(
      {GCM::Polynomial(0x05df800000000000, 0x19464ea44524eaf9),
       GCM::Polynomial(0xe818000000000000, 0x0000bf66d09ce402),
       GCM::Polynomial(0x0000000000000000, 0x00000000dead0716),
       GCM::Polynomial::one()});

  CHECK(factor_a * factor_b == product);
  auto [quotient, remainder] = product.divmod(factor_a);
  CHECK(quotient == factor_b);
  CHECK(remainder.empty());
}

TEST_CASE("Cantor-Zassenhaus polynomial exponentation with zero result") {
  GCM::CantorZassenhaus::Polynomial x(
      {GCM::Polynomial::from_gcm_bytes(
           Botan::hex_decode("0000000000244141663CE8FDA40E6BD1")),
       GCM::Polynomial::from_gcm_bytes(
           Botan::hex_decode("0000000000244141663CE8FDA40E6BD1")),
       GCM::Polynomial::from_gcm_bytes(
           Botan::hex_decode("0000000000244141663CE8FDA40E6BD1"))});
  GCM::CantorZassenhaus::Polynomial mod(
      {GCM::Polynomial::from_gcm_bytes(
           Botan::hex_decode("000000000000000001B88015EB95DB33")),
       GCM::Polynomial::from_gcm_bytes(
           Botan::hex_decode("000000000000000001B88015EB95DB33")),
       GCM::Polynomial::from_gcm_bytes(
           Botan::hex_decode("000000000000000001B88015EB95DB33"))});
  auto res = x.pow(_mm_setr_epi32(1000000, 0, 0, 0), mod);
  CHECK(res.empty());
}
#endif