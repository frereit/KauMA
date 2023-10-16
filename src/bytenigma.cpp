#include <cstdint>
#include <vector>

#include "bytenigma.hpp"

/// @brief Encrypt the given \p input using bytenigma encryption, given a
/// set of \p rotors .
/// @param input the input bytes to be encrypted (iteratively)
/// @param rotors the initial rotor configuration
/// @return the encrypted data
std::vector<std::uint8_t>
Bytenigma::bytenigma(const std::vector<std::uint8_t> &input,
                     const std::vector<std::vector<std::uint8_t>> &rotors) {
  (void)input;
  (void)rotors;
  return std::vector<std::uint8_t>();
}
