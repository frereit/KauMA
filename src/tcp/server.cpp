#include <arpa/inet.h>
#include <cstdint>
#include <iostream>
#include <stdexcept>

#include "tcp/server.hpp"

TCP::Server::Server(const std::uint16_t port)
    : m_fd(socket(AF_INET, SOCK_STREAM, 0)) {
  struct sockaddr_in listen_addr {};

  if (!this->m_fd) {
    throw std::runtime_error("Failed to open TCP socket");
  }
  listen_addr.sin_family = AF_INET;
  listen_addr.sin_addr.s_addr = INADDR_ANY;
  listen_addr.sin_port = htons(port);
  if (bind(this->m_fd, (struct sockaddr *)&listen_addr, sizeof(listen_addr)) <
      0) {
    throw std::runtime_error("Failed to bind the socket to port");
  }

  if (listen(this->m_fd, TCP::MAX_CONNECTION_QUEUE) < 0) {
    throw std::runtime_error("Failed to listen on the main socket");
  }
}

TCP::Server::~Server() { close(this->m_fd); }

[[noreturn]] void TCP::Server::loop_forever() {
  while (true) {
    this->accept_and_spawn();
  }
}

void TCP::Server::accept_and_spawn() {
  struct sockaddr_in client_addr {};
  socklen_t client_addr_size = sizeof(client_addr);
  auto client =
      TCP::Communicator(accept(this->m_fd, (struct sockaddr *)&client_addr,
                               (socklen_t *)&client_addr_size));
  std::cerr << "new client from " << inet_ntoa(client_addr.sin_addr) << ":"
            << client_addr.sin_port << "\n";

  if (!client) {
    std::cerr << "Failed to open client connection\n";
    return;
  }

  std::cerr << std::flush;
  std::cout << std::flush;
  int pid = fork();
  if (pid < 0) {
    std::cerr << "Failed to accept new connection\n";
    return;
  }
  if (pid == 0) {
    _exit(this->handle_client(std::move(client)));
  }
}
