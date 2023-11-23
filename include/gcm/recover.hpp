#pragma once
#include <cstdint>
#include <vector>

#include "gcm/cantor_zassenhaus/polynomial.hpp"
#include "gcm/encryptor.hpp"

namespace GCM::Recovery {
/// @brief Recover a valid auth tag
/// @param msg1 A message with ciphertext, associated data and auth tag
/// @param msg2 Another message with ciphertext, associated data and auth tag
/// @param unknown A message with ciphertext and associated data, but no auth
/// tag.
/// @return the auth tag for \p unknown .
std::vector<std::uint8_t> recover_auth_tag(GCM::EncryptionResult msg1,
                                           GCM::EncryptionResult msg2,
                                           GCM::EncryptionResult unknown);

/// @brief Convert a ciphertext and associated data to a list of Polynomials
/// @param ciphertext the ciphertext to use for GHASH
/// @param associated_data the associated data for GHASH
/// @return the polynomials, in the order that they were used in
std::vector<GCM::Polynomial>
as_ghash_polys(std::vector<std::uint8_t> ciphertext,
               std::vector<std::uint8_t> associated_data);
} // namespace GCM::Recovery