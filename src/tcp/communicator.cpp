#include <arpa/inet.h>
#include <memory>
#include <netdb.h>
#include <stdexcept>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "tcp/communicator.hpp"

// Specialize std::default_delete to ensure addrinfo gets deleted correctly
template <> struct std::default_delete<struct addrinfo> {
  default_delete() = default;
  template <class U> constexpr default_delete(default_delete<U>) noexcept {}
  void operator()(struct addrinfo *p) const noexcept { freeaddrinfo(p); }
};

TCP::Communicator::Communicator(const std::string &hostname,
                                const std::uint16_t port) {
  struct addrinfo hints, *raw_resolved;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;     // search for any IP (IPv4 or IPv6)
  hints.ai_socktype = SOCK_STREAM; // search for TCP
  hints.ai_flags |= AI_CANONNAME;

  if (getaddrinfo(hostname.c_str(), std::to_string(port).c_str(), &hints,
                  &raw_resolved) != 0) {
    throw std::runtime_error("Failed to resolve hostname");
  }
  auto resolved = std::unique_ptr<addrinfo>(raw_resolved);

  // SAFETY: we take a raw pointer from std::unique_ptr.
  //         This is fine because the attempt will always be
  //         released before resolved.
  for (struct addrinfo *attempt = resolved.get(); attempt != nullptr;
       attempt = attempt->ai_next) {
    this->m_fd = socket(attempt->ai_family, SOCK_STREAM, 0);
    if (!this->m_fd) {
      throw std::runtime_error("Failed to open socket");
    }
    if (connect(this->m_fd, attempt->ai_addr, attempt->ai_addrlen) == 0) {
      // connection established
      return;
    }
    close(this->m_fd);
  }

  // no valid address found
  throw std::runtime_error("Failed to connect");
}