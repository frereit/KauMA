#include <algorithm>
#include <cassert>
#include <cppcodec/base64_default_rfc4648.hpp>
#include <nlohmann/json.hpp>
#include <ranges>
#include <tuple>
#include <vector>

#include "gcm/cantor_zassenhaus/polynomial.hpp"

GCM::CantorZassenhaus::Polynomial &
GCM::CantorZassenhaus::Polynomial::operator+=(
    GCM::CantorZassenhaus::Polynomial rhs) {
  if (this->m_coeffs.size() < rhs.m_coeffs.size()) {
    this->m_coeffs.resize(rhs.m_coeffs.size(), GCM::Polynomial(0));
  } else if (rhs.m_coeffs.size() < this->m_coeffs.size()) {
    rhs.m_coeffs.resize(this->m_coeffs.size(), GCM::Polynomial(0));
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
      this->degree() + rhs.degree() + 1, GCM::Polynomial(0)));

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
  GCM::CantorZassenhaus::Polynomial q({out_degree + 1, GCM::Polynomial(0)}),
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

void GCM::CantorZassenhaus::Polynomial::ensure_monic() {
  this->ensure_normalized();
  while (this->m_coeffs.size() > 0 &&
         this->m_coeffs.back() == GCM::Polynomial(0)) {
    this->m_coeffs.pop_back();
  }
  for (std::size_t i = 0; i < this->m_coeffs.size(); ++i) {
    this->m_coeffs.at(i) /= this->m_coeffs.back();
  }
}

void GCM::CantorZassenhaus::Polynomial::ensure_normalized() {
  while (this->m_coeffs.size() > 0 &&
         this->m_coeffs.back() == GCM::Polynomial(0)) {
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

#include <gcm/cantor_zassenhaus/polynomial.hpp>

TEST_CASE("Cantor-Zassenhaus polynomial arithmetic") {
  GCM::CantorZassenhaus::Polynomial product{std::vector<GCM::Polynomial>{
      GCM::Polynomial(std::bitset<128>(
          "00100001100110001110100111101101110000000101000011011001010011011101"
          "001000011010010110000000000000000000001011000011100101011110")),
      GCM::Polynomial(std::bitset<128>(
          "00000110100110100000101101100100000000001110100000111010100110111001"
          "010100000000000000000000000000000000000000111000000111011000")),
      GCM::Polynomial(std::bitset<128>(
          "01111000010010000011101111110011100010010111101000100011000000000000"
          "000000000000000000000000000000000000000000000001110000101111")),
      GCM::Polynomial(std::bitset<128>(
          "0001111100011111101101100111101100000000000000000000000000000000000"
          "0"
          "000000000000000000000000000000000000000000000000000000000000")),
      GCM::Polynomial(1)}};

  GCM::CantorZassenhaus::Polynomial factor_a(std::vector<GCM::Polynomial>{
      GCM::Polynomial(std::bitset<128>(
          "01110111111111110000001100000000000000000000000000000000000000000000"
          "000000000000000000000000000000000000000000000000000000000000")),
      GCM::Polynomial(1)});

  GCM::CantorZassenhaus::Polynomial factor_b(std::vector<GCM::Polynomial>{
      GCM::Polynomial(std::bitset<128>(
          "10011111010101110010010010100010001001010111001001100010100110000000"
          "000000000000000000000000000000000000000000011111101110100000")),
      GCM::Polynomial(std::bitset<128>(
          "01000000001001110011100100001011011001101111110100000000000000000000"
          "000000000000000000000000000000000000000000000001100000010111")),
      GCM::Polynomial(std::bitset<128>(
          "01101000111000001011010101111011000000000000000000000000000000000000"
          "000000000000000000000000000000000000000000000000000000000000")),
      GCM::Polynomial(1)});

  CHECK(factor_a * factor_b == product);
  auto [quotient, remainder] = product.divmod(factor_a);
  CHECK(quotient == factor_b);
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
  auto res = x.pow(std::bitset<20>(1000000), mod);
  CHECK(res ==
        GCM::CantorZassenhaus::Polynomial(std::vector<GCM::Polynomial>()));
}
#endif