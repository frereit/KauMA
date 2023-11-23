#pragma once
#include <vector>

#include "gcm/cantor_zassenhaus/polynomial.hpp"

namespace GCM::CantorZassenhaus {

GCM::CantorZassenhaus::Polynomial gcd(GCM::CantorZassenhaus::Polynomial a,
                                      GCM::CantorZassenhaus::Polynomial b);
} // namespace GCM::CantorZassenhaus