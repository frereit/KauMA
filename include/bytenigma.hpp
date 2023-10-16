#pragma once
#include <cstdint>
#include <vector>

/**
 * @brief An enigma-like encryption which works by chaining a variable number of
 * rotors to encrypt a sequence of bytes.
 *
 * Each rotor is specified by a `uint8_t rotor[256]` array, which maps a given
 * input byte `x` to `rotor[x]`.
 *
 * To encrypt a byte, it is first processed in a "forward-pass" through the
 * rotors, mapping the byte as specified by each rotor. After the forward-pass,
 * the bitwise-complement of the partially-encrypted byte is processed in a
 * "backward-pass" through the rotors.
 *
 * Once a byte has been fully encrypted, i.e. both a forward- and backward-pass
 * have been performed, the first rotor is turned. A rotor-turn means that the
 * "outputs" are moved up by one, so that `rotor_next[x]` = `rotor[x+1]` (and
 * `rotor_next[255] = rotor[0]`). Iff a rotor-turn was performed with `rotor[0]
 * == 0`, then the next rotor is also turned according to the same algorithm.
 *
 * If the last rotor in the chain is turned no action is taken.
 */
namespace Bytenigma {
std::vector<std::uint8_t>
bytenigma(const std::vector<std::uint8_t> &input,
          const std::vector<std::vector<std::uint8_t>> &rotors);
}