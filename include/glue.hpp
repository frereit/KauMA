#pragma once
#include <nlohmann/json.hpp>

namespace Glue {
typedef std::function<nlohmann::json(const nlohmann::json &)> glue_function;

extern const std::map<std::string, Glue::glue_function> ACTIONS;

nlohmann::json execute_action(const nlohmann::json &input);
} // namespace Glue