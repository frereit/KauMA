#include <emmintrin.h>
#include <nlohmann/json.hpp>
#include <smmintrin.h>
#include <stdexcept>
#include <vector>

#include "actions.hpp"
#include "cppcodec/base64_rfc4648.hpp"
#include "gcm/cantor_zassenhaus/factorize.hpp"
#include "gcm/cantor_zassenhaus/polynomial.hpp"
#include "gcm/polynomial.hpp"

using json = nlohmann::json;

json Actions::gcm_block2poly(const json &input) {
  std::vector<std::uint8_t> gcm_bytes =
      cppcodec::base64_rfc4648::decode(input["block"].get<std::string>());
  auto poly = GCM::Polynomial::from_gcm_bytes(gcm_bytes);
  return json({{"exponents", poly.to_exponents()}});
}

json Actions::gcm_poly2block(const json &input) {
  std::vector<std::uint8_t> exponents =
      input["exponents"].get<std::vector<std::uint8_t>>();
  auto poly = GCM::Polynomial::from_exponents(exponents);
  return json(
      {{"block", cppcodec::base64_rfc4648::encode(poly.to_gcm_bytes())}});
}

json Actions::gcm_clmul(const json &input) {
  std::vector<std::uint8_t> a_bytes =
      cppcodec::base64_rfc4648::decode(input["a"].get<std::string>());
  std::vector<std::uint8_t> b_bytes =
      cppcodec::base64_rfc4648::decode(input["b"].get<std::string>());

  auto a = GCM::Polynomial::from_gcm_bytes(a_bytes);
  auto b = GCM::Polynomial::from_gcm_bytes(b_bytes);

  return json({{"a_times_b",
                cppcodec::base64_rfc4648::encode((a * b).to_gcm_bytes())}});
}

json Actions::gcm_poly_add(const json &input) {
  auto a = GCM::CantorZassenhaus::Polynomial::from_json(input["a"]);
  auto b = GCM::CantorZassenhaus::Polynomial::from_json(input["b"]);

  return json({{"result", (a + b).to_json()}});
}

json Actions::gcm_poly_mul(const json &input) {
  auto a = GCM::CantorZassenhaus::Polynomial::from_json(input["a"]);
  auto b = GCM::CantorZassenhaus::Polynomial::from_json(input["b"]);

  return json({{"result", (a * b).to_json()}});
}

json Actions::gcm_poly_gcd(const json &input) {
  auto a = GCM::CantorZassenhaus::Polynomial::from_json(input["a"]);
  auto b = GCM::CantorZassenhaus::Polynomial::from_json(input["b"]);
  auto gcd = GCM::CantorZassenhaus::gcd(a, b);
  gcd.ensure_monic();
  return json({{"result", gcd.to_json()}});
}

json Actions::gcm_poly_div(const json &input) {
  auto a = GCM::CantorZassenhaus::Polynomial::from_json(input["a"]);
  auto b = GCM::CantorZassenhaus::Polynomial::from_json(input["b"]);
  auto [q, _r] = a.divmod(b);
  return json({{"result", q.to_json()}});
}

json Actions::gcm_poly_mod(const json &input) {
  auto a = GCM::CantorZassenhaus::Polynomial::from_json(input["a"]);
  auto b = GCM::CantorZassenhaus::Polynomial::from_json(input["b"]);
  auto [_q, r] = a.divmod(b);
  return json({{"result", r.to_json()}});
}

json Actions::gcm_poly_pow(const json &input) {
  auto base = GCM::CantorZassenhaus::Polynomial::from_json(input["base"]);
  auto out = GCM::CantorZassenhaus::Polynomial({GCM::Polynomial::one()});
  if (!input["exponent"].is_number_unsigned()) {
    throw std::runtime_error(
        "Exponent is not an unsigned integer. Is it too large?\n");
  }
  std::uint64_t exponent = input["exponent"].get<std::uint64_t>();
  std::cerr << "Exponent: " << exponent << "\n";
  while (exponent > 0) {
    if (exponent & 1) {
      out *= base;
    }
    exponent >>= 1;
    base *= base;
  }
  return json({{"result", out.to_json()}});
}

json Actions::gcm_poly_powmod(const json &input) {
  auto base = GCM::CantorZassenhaus::Polynomial::from_json(input["base"]);
  auto modulo = GCM::CantorZassenhaus::Polynomial::from_json(input["modulo"]);
  if (!input["exponent"].is_number_unsigned()) {
    throw std::runtime_error(
        "Exponent is not an unsigned integer. Is it too large?\n");
  }
  std::uint64_t exponent_low = input["exponent"].get<std::uint64_t>();
  __m128i exponent =
      _mm_setr_epi64(_mm_set_pi64x(exponent_low), _mm_set_pi64x(0));
  return json({{"result", (base.pow(exponent, modulo)).to_json()}});
}