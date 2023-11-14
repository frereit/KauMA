#include <algorithm>
#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

#include "cppcodec/base64_rfc4648.hpp"
#include "glue.hpp"

using json = nlohmann::json;

/// @brief executes a specific action by parsing the arguments from the JSON and
/// calling the correct library function
/// @param input The labwork-docker JSON specification
/// @return the JSON resulting from the action
/// @throws std::out_of_range if the given action is not known
/// @throws std::runtime_error if the JSON does not conform to the
/// labwork-docker specification
json Glue::execute_action(const json &input) {
  Glue::glue_function func;
  try {
    func = Glue::ACTIONS.at(input["action"]);
  } catch (const std::out_of_range &ex) {
    throw std::out_of_range("Unknown action");
  }
  return func(input);
}