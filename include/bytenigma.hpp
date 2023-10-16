#pragma once
#include <cstdint>
#include <vector>

namespace Bytenigma {
/**
 *  @brief An enigma-like encryption which works by chaining a variable number
 * of to encrypt a sequence of bytes.
 * @param rotors The initial rotor configuration of the bytenigma machine
 * Each rotor is specified by a `uint8_t rotor[256]` array, which maps a given
 * input byte `x` to `rotor[x]`.
 *
 * To encrypt a byte, it is first processed in a "forward-pass" through the
 * rotors, mapping the byte as specified by each rotor. After the
 * forward-pass, the bitwise-complement of the partially-encrypted byte is
 * processed in a "backward-pass" through the rotors.
 *
 * Once a byte has been fully encrypted, i.e. both a forward- and
 * backward-pass have been performed, the first rotor is turned. A rotor-turn
 * means that the "outputs" are moved up by one, so that `rotor_next[x]` =
 * `rotor[x+1]` (and `rotor_next[255] = rotor[0]`). Iff a rotor-turn was
 * performed with `rotor[0]
 * == 0`, then the next rotor is also turned according to the same algorithm.
 *
 * If the last rotor in the chain is turned no action is taken.
 */
class Bytenigma {
public:
  /// @brief create a new bytenigma machine
  /// @param rotors the initial rotor configuration
  Bytenigma(const std::vector<std::vector<std::uint8_t>> &rotors)
      : m_rotors(rotors) {
    m_rotor_positions = std::vector<std::uint8_t>(rotors.size());
  }

  /// @brief input a single byte into the machine, process it (modifying the
  /// internal state), and return the resulting byte.
  /// @param input the byte to process
  /// @return the encrypted byte
  std::uint8_t process_byte(std::uint8_t input);

  /// @brief input multiple bytes after another into the bytenigma machine and
  /// process all of them.
  /// @param input the sequence of input bytes to process. \p input [0] is
  /// processed first.
  /// @return the bytes, in the same order as in the input, encrypted.
  std::vector<std::uint8_t>
  process_bytes(const std::vector<std::uint8_t> &input);

// Make the member functions public for test builds
#ifndef TEST
private:
#endif

  /// @brief pass a given byte through the machine, starting at rotor 0
  /// @param input the input byte to be passed through
  /// @return the result at the end of the machine
  std::uint8_t forward_pass(std::uint8_t input);

  /// @brief pass a given byte backwards through the machine, starting at rotor
  /// `n`
  /// @warning The rotors are *not* automatically turned when calling this
  /// method.
  /// @param input  the input byte to be passed through
  /// @return the result at the beginning of the machine
  std::uint8_t backward_pass(std::uint8_t input);

  /// @brief Turn the rotor at the given \p index . This automatically turns the
  /// next rotor if an overflow occurred.
  /// @param index the index of the rotor to turn.
  void turn_rotor(const std::uint8_t &index);

  const std::vector<std::vector<std::uint8_t>> m_rotors;
  std::vector<std::uint8_t> m_rotor_positions;
};

} // namespace Bytenigma