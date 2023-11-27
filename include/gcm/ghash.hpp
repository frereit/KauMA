#pragma once
#include <cstdint>
#include <deque>
#include <vector>

#include "gcm/polynomial.hpp"

namespace GCM {

/// @brief A streamable GHASH implementation
class GHASH {

  static constexpr std::uint64_t BLOCK_SIZE = 16;

public:
  /// @brief Start a new GHASH computation
  /// @param associated_data associated data must be known when beginning the
  /// computation
  /// @param auth_key the GHASH auth key
  GHASH(std::vector<std::uint8_t> associated_data,
        std::vector<std::uint8_t> auth_key);

  /// @brief add ciphertext to the GHASH
  /// @param ciphertext ciphertext of any length
  void update(std::vector<std::uint8_t> ciphertext);

  /// @brief finalize the computation and return the result
  /// @return the authentication tag for all inserted data
  std::vector<std::uint8_t> finalize();

private:
  /// @brief pad the current ciphertext block with zeros and add it to the tag
  void pad_current_block();

  /// @brief the current auth tag
  GCM::Polynomial m_auth_tag;

  const GCM::Polynomial m_auth_key;
  const std::uint64_t m_associated_data_bitlength;
  std::uint64_t m_ciphertext_bitlength;

  /// @brief the ciphertext buffer contains all "remaining" ciphertext data. Its
  /// length is always less than GCM::GHASH::BLOCK_SIZE. It is a std::deque
  /// because we insert at the end and remove from the beginning.
  std::deque<std::uint8_t> m_ciphertext_buffer;
  bool m_finalized;
};

/// @brief compute an auth tag for a given ciphertext
/// @param ciphertext the ciphertext
/// @param associated_data the associated data
/// @param key the auth key to use
/// @return the auth tag
/// @note this is a convience function. For streamable hashing, use the
/// GCM::GHASH class.
std::vector<std::uint8_t> ghash(std::vector<std::uint8_t> ciphertext,
                                std::vector<std::uint8_t> associated_data,
                                std::vector<std::uint8_t> key);
} // namespace GCM