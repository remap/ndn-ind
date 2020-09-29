/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (C) 2020 Operant Networks, Incorporated.
 * @author: Jeff Thompson <jefft0@gmail.com>
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

/**
 * This example receives a secured interest which was created with Name-based
 * Access Control using a group content key (GCK). When this receives the
 * secured interest, it gets the GCK from the access manager and uses it to
 * decrypt the message which was sent in the Interest's ApplicationParameters
 * field. Then this creates a response message, encrypts it and sends a response
 * Data packet. This example works with test-access-manager and
 * test-secured-interest-sender.
 */

#include <cstdlib>
#include <iostream>
#include <time.h>
#include <unistd.h>
#include <ndn-ind/face.hpp>
#include <ndn-ind/security/validator-null.hpp>
#include <ndn-ind/encrypt/encryptor-v2.hpp>
#include <ndn-ind/encrypt/decryptor-v2.hpp>

using namespace std;
using namespace ndn;

/**
 * Add a hard-coded identity to the KeyChain for the responder and return its
 * identity name. In a production application, this would simply access the
 * identity in the KeyChain on disk.
 * @param keyChain The KeyChain for the identity.
 * @return The identity name.
 */
static Name
getResponderName(KeyChain& keyChain)
{
  const uint8_t secondMemberSafeBagEncoding[] = {
0x80, 0xfd, 0x07, 0xd3, 0x06, 0xfd, 0x02, 0xb9, 0x07, 0x2e, 0x08, 0x06, 0x73, 0x65, 0x63, 0x6f,
0x6e, 0x64, 0x08, 0x04, 0x75, 0x73, 0x65, 0x72, 0x08, 0x03, 0x4b, 0x45, 0x59, 0x08, 0x08, 0x46,
0x7e, 0xa8, 0xc5, 0xf6, 0x5c, 0xb7, 0x55, 0x08, 0x04, 0x73, 0x65, 0x6c, 0x66, 0x08, 0x09, 0xfd,
0x00, 0x00, 0x01, 0x74, 0xb1, 0x7b, 0xd9, 0xd9, 0x14, 0x09, 0x18, 0x01, 0x02, 0x19, 0x04, 0x00,
0x36, 0xee, 0x80, 0x15, 0xfd, 0x01, 0x26, 0x30, 0x82, 0x01, 0x22, 0x30, 0x0d, 0x06, 0x09, 0x2a,
0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x01, 0x05, 0x00, 0x03, 0x82, 0x01, 0x0f, 0x00, 0x30,
0x82, 0x01, 0x0a, 0x02, 0x82, 0x01, 0x01, 0x00, 0xb3, 0x8c, 0x40, 0x89, 0xf4, 0x5d, 0x0b, 0xc6,
0x97, 0x4c, 0x6c, 0x50, 0x54, 0xa1, 0x05, 0x86, 0x46, 0x57, 0x7e, 0x57, 0xc0, 0x0d, 0xb0, 0xf6,
0xae, 0xc1, 0x12, 0x48, 0x4a, 0x4d, 0x78, 0x75, 0x9d, 0xae, 0x2c, 0x11, 0xed, 0xc9, 0xce, 0x97,
0x01, 0xad, 0x32, 0xff, 0x35, 0x2f, 0x53, 0xc3, 0x58, 0xe6, 0x41, 0xa6, 0xaa, 0x1c, 0xbf, 0xc5,
0x25, 0x0e, 0x2d, 0xe7, 0x19, 0xb3, 0x6a, 0x8d, 0xee, 0xe6, 0x8c, 0x01, 0xa2, 0xe1, 0x83, 0x31,
0x17, 0xfe, 0xaf, 0x11, 0xa6, 0x07, 0x0b, 0x79, 0xa3, 0xd9, 0xb1, 0x07, 0xca, 0xe4, 0x32, 0x3e,
0xe7, 0x39, 0x95, 0x36, 0x36, 0xd9, 0xd7, 0x08, 0xaa, 0xc3, 0x94, 0x71, 0xbb, 0x94, 0x89, 0xd8,
0x3f, 0x4a, 0xb7, 0xc2, 0x9a, 0x9a, 0x91, 0xa5, 0xa7, 0x11, 0x40, 0x3f, 0xca, 0x6c, 0xb2, 0x63,
0x41, 0x34, 0xb7, 0xde, 0x14, 0x40, 0xbc, 0x7d, 0x0e, 0x86, 0x30, 0xad, 0x80, 0x54, 0x8f, 0x84,
0xf3, 0x9c, 0x82, 0x86, 0xf1, 0xcb, 0x5a, 0xa1, 0x92, 0xa2, 0x70, 0x48, 0xa2, 0x82, 0x56, 0x04,
0x9f, 0x82, 0x21, 0x55, 0xeb, 0x9a, 0xd3, 0x4d, 0x2b, 0x29, 0x44, 0x90, 0x3f, 0xa5, 0x80, 0x8f,
0xad, 0xa8, 0x90, 0x71, 0x85, 0x36, 0xd4, 0x75, 0x3b, 0x4b, 0x52, 0x0d, 0xa4, 0x57, 0x1a, 0x53,
0xef, 0x04, 0x35, 0x40, 0x30, 0x0f, 0xc0, 0x93, 0x5c, 0x87, 0x15, 0x7d, 0x11, 0xf4, 0xb8, 0xa8,
0xe4, 0x62, 0xdb, 0x85, 0xc4, 0xe2, 0xf1, 0x8a, 0x43, 0xdb, 0x01, 0x9f, 0x9a, 0xdb, 0x46, 0xab,
0xd1, 0xd4, 0x07, 0xaa, 0x4b, 0xf8, 0xb1, 0xe8, 0xaa, 0x80, 0x82, 0xc6, 0x06, 0x14, 0xb6, 0x08,
0x85, 0x7b, 0xb3, 0xfc, 0xb0, 0x2a, 0x68, 0x1d, 0xe9, 0xac, 0xeb, 0xf8, 0x93, 0xea, 0x3b, 0x67,
0x49, 0x10, 0x79, 0x11, 0x56, 0x5b, 0x2d, 0x63, 0x02, 0x03, 0x01, 0x00, 0x01, 0x16, 0x4e, 0x1b,
0x01, 0x01, 0x1c, 0x1f, 0x07, 0x1d, 0x08, 0x06, 0x73, 0x65, 0x63, 0x6f, 0x6e, 0x64, 0x08, 0x04,
0x75, 0x73, 0x65, 0x72, 0x08, 0x03, 0x4b, 0x45, 0x59, 0x08, 0x08, 0x46, 0x7e, 0xa8, 0xc5, 0xf6,
0x5c, 0xb7, 0x55, 0xfd, 0x00, 0xfd, 0x26, 0xfd, 0x00, 0xfe, 0x0f, 0x32, 0x30, 0x32, 0x30, 0x30,
0x39, 0x32, 0x31, 0x54, 0x31, 0x36, 0x32, 0x35, 0x31, 0x39, 0xfd, 0x00, 0xff, 0x0f, 0x32, 0x30,
0x34, 0x30, 0x30, 0x39, 0x31, 0x36, 0x54, 0x31, 0x36, 0x32, 0x35, 0x31, 0x38, 0x17, 0xfd, 0x01,
0x00, 0xa2, 0xc8, 0xc5, 0x17, 0x26, 0x46, 0x89, 0x01, 0x29, 0x15, 0xb9, 0x5c, 0x84, 0x38, 0x03,
0x54, 0xc9, 0x9e, 0x62, 0x19, 0xa6, 0xaa, 0x43, 0xac, 0xcb, 0x32, 0x8d, 0xd2, 0x1b, 0x8d, 0x47,
0x24, 0xbf, 0x49, 0x54, 0xb4, 0x1c, 0x40, 0x57, 0x88, 0x2a, 0x83, 0x61, 0xa5, 0x58, 0x3c, 0x74,
0x35, 0x61, 0x23, 0x75, 0x67, 0x4c, 0xfc, 0x7f, 0xcf, 0x48, 0x1f, 0x41, 0x16, 0xb8, 0x70, 0x1f,
0x91, 0xfe, 0xa0, 0x16, 0x76, 0x6c, 0xc7, 0x7a, 0xf0, 0xcc, 0x14, 0xb9, 0xd5, 0xed, 0x19, 0xe9,
0xec, 0xa0, 0x88, 0xa7, 0xb3, 0xc0, 0xe2, 0xd6, 0x71, 0x22, 0xa8, 0x70, 0xfa, 0x64, 0x54, 0x1b,
0x46, 0x2e, 0x20, 0xd0, 0x39, 0xc8, 0x2f, 0xb8, 0x70, 0xdc, 0x81, 0xe6, 0x70, 0xd5, 0x6f, 0x6e,
0x94, 0x75, 0xee, 0xd9, 0xd3, 0x75, 0x74, 0xfe, 0x87, 0xaa, 0x25, 0x29, 0x71, 0xbd, 0x62, 0xb7,
0x70, 0x22, 0x30, 0x4a, 0x69, 0xed, 0x07, 0x12, 0xab, 0x21, 0x84, 0xb1, 0x1f, 0x79, 0xce, 0xce,
0x9a, 0x0a, 0x55, 0x1d, 0x16, 0xf7, 0x3c, 0x9a, 0xd2, 0x52, 0x8b, 0x93, 0xb1, 0x82, 0xda, 0xdd,
0x69, 0xf2, 0xcc, 0x69, 0xfd, 0x80, 0x26, 0x64, 0xb8, 0xe5, 0x81, 0xd3, 0x93, 0xb0, 0xdc, 0xe0,
0x87, 0xa8, 0x52, 0x39, 0x02, 0xa3, 0x38, 0xd2, 0x4b, 0x11, 0x64, 0x78, 0xff, 0x18, 0x65, 0x11,
0xb1, 0x92, 0xcb, 0x37, 0x29, 0xdd, 0x85, 0x67, 0x79, 0x20, 0x73, 0xa0, 0xf0, 0xce, 0xfe, 0x45,
0xe1, 0x85, 0xbc, 0xb6, 0x46, 0x14, 0x9c, 0xb7, 0xa1, 0xca, 0xa8, 0x8c, 0x9d, 0xcf, 0xd1, 0x70,
0x85, 0x31, 0x42, 0x64, 0xc6, 0x87, 0x95, 0x9f, 0x01, 0x32, 0xcc, 0x3a, 0x44, 0x14, 0xce, 0x20,
0xa1, 0x4a, 0xa3, 0x49, 0x6c, 0xc1, 0x25, 0xd5, 0x10, 0x7e, 0x62, 0x4b, 0xa1, 0x7a, 0x8e, 0x0f,
0x07, 0x81, 0xfd, 0x05, 0x12, 0x30, 0x82, 0x05, 0x0e, 0x30, 0x40, 0x06, 0x09, 0x2a, 0x86, 0x48,
0x86, 0xf7, 0x0d, 0x01, 0x05, 0x0d, 0x30, 0x33, 0x30, 0x1b, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86,
0xf7, 0x0d, 0x01, 0x05, 0x0c, 0x30, 0x0e, 0x04, 0x08, 0x4a, 0x4f, 0x72, 0xab, 0x2f, 0xe1, 0xa5,
0x27, 0x02, 0x02, 0x08, 0x00, 0x30, 0x14, 0x06, 0x08, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x03,
0x07, 0x04, 0x08, 0x14, 0x09, 0x60, 0x5e, 0x47, 0x3d, 0x41, 0xf3, 0x04, 0x82, 0x04, 0xc8, 0x71,
0x97, 0x5b, 0x36, 0x13, 0xeb, 0xf8, 0x15, 0xa0, 0x72, 0xb3, 0x4d, 0x37, 0xf8, 0xd8, 0x89, 0xda,
0x41, 0x46, 0x51, 0xb0, 0x14, 0xab, 0x73, 0xa3, 0x50, 0xe5, 0x8e, 0x4c, 0x1e, 0xb3, 0x27, 0x0a,
0xb6, 0xfd, 0x65, 0xd7, 0xa9, 0x41, 0x5e, 0x15, 0xa6, 0xb5, 0x99, 0xec, 0xe0, 0x47, 0x7c, 0x91,
0x84, 0x0c, 0xca, 0x5d, 0x67, 0x46, 0xff, 0x51, 0x44, 0xbf, 0x70, 0x56, 0xa3, 0x41, 0x28, 0xe6,
0x9b, 0xaf, 0x31, 0xf1, 0xa7, 0xed, 0xd5, 0xca, 0x8f, 0x67, 0x88, 0x9a, 0x88, 0xb1, 0x0a, 0xff,
0xc5, 0x0b, 0xeb, 0xc9, 0xdd, 0x27, 0xb8, 0x94, 0x61, 0xa4, 0x43, 0xcd, 0xe6, 0x9a, 0xde, 0xab,
0xb3, 0xac, 0x1d, 0x11, 0x5d, 0x5d, 0x72, 0x33, 0xa5, 0xf8, 0xb9, 0x6b, 0x22, 0x1e, 0xd8, 0xcd,
0xa4, 0xf5, 0x36, 0xc6, 0xfd, 0xd4, 0xac, 0x8c, 0x06, 0xe6, 0x63, 0xe7, 0xda, 0x9b, 0xec, 0xeb,
0x13, 0x5b, 0x0a, 0x6c, 0x64, 0x6e, 0x9f, 0x67, 0xd7, 0x51, 0x71, 0x4e, 0x3f, 0x3a, 0xbc, 0x96,
0x90, 0xd2, 0x5e, 0x56, 0x12, 0xf9, 0x6c, 0x55, 0xa7, 0xce, 0x75, 0xc3, 0xe3, 0xdf, 0x74, 0x85,
0x62, 0x42, 0xb2, 0x8a, 0x78, 0xd3, 0xe6, 0x62, 0xf2, 0x10, 0xbc, 0x80, 0xb5, 0x7a, 0xee, 0xad,
0xde, 0x6a, 0x2b, 0x44, 0xc9, 0x8f, 0xaf, 0x1d, 0xfa, 0x1c, 0xfc, 0x35, 0x67, 0x73, 0x85, 0x9a,
0x49, 0x8c, 0xe5, 0x24, 0xca, 0xd9, 0x33, 0x06, 0x73, 0xb9, 0xf2, 0xfd, 0x75, 0x8d, 0x3e, 0x79,
0x04, 0xaf, 0x33, 0x42, 0xda, 0xce, 0x6d, 0x02, 0x1d, 0x99, 0xbe, 0x7a, 0x8b, 0x09, 0xac, 0x60,
0x71, 0x24, 0x2d, 0x66, 0x4a, 0xe2, 0xcf, 0x14, 0xf4, 0x22, 0x93, 0x3a, 0x9e, 0x60, 0x2f, 0x85,
0xfb, 0x92, 0x08, 0x0e, 0x62, 0xe5, 0x8f, 0x83, 0x05, 0x9c, 0xc2, 0x73, 0x43, 0x47, 0x9c, 0x2e,
0xa3, 0x2f, 0x6e, 0x40, 0x49, 0x7a, 0x0a, 0x10, 0x15, 0x72, 0xd5, 0xca, 0xfd, 0x34, 0xff, 0xaf,
0x1d, 0x03, 0x8a, 0x03, 0xcc, 0xee, 0xc4, 0x3b, 0x98, 0x18, 0x36, 0xff, 0x4f, 0xaa, 0x5a, 0x01,
0x5a, 0xed, 0x50, 0xc9, 0x4b, 0x98, 0x5d, 0xb3, 0x2b, 0x55, 0x48, 0x1c, 0xa3, 0x28, 0x0e, 0x55,
0x20, 0xd5, 0xb2, 0x1c, 0x88, 0xa8, 0x48, 0xa2, 0xc0, 0xe5, 0xff, 0x36, 0x6c, 0xb8, 0x86, 0x07,
0xce, 0x8b, 0xd8, 0xe0, 0x78, 0x9a, 0x8b, 0x8a, 0xcc, 0x25, 0x3d, 0xc3, 0xae, 0xb7, 0xdf, 0xd1,
0x73, 0xe5, 0xa2, 0xc2, 0x5e, 0xe2, 0x1f, 0x81, 0x3e, 0x43, 0x90, 0xa6, 0x0b, 0x0c, 0xc2, 0x12,
0xba, 0x1c, 0xd0, 0x13, 0x66, 0x2f, 0x86, 0x46, 0x8a, 0xee, 0xaf, 0xa9, 0x20, 0xf8, 0xd0, 0x12,
0xd2, 0xb0, 0xc4, 0x90, 0x0c, 0x05, 0xa3, 0x6f, 0x27, 0xfd, 0x02, 0xc0, 0x9f, 0x25, 0xa1, 0xd0,
0xf6, 0x6f, 0x35, 0x5c, 0x9e, 0x6f, 0x73, 0xce, 0xe2, 0xbd, 0x2f, 0x98, 0x46, 0x8e, 0x88, 0xd4,
0x9c, 0x9d, 0x83, 0xd7, 0xa8, 0x38, 0x1f, 0x86, 0x2b, 0x49, 0x46, 0x17, 0x2c, 0x1b, 0x58, 0x74,
0x7f, 0xd4, 0xbb, 0x20, 0x67, 0x29, 0xdd, 0x90, 0xd2, 0xf5, 0x68, 0x05, 0x97, 0x01, 0xa2, 0x8d,
0x43, 0xb7, 0x0e, 0x79, 0x08, 0x6b, 0x09, 0x14, 0x97, 0x35, 0x49, 0xc9, 0x1d, 0x6f, 0xf8, 0x32,
0xaf, 0x42, 0xcd, 0xbc, 0xa6, 0xf1, 0xe5, 0x4e, 0xb3, 0x20, 0x85, 0x8a, 0x12, 0xfe, 0x82, 0xe1,
0x54, 0xf6, 0x05, 0xce, 0xc1, 0xd2, 0x57, 0x01, 0x2b, 0xc1, 0xc5, 0xbc, 0x3c, 0xfe, 0xee, 0x19,
0x1a, 0x13, 0xa6, 0xa5, 0x99, 0x52, 0xd9, 0x84, 0x8f, 0x2a, 0x6d, 0x01, 0x8f, 0x22, 0x0e, 0x78,
0x2e, 0x7e, 0x34, 0xdf, 0xa8, 0x28, 0x2d, 0x46, 0x08, 0x14, 0x7a, 0xb0, 0x7c, 0x1c, 0x0a, 0x76,
0x73, 0xc4, 0x0b, 0xc4, 0xeb, 0xc6, 0x21, 0x6b, 0x37, 0x50, 0x2e, 0xcc, 0x68, 0x87, 0x34, 0x12,
0x8d, 0x9d, 0x82, 0xe2, 0x5f, 0x3c, 0x1f, 0x07, 0xd3, 0x1c, 0x6a, 0x50, 0x43, 0xe2, 0xa1, 0xba,
0x40, 0x9d, 0xb1, 0xcf, 0x40, 0x76, 0xe7, 0x7c, 0xe3, 0x0e, 0x86, 0x38, 0x67, 0x5a, 0x1e, 0x7e,
0x7f, 0x91, 0x7a, 0x9b, 0x05, 0x36, 0xc7, 0x92, 0x4f, 0xf0, 0x56, 0x23, 0x46, 0x11, 0xf4, 0x2f,
0x5e, 0x8d, 0x64, 0x5e, 0x82, 0x7f, 0x97, 0x5b, 0xfe, 0xd9, 0xf3, 0xc0, 0x6d, 0x5a, 0x79, 0xe0,
0x77, 0x15, 0x23, 0x83, 0x78, 0xf0, 0x88, 0xbc, 0x77, 0x41, 0x89, 0x46, 0x4a, 0xab, 0xb8, 0xaa,
0x0d, 0x1e, 0x80, 0x3b, 0x59, 0xcc, 0xbf, 0x43, 0xec, 0xfc, 0xd7, 0x15, 0xdb, 0xe9, 0xbc, 0x4d,
0xd0, 0x4e, 0x2c, 0x91, 0x38, 0x21, 0xfd, 0xe5, 0x80, 0xaf, 0x54, 0x68, 0x52, 0x12, 0x71, 0x22,
0x39, 0x41, 0x45, 0xac, 0x9c, 0x3d, 0xa7, 0x2e, 0xbf, 0x02, 0x93, 0xc0, 0x05, 0x4d, 0x45, 0xa0,
0xbf, 0xf9, 0xcc, 0x5f, 0xa9, 0xb7, 0x56, 0x62, 0x41, 0x6c, 0x09, 0xd0, 0xfa, 0xae, 0x6a, 0x9d,
0xb7, 0x8c, 0x35, 0x3e, 0x4a, 0xeb, 0xdc, 0xfc, 0x37, 0x1f, 0xbd, 0xb3, 0xbe, 0x05, 0x93, 0xbb,
0x77, 0xa4, 0x00, 0xba, 0x3b, 0x5e, 0x6b, 0xb5, 0x7d, 0x6b, 0x53, 0x3c, 0x95, 0xb7, 0xbf, 0x06,
0x12, 0x0b, 0x68, 0x59, 0x74, 0xdb, 0xde, 0xb6, 0x84, 0x5d, 0xf2, 0x04, 0x1f, 0x88, 0xb4, 0xd8,
0xf5, 0x4a, 0x8f, 0xd1, 0x26, 0x0d, 0xe8, 0xf1, 0xc1, 0xfa, 0x8a, 0x45, 0xc5, 0xc7, 0x1f, 0x82,
0x41, 0xa4, 0x0e, 0xe9, 0x1c, 0xdf, 0x60, 0x94, 0xe0, 0x44, 0x51, 0x29, 0xee, 0x73, 0xc4, 0xa5,
0xba, 0xbc, 0xc0, 0xe2, 0xb3, 0xb7, 0x3a, 0x55, 0xb9, 0x83, 0x91, 0xa5, 0x52, 0xdb, 0x33, 0xca,
0x82, 0x21, 0xca, 0xe0, 0x96, 0xf5, 0x83, 0x9d, 0x1f, 0x29, 0xcc, 0x45, 0x8b, 0x19, 0x55, 0xa9,
0x91, 0xdb, 0xdb, 0x58, 0x41, 0x81, 0xf6, 0xbc, 0xee, 0x68, 0x2e, 0xc1, 0x37, 0x89, 0xef, 0xbd,
0x19, 0x37, 0xae, 0xe1, 0x90, 0x54, 0x3e, 0x5d, 0x13, 0x3f, 0x8f, 0x6c, 0x12, 0xd0, 0xac, 0x28,
0x37, 0xcd, 0xca, 0xa7, 0xcc, 0x38, 0xf3, 0xf3, 0xea, 0x7b, 0x25, 0x24, 0xab, 0x92, 0x2c, 0xea,
0xab, 0x06, 0x81, 0xc1, 0xbd, 0x7e, 0xd9, 0x70, 0x84, 0xd9, 0x37, 0x2f, 0x34, 0xdc, 0x4c, 0x0a,
0x28, 0x98, 0x9d, 0x8c, 0x4d, 0x4f, 0x4b, 0x8f, 0x16, 0x55, 0xb2, 0x0f, 0x9d, 0x07, 0xf6, 0x4b,
0x6d, 0x43, 0xa6, 0x76, 0x84, 0x15, 0xd7, 0x75, 0x00, 0xd5, 0x71, 0x9c, 0x13, 0xdd, 0x2e, 0x7d,
0xf3, 0x9b, 0x38, 0x92, 0x14, 0xe5, 0xd5, 0xfc, 0x1f, 0xe5, 0x30, 0xa8, 0x3e, 0xf4, 0x9a, 0x87,
0x47, 0x99, 0xcf, 0x10, 0xec, 0xcb, 0xa5, 0x42, 0x33, 0x1e, 0xf7, 0x19, 0xe3, 0x9d, 0x7c, 0x01,
0xcb, 0x89, 0xf3, 0xde, 0xf1, 0xdd, 0x5e, 0x90, 0xad, 0xb6, 0x3e, 0x13, 0x9d, 0xb4, 0xd2, 0xcd,
0x26, 0x5b, 0x84, 0xf9, 0xe6, 0xf3, 0x1b, 0xb7, 0x47, 0xd1, 0x39, 0x44, 0x4e, 0xf0, 0x99, 0xce,
0x36, 0x05, 0x01, 0xf6, 0xd5, 0xd9, 0x0c, 0x18, 0x1c, 0x95, 0xcd, 0x0a, 0x6a, 0x09, 0x7d, 0xc7,
0xfd, 0x01, 0xff, 0x9f, 0x87, 0x83, 0xf0, 0x44, 0x33, 0x8f, 0x6d, 0x51, 0xae, 0x51, 0xa9, 0xf1,
0xb6, 0xdd, 0x76, 0x29, 0xfa, 0x57, 0x60, 0x61, 0xdc, 0xe4, 0xb9, 0x47, 0xcb, 0x80, 0x92, 0xb0,
0xad, 0x09, 0xcf, 0x09, 0xc6, 0x01, 0x51, 0xb0, 0xe6, 0x6b, 0x2d, 0xb5, 0xa8, 0x44, 0xe0, 0x8a,
0xb7, 0x2f, 0x43, 0x0c, 0xb5, 0x50, 0x20, 0xe1, 0xe6, 0xd9, 0x81, 0xee, 0x1b, 0xf0, 0xf1, 0x1b,
0x26, 0x9f, 0xd1, 0x75, 0x41, 0xa9, 0xaf, 0x66, 0x3f, 0x9d, 0x2b, 0x82, 0x95, 0xaf, 0xd1, 0xf5,
0x78, 0xea, 0xba, 0xec, 0xf4, 0x4f, 0xbe, 0x0c, 0x23, 0x84, 0x04, 0x33, 0xd4, 0x8c, 0x24, 0x54,
0x3c, 0xae, 0x20, 0xea, 0xf9, 0xe6, 0xde, 0x01, 0xd9, 0x5a, 0xbe, 0xb3, 0x38, 0x79, 0xd0, 0x40,
0xb3, 0x00, 0xd8, 0x89, 0x3e, 0x5e, 0x9e, 0x62, 0x87, 0x3a, 0xda, 0x9b, 0xdb, 0x4b, 0x3f, 0x1e,
0xbc, 0xe4, 0x7b, 0x32, 0x3b, 0x7d, 0x05, 0x51, 0x63, 0xc1, 0xc8, 0x3c, 0x96, 0xd5, 0xce, 0x60,
0xf2, 0xa5, 0x32, 0x35, 0xa1, 0x34, 0xbc, 0x75, 0x23, 0x99, 0xfb, 0x9a, 0x6f, 0x0f, 0x18, 0xa7,
0xfb, 0x19, 0xfb, 0x18, 0xe3, 0x24, 0x25, 0x3b, 0xe7, 0x8b, 0x0d, 0xba, 0x74, 0xc2, 0x14, 0x46,
0x1c, 0x08, 0xc2, 0x3e, 0x2e, 0xda, 0x05, 0x23, 0x95, 0x7d, 0x2e, 0x99, 0xf1, 0xc1, 0xaf, 0x2b,
0x29, 0x42, 0x40, 0x72, 0x3e, 0x9c, 0x9b, 0xcd, 0x70, 0x50, 0xc3, 0xcb, 0x21, 0x12, 0xad, 0x44,
0xc6, 0xce, 0x61, 0x0e, 0x9a, 0x73, 0xcd, 0xd1, 0xae, 0xf6, 0xc3, 0x03, 0x3d, 0x8a, 0xd4, 0xfd,
0x4b, 0x04, 0x79, 0xef, 0x4e, 0x35, 0xd2, 0x1b, 0xb7, 0x22, 0xd4, 0x83, 0xc8, 0xc2, 0x09, 0xad,
0xe4, 0x5c, 0x9f, 0x78, 0x2a, 0xce, 0x9a, 0x74, 0x6c, 0x86, 0xde, 0x07, 0x2d, 0x25, 0x9a, 0xab,
0xc2, 0x9a, 0x7f, 0x91, 0x9a, 0xdf, 0x3e
  };

  SafeBag secondMemberSafeBag
    (ndn::Blob(secondMemberSafeBagEncoding, sizeof(secondMemberSafeBagEncoding)));
  string safeBagPassword = "password";
  keyChain.importSafeBag
    (secondMemberSafeBag, (const uint8_t*)safeBagPassword.c_str(),
     safeBagPassword.size());
  return keyChain.getDefaultIdentity();
}

