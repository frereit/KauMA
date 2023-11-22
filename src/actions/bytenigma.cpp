#include <nlohmann/json.hpp>

#include "actions.hpp"
#include "bytenigma.hpp"
#include "cppcodec/base64_rfc4648.hpp"

using json = nlohmann::json;

json Actions::bytenigma(const json &input) {
  std::vector<std::uint8_t> raw_input =
      cppcodec::base64_rfc4648::decode(input["input"].get<std::string>());
  auto rotors = input["rotors"].get<std::vector<std::vector<std::uint8_t>>>();
  Bytenigma::Bytenigma bytenigma = Bytenigma::Bytenigma(rotors);
  std::vector<std::uint8_t> raw_output = bytenigma.process_bytes(raw_input);
  json output = {{"output", cppcodec::base64_rfc4648::encode(raw_output)}};
  return output;
}