#include <algorithm>
#include <botan/block_cipher.h>
#include <botan/hex.h>
#include <functional>
#include <iostream>
#include <iterator>
#include <stdexcept>

#include "bytemanipulation.hpp"
#include "gcm/encryptor.hpp"

std::vector<std::uint8_t> GCM::Encryptor::gen_ctr_block(std::uint32_t ctr) {
  ctr = ByteManipulation::swap_for_endianness(ctr, std::endian::big);
  std::vector<std::uint8_t> block = m_nonce;
  block.insert(block.end(), reinterpret_cast<std::uint8_t *>(&ctr),
               reinterpret_cast<std::uint8_t *>(&ctr) + sizeof(ctr));
  return block;
}

#ifdef TEST
#include "doctest.h"
#include <botan/hex.h>
#include <limits>

TEST_CASE("test counter mode block generator") {
  std::vector<std::uint8_t> key(16);
  std::vector<std::uint8_t> nonce =
      Botan::hex_decode("aa1d5a0aa1ea09f6ff91e534");
  GCM::Encryptor e = GCM::Encryptor(key, nonce);
  CHECK(e.gen_ctr_block(0) ==
        Botan::hex_decode("aa1d5a0aa1ea09f6ff91e53400000000"));
  CHECK(e.gen_ctr_block(1) ==
        Botan::hex_decode("aa1d5a0aa1ea09f6ff91e53400000001"));
  CHECK(e.gen_ctr_block(0xc479) ==
        Botan::hex_decode("aa1d5a0aa1ea09f6ff91e5340000c479"));
  CHECK(e.gen_ctr_block(0x2001d767) ==
        Botan::hex_decode("aa1d5a0aa1ea09f6ff91e5342001d767"));
  CHECK(e.gen_ctr_block(std::numeric_limits<std::uint32_t>::max()) ==
        Botan::hex_decode("aa1d5a0aa1ea09f6ff91e534ffffffff"));
}
#endif