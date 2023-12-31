#include <algorithm>
#include <cassert>
#include <cppcodec/base64_default_rfc4648.hpp>
#include <iostream>
#include <vector>

#include "bytemanipulation.hpp"
#include "gcm/cantor_zassenhaus/factorize.hpp"
#include "gcm/ghash.hpp"
#include "gcm/polynomial.hpp"
#include "gcm/recover.hpp"

std::vector<std::uint8_t> GCM::Recovery::recover_auth_tag(
    GCM::EncryptionResult msg1, GCM::EncryptionResult msg2,
    GCM::EncryptionResult msg3, GCM::EncryptionResult msg4) {
  GCM::CantorZassenhaus::Polynomial f = GCM::Recovery::gen_poly(msg1, msg2);
  std::cerr << "f = " << f << "\n";
  std::vector<GCM::Polynomial> h_candidates = GCM::CantorZassenhaus::zeros(f);

  std::cerr << "Searching for valid h using msg3 auth tag "
            << cppcodec::base64_rfc4648::encode(msg3.auth_tag) << "\n";
  while (h_candidates.size() > 0 &&
         GCM::Recovery::gen_auth_tag(msg1, msg3, h_candidates.back()) !=
             msg3.auth_tag) {
    h_candidates.pop_back();
  }
  assert(h_candidates.size() > 0 && "No candidates for H found.");

  std::cerr << "Valid h = " << h_candidates.back() << "\n";
  return GCM::Recovery::gen_auth_tag(msg1, msg4, h_candidates.back());
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

GCM::CantorZassenhaus::Polynomial
GCM::Recovery::gen_poly(GCM::EncryptionResult msg1,
                        GCM::EncryptionResult msg2) {
  auto tu = GCM::Polynomial::from_gcm_bytes(msg1.auth_tag);
  auto tv = GCM::Polynomial::from_gcm_bytes(msg2.auth_tag);
  std::vector<GCM::Polynomial> msg1_polys =
      GCM::Recovery::as_ghash_polys(msg1.ciphertext, msg1.associated_data);
  std::cerr << "msg1 GHASH blocks:";
  for (auto &poly : msg1_polys)
    std::cerr << " " << poly;
  std::cerr << "\n";
  std::vector<GCM::Polynomial> msg2_polys =
      GCM::Recovery::as_ghash_polys(msg2.ciphertext, msg2.associated_data);
  std::cerr << "msg2 GHASH blocks:";
  for (auto &poly : msg2_polys)
    std::cerr << " " << poly;
  std::cerr << "\n";
  std::size_t degree = std::max(msg1_polys.size(), msg2_polys.size());
  GCM::CantorZassenhaus::Polynomial f(
      std::vector<GCM::Polynomial>(degree + 1, GCM::Polynomial::zero()));
  f.coefficient(0) = tu - tv;
  for (std::size_t i = 1; i <= degree; ++i) {
    if (i <= msg1_polys.size()) {
      f.coefficient(i) += msg1_polys.at(msg1_polys.size() - i);
    }
    if (i <= msg2_polys.size())
      f.coefficient(i) -= msg2_polys.at(msg2_polys.size() - i);
  }
  return f;
}

std::vector<std::uint8_t>
GCM::Recovery::gen_auth_tag(GCM::EncryptionResult msg1,
                            GCM::EncryptionResult msg2, GCM::Polynomial h) {
  std::cerr << "Generating auth tag using h = " << h << "\n";

  std::vector<std::uint8_t> mask = GCM::Recovery::gen_auth_tag_mask(msg1, h);
  std::cerr << "\tmask = " << cppcodec::base64_rfc4648::encode(mask) << "\n";

  std::vector<std::uint8_t> tag =
      GCM::ghash(msg2.ciphertext, msg2.associated_data, h.to_gcm_bytes());
  std::cerr << "\ttag = " << cppcodec::base64_rfc4648::encode(tag) << "\n";

  std::transform(tag.begin(), tag.end(), mask.begin(), tag.begin(),
                 std::bit_xor<std::uint8_t>());
  std::cerr << "\tmask ^ tag = " << cppcodec::base64_rfc4648::encode(tag)
            << "\n";

  return tag;
}

std::vector<std::uint8_t>
GCM::Recovery::gen_auth_tag_mask(GCM::EncryptionResult msg, GCM::Polynomial h) {
  std::vector<std::uint8_t> raw_tag =
      GCM::ghash(msg.ciphertext, msg.associated_data, h.to_gcm_bytes());
  std::vector<std::uint8_t> mask = msg.auth_tag;
  std::transform(raw_tag.begin(), raw_tag.end(), mask.begin(), mask.begin(),
                 std::bit_xor<std::uint8_t>());
  return mask;
}