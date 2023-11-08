#pragma once
#include <array>
#include <cstdint>
#include <unistd.h>
#include <vector>

#include "tcp/server.hpp"

/**
 * @brief Server and client implementation showcasing a padding oracle attack
 * against AES-256-CBC.
 */
namespace PaddingOracle {

std::uint8_t constexpr BLOCK_SIZE = 16;
typedef std::array<std::uint8_t, BLOCK_SIZE> block;

/**
 * @brief A TCP-server providing an interface against which to demonstrate the
 * padding oracle attack.
 *
 * The padding oracle protocol works as follows:
 *  -# client: 16 bytes ciphertext
 *  -# while \f$n > 0\f$:
 *    -# client: 2 bytes Q block count (\f$n\f$) (little endian)
 *    -# client: \f$n\f$ times
 *      - 16 bytes Q block
 *    -# server: \f$n\f$ times:
 *      - 1 if Q block \f$n\f$ resulted in correct padding
 *      - 0 otherwise
 */
class XORCBCServer : public TCP::Server {
public:
  /// @brief Initialize the server at the specified port
  /// @param key The XOR key to use for decryption
  /// @param port The TCP-port to listen on
  /// @note Listens on all interfaces and IPv4 only
  XORCBCServer(block key, const std::uint16_t port)
      : TCP::Server(port), m_key(key) {}

private:
  /// @brief provide a padding oracle for a given \p client
  /// @param client the TCP client
  /// @return 0 on success, 1 otherwise
  int handle_client(TCP::Communicator client);

  /// @brief decrypt a block using XOR-CBC
  /// @param q the previous ciphertext to use for CBC decryption
  /// @param ciphertext the ciphertext to decrypt
  /// @return the plaintext
  block decrypt(const block &q, const block &ciphertext);

  /// @brief check if a given \p block contains correct padding
  /// @param block the block to validate
  /// @return true if the padding is valid, false otherwise
  bool validate_padding(const block &block);
  block m_key;
};
} // namespace PaddingOracle