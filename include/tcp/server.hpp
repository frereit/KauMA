#pragma once
#include <atomic>
#include <cstdint>
#include <unistd.h>
#include <vector>

#include "tcp/communicator.hpp"

/**
 * @brief Abstractions providing communication over TCP
 */
namespace TCP {

int constexpr MAX_CONNECTION_QUEUE = 10;

/**
 * @brief A multiprocessing TCP server
 */
class Server {
public:
  /// @brief initialize the server at the specified port
  /// @param port the TCP-port to listen on
  /// @note listens on all interfaces and IPv4 only
  Server(const std::uint16_t port);
  virtual ~Server();

  /// @brief keep accepting new connections forever
  [[noreturn]] void loop_forever();

  /// @brief accept a single new connection, start handling it in a new process
  /// and return
  void accept_and_spawn();

private:
  /// @brief called with a client connection once it has been accepted
  /// @param client the client to communicate with
  /// @return the return value passed to _exit once this client disconnects
  virtual int handle_client(TCP::Communicator client) = 0;
  int m_fd;
};
} // namespace TCP