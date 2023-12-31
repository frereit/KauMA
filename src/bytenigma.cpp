#include <cstdint>
#include <ranges>
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
  auto output = std::vector<std::uint8_t>(input.size());
  for (std::size_t i : std::views::iota(0u, input.size())) {
    output.at(i) = (this->process_byte(input.at(i)));
  }
  return output;
}

std::uint8_t Bytenigma::Bytenigma::forward_pass(std::uint8_t input) {
  for (std::size_t i : std::views::iota(0u, m_rotors.size())) {
    const std::vector<std::uint8_t> &rotor = m_rotors.at(i);
    std::uint8_t position = m_rotor_positions[i];
    input = rotor.at((input + position) % rotor.size());
  }
  return input;
}

std::uint8_t Bytenigma::Bytenigma::backward_pass(std::uint8_t input) {
  // Walk backwards through the m_inv_rotors list (starting with rotor `n`)
  for (std::size_t i :
       std::views::iota(0u, m_inv_rotors.size()) | std::views::reverse) {
    const std::vector<std::uint8_t> &rotor = m_inv_rotors.at(i);
    std::uint8_t position = m_rotor_positions[i];
    input = rotor.at((input) % rotor.size());

    // Underflow is well-defined behavior:
    // > Unsigned integers shall obey the laws of arithmetic modulo 2n where n
    // is the number of bits in the value > representation of that particular
    // size of integer.
    input -= position;
  }
  return input;
}

void Bytenigma::Bytenigma::turn_rotor(const std::size_t &index) {
  // Recursively turn the rotor at `index` and all rotors to the right of it as
  // long as an overflow occurs
  const std::vector<std::uint8_t> &rotor = m_rotors.at(index);
  std::size_t old_position = m_rotor_positions.at(index);
  m_rotor_positions.at(index) = (old_position + 1) % rotor.size();

  // check if we also need to rotate the next rotor by checking
  // if the top-most element is currently a 0
  if (rotor.at(old_position) == 0 && (index + 1) < m_rotors.size()) {
    this->turn_rotor(index + 1);
  };
}

void Bytenigma::Bytenigma::calculate_inverse_rotors() {
  m_inv_rotors = std::vector<std::vector<std::uint8_t>>(m_rotors.size());
  for (std::size_t i : std::views::iota(0u, m_rotors.size())) {
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
  for (std::size_t i = 0; i < 3; ++i) {
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

TEST_CASE("test bytenigma backward pass") {
  auto rotors = std::vector<std::vector<std::uint8_t>>();
  for (std::size_t i = 0; i < 3; ++i) {
    auto rotor = std::vector<std::uint8_t>();
    for (std::size_t j = 0; j < 256; ++j) {
      rotor.push_back((j + 1) % 256); // 0->1, 1->2 and so on
    }
    rotors.push_back(rotor);
  }
  Bytenigma::Bytenigma enigma = Bytenigma::Bytenigma(rotors);
  CHECK(enigma.backward_pass(3) == 0);
  CHECK(enigma.backward_pass(53) == 50);
  CHECK(enigma.backward_pass(2) == 255);
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

TEST_CASE("test rotor rotations") {
  auto rotors = std::vector<std::vector<std::uint8_t>>();
  auto rotor = std::vector<std::uint8_t>();
  for (std::size_t j = 0; j < 256; ++j) {
    rotor.push_back(j); // 0->0, 1->1 and so on
  }
  rotors.push_back(rotor);
  rotors.push_back(rotor);

  auto rotor2 = std::vector<std::uint8_t>();
  for (std::size_t j = 0; j < 256; ++j) {
    rotor2.push_back((j + 1) % 256); // 0->0, 1->1 and so on
  }
  rotors.push_back(rotor2);
  rotors.push_back(rotor2);

  Bytenigma::Bytenigma enigma = Bytenigma::Bytenigma(rotors);
  enigma.turn_rotor(0);
  // both rotors should have been turned
  std::vector<std::uint8_t> expected_positions = {1, 1, 1, 0};
  CHECK(enigma.m_rotor_positions == expected_positions);
}
#endif