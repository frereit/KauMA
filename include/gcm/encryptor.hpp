#pragma once
#include <botan/block_cipher.h>
#include <cstdint>
#include <vector>

namespace GCM {

class Encryptor {
public:
  Encryptor(const std::unique_ptr<Botan::BlockCipher> cipher,
            const std::vector<std::uint8_t> &nonce);

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

  std::vector<std::uint8_t> gen_ctr_block(std::uint32_t ctr);
  std::vector<std::uint8_t> encrypt(std::vector<std::uint8_t> plaintext);
  std::vector<std::uint8_t>
  authenticate(std::vector<std::uint8_t> ciphertext,
               std::vector<std::uint8_t> associated_data);
  std::vector<std::uint8_t> ghash(std::vector<std::uint8_t> ciphertext,
                                  std::vector<std::uint8_t> associated_data,
                                  std::vector<std::uint8_t> key);

  const std::unique_ptr<Botan::BlockCipher> m_cipher;
  const std::vector<std::uint8_t> &m_nonce;
};

} // namespace GCM