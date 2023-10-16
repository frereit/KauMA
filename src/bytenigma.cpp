#include <cstdint>
#include <vector>

#include "bytenigma.hpp"

Bytenigma::Bytenigma::Bytenigma(
    const std::vector<std::vector<std::uint8_t>> &rotors)
    : m_rotors(rotors), m_inv_rotors() {
  m_rotor_positions = std::vector<std::uint8_t>(rotors.size());
  this->calculate_inverse_rotors();
}

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
  for (std::size_t i = 0; i < m_rotors.size(); ++i) {
    std::vector<std::uint8_t> rotor = m_rotors[i];
    std::uint8_t position = m_rotor_positions[i];
    input = rotor.at((input + position) % rotor.size());
  }
  return input;
}

std::uint8_t Bytenigma::Bytenigma::backward_pass(std::uint8_t input) {
  (void)input;
  return std::uint8_t();
}

void Bytenigma::Bytenigma::turn_rotor(const std::uint8_t &index) {
  (void)index;
}

void Bytenigma::Bytenigma::calculate_inverse_rotors() {
  m_inv_rotors = std::vector<std::vector<std::uint8_t>>(m_rotors.size());
  for (std::size_t i = 0; i < m_rotors.size(); ++i) {
    auto inv_rotor = std::vector<std::uint8_t>(m_rotors.at(i).size());
    for (std::size_t in = 0; in < inv_rotor.size(); ++in) {
      inv_rotor.at(m_rotors.at(i).at(in)) = static_cast<std::uint8_t>(in);
    }
    m_inv_rotors.at(i) = inv_rotor;
  }
}

#ifdef TEST
#include "doctest.h"

TEST_CASE("test bytenigma forward pass") {
  auto rotors = std::vector<std::vector<std::uint8_t>>();
  for (size_t i = 0; i < 3; ++i) {
    auto rotor = std::vector<std::uint8_t>();
    for (std::size_t j = 0; j < 256; ++j) {
      rotor.push_back((j + 1) % 256); // 0->1, 1->2 and so on
    }
    rotors.push_back(rotor);
  }
  Bytenigma::Bytenigma enigma = Bytenigma::Bytenigma(rotors);
  CHECK(enigma.forward_pass(0) == 3);
  CHECK(enigma.forward_pass(50) == 53);
  CHECK(enigma.forward_pass(255) == 2);
}

TEST_CASE("test bytenigma inverted rotors are calculated correctly") {
  auto rotors = std::vector<std::vector<std::uint8_t>>();
  for (std::size_t i = 0; i < 3; ++i) {
    auto rotor = std::vector<std::uint8_t>();
    for (std::size_t j = 0; j < 256; ++j) {
      rotor.push_back((j + 1) % 256); // 0->1, 1->2 and so on
    }
    rotors.push_back(rotor);
  }


  auto inverted_rotors = std::vector<std::vector<std::uint8_t>>();
  for (std::size_t i = 0; i < 3; ++i) {
    auto rotor = std::vector<std::uint8_t>();
    for (std::size_t j = 0; j < 256; ++j) {
      if (j > 0) {
        rotor.push_back(j - 1);
      } else {
        rotor.push_back(255);
      }
    }
    inverted_rotors.push_back(rotor);
  }

  Bytenigma::Bytenigma enigma = Bytenigma::Bytenigma(rotors);
  CHECK(enigma.m_inv_rotors == inverted_rotors);
}
#endif