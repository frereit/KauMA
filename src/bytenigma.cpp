#include <cstdint>
#include <vector>

#include "bytenigma.hpp"

std::uint8_t Bytenigma::Bytenigma::process_byte(std::uint8_t input) {
  std::uint8_t forward = this->forward_pass(input);
  std::uint8_t complement = ~forward;
  std::uint8_t backward = this->backward_pass(complement);
  this->turn_rotor(0);
  return backward;
}

std::vector<std::uint8_t>
Bytenigma::Bytenigma::process_bytes(const std::vector<std::uint8_t> &input) {
  std::vector<std::uint8_t> output;
  for (std::uint8_t byte : input) {
    output.push_back(this->process_byte(byte));
  }
  return output;
}

std::uint8_t Bytenigma::Bytenigma::forward_pass(std::uint8_t input) {
  (void)input;
  return std::uint8_t();
}

std::uint8_t Bytenigma::Bytenigma::backward_pass(std::uint8_t input) {
  (void)input;
  return std::uint8_t();
}

void Bytenigma::Bytenigma::turn_rotor(const std::uint8_t &index) {
  (void)index;
}
