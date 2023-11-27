#pragma once
#include <botan/block_cipher.h>
#include <cstdint>
#include <vector>

#include "gcm/ghash.hpp"

namespace GCM {

/// @brief A ciphertext and associated auth tag
struct EncryptionResult {
  const std::vector<std::uint8_t> ciphertext;
  const std::vector<std::uint8_t> associated_data;
  const std::vector<std::uint8_t> auth_tag;
};

class Encryptor {
public:
  /// @brief start a new encryption process
  /// @param associated_data the associated data for the encryption
  /// @param cipher the block cipher for CTR mode
  /// @param nonce the nonce to use for CTR mode
  Encryptor(std::vector<std::uint8_t> associated_data,
            const std::unique_ptr<Botan::BlockCipher> cipher,
            const std::vector<std::uint8_t> &nonce);

  /// @brief encrypt a given plaintext and update the internal state
  /// @param plaintext the plaintext to encrypt
  /// @return the ciphertext correspnding to the plaintext
  std::vector<std::uint8_t> update(std::vector<std::uint8_t> plaintext);

  /// @brief compute the auth tag for the associated data and all encrypted
  /// plaintext
  /// @return the auth tag
  std::vector<std::uint8_t> finalize();

  /// @brief generate the block used to generate the auth tag mask
  /// @return the block \f$Y_0 = \mathrm{Nonce} || \mathrm{Ctr 1}\f$
  /// @note This is only exposed because it is a required output for the
  /// assignment. Exposing internal primitives to the consumer is usually a bad
  /// idea because it leads allows for incorrect usage of the Encryptor class.
  std::vector<std::uint8_t> y0();

  /// @brief generate the auth key
  /// @return the auth key block \f$H = E(0)\f$
  /// @note This is only exposed because it is a required output for the
  /// assignment. Exposing internal primitives to the consumer is usually a bad
  /// idea because it leads allows for incorrect usage of the Encryptor class.
  std::vector<std::uint8_t> h();

#ifndef TEST
private:
#endif

  std::vector<std::uint8_t> y_block(std::uint32_t ctr);

  const std::unique_ptr<Botan::BlockCipher> m_cipher;
  std::vector<std::uint8_t> m_y0;
  std::uint32_t m_y;
  std::vector<std::uint8_t> m_keystream;
  GCM::GHASH m_hasher;
};

} // namespace GCM