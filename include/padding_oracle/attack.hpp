#pragma once
#include <cstdint>
#include <string>
#include <vector>

#include "padding_oracle/server.hpp"
#include "tcp/communicator.hpp"

namespace PaddingOracle {

/// @brief demonstrate the padding oracle attack against a TCP server
class Attack {
public:
  Attack(const std::string &hostname, std::uint16_t port)
      : m_hostname(hostname), m_port(port) {}

  /// @brief recover the plaintext using the padding oracle attack
  /// @param iv the IV used during encryption
  /// @param ciphertext the ciphertext to decrypt. The length must be a multiple
  /// of PaddingOracle::BLOCK_SIZE
  /// @return the recovered plaintext
  /// @throws std::runtime error if the ciphertext length is not a multiple of
  /// BLOCK_SIZE
  std::vector<std::uint8_t>
  recover_plaintext(const block &iv,
                    const std::vector<std::uint8_t> &ciphertext);

  /// @brief recover the cleartext block for a corresponding ciphertext block
  /// @param prev the previous block or IV
  /// @param block the block to decrypt
  /// @return the decrypted block
  block recover_block(const block &prev, const block &block);

private:
  /// @brief recover the \f$D(C)\f$ block for a corresponding ciphertext block.
  /// Note that this does not equal the plaintext because CBC has not been
  /// applied yet.
  /// @param block the block to decrypt
  /// @return the \f$D(C)\$ block
  block recover_raw_block(const block &block);

  /// @brief recover a single byte using the padding oracle attack
  /// @param target the target against which to demonstrate the attack
  /// @param index the index of the byte to recover must be less than
  /// BLOCK_SIZE)
  /// @param raw_recovered the already decrypted bytes. All bytes after \p index
  /// must be valid decrypted bytes
  /// @return the \f$D(C)\f$ bytes at \p index
  std::uint8_t recover_byte(TCP::Communicator &target, std::size_t index,
                            PaddingOracle::block raw_recovered);
  std::string m_hostname;
  std::uint16_t m_port;
};
} // namespace PaddingOracle