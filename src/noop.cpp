#include <cstdint>
#include <vector>

#include "noop.hpp"

/// @brief return the input unchanged
/// @param input the input data
/// @return the input data
std::vector<std::uint8_t> Noop::noop(const std::vector<std::uint8_t> &input) {
  return input;
}

#ifdef TEST
#include "doctest.h"
#include <cstdint>
#include <vector>

TEST_CASE("test noop does nothing") {
  std::vector<std::uint8_t> input{1, 2, 3, 4, 5};
  auto result = Noop::noop(input);
  CHECK(result.size() == input.size());
  CHECK(std::equal(result.begin(), result.end(), input.begin()));
}
#endif