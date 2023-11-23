#include <cassert>

#include "gcm/cantor_zassenhaus/factorize.hpp"

std::vector<GCM::Polynomial>
GCM::CantorZassenhaus::zeros(GCM::CantorZassenhaus::Polynomial x) {
  x.ensure_monic();
  std::vector<GCM::CantorZassenhaus::Polynomial> factors{x};
  bool found_new_factors = true;
  while (found_new_factors) {
    found_new_factors = false;
    std::vector<std::size_t> to_remove{};
    std::size_t size = factors.size();
    for (std::size_t i = 0; i < size; ++i) {
      if (factors.at(i).degree() == 1) {
        continue;
      }
      std::vector<GCM::CantorZassenhaus::Polynomial> subfactors;
      while ((subfactors = cantor_zassenhaus(x, factors.at(i))).size() != 2)
        ;
      to_remove.push_back(i);
      factors.push_back(subfactors.at(0));
      factors.push_back(subfactors.at(1));
      found_new_factors = true;
    }
    for (const std::size_t &i : to_remove) {
      factors.erase(factors.begin() + i);
    }
  }

  std::vector<GCM::Polynomial> zeros;
  for (std::size_t i = 0; i < factors.size(); ++i) {
    assert(factors.at(i).degree() == 1);
    zeros.push_back(factors.at(i).coefficient(0));
  }
  return zeros;
}

std::vector<GCM::CantorZassenhaus::Polynomial>
GCM::CantorZassenhaus::cantor_zassenhaus(GCM::CantorZassenhaus::Polynomial f,
                                         GCM::CantorZassenhaus::Polynomial p) {
  GCM::CantorZassenhaus::Polynomial h =
      GCM::CantorZassenhaus::Polynomial::random(f.degree() - 1);
  GCM::CantorZassenhaus::Polynomial g =
      // This 127 bit number is (2^128 - 1) / 3.
      h.pow(std::bitset<127>(
                "10101010101010101010101010101010101010101010101010101010101010"
                "10101"
                "010101010101010101010101010101010101010101010101010101010101"),
            f) -
      GCM::CantorZassenhaus::Polynomial({GCM::Polynomial(1)});
  GCM::CantorZassenhaus::Polynomial q = GCM::CantorZassenhaus::gcd(p, g);
  q.ensure_monic();
  p.ensure_monic();
  if (q != GCM::CantorZassenhaus::Polynomial({GCM::Polynomial(1)}) && q != p) {
    GCM::CantorZassenhaus::Polynomial k1 = q;
    auto [k2, remainder] = p.divmod(q);
    k2.ensure_monic();
    assert(remainder == GCM::CantorZassenhaus::Polynomial({}) &&
           "Expected no remainder dividing by gcd.");
    assert(k1 * k2 == p && "Found factors do not multiply to the polynomial");
    return {k1, k2};
  }
  return std::vector<GCM::CantorZassenhaus::Polynomial>();
}

GCM::CantorZassenhaus::Polynomial
GCM::CantorZassenhaus::gcd(GCM::CantorZassenhaus::Polynomial a,
                           GCM::CantorZassenhaus::Polynomial b) {
  while (!b.empty()) {
    auto [_quotient, remainder] = a.divmod(b);
    a = b;
    b = remainder;
  }
  return a;
}