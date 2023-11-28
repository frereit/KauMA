#include <cassert>
#include <cstdint>
#include <iostream>
#include <vector>

#include "gcm/cantor_zassenhaus/factorize.hpp"

std::vector<GCM::Polynomial>
GCM::CantorZassenhaus::zeros(GCM::CantorZassenhaus::Polynomial x) {
  x.ensure_monic();
  std::cerr << "Finding zeros for " << x << "\n";
  std::vector<GCM::CantorZassenhaus::Polynomial> factors{};
  std::vector<GCM::CantorZassenhaus::Polynomial> new_factors{x};
  std::vector<GCM::CantorZassenhaus::Polynomial> final_factors{};
  while (new_factors.size() != 0) {
    factors = new_factors;
    new_factors.clear();
    for (std::size_t i = 0; i < factors.size(); ++i) {
      if (factors.at(i).degree() == 1) {
        final_factors.push_back(factors.at(i));
        continue;
      }
      std::vector<GCM::CantorZassenhaus::Polynomial> subfactors;
      while ((subfactors = cantor_zassenhaus(x, factors.at(i))).size() != 2)
        ;
      new_factors.push_back(subfactors.at(0));
      new_factors.push_back(subfactors.at(1));
    }
  }

  std::vector<GCM::Polynomial> zeros;
  for (std::size_t i = 0; i < final_factors.size(); ++i) {
    assert(final_factors.at(i).degree() == 1);
    std::cerr << "Zero at " << final_factors.at(i).coefficient(0) << "\n";
    zeros.push_back(final_factors.at(i).coefficient(0));
  }
  return zeros;
}

std::vector<GCM::CantorZassenhaus::Polynomial>
GCM::CantorZassenhaus::cantor_zassenhaus(GCM::CantorZassenhaus::Polynomial f,
                                         GCM::CantorZassenhaus::Polynomial p) {
  std::cerr << "CZ(" << f << ", " << p << ")\n";

  GCM::CantorZassenhaus::Polynomial h =
      GCM::CantorZassenhaus::Polynomial::random(f.degree() - 1);
  std::cerr << "\th = " << h << "\n";

  GCM::CantorZassenhaus::Polynomial g =
      // This 127 bit number is (2^128 - 1) / 3.
      h.pow(std::bitset<127>(
                "10101010101010101010101010101010101010101010101010101010101010"
                "10101"
                "010101010101010101010101010101010101010101010101010101010101"),
            f) -
      GCM::CantorZassenhaus::Polynomial({GCM::Polynomial(1)});
  std::cerr << "\tg = " << g << "\n";

  GCM::CantorZassenhaus::Polynomial q = GCM::CantorZassenhaus::gcd(p, g);
  q.ensure_monic();
  std::cerr << "\tq = " << q << "\n";

  p.ensure_monic();
  if (q != GCM::CantorZassenhaus::Polynomial({GCM::Polynomial(1)}) && q != p) {
    GCM::CantorZassenhaus::Polynomial k1 = q;
    auto [k2, remainder] = p.divmod(q);
    k2.ensure_monic();
    assert(remainder == GCM::CantorZassenhaus::Polynomial({}) &&
           "Expected no remainder dividing by gcd.");
    assert(k1 * k2 == p && "Found factors do not multiply to the polynomial");
    std::cerr << "\tk1 = " << k1 << "\n";
    std::cerr << "\tk2 = " << k2 << "\n";
    std::cerr << "\tCZ success.\n";
    return {k1, k2};
  }
  std::cerr << "\tCZ failure.\n";
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