#pragma once
#include <array>
#include <bit>
#include <concepts>
#include <cstdint>
#include <exception>
#include <sys/socket.h>
#include <unistd.h>

#include "polyfill.hpp"

namespace TCP {

/// @brief An abstraction over a raw file descriptor providing an interface to
/// communicate with a peer
/// @note the Communicator takes ownership of the file descriptor, closing it in
/// its destructor (RAII)
class Communicator {
public:
  /// @brief create a new Communicator over the given file descriptor
  /// @param fd the file descriptor with an active connection
  Communicator(int fd) : m_fd(fd) {}
  Communicator(Communicator &&other) {
    this->m_fd = other.m_fd;
    other.m_fd = -1;
  }

  ~Communicator() {
    if (this->m_fd != -1)
      close(this->m_fd);
  }

  /// @brief Returns true if the Communicator is open
  explicit operator bool() const { return this->m_fd != -1; }

  /// @brief read an integer from the file descriptor
  /// @tparam T the integer type which should be read
  /// @param endianness the endianness to assume when reading the value
  /// @return the value that was read from the file descriptor
  /// @throws std::runtime_error if reading failed
  template <std::integral T> T read_num(std::endian endianness) {
    T result;
    std::size_t total = 0;
    while (total < sizeof(T)) {
      std::size_t bytes_read =
          read(this->m_fd, reinterpret_cast<char *>(&result) + total,
               sizeof(T) - total);
      if (bytes_read > 0) {
        total += bytes_read;
      } else {
        // EOF or error
        throw std::runtime_error("Failed to read from socket");
      }
    }
    if (endianness != std::endian::native) {
      result = byteswap(result);
    }
    return result;
  }

  /// @brief write an integer to the file descriptor
  /// @tparam T the integer type which should be written
  /// @param value the value to be written to the channel
  /// @param endianness the endianness to use when writing the value
  /// @throws std::runtime_error if writing failed
  template <std::integral T> void write_num(T value, std::endian endianness) {
    if (endianness != std::endian::native) {
      value = byteswap(value);
    }
    std::size_t total = 0;
    while (total < sizeof(T)) {
      std::size_t bytes_written =
          write(this->m_fd, reinterpret_cast<char *>(&value) + total,
                sizeof(T) - total);
      if (bytes_written > 0) {
        total += bytes_written;
      } else {
        // EOF or error
        throw std::runtime_error("Failed to read from socket");
      }
    }
  }

  /// @brief read raw bytes from the file descriptor
  /// @tparam T number of bytes to be read
  /// @return the bytes read in network order
  template <std::size_t T> std::array<std::uint8_t, T> read_raw() {
    std::array<std::uint8_t, T> result;
    for (std::size_t i = 0; i < T; ++i) {
      result[i] = this->read_num<std::uint8_t>(std::endian::native);
    }
    return result;
  }

  /// @brief write raw bytes to the file descriptor
  /// @tparam T number of bytes to be written
  /// @param value the bytes to write to the file descriptor
  template <std::size_t T> void write_raw(std::array<std::uint8_t, T> value) {
    for (std::size_t i = 0; i < T; ++i) {
      this->write_num(value[i], std::endian::native);
    }
  }

private:
  int m_fd;

  Communicator(const Communicator &) = delete;
  Communicator &operator=(const Communicator &) = delete;
};
} // namespace TCP