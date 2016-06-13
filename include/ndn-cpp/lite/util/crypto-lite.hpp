/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2016 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version, with the additional exemption that
 * compiling, linking, and/or using OpenSSL is allowed.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * A copy of the GNU Lesser General Public License is in the file COPYING.
 */

#ifndef NDN_CRYPTOLITE_HPP
#define NDN_CRYPTOLITE_HPP

#include "blob-lite.hpp"

namespace ndn {

/**
 * CryptoLite has static methods for basic cryptography operations.
 */
class CryptoLite {
public:
  /**
   * Compute the sha-256 digest of data.
   * @param data Pointer to the input byte array.
   * @param dataLength The length of data.
   * @param digest A pointer to a buffer of size ndn_SHA256_DIGEST_SIZE to
   * receive the digest.
   */
  static void
  digestSha256(const uint8_t *data, size_t dataLength, uint8_t *digest);

  /**
   * Compute the sha-256 digest of data.
   * @param data The input byte array.
   * @param digest A pointer to a buffer of size ndn_SHA256_DIGEST_SIZE to
   * receive the digest.
   */
  static void
  digestSha256(const BlobLite& data, uint8_t *digest)
  {
    digestSha256(data.buf(), data.size(), digest);
  }

  /**
   * Compute the HMAC with sha-256 of data, as defined in
   * http://tools.ietf.org/html/rfc2104#section-2 .
   * @param key A pointer to buffer with the key.
   * @param keyLength The length of key.
   * @param data A pointer to the input byte array.
   * @param dataLength The length of data.
   * @param digest A pointer to a buffer of size ndn_SHA256_DIGEST_SIZE to receive
   * the HMAC digest.
   */
  static void
  computeHmacWithSha256
    (const uint8_t *key, size_t keyLength, const uint8_t *data, size_t dataLength,
     uint8_t *digest);

  /**
   * Compute the HMAC with sha-256 of data, as defined in
   * http://tools.ietf.org/html/rfc2104#section-2 .
   * @param key The key.
   * @param data The input byte array.
   * @param digest A pointer to a buffer of size ndn_SHA256_DIGEST_SIZE to
   * receive the HMAC digest.
   */
  static void
  computeHmacWithSha256
    (const BlobLite& key, const BlobLite& data, uint8_t *digest)
  {
    computeHmacWithSha256(key.buf(), key.size(), data.buf(), data.size(), digest);
  }
};

}

#endif
