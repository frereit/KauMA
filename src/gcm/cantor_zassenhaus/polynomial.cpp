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
