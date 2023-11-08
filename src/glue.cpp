#include <algorithm>
#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

#include "bytenigma.hpp"
#include "cppcodec/base64_rfc4648.hpp"
#include "glue.hpp"
#include "noop.hpp"
#include "padding_oracle/attack.hpp"
#include "padding_oracle/server.hpp"

using json = nlohmann::json;

/// @brief the glue_functions generate the appropriate function arguments from
/// the JSON structure and format the return value as appropriate JSON
extern const std::map<std::string, Glue::glue_function> Glue::ACTIONS = {
    {"noop",
     [](const json &input) {
       std::vector<std::uint8_t> raw_input =
           cppcodec::base64_rfc4648::decode(input["input"].get<std::string>());

       std::vector<std::uint8_t> raw_output = Noop::noop(raw_input);

       json output = {{"output", cppcodec::base64_rfc4648::encode(raw_output)}};
       return output;
     }},
    {"bytenigma",
     [](const json &input) {
       std::vector<std::uint8_t> raw_input =
           cppcodec::base64_rfc4648::decode(input["input"].get<std::string>());
       auto rotors =
           input["rotors"].get<std::vector<std::vector<std::uint8_t>>>();
       Bytenigma::Bytenigma bytenigma = Bytenigma::Bytenigma(rotors);
       std::vector<std::uint8_t> raw_output =
           bytenigma.process_bytes(raw_input);
       json output = {{"output", cppcodec::base64_rfc4648::encode(raw_output)}};
       return output;
     }},
    {"padding-oracle-server",
     [](const json &input) {
       PaddingOracle::block key;
       std::vector<std::uint8_t> raw_key =
           cppcodec::base64_rfc4648::decode(input["key"].get<std::string>());
       if (raw_key.size() != PaddingOracle::BLOCK_SIZE) {
         throw std::runtime_error("Invalid key size!");
       }
       std::copy_n(raw_key.begin(), PaddingOracle::BLOCK_SIZE, key.begin());
       std::uint16_t port = input["port"].get<std::uint16_t>();
       PaddingOracle::XORCBCServer server{key, port};
       server.loop_forever();
       return json();
     }},
    {"padding-oracle-attack", [](const json &input) {
       PaddingOracle::block iv;
       std::vector<std::uint8_t> raw_iv =
           cppcodec::base64_rfc4648::decode(input["iv"].get<std::string>());
       if (raw_iv.size() != iv.size()) {
         throw std::runtime_error("Invalid iv size!");
       }
       std::copy_n(raw_iv.begin(), iv.size(), iv.begin());
       std::vector<std::uint8_t> ciphertext = cppcodec::base64_rfc4648::decode(
           input["ciphertext"].get<std::string>());
       std::string hostname = input["hostname"].get<std::string>();
       std::uint16_t port = input["port"].get<std::uint16_t>();
       PaddingOracle::Attack attack{hostname, port};
       std::vector<std::uint8_t> plaintext =
           attack.recover_plaintext(iv, ciphertext);
       return json({{"plaintext", cppcodec::base64_rfc4648::encode(plaintext)}});
     }}};

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