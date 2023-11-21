#include <algorithm>
#include <bit>
#include <botan/block_cipher.h>
#include <botan/hex.h>
#include <cassert>
#include <functional>
#include <iostream>
#include <iterator>
#include <stdexcept>

#include "bytemanipulation.hpp"
#include "gcm/encryptor.hpp"

GCM::Encryptor::Encryptor(std::unique_ptr<Botan::BlockCipher> cipher,
                          const std::vector<std::uint8_t> &nonce)
    : m_cipher(std::move(cipher)), m_nonce(nonce) {
  // SAFETY: This requirement is used in GCM::Encryptor::ghash.
  assert(m_cipher->block_size() == 16 &&
         "GCM is only implemented for ciphers with 16-byte blocks.");
  assert(m_nonce.size() == 12 &&
         "GCM is only implemented for a 12-byte nonce with a 4-byte counter.");
}

std::vector<std::uint8_t> GCM::Encryptor::y0() {
  return this->gen_ctr_block(1);
}

std::vector<std::uint8_t> GCM::Encryptor::h() {
  std::vector<std::uint8_t> auth_key(this->m_cipher->block_size());
  this->m_cipher->encrypt(auth_key);
  return auth_key;
}

std::vector<std::uint8_t> GCM::Encryptor::gen_ctr_block(std::uint32_t ctr) {
  std::vector<std::uint8_t> block = m_nonce;
  ByteManipulation::append_as_bytes(ctr, std::endian::big, block);
  return block;
}

std::vector<std::uint8_t>
GCM::Encryptor::encrypt(std::vector<std::uint8_t> plaintext) {
  assert(plaintext.size() % this->m_cipher->block_size() == 0 &&
         "Plaintext length must be a multiple of block size");
  std::vector<std::uint8_t> ciphertext, keystream;
  std::uint32_t ctr = 2;
  for (const std::uint8_t &pt : plaintext) {
    if (keystream.size() == 0) {
      // No bytes left in the keystream so we need to encrypt a new block
      keystream = this->gen_ctr_block(ctr++);
      this->m_cipher->encrypt(keystream);
    }
    ciphertext.push_back(pt ^ keystream.at(0));
    keystream.erase(keystream.begin());
  }
  return ciphertext;
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