#include <botan/hex.h>
#include <nlohmann/json.hpp>
#include <string>

#include "actions.hpp"
#include "cppcodec/base64_rfc4648.hpp"
#include "gcm/cantor_zassenhaus/factorize.hpp"

using json = nlohmann::json;

json Actions::cantor_zassenhaus(const json &input) {
  std::vector<std::string> f_coeffs =
      input["f"].get<std::vector<std::string>>();
  std::vector<GCM::Polynomial> coefficients;
  for (std::size_t i = 0; i < f_coeffs.size(); ++i) {
    coefficients.push_back(
        GCM::Polynomial::from_gcm_bytes(Botan::hex_decode(f_coeffs.at(i))));
  }
  GCM::CantorZassenhaus::Polynomial f(coefficients);
  std::vector<GCM::Polynomial> zeros = GCM::CantorZassenhaus::zeros(f);
  std::vector<std::string> output;
  for (std::size_t i = 0; i < zeros.size(); ++i) {
    output.push_back(Botan::hex_encode(zeros.at(i).to_gcm_bytes()));
  }
  return json({{"zeros", output}});
}