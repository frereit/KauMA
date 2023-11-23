#include <algorithm>
#include <cassert>
#include <vector>

#include "bytemanipulation.hpp"
#include "gcm/cantor_zassenhaus/factorize.hpp"
#include "gcm/ghash.hpp"
#include "gcm/polynomial.hpp"
#include "gcm/recover.hpp"

std::vector<std::uint8_t>
GCM::Recovery::recover_auth_tag(GCM::EncryptionResult msg1,
                                GCM::EncryptionResult msg2,
                                GCM::EncryptionResult unknown) {
  auto tu = GCM::Polynomial::from_gcm_bytes(msg1.auth_tag);
  auto tv = GCM::Polynomial::from_gcm_bytes(msg2.auth_tag);
  std::vector<GCM::Polynomial> msg1_polys =
      GCM::Recovery::as_ghash_polys(msg1.ciphertext, msg1.associated_data);
  std::vector<GCM::Polynomial> msg2_polys =
      GCM::Recovery::as_ghash_polys(msg2.ciphertext, msg2.associated_data);
  std::size_t degree = std::max(msg1_polys.size(), msg2_polys.size());
  GCM::CantorZassenhaus::Polynomial f(
      std::vector<GCM::Polynomial>(degree + 1, GCM::Polynomial(0)));
  f.coefficient(0) = tu - tv;
  for (std::size_t i = 1; i <= degree; ++i) {
    std::size_t index = degree - i;
    if (index < msg1_polys.size())
      f.coefficient(i) += msg1_polys.at(index);
    if (index < msg2_polys.size())
      f.coefficient(i) -= msg2_polys.at(index);
  }

  std::vector<GCM::Polynomial> h_candidates = GCM::CantorZassenhaus::zeros(f);
  assert(h_candidates.size() > 0 && "No candidates for H found.");

  // FIXME: Using the first H is a guess. We have no way of validating which H leads to the correct E(Y0).
  GCM::Polynomial h = h_candidates.front();

  // find E_k(Y0)
  std::vector<std::uint8_t> raw_msg1_tag =
      GCM::ghash(msg1.ciphertext, msg1.associated_data, h.to_gcm_bytes());
  std::vector<std::uint8_t> mask = msg1.auth_tag;
  std::transform(raw_msg1_tag.begin(), raw_msg1_tag.end(), mask.begin(),
                 mask.begin(), std::bit_xor<std::uint8_t>());

  // Calculate new auth tag
  std::vector<std::uint8_t> raw_tag =
      GCM::ghash(unknown.ciphertext, unknown.associated_data, h.to_gcm_bytes());

  std::transform(raw_tag.begin(), raw_tag.end(), mask.begin(), raw_tag.begin(),
                 std::bit_xor<std::uint8_t>());

  return raw_tag;
}

std::vector<GCM::Polynomial>
GCM::Recovery::as_ghash_polys(std::vector<std::uint8_t> ciphertext,
                              std::vector<std::uint8_t> associated_data) {
  std::vector<GCM::Polynomial> out;
  for (const auto &v : {associated_data, ciphertext}) {
    for (std::size_t i = 0; i < v.size(); i += 16) {
      std::vector<std::uint8_t>::const_iterator end;
      if (i + 16 >= v.size()) {
        end = v.end();
      } else {
        end = v.begin() + i + 16;
      }

      std::vector<std::uint8_t> block(v.begin() + i, end);
      block.resize(16, 0);
      out.push_back(GCM::Polynomial::from_gcm_bytes(block));
    }
  }

  std::vector<std::uint8_t> length;
  ByteManipulation::append_as_bytes<std::uint64_t>(associated_data.size() * 8,
                                                   std::endian::big, length);
  ByteManipulation::append_as_bytes<std::uint64_t>(ciphertext.size() * 8,
                                                   std::endian::big, length);
  out.push_back(GCM::Polynomial::from_gcm_bytes(length));
  return out;
}
