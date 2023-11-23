#pragma once
#include <cstdint>
#include <vector>

namespace GCM {
std::vector<std::uint8_t> ghash(std::vector<std::uint8_t> ciphertext,
                                std::vector<std::uint8_t> associated_data,
                                std::vector<std::uint8_t> key);
}