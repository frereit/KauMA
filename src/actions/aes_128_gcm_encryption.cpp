#include <botan/block_cipher.h>
#include <nlohmann/json.hpp>
#include <vector>
#include <ranges>

#include "actions.hpp"
#include "cppcodec/base64_rfc4648.hpp"
#include "gcm/encryptor.hpp"

using json = nlohmann::json;

json Actions::aes_128_gcm_encrypt(const json &input) {
  std::vector<std::uint8_t> key =
      cppcodec::base64_rfc4648::decode(input["key"].get<std::string>());
  std::vector<std::uint8_t> nonce =
      cppcodec::base64_rfc4648::decode(input["nonce"].get<std::string>());
  std::vector<std::uint8_t> associated_data = cppcodec::base64_rfc4648::decode(
      input["associated_data"].get<std::string>());
  std::vector<std::uint8_t> plaintext =
      cppcodec::base64_rfc4648::decode(input["plaintext"].get<std::string>());

  auto aes = Botan::BlockCipher::create_or_throw("AES-128");
  aes->set_key(key);
  GCM::Encryptor e = GCM::Encryptor(associated_data, std::move(aes), nonce);
  std::vector<std::uint8_t> ciphertext = e.update(plaintext);
  std::vector<std::uint8_t> auth_tag = e.finalize();

  return json({{"ciphertext", cppcodec::base64_rfc4648::encode(ciphertext)},
               {"auth_tag", cppcodec::base64_rfc4648::encode(auth_tag)},
               {"Y0", cppcodec::base64_rfc4648::encode(e.y0())},
               {"H", cppcodec::base64_rfc4648::encode(e.h())}});
}
