#pragma once
#include <cstdint>
#include <vector>

namespace GCM {

class Encryptor {
public:
  Encryptor(const std::vector<std::uint8_t> &key,
            const std::vector<std::uint8_t> &nonce)
      : m_key(key), m_nonce(nonce) {}

#ifndef TEST
private:
#endif
  std::vector<std::uint8_t> gen_ctr_block(std::uint32_t ctr);

  const std::vector<std::uint8_t> &m_key;
  const std::vector<std::uint8_t> &m_nonce;
};

} // namespace GCM