#pragma once
#include <nlohmann/json.hpp>

#include "actions.hpp"

namespace Glue {
typedef std::function<nlohmann::json(const nlohmann::json &)> glue_function;

const std::map<std::string, Glue::glue_function> ACTIONS = {
    {"noop", Actions::noop},
    {"bytenigma", Actions::bytenigma},
    {"padding-oracle-server", Actions::padding_oracle_server},
    {"padding-oracle-attack", Actions::padding_oracle_attack},
    {"gcm-block2poly", Actions::gcm_block2poly},
    {"gcm-poly2block", Actions::gcm_poly2block},
    {"gcm-clmul", Actions::gcm_clmul},
    {"gcm-encrypt", Actions::aes_128_gcm_encrypt}};

nlohmann::json execute_action(const nlohmann::json &input);
} // namespace Glue