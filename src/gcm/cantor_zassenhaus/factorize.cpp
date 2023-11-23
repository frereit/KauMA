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

#ifdef TEST
#include <botan/hex.h>

#include "doctest.h"

GCM::Polynomial from_hex(std::string hex) {
  return GCM::Polynomial::from_gcm_bytes(Botan::hex_decode(hex));
}

TEST_CASE("test multiplication and divmod") {
  GCM::CantorZassenhaus::Polynomial f(
      {from_hex("7A9C3400001A584BB29B0A03B7971984"),
       from_hex("1B81C000000000A9D95C170026D05960"),
       from_hex("F43800000000000000C45E91CFDC121E"),
       from_hex("000000000000000000000000DE6DF8F8"),
       from_hex("80000000000000000000000000000000")});
  GCM::CantorZassenhaus::Polynomial a(
      {from_hex("00000000000000000000000000C0FFEE"),
       from_hex("80000000000000000000000000000000")});
  GCM::CantorZassenhaus::Polynomial b(
      {from_hex("05DF80000000000019464EA44524EAF9"),
       from_hex("E8180000000000000000BF66D09CE402"),
       from_hex("000000000000000000000000DEAD0716"),
       from_hex("80000000000000000000000000000000")});
  CHECK(a * b == f);

  auto [quotient, mod] = f.divmod(a);
  CHECK(quotient == b);
  CHECK(mod.empty());
}

TEST_CASE("test cantor zassenhaus") {
  GCM::CantorZassenhaus::Polynomial f(
      {from_hex("7A9C3400001A584BB29B0A03B7971984"),
       from_hex("1B81C000000000A9D95C170026D05960"),
       from_hex("F43800000000000000C45E91CFDC121E"),
       from_hex("000000000000000000000000DE6DF8F8"),
       from_hex("80000000000000000000000000000000")});

  std::vector<GCM::Polynomial> expected = {
      from_hex("000000000000000000000000DEADBEEF"),
      from_hex("0000000000000000000000000000ABCD"),
      from_hex("00000000000000000000000000001234"),
      from_hex("00000000000000000000000000C0FFEE")};

  std::vector<GCM::Polynomial> actual = GCM::CantorZassenhaus::zeros(f);
  CHECK(expected == actual);
}
#endif