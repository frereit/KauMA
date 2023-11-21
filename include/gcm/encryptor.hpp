#pragma once
#include <botan/block_cipher.h>
#include <cstdint>
#include <vector>

namespace GCM {

class Encryptor {
public:
  Encryptor(const std::unique_ptr<Botan::BlockCipher> cipher,
            const std::vector<std::uint8_t> &nonce);


#ifndef TEST
private:
#endif

  std::vector<std::uint8_t> gen_ctr_block(std::uint32_t ctr);
  std::vector<std::uint8_t> encrypt(std::vector<std::uint8_t> plaintext);

  const std::unique_ptr<Botan::BlockCipher> m_cipher;
  const std::vector<std::uint8_t> &m_nonce;
};

} // namespace GCM