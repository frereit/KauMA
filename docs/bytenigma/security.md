# BYTENIGMA Security

In this page, various possible attacks and weaknesses both on the BYTENIGMA algorithm itself and my implementation will be documented.

## Indistinguishability & Bias

There is a trivial bias in the output: Any given input byte \f$x\f$ can never be encrypted to itself. This can be proven as follows:

- When a byte \f$x\f$ is passed "forward" through the rotors, it takes a "path" \f$x\mapsto \cdots \mapsto y\f$ through the rotors. This "path" is then the only path in the current configuration which maps \f$x\f$ to \f$y\f$. 
- At the end of the rotors, the bitwise complement of \f$y\f$ is taken and passed back through the machine. By nature of a bitwise complement, is is guaranteed that \f$y \neq \bar{y}\f$.
- The fully encrypted byte is then taken "backward" through the rotors, with a "path" \f$\bar{y} \mapsto \cdots \mapsto x'\f$.
- Because \f$y \neq \bar{y}\f$ and the paths are unique for a given rotor configuration, it is guaranteed that \f$x \neq x'\f$.

This can also be experimentally verified by encrypting 131072 (1 MiB) null-bytes and printing the least common outputs:

~~~~~~~~~~~~~{.py}
from collections import Counter
import json
import base64

with open("examples/bytenigma/2.out.json") as f:
    output = base64.b64decode(json.load(f)["output"])

dist = Counter()
dist.update({x: 0 for x in range(256)})
dist.update(output)

for key, value in dist.most_common()[:-5:-1]:
    print(key, value)
~~~~~~~~~~~~~

~~~~~~~~~~~~~
0 0
215 3967
41 3967
140 3984
~~~~~~~~~~~~~

This shows that the null-byte appeared exactly zero times in the output.

Due to this fact, indistinguishability is not given. A truly random output would have an equal distribution over all possible bytes, instead of excluding the input bytes.

## Linear rotors

The security of the machine strictly depends on non-linearity of the rotors. This is simply demonstrated by constructing rotors in a strictly linear fashion and encryption 1 MiB null-bytes:

~~~~~~~~~~~~~{.json}
{
    "action": "bytenigma",
    "rotors": [ 
        [32, 33, 34, 35, 36, 37, ..., 31],
        [237, 238, 239, 240, 241, 242, ..., 236],
        [150, 151, 152, 153, 154, 155, ...., 149]
    ],
    "input": "AAAA ... AAA=="
}
~~~~~~~~~~~~~

Visualising the first 512 output bytes clearly shows a linear pattern:

~~~~~~~~~~~~~{.py}
import matplotlib.pyplot as plt
from collections import Counter
import json
import base64

with open("examples/bytenigma/all_linear_rotors.out.json") as f:
    output = base64.b64decode(json.load(f)["output"])

plt.plot([x for i, x in enumerate(output[:512])])
plt.grid(True)
plt.savefig("linear.png")
~~~~~~~~~~~~~

![Value of the first 512 output bytes encrypted using linear rotors](linear.png)

## Finding the position of the 0 in the first rotor

Using a chosen plaintext attack, it is trivial to find the index of the `0` in the first rotor.

Finding the index of the `0` in the first rotor is equivalent to finding the index at which the second rotor turns. We know for a fact that this happens exactly once during the first 256 bytes encrypted using the machine.

We can force the output of the first rotor during the forward pass to be the same in every one of those 256 encrypted bytes, simply by sending bytes in decending order. For example, if we encrypt the byte `0xff`, lets assume `rotor[0][0xff]` is some value \f$x\f$. After the byte is encrypted, the first rotor guaranteed to be turned by one. If the next encrypted byte sent is `0xfe`, it will be mapepd to `rotor[0][0xfe + 1]` (due to the rotation by one), which is equivalent to \f$x\f$.

Once the output of the first rotor in the forward pass is the same, the path taken through the rest of the machine is guaranteed to be the same, as long as no other rotor turned. This means that the input to the first rotor in the backward pass is also guarnteed to be the same. Since between the two bytes `rotor[0]` was turned exactly by one, the same input \f$y\f$ will be mapped to \f$z\f$ and \f$z - 1\f$ respectively.

Thus, when encrypting the sequence `[0xff, 0xfe, 0xfd, ..., 0]`, the output will be linear until the second rotor turns. If and only if the second rotor turns, linearity breaks. We can thus simply analyse the output to find this point, yielding the index of the `0` in the first rotor.

This can simply be demonstracted by plotting the encrypted bytes using the `[0xff, ..., 0]` sequence as input:

~~~~~~~~~~~~~{.py}
import matplotlib.pyplot as plt
import json
import base64

with open("examples/bytenigma/backwards.out.json") as f:
    output = base64.b64decode(json.load(f)["output"])
for i, (p, x) in enumerate(zip(output, output[1:])):
    if x != (p - 1)%256:
        print("linearity breaks at", i)

plt.plot(list(output), "o")
plt.grid(True)
plt.savefig("backwards.png")
~~~~~~~~~~~~~

![Linearity break at index 0x66 indicates that the second rotor has turned](backwards.png)

In the plot, it is clearly evident that the linearity breaks and the script outputs that the linearity breaks at 102.

Indeed validation shows that `rotors[0][102] == 0`.

We have shown that is is possible, using a single chosen plaintext, to recover the position of the `0` in the first rotor.

## Missing Nonce 

The BYTENIGMA encryption is equivalent to a polyalphabetic encryption, where each position in the cleartext gets assigned an alphabet based on the rotor configuration.

However because no nonce is used, key reuse is fatal. If an attacker has a single pair of known plaintext and corresponding ciphertext, they can deduce not only the mappings for the specific bytes from the plaintext, but also make assumptions about the alphabets. By the same logic used to find the `0` in the first rotor, if we assume the second rotor did not turn, \f$E_i(x)\f$ (encryption of byte \f$x\f$ at index \f$i\f$) is equivalent to \f$E_{i+1}(x-1) + 1 \pmod{256}\f$.

This potentially allows an attacker to recover parts of an unknown ciphertext, given a single known plaintext pair. Thus, key use must be avoided.

Note: In the original enigma, this was somewhat prevented, by encrypting a "message key" at the beginning of each message using the "daily key", and encrypting the rest of the message using the "message key". However, as Rejewski's characteristic method proves, this was not fully effective.

## Side Channel Attack

There is an obvious side-channel attack on this specific implementation of BYTENIGMA. Let's take a look at the implementation of `Bytenigma::turn_rotor`:

~~~~~~~~~~~~~{.cpp}
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
~~~~~~~~~~~~~

Notice the recursive nature of this implementation. If and only if a rotor overflowed and there are more rotors to turn, a recursive call is made. This will incur more executed instructions than if no overflow occurred. We can thus expect that it is feasible to deduce the position of the `0` of all rotors except the last rotor in the chain by counting the number of instruction executed, or indirectly by measuring the execution time when encryption a single byte.

This should be fixed by making the execution time independent of an overflow, e.g. by using branchless programming. A rotor should be turned by `0` if the previous rotor did not overflow, and turned by `1` if the previous rotor overflowed.
