#include <cstdint>
#include <vector>

#include "bytemanipulation.hpp"
#include "gcm/ghash.hpp"
#include "gcm/polynomial.hpp"

std::vector<std::uint8_t> GCM::ghash(std::vector<std::uint8_t> ciphertext,
                                     std::vector<std::uint8_t> associated_data,
                                     std::vector<std::uint8_t> key) {
  GCM::Polynomial auth_tag = GCM::Polynomial(std::bitset<128>(0));
  GCM::Polynomial auth_key = GCM::Polynomial::from_gcm_bytes(key);

  for (const auto &v : {associated_data, ciphertext}) {
    for (std::size_t i = 0; i < v.size(); i += key.size()) {
      std::vector<std::uint8_t>::const_iterator end;
      if (i + key.size() >= v.size()) {
        end = v.end();
      } else {
        end = v.begin() + i + key.size();
      }

      std::vector<std::uint8_t> block(v.begin() + i, end);
      block.resize(key.size(), 0);
      auth_tag += GCM::Polynomial::from_gcm_bytes(block);
      auth_tag *= auth_key;
    }
  }

  std::vector<std::uint8_t> length;
  ByteManipulation::append_as_bytes<std::uint64_t>(associated_data.size() * 8,
                                                   std::endian::big, length);
  ByteManipulation::append_as_bytes<std::uint64_t>(ciphertext.size() * 8,
                                                   std::endian::big, length);
  auth_tag += GCM::Polynomial::from_gcm_bytes(length);
  auth_tag *= auth_key;

  return auth_tag.to_gcm_bytes();
}