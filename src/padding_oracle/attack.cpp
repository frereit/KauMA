#include <cstdint>
#include <ranges>
#include <stdexcept>
#include <vector>

#include "padding_oracle/attack.hpp"
#include "padding_oracle/server.hpp"

std::vector<std::uint8_t> PaddingOracle::Attack::recover_plaintext(
    const PaddingOracle::block &iv,
    const std::vector<std::uint8_t> &ciphertext) {
  if (ciphertext.size() == 0 ||
      ciphertext.size() % PaddingOracle::BLOCK_SIZE != 0) {
    throw std::runtime_error(
        "Invalid ciphertext length! Must be a multiple of BLOCK_SIZE");
  }

  std::vector<std::uint8_t> cleartext;

  // FIXME: Once C++23 lands, use std::ranges::views::chunk
  PaddingOracle::block previous = iv, block;

  for (std::size_t i = 0; i < ciphertext.size(); i += BLOCK_SIZE) {
    std::copy_n(ciphertext.begin() + i, block.size(), block.begin());
    PaddingOracle::block recovered = this->recover_block(previous, block);
    cleartext.insert(cleartext.end(), recovered.begin(), recovered.end());
    previous = block;
  }
  return cleartext;
}

PaddingOracle::block
PaddingOracle::Attack::recover_block(const PaddingOracle::block &prev,
                                     const PaddingOracle::block &block) {
  PaddingOracle::block recovered = this->recover_raw_block(block);
  // Apply CBC
  for (std::size_t i = 0; i < prev.size(); ++i) {
    recovered.at(i) ^= prev.at(i);
  }
  return recovered;
}

PaddingOracle::block
PaddingOracle::Attack::recover_raw_block(const PaddingOracle::block &block) {
  TCP::Communicator target = TCP::Communicator(this->m_hostname, this->m_port);
  target.write_raw(block);

  PaddingOracle::block raw_recovered = PaddingOracle::block();
  for (std::size_t i : std::views::iota(0u, 16u) | std::views::reverse) {
    raw_recovered.at(i) = this->recover_byte(target, i, raw_recovered);
  }
  target.write_num<std::uint16_t>(0, std::endian::little);

  return raw_recovered;
}

std::uint8_t
PaddingOracle::Attack::recover_byte(TCP::Communicator &target,
                                    std::size_t index,
                                    PaddingOracle::block raw_recovered) {
  // 1. We are going to send 256 bytes
  target.write_num<std::uint16_t>(256, std::endian::little);

  // Initialize with zeros
  PaddingOracle::block payload = PaddingOracle::block();

  // Modify all bytes after the index to be correct padding
  const std::uint8_t padding_byte = PaddingOracle::BLOCK_SIZE - index;
  for (std::size_t i : std::ranges::views::iota(index + 1, payload.size())) {
    payload.at(i) = padding_byte ^ raw_recovered.at(i);
  }

  // Send all candidate bytes for index
  for (std::uint8_t candidate : std::ranges::views::iota(0u, 256u)) {
    payload.at(index) = candidate;
    target.write_raw(payload);
  }

  std::array<std::uint8_t, 256> responses = target.read_raw<256>();

  // Find out which candidate byte caused correct padding
  for (std::uint8_t candidate : std::ranges::views::iota(0u, 256u)) {
    bool valid_padding = responses.at(candidate) == 1;
    // Special case: We need to validate that the padding is correct for the
    // last byte
    if (valid_padding && padding_byte == 1) {
      PaddingOracle::block validator;
      for (std::size_t i :
           std::ranges::views::iota(0u, PaddingOracle::BLOCK_SIZE)) {
        validator.at(i) = 0xff;
      }
      validator.back() = candidate;
      target.write_num<std::uint16_t>(1, std::endian::little);
      target.write_raw(validator);
      valid_padding = target.read_num<std::uint8_t>(std::endian::little);
    }
    if (valid_padding) {
      return candidate ^ padding_byte;
    }
  }
  throw std::runtime_error("Failed to find any valid byte");
}