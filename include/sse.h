#pragma once
#include <cstdint>
#include <emmintrin.h>
#include <smmintrin.h>
#include <iostream>
#include <iomanip>

#include "bytemanipulation.hpp"

namespace SSE {

/// @brief create a sse vector with a 1 at the given index
/// @param index the index to set
/// @return a 128 bit vector
inline __m128i from_index(std::uint8_t index) {
  std::uint64_t high = 0;
  std::uint64_t low = 0;
  if (index >= 64)
    high = 1llu << (index - 64);
  else
    low = 1llu << index;
  return _mm_setr_epi64(_mm_set_pi64x(low), _mm_set_pi64x(high));
}

/// @brief test if index \p index of \p x is set.
/// @param x the 128 bit value to test
/// @param index the index to test
/// @return true if the bit is set, false otherwise
inline bool test(__m128i x, std::uint8_t index) {
  return !_mm_testz_si128(x, SSE::from_index(index));
}

} // namespace SSE