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
} // namespace Actions