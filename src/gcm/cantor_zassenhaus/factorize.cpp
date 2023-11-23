#include <cassert>

#include "gcm/cantor_zassenhaus/factorize.hpp"

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