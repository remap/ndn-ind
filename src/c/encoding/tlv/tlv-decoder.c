/**
 * Copyright (C) 2014 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * Derived from tlv.hpp by Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include "../../util/endian.h"
#include "tlv-decoder.h"

ndn_Error 
ndn_TlvDecoder_readExtendedVarNumber(struct ndn_TlvDecoder *self, unsigned int firstOctet, uint64_t *varNumber)
{
  // This is a private function so we know firstOctet >= 253.
  if (firstOctet == 253) {
    if (self->offset + 2 > self->inputLength)
      return NDN_ERROR_read_past_the_end_of_the_input;

    // kind of dangerous... but should be efficient.
    uint16_t value = *(uint16_t *)(self->input + self->offset);
    *varNumber = (uint64_t)be16toh(value);
    self->offset += 2;
  }
  else if (firstOctet == 254) {
    if (self->offset + 4 > self->inputLength)
      return NDN_ERROR_read_past_the_end_of_the_input;

    // kind of dangerous... but should be efficient.
    uint32_t value = *(uint32_t *)(self->input + self->offset);
    *varNumber = (uint64_t)be32toh(value);
    self->offset += 4;
  }
  else {
    // value == 255.
    if (self->offset + 8 > self->inputLength)
      return NDN_ERROR_read_past_the_end_of_the_input;

    // kind of dangerous... but should be efficient.
    uint64_t value = *(uint64_t *)(self->input + self->offset);
    *varNumber = be64toh(value);
    self->offset += 8;
  }

  return NDN_ERROR_success;
}

ndn_Error 
ndn_TlvDecoder_readTypeAndLength(struct ndn_TlvDecoder *self, unsigned int expectedType, size_t *length)
{
  ndn_Error error;
  uint64_t type;
  if ((error = ndn_TlvDecoder_readVarNumber(self, &type)))
    return error;

  if (type != (uint64_t)expectedType)
    return NDN_ERROR_did_not_get_the_expected_TLV_type;
  
  uint64_t lengthVarNumber;
  if ((error = ndn_TlvDecoder_readVarNumber(self, &lengthVarNumber)))
    return error;
  
  // Silently ignore if the length is larger than size_t.
  *length = (size_t)lengthVarNumber;
  if (self->offset + *length > self->inputLength)
    return NDN_ERROR_TLV_length_exceeds_buffer_length;
  
  return NDN_ERROR_success;  
}

ndn_Error 
ndn_TlvDecoder_readExtendedNonNegativeInteger(struct ndn_TlvDecoder *self, size_t length, uint64_t *integer)
{
  // This is a private function so we know length != 1.
  switch (length) {
    case 2:
      if (self->offset + 2 > self->inputLength)
        return NDN_ERROR_read_past_the_end_of_the_input;
      
      {
        // kind of dangerous... but should be efficient.
        uint16_t value = *(uint16_t *)(self->input + self->offset);
        *integer = (uint64_t)be16toh(value);
      }
      self->offset += 2;
      return NDN_ERROR_success;
    case 4:
      if (self->offset + 4 > self->inputLength)
        return NDN_ERROR_read_past_the_end_of_the_input;
      
      {
        // kind of dangerous... but should be efficient.
        uint32_t value = *(uint32_t *)(self->input + self->offset);
        *integer = (uint64_t)be32toh(value);
      }
      self->offset += 4;
      return NDN_ERROR_success;
    case 8:
      if (self->offset + 8 > self->inputLength)
        return NDN_ERROR_read_past_the_end_of_the_input;
      
      {
        // kind of dangerous... but should be efficient.
        uint64_t value = *(uint64_t *)(self->input + self->offset);
        *integer = be64toh(value);
      }
      self->offset += 8;
      return NDN_ERROR_success;
  }
  
  return NDN_ERROR_Invalid_length_for_nonNegativeInteger;
}