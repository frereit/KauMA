#include <botan/hex.h>
#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

#include "actions.hpp"
#include "cppcodec/base64_rfc4648.hpp"
#include "gcm/encryptor.hpp"
#include "gcm/recover.hpp"

using json = nlohmann::json;

json Actions::gcm_recover(const json &input) {
  json raw_msg1 = input["msg1"].get<json>();
  json raw_msg2 = input["msg2"].get<json>();
  json raw_msg3 = input["msg3"].get<json>();
  GCM::EncryptionResult msg1 = {
      cppcodec::base64_rfc4648::decode(
          raw_msg1["ciphertext"].get<std::string>()),
      cppcodec::base64_rfc4648::decode(
          raw_msg1["associated_data"].get<std::string>()),
      cppcodec::base64_rfc4648::decode(raw_msg1["auth_tag"].get<std::string>()),
  };
  GCM::EncryptionResult msg2 = {
      cppcodec::base64_rfc4648::decode(
          raw_msg2["ciphertext"].get<std::string>()),
      cppcodec::base64_rfc4648::decode(
          raw_msg2["associated_data"].get<std::string>()),
      cppcodec::base64_rfc4648::decode(raw_msg2["auth_tag"].get<std::string>()),
  };
  GCM::EncryptionResult msg3 = {
      cppcodec::base64_rfc4648::decode(
          raw_msg3["ciphertext"].get<std::string>()),
      cppcodec::base64_rfc4648::decode(
          raw_msg3["associated_data"].get<std::string>()),
      std::vector<std::uint8_t>()};
  std::vector<std::uint8_t> auth_tag =
      GCM::Recovery::recover_auth_tag(msg1, msg2, msg3);
  return json({{"msg3_tag", cppcodec::base64_rfc4648::encode(auth_tag)}});
}