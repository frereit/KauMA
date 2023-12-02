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
    {"gcm-encrypt", Actions::aes_128_gcm_encrypt},
    {"cantor-zassenhaus", Actions::cantor_zassenhaus},
    {"gcm-recover", Actions::gcm_recover},
    {"gcm-poly-add", Actions::gcm_poly_add},
    {"gcm-poly-mul", Actions::gcm_poly_mul},
    {"gcm-poly-gcd", Actions::gcm_poly_gcd},
    {"gcm-poly-div", Actions::gcm_poly_div},
    {"gcm-poly-mod", Actions::gcm_poly_mod},
    {"gcm-poly-pow", Actions::gcm_poly_pow},
    {"gcm-poly-powmod", Actions::gcm_poly_powmod}};

nlohmann::json execute_action(const nlohmann::json &input);
} // namespace Glue