#include <cassert>
#include <cstdint>
#include <deque>
#include <stdexcept>
#include <vector>

#include "bytemanipulation.hpp"
#include "gcm/ghash.hpp"
#include "gcm/polynomial.hpp"

GCM::GHASH::GHASH(std::vector<std::uint8_t> associated_data,
                  std::vector<std::uint8_t> auth_key)
    : m_auth_tag(GCM::Polynomial::zero()),
      m_auth_key(GCM::Polynomial::from_gcm_bytes(auth_key)),
      m_associated_data_bitlength(associated_data.size() * 8),
      m_ciphertext_bitlength(0),
      m_ciphertext_buffer(std::deque<std::uint8_t>(0)), m_finalized(false) {
  // Add associated data to the beginning of the "stream", padded with 0s
  this->update(associated_data);
  this->pad_current_block();

  // Set the ciphertext bitlength back to 0. This was updated by the call to
  // "update".
  this->m_ciphertext_bitlength = 0;
}

void GCM::GHASH::update(std::vector<std::uint8_t> ciphertext) {
  if (m_finalized) {
    throw std::runtime_error("Cannot update finalized GHASH.");
  }
  m_ciphertext_buffer.insert(m_ciphertext_buffer.end(), ciphertext.begin(),
                             ciphertext.end());
  while (m_ciphertext_buffer.size() >= GCM::GHASH::BLOCK_SIZE) {
    std::vector<std::uint8_t> block(m_ciphertext_buffer.begin(),
                                    m_ciphertext_buffer.begin() +
                                        GCM::GHASH::BLOCK_SIZE);
    m_ciphertext_buffer.erase(m_ciphertext_buffer.begin(),
                              m_ciphertext_buffer.begin() +
                                  GCM::GHASH::BLOCK_SIZE);
    m_auth_tag += GCM::Polynomial::from_gcm_bytes(block);
    m_auth_tag *= m_auth_key;
    m_ciphertext_bitlength += block.size() * 8;
  }
}

std::vector<std::uint8_t> GCM::GHASH::finalize() {
  // Pad the remaining ciphertext
  this->pad_current_block();

  // Create the length field
  std::vector<std::uint8_t> length;
  ByteManipulation::append_as_bytes<std::uint64_t>(m_associated_data_bitlength,
                                                   std::endian::big, length);
  ByteManipulation::append_as_bytes<std::uint64_t>(m_ciphertext_bitlength,
                                                   std::endian::big, length);
  this->update(length);
  this->m_finalized = true;
  return m_auth_tag.to_gcm_bytes();
}

void GCM::GHASH::pad_current_block() {
  std::uint64_t padding_length =
      GCM::GHASH::BLOCK_SIZE - m_ciphertext_buffer.size();
  if (padding_length == GCM::GHASH::BLOCK_SIZE)
    return;
  this->update(std::vector<std::uint8_t>(padding_length));
  m_ciphertext_bitlength -= padding_length * 8;
}

std::vector<std::uint8_t> GCM::ghash(std::vector<std::uint8_t> ciphertext,
                                     std::vector<std::uint8_t> associated_data,
                                     std::vector<std::uint8_t> key) {
  GCM::GHASH hasher = GCM::GHASH(associated_data, key);
  hasher.update(ciphertext);
  return hasher.finalize();
}