// Set this false to exit the application.
static bool isRunning = true;

static void
onInterest
  (const ptr_lib::shared_ptr<const Interest>& interest, Face* face,
   const ptr_lib::shared_ptr<EncryptorV2>& encryptor,
   const ptr_lib::shared_ptr<DecryptorV2>& decryptor, KeyChain* nacKeyChain)
{
  decryptor->decrypt
    (*interest,
     [=](auto& plainData) {
       cout << "Received message: " << plainData.toRawStr() << endl;

       // Make a response Data packet with encrypted content, sign and send it.
       auto data = ptr_lib::make_shared<Data>(interest->getName());
       Blob responseContent = Blob::fromRawStr(plainData.toRawStr() + " - response");
       data->setContent(responseContent);

       encryptor->encrypt
         (data,
          [=](auto&, auto&) {
            nacKeyChain->sign(*data);
            face->putData(*data);
            cout << "Sent response:    " << responseContent.toRawStr() << endl;
            isRunning = false;
          });
     },
     [](auto errorCode, const std::string& message) {
       cout << "DecryptorV2 error: " << message << endl;
       isRunning = false;
     });
}

int
main(int argc, char** argv)
{
  try {
    // Silence the warning from Interest wire encode.
    Interest::setDefaultCanBePrefix(true);

    KeyChain systemKeyChain;
    // The default Face will connect using a Unix socket, or to "localhost".
    Face face;
    face.setCommandSigningInfo
      (systemKeyChain, systemKeyChain.getDefaultCertificateName());

    // Create an in-memory key chain and get the decryptor identity.
    auto nacKeyChain = ptr_lib::make_shared<KeyChain>("pib-memory:", "tpm-memory:");
    Name responderName = getResponderName(*nacKeyChain);

    // In a production application, use a validator which has access to the
    // certificates of the access manager and the sender.
    auto validator = ptr_lib::make_shared<ValidatorNull>();
    // Assume the access manager is the default identity on this computer, the
    // same as in test-access-manager.
    auto accessManagerName =
      systemKeyChain.getPib().getIdentity(systemKeyChain.getDefaultIdentity())->getName();
    Name accessPrefix = Name(accessManagerName).append(Name("NAC/test-group"));
    // Create the DecryptorV2 to decrypt the secured Interest.
    auto decryptor = ptr_lib::make_shared<ndn::DecryptorV2>
      (nacKeyChain->getPib().getIdentity(responderName)->getDefaultKey().get(),
       validator.get(), nacKeyChain.get(), &face);
    // Create the EncryptorV2 to encrypt the reply Data packet.
    auto encryptor = ptr_lib::make_shared<EncryptorV2>
      (accessPrefix,
       [](auto errorCode, const std::string& message) {
         cout << "EncryptorV2 error: " << message << endl;
         isRunning = false;
       },
       nacKeyChain->getPib().getIdentity(responderName)->getDefaultKey().get(),
       validator.get(), nacKeyChain.get(), &face, ndn_EncryptAlgorithmType_AesCbc);

    Name messagePrefix("/test-secured-interest");
    face.registerPrefix(
      messagePrefix,
      [=](auto&, auto& interest, auto& interestFace, auto, auto&) {
        // Validate the Interest signature.
        validator->validate
          (*interest,
           [=, &interestFace](auto&) {
             // The Interest signature is valid. Now decrypt.
             onInterest(interest, &interestFace, encryptor, decryptor, nacKeyChain.get());
           },
           [](auto&, auto& error) { 
             cout << "Validate Interest failure: " << error.toString() << endl;
             isRunning = false;
           });
      },
      [](auto& p) { 
        cout << "Register failed for " << p->toUri() << endl;
        isRunning = false;
      });

    // The main event loop. Run until something sets isRunning false.
    while (isRunning) {
      face.processEvents();
      // We need to sleep for a few milliseconds so we don't use 100% of the CPU.
      usleep(10000);
    }
  } catch (std::exception& e) {
    cout << "exception: " << e.what() << endl;
  }
  return 0;
}
