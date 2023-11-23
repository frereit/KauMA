#include <algorithm>
#include <bit>
#include <botan/block_cipher.h>
#include <cassert>
#include <functional>
#include <iostream>
#include <iterator>
#include <stdexcept>

#include "bytemanipulation.hpp"
#include "gcm/encryptor.hpp"
#include "gcm/ghash.hpp"
#include "gcm/polynomial.hpp"

GCM::Encryptor::Encryptor(std::unique_ptr<Botan::BlockCipher> cipher,
                          const std::vector<std::uint8_t> &nonce)
    : m_cipher(std::move(cipher)) {
  // SAFETY: This requirement is used in GCM::Encryptor::ghash.
  assert(m_cipher->block_size() == 16 &&
         "GCM is only implemented for ciphers with 16-byte blocks.");
  if (nonce.size() == 12) {
    m_y0 = nonce;
    m_y0.resize(this->m_cipher->block_size(), 0);
    m_y0.back() = 1;
  } else {
    m_y0 = GCM::ghash(nonce, {}, this->h());
  }
}

GCM::EncryptionResult GCM::Encryptor::encrypt_and_authenticate(
    std::vector<std::uint8_t> plaintext,
    std::vector<std::uint8_t> associated_data) {
  std::vector<std::uint8_t> ciphertext = this->encrypt(plaintext);
  std::vector<std::uint8_t> auth_tag =
      this->authenticate(ciphertext, associated_data);
  return {ciphertext, associated_data, auth_tag};
}

std::vector<std::uint8_t> GCM::Encryptor::y0() { return this->m_y0; }

std::vector<std::uint8_t> GCM::Encryptor::h() {
  std::vector<std::uint8_t> auth_key(this->m_cipher->block_size());
  this->m_cipher->encrypt(auth_key);
  return auth_key;
}

std::vector<std::uint8_t> GCM::Encryptor::y_block(std::uint32_t y) {
  std::vector<std::uint8_t> block(m_y0.begin(), m_y0.begin() + 12);
  std::vector<std::uint8_t> val_bytes(m_y0.begin() + 12, m_y0.end());
  std::uint32_t val =
      ByteManipulation::from_bytes<std::uint32_t>(val_bytes, std::endian::big);
  val += y;
  ByteManipulation::append_as_bytes(val, std::endian::big, block);
  return block;
}

std::vector<std::uint8_t>
GCM::Encryptor::encrypt(std::vector<std::uint8_t> plaintext) {
  std::vector<std::uint8_t> ciphertext, keystream;
  std::uint32_t y = 1;
  for (const std::uint8_t &pt : plaintext) {
    if (keystream.size() == 0) {
      // No bytes left in the keystream so we need to encrypt a new block
      keystream = this->y_block(y++);
      this->m_cipher->encrypt(keystream);
    }
    ciphertext.push_back(pt ^ keystream.at(0));
    keystream.erase(keystream.begin());
  }
  return ciphertext;
}

std::vector<std::uint8_t>
GCM::Encryptor::authenticate(std::vector<std::uint8_t> ciphertext,
                             std::vector<std::uint8_t> associated_data) {
  std::vector<std::uint8_t> auth_tag =
      GCM::ghash(ciphertext, associated_data, this->h());

  std::vector<std::uint8_t> auth_tag_mask = this->y0();
  this->m_cipher->encrypt(auth_tag_mask);

  std::transform(auth_tag.begin(), auth_tag.end(), auth_tag_mask.begin(),
                 auth_tag.begin(), std::bit_xor<uint8_t>());
  return auth_tag;
}

#ifdef TEST
#include "doctest.h"
#include <botan/hex.h>
#include <limits>

TEST_CASE("test counter mode block generator with 12 byte nonce") {
  std::vector<std::uint8_t> key(16);
  auto aes = Botan::BlockCipher::create_or_throw("AES-128");
  aes->set_key(key);
  std::vector<std::uint8_t> nonce =
      Botan::hex_decode("aa1d5a0aa1ea09f6ff91e534");
  GCM::Encryptor e = GCM::Encryptor(std::move(aes), nonce);
  CHECK(Botan::hex_encode(e.y_block(0)) == "AA1D5A0AA1EA09F6FF91E53400000001");
  CHECK(Botan::hex_encode(e.y_block(0xc478)) ==
        "AA1D5A0AA1EA09F6FF91E5340000C479");
  CHECK(Botan::hex_encode(e.y_block(0x2001d766)) ==
        "AA1D5A0AA1EA09F6FF91E5342001D767");
  CHECK(Botan::hex_encode(e.y_block(std::numeric_limits<std::uint32_t>::max() -
                                    1)) == "AA1D5A0AA1EA09F6FF91E534FFFFFFFF");
}

TEST_CASE("test counter mode block generator with 8 byte nonce") {
  std::vector<std::uint8_t> key =
      Botan::hex_decode("feffe9928665731c6d6a8f9467308308");
  std::vector<std::uint8_t> nonce = Botan::hex_decode("cafebabefacedbad");
  auto aes = Botan::BlockCipher::create_or_throw("AES-128");
  aes->set_key(key);
  GCM::Encryptor e = GCM::Encryptor(std::move(aes), nonce);
  CHECK(Botan::hex_encode(e.y_block(0)) == "C43A83C4C4BADEC4354CA984DB252F7D");
  CHECK(Botan::hex_encode(e.y_block(1)) == "C43A83C4C4BADEC4354CA984DB252F7E");
  CHECK(Botan::hex_encode(e.y_block(3)) == "C43A83C4C4BADEC4354CA984DB252F80");
  CHECK(Botan::hex_encode(e.y_block(4)) == "C43A83C4C4BADEC4354CA984DB252F81");
}
#endif