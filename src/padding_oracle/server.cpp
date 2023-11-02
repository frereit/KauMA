#include <endian.h>
#include <iostream>

#include "padding_oracle/server.hpp"

int PaddingOracle::XORCBCServer::handle_client(TCP::Communicator client) {
  PaddingOracle::block ciphertext =
      client.read_raw<PaddingOracle::BLOCK_SIZE>();

  while (true) {
    auto count = client.read_num<std::uint16_t>(std::endian::little);

    if (count == 0) {
      break;
    }

    for (std::uint16_t i = 0; i < count; ++i) {
      PaddingOracle::block q = client.read_raw<PaddingOracle::BLOCK_SIZE>();

      // Decrypt
      PaddingOracle::block cleartext = this->decrypt(q, ciphertext);

      // Validate padding
      bool valid = this->validate_padding(cleartext);
      client.write_num(static_cast<std::uint8_t>(valid), std::endian::little);
    }
  }
  return 0;
}

PaddingOracle::block
PaddingOracle::XORCBCServer::decrypt(const PaddingOracle::block &q,
                                     const PaddingOracle::block &ciphertext) {
  PaddingOracle::block cleartext;
  for (std::size_t j = 0; j < sizeof(q); ++j) {
    cleartext[j] = q[j] ^ ciphertext[j] ^ this->m_key[j];
  }
  return cleartext;
}

bool PaddingOracle::XORCBCServer::validate_padding(
    const PaddingOracle::block &block) {
  std::uint8_t padding_count = block[PaddingOracle::BLOCK_SIZE - 1];
  std::uint8_t valid = padding_count > 0;
  for (std::size_t j = 0; j < padding_count; ++j) {
    if (block[PaddingOracle::BLOCK_SIZE - 1 - j] != padding_count) {
      valid = 0;
    }
  }
  return valid;
}
