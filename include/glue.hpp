#pragma once
#include <nlohmann/json.hpp>

#include "actions.hpp"

namespace Glue {
typedef std::function<nlohmann::json(const nlohmann::json &)> glue_function;

const std::map<std::string, Glue::glue_function> ACTIONS = {
    {"noop", Actions::noop},
    {"bytenigma", Actions::bytenigma},
    {"padding-oracle-server", Actions::padding_oracle_server},
    {"padding-oracle-attack", Actions::padding_oracle_attack}};

nlohmann::json execute_action(const nlohmann::json &input);
} // namespace Glue