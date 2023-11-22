#include <nlohmann/json.hpp>
#include <vector>

#include "actions.hpp"
#include "cppcodec/base64_rfc4648.hpp"
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