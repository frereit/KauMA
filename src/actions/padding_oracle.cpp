#include <nlohmann/json.hpp>

#include "actions.hpp"
#include "cppcodec/base64_rfc4648.hpp"
#include "padding_oracle/attack.hpp"
#include "padding_oracle/server.hpp"

using json = nlohmann::json;

json Actions::padding_oracle_server(const json &input) {
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
}

json Actions::padding_oracle_attack(const json &input) {
  PaddingOracle::block iv;
  std::vector<std::uint8_t> raw_iv =
      cppcodec::base64_rfc4648::decode(input["iv"].get<std::string>());
  if (raw_iv.size() != iv.size()) {
    throw std::runtime_error("Invalid iv size!");
  }
  std::copy_n(raw_iv.begin(), iv.size(), iv.begin());
  std::vector<std::uint8_t> ciphertext =
      cppcodec::base64_rfc4648::decode(input["ciphertext"].get<std::string>());
  std::string hostname = input["hostname"].get<std::string>();
  std::uint16_t port = input["port"].get<std::uint16_t>();
  PaddingOracle::Attack attack{hostname, port};
  std::vector<std::uint8_t> plaintext =
      attack.recover_plaintext(iv, ciphertext);
  return json({{"plaintext", cppcodec::base64_rfc4648::encode(plaintext)}});
}