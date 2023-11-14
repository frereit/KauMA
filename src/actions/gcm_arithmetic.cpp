#include <nlohmann/json.hpp>
#include <vector>

#include "actions.hpp"
#include "cppcodec/base64_rfc4648.hpp"
#include "f128_gcm/polynomial.hpp"

using json = nlohmann::json;

json Actions::gcm_block2poly(const json &input) {
  std::vector<std::uint8_t> gcm_bytes =
      cppcodec::base64_rfc4648::decode(input["block"].get<std::string>());
  auto poly = F128_GCM::Polynomial::from_gcm_bytes(gcm_bytes);
  return json({{"exponents", poly.to_exponents()}});
}

json Actions::gcm_poly2block(const json &input) {
  std::vector<std::uint8_t> exponents =
      input["exponents"].get<std::vector<std::uint8_t>>();
  auto poly = F128_GCM::Polynomial::from_exponents(exponents);
  return json(
      {{"block", cppcodec::base64_rfc4648::encode(poly.to_gcm_bytes())}});
}