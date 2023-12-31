#pragma once
#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace Actions {
json noop(const json &input);
json bytenigma(const json &input);
json padding_oracle_server(const json &input);
json padding_oracle_attack(const json &input);
json gcm_block2poly(const json &input);
json gcm_poly2block(const json &input);
json gcm_clmul(const json &input);
json aes_128_gcm_encrypt(const json &input);
json cantor_zassenhaus(const json &input);
json gcm_recover(const json &input);
json gcm_poly_add(const json &input);
json gcm_poly_mul(const json &input);
json gcm_poly_gcd(const json &input);
json gcm_poly_div(const json &input);
json gcm_poly_mod(const json &input);
json gcm_poly_pow(const json &input);
json gcm_poly_powmod(const json &input);
} // namespace Actions