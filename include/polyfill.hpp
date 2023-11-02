#include <algorithm>
#include <concepts>
#include <ranges>

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
