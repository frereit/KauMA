#pragma once
#include <vector>

#include "gcm/cantor_zassenhaus/polynomial.hpp"

namespace GCM::CantorZassenhaus {
/// @brief find solutions for the equation p = 0.
/// @param p the polynomial to find zeros for
/// @return the zeros of the given polynomial (as GCM::Polynomials).
std::vector<GCM::Polynomial> zeros(GCM::CantorZassenhaus::Polynomial p);

/// @brief Attempt to find two factors for p in the given polynomial ring f
/// @param f the reduction polynomial
/// @param p the polynomial to factorize
/// @return a vector of two elements if factors were found, an empty vector
/// otherwise.
std::vector<GCM::CantorZassenhaus::Polynomial>
cantor_zassenhaus(GCM::CantorZassenhaus::Polynomial f,
                  GCM::CantorZassenhaus::Polynomial p);

GCM::CantorZassenhaus::Polynomial gcd(GCM::CantorZassenhaus::Polynomial a,
                                      GCM::CantorZassenhaus::Polynomial b);
} // namespace GCM::CantorZassenhaus