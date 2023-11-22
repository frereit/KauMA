#include <nlohmann/json.hpp>

#include "actions.hpp"
#include "cppcodec/base64_rfc4648.hpp"
#include "noop.hpp"

using json = nlohmann::json;

json Actions::noop(const json &input) {
  std::vector<std::uint8_t> raw_input =
      cppcodec::base64_rfc4648::decode(input["input"].get<std::string>());

  std::vector<std::uint8_t> raw_output = Noop::noop(raw_input);

  json output = {{"output", cppcodec::base64_rfc4648::encode(raw_output)}};
  return output;
}