#pragma once
#include <algorithm>
#include <bit>
#include <cassert>
#include <concepts>
#include <cstdint>
#include <ranges>
#include <vector>

namespace ByteManipulation {

// https://en.cppreference.com/w/cpp/numeric/byteswap
// FIXME: This can be replaced with std::byteswap once
//        C++23 is released.
template <std::integral T> constexpr T byteswap(T value) noexcept {
  static_assert(std::has_unique_object_representations_v<T>,
                "T may not have padding bits");
  auto value_representation =
      std::bit_cast<std::array<std::byte, sizeof(T)>>(value);
  std::ranges::reverse(value_representation);
  return std::bit_cast<T>(value_representation);
}

/// @brief Swap the endianness of a value if the host endianness does not match
/// the given endianness.
/// @tparam T the size of the value to convert (e.g. std::uint64_t)
/// @param value the value to convert
/// @param endianness the endianness to convert from or to.
/// @return
template <std::integral T>
constexpr T swap_for_endianness(T value, std::endian endianness) noexcept {
  if (std::endian::native != endianness) {
    value = ByteManipulation::byteswap(value);
  }
  return value;
}

/// @brief append a value to a vector of bytes
/// @tparam T the size of the value to append (e.g. std::uint64_t)
/// @param value the value to append
/// @param endianness the endianness to use
/// @param out the vector to which to append the data
template <std::integral T>
inline void append_as_bytes(T value, std::endian endianness,
                            std::vector<std::uint8_t> &out) {
  value = swap_for_endianness(value, endianness);
  out.insert(out.end(), reinterpret_cast<std::uint8_t *>(&value),
             reinterpret_cast<std::uint8_t *>(&value) + sizeof(value));
}

/// @brief construct a value from a sequence of bytes
/// @tparam T the value type to create
/// @param in the sequence of bytes
/// @param endianness the endianness of the byte sequence
/// @return the constructed value
template <std::integral T>
inline T from_bytes(std::vector<std::uint8_t> in, std::endian endianness) {
  T val = 0;
  assert(in.size() == sizeof(T) &&
         "Attempted conversion with invalid byte length");
  std::copy_n(in.begin(), sizeof(T), reinterpret_cast<std::uint8_t *>(&val));
  return swap_for_endianness(val, endianness);
}
} // namespace ByteManipulation