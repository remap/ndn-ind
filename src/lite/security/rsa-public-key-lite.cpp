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

#include "../../c/security/rsa-public-key.h"
#include <ndn-cpp/lite/security/rsa-public-key-lite.hpp>

namespace ndn {

RsaPublicKeyLite::RsaPublicKeyLite()
{
  ndn_RsaPublicKey_initialize(this);
}

RsaPublicKeyLite::~RsaPublicKeyLite()
{
  ndn_RsaPublicKey_finalize(this);
}

ndn_Error
RsaPublicKeyLite::decode
  (const uint8_t* publicKeyDer, size_t publicKeyDerLength)
{
  return ndn_RsaPublicKey_decode(this, publicKeyDer, publicKeyDerLength);
}

bool
RsaPublicKeyLite::verifyWithSha256
  (const uint8_t* signature, size_t signatureLength, const uint8_t* data,
   size_t dataLength) const
{
  return ndn_RsaPublicKey_verifyWithSha256
    (this, signature, signatureLength, data, dataLength) != 0;
}

ndn_Error
RsaPublicKeyLite::encrypt
  (const uint8_t* plainData, size_t plainDataLength,
   ndn_EncryptAlgorithmType algorithmType, const uint8_t* encryptedData,
   size_t& encryptedDataLength) const
{
  return ndn_RsaPublicKey_encrypt
    (this, plainData, plainDataLength, algorithmType, encryptedData,
     &encryptedDataLength);
}

}