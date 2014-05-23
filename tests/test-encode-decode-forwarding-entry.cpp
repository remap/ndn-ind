/**
 * Copyright (C) 2013-2014 Regents of the University of California.
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
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * A copy of the GNU General Public License is in the file COPYING.
 */

#include <cstdlib>
#include <iostream>
#include <ndn-cpp/interest.hpp>
#include <ndn-cpp/data.hpp>
#include <ndn-cpp/forwarding-entry.hpp>
#include <ndn-cpp/security/key-chain.hpp>
#include <ndn-cpp/encoding/binary-xml-wire-format.hpp>

using namespace std;
using namespace ndn;

uint8_t BinaryXmlInterest[] = {
0x01, 0xd2, 0xf2, 0xfa, 0xa5, 0x6e, 0x64, 0x6e, 0x78, 0x00, 0xfa, 0x02, 0x85, 0xe0, 0xa0, 0x1e, 0x09, 0x39, 0x68,
0xf9, 0x74, 0x0c, 0xe7, 0xf4, 0x36, 0x1b, 0xab, 0xf5, 0xbb, 0x05, 0xa4, 0xe5, 0x5a, 0xac, 0xa5, 0xe5, 0x8f, 0x73,
0xed, 0xde, 0xb8, 0xe0, 0x13, 0xaa, 0x8f, 0x00, 0xfa, 0xcd, 0x70, 0x72, 0x65, 0x66, 0x69, 0x78, 0x72, 0x65, 0x67,
0x00, 0xfa, 0x29, 0xad, 0x04, 0x82, 0x02, 0xaa, 0x03, 0xb2, 0x08, 0x85, 0x4d, 0xdb, 0xf6, 0x97, 0x79, 0xcd, 0xf4,
0xef, 0x74, 0xbe, 0x99, 0x47, 0x44, 0x78, 0xc8, 0xbc, 0x3c, 0xa0, 0x87, 0x3e, 0x0f, 0xfa, 0x1f, 0xa6, 0x01, 0x20,
0xaa, 0x27, 0x6d, 0xb1, 0x22, 0xb8, 0x34, 0x04, 0xe5, 0x95, 0xa8, 0xa3, 0xca, 0xea, 0xf0, 0x96, 0x30, 0x27, 0x66,
0x58, 0xba, 0x4e, 0x7b, 0xea, 0xad, 0xb4, 0xb9, 0x1a, 0x8c, 0xc5, 0x8e, 0x19, 0xac, 0x4a, 0x42, 0x28, 0x95, 0x07,
0xed, 0x8d, 0x60, 0x9a, 0xa9, 0xbe, 0xf6, 0x1a, 0x5a, 0x50, 0x7f, 0x34, 0x9c, 0x83, 0xd2, 0x94, 0x4b, 0x8c, 0x16,
0xfe, 0xcf, 0xd9, 0x0d, 0x4a, 0x40, 0xdd, 0xb8, 0x68, 0x75, 0x92, 0xc0, 0xa5, 0x75, 0x17, 0x56, 0x42, 0x35, 0xb2,
0xe3, 0x59, 0xdb, 0x54, 0xf5, 0x1a, 0x37, 0xe1, 0xac, 0x39, 0xe5, 0x18, 0xa2, 0x19, 0x6e, 0x3f, 0xfd, 0xa7, 0xeb,
0x2f, 0xb3, 0x01, 0xf3, 0xc4, 0x04, 0xdd, 0x00, 0x00, 0xf2, 0x00, 0x01, 0xa2, 0x03, 0xe2, 0x02, 0x85, 0xef, 0x7c,
0x4f, 0x5d, 0x47, 0x43, 0xa8, 0xb8, 0x58, 0x6e, 0xa2, 0xe7, 0x41, 0xb7, 0xfc, 0x39, 0xd1, 0xdc, 0x0d, 0xbe, 0x1b,
0x19, 0x30, 0xe7, 0x87, 0xcf, 0xd1, 0xd8, 0x33, 0xea, 0x7a, 0x61, 0x00, 0x02, 0xba, 0xb5, 0x04, 0xfc, 0xe9, 0xe2,
0x53, 0xd7, 0x00, 0x01, 0xe2, 0x01, 0x82, 0x19, 0xfd, 0x30, 0x82, 0x01, 0x9b, 0x30, 0x82, 0x01, 0x04, 0x02, 0x09,
0x00, 0xb7, 0xd8, 0x5c, 0x90, 0x6b, 0xad, 0x52, 0xee, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d,
0x01, 0x01, 0x05, 0x05, 0x00, 0x30, 0x12, 0x31, 0x10, 0x30, 0x0e, 0x06, 0x03, 0x55, 0x04, 0x03, 0x13, 0x07, 0x61,
0x78, 0x65, 0x6c, 0x63, 0x64, 0x76, 0x30, 0x1e, 0x17, 0x0d, 0x31, 0x32, 0x30, 0x34, 0x32, 0x38, 0x32, 0x33, 0x34,
0x34, 0x33, 0x37, 0x5a, 0x17, 0x0d, 0x31, 0x32, 0x30, 0x35, 0x32, 0x38, 0x32, 0x33, 0x34, 0x34, 0x33, 0x37, 0x5a,
0x30, 0x12, 0x31, 0x10, 0x30, 0x0e, 0x06, 0x03, 0x55, 0x04, 0x03, 0x13, 0x07, 0x61, 0x78, 0x65, 0x6c, 0x63, 0x64,
0x76, 0x30, 0x81, 0x9f, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x01, 0x05, 0x00,
0x03, 0x81, 0x8d, 0x00, 0x30, 0x81, 0x89, 0x02, 0x81, 0x81, 0x00, 0xe1, 0x7d, 0x30, 0xa7, 0xd8, 0x28, 0xab, 0x1b,
0x84, 0x0b, 0x17, 0x54, 0x2d, 0xca, 0xf6, 0x20, 0x7a, 0xfd, 0x22, 0x1e, 0x08, 0x6b, 0x2a, 0x60, 0xd1, 0x6c, 0xb7,
0xf5, 0x44, 0x48, 0xba, 0x9f, 0x3f, 0x08, 0xbc, 0xd0, 0x99, 0xdb, 0x21, 0xdd, 0x16, 0x2a, 0x77, 0x9e, 0x61, 0xaa,
0x89, 0xee, 0xe5, 0x54, 0xd3, 0xa4, 0x7d, 0xe2, 0x30, 0xbc, 0x7a, 0xc5, 0x90, 0xd5, 0x24, 0x06, 0x7c, 0x38, 0x98,
0xbb, 0xa6, 0xf5, 0xdc, 0x43, 0x60, 0xb8, 0x45, 0xed, 0xa4, 0x8c, 0xbd, 0x9c, 0xf1, 0x26, 0xa7, 0x23, 0x44, 0x5f,
0x0e, 0x19, 0x52, 0xd7, 0x32, 0x5a, 0x75, 0xfa, 0xf5, 0x56, 0x14, 0x4f, 0x9a, 0x98, 0xaf, 0x71, 0x86, 0xb0, 0x27,
0x86, 0x85, 0xb8, 0xe2, 0xc0, 0x8b, 0xea, 0x87, 0x17, 0x1b, 0x4d, 0xee, 0x58, 0x5c, 0x18, 0x28, 0x29, 0x5b, 0x53,
0x95, 0xeb, 0x4a, 0x17, 0x77, 0x9f, 0x02, 0x03, 0x01, 0x00, 0x01, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86,
0xf7, 0x0d, 0x01, 0x01, 0x05, 0x05, 0x00, 0x03, 0x81, 0x81, 0x00, 0xcb, 0x3a, 0xb0, 0x35, 0x7d, 0x7c, 0xd2, 0xae,
0x97, 0xed, 0x50, 0x1e, 0x51, 0xa3, 0xa0, 0xe2, 0x81, 0x7d, 0x41, 0x8e, 0x47, 0xfb, 0x17, 0x90, 0x64, 0x77, 0xaf,
0x61, 0x49, 0x5a, 0x7e, 0x8d, 0x87, 0x89, 0x14, 0x10, 0x65, 0xb0, 0x82, 0xd0, 0x01, 0xf4, 0xb1, 0x51, 0x93, 0xd0,
0xb4, 0x3f, 0xb6, 0x61, 0xcd, 0xe2, 0x0a, 0x64, 0x98, 0x37, 0x2c, 0x6a, 0xbb, 0xd3, 0xdc, 0xb9, 0xf0, 0xd1, 0x26,
0x59, 0xef, 0x07, 0xb3, 0xc6, 0xdb, 0xdf, 0x8b, 0xdf, 0x2f, 0x65, 0x47, 0x7e, 0xed, 0x7a, 0xdc, 0xd4, 0x57, 0xd7,
0x93, 0xb1, 0xc2, 0x7b, 0xad, 0xda, 0x7c, 0x5a, 0xde, 0x80, 0xce, 0x95, 0xb7, 0xd8, 0x82, 0x7f, 0xe7, 0x8c, 0x8a,
0x35, 0xf3, 0xfb, 0x4b, 0xa6, 0x48, 0xa0, 0x81, 0xbe, 0x2c, 0xfe, 0x84, 0x23, 0x1a, 0xba, 0xb3, 0xc2, 0xb5, 0x31,
0x74, 0x6d, 0xf2, 0xe0, 0x49, 0x2b, 0x00, 0x00, 0x00, 0x01, 0x9a, 0x02, 0xd5, 0x05, 0x8a, 0x04, 0xca, 0xbe, 0x73,
0x65, 0x6c, 0x66, 0x72, 0x65, 0x67, 0x00, 0xf2, 0xfa, 0xa5, 0x6d, 0x65, 0x6b, 0x69, 0x00, 0x00, 0x04, 0xfa, 0x8e,
0x33, 0x00, 0x03, 0xd2, 0xd6, 0x32, 0x31, 0x34, 0x37, 0x34, 0x38, 0x33, 0x36, 0x34, 0x37, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x02, 0xd2, 0x8e, 0x31, 0x00, 0x00,
1
};

static inline string toString(const vector<uint8_t>& v)
{
  if (!&v)
    return "";
  
  return string(&v[0], &v[0] + v.size());
}

static void dumpForwardingEntry(const ForwardingEntry& forwardingEntry) 
{
  cout << "action: " << forwardingEntry.getAction() << endl;
  cout << "prefix: " << forwardingEntry.getPrefix().toUri() << endl;
  cout << "publisherPublicKeyDigest: " 
       << (forwardingEntry.getPublisherPublicKeyDigest().getPublisherPublicKeyDigest().size() > 0 ? 
           forwardingEntry.getPublisherPublicKeyDigest().getPublisherPublicKeyDigest().toHex() : "<none>") << endl;
  cout << "faceId: ";
  if (forwardingEntry.getFaceId() >= 0)
    cout << forwardingEntry.getFaceId() << endl;
  else
    cout << "<none>" << endl;
  
  cout << "forwardingFlags:";
  if (forwardingEntry.getForwardingFlags().getActive())
    cout << " active";
  if (forwardingEntry.getForwardingFlags().getChildInherit())
    cout << " childInherit";
  if (forwardingEntry.getForwardingFlags().getAdvertise())
    cout << " advertise";
  if (forwardingEntry.getForwardingFlags().getLast())
    cout << " last";
  if (forwardingEntry.getForwardingFlags().getCapture())
    cout << " capture";
  if (forwardingEntry.getForwardingFlags().getLocal())
    cout << " local";
  if (forwardingEntry.getForwardingFlags().getTap())
    cout << " tap";
  if (forwardingEntry.getForwardingFlags().getCaptureOk())
    cout << " captureOk";
  cout << endl;
  
  cout << "freshnessPeriod (milliseconds): ";
  if (forwardingEntry.getFreshnessPeriod() >= 0)
    cout << forwardingEntry.getFreshnessPeriod() << endl;
  else
    cout << "<none>" << endl;
}

/**
 * Show the interest name and scope, and expect the name to have 4 components where the last component is a data packet 
 * whose content is a forwarding entry.
 */
static void dumpInterestWithForwardingEntry(const Interest& interest)
{
  if (interest.getName().size() != 4) {
    cout << "Error: expected the interest name to have 4 components.  Got: " << interest.getName().toUri() << endl;
    return;
  }

  cout << "scope: ";
  if (interest.getScope() >= 0)
    cout << interest.getScope() << endl;
  else
    cout << "<none>" << endl;
  cout << "name[0]: " << toString(*interest.getName()[0].getValue()) << endl;
  cout << "name[1]: " << interest.getName()[1].getValue().toHex() << endl;
  cout << "name[2]: " << toString(*interest.getName()[2].getValue()) << endl;
  cout << "name[3] decoded as Data, showing content as ForwardingEntry: " << endl;
  
  Data data;
    // Note, until the registration protocol is defined for the new TLV forwarder, we use the old BinaryXml format.
  data.wireDecode(*interest.getName()[3].getValue(), *BinaryXmlWireFormat::get());
  
  ForwardingEntry forwardingEntry;
  forwardingEntry.wireDecode(*data.getContent(), *BinaryXmlWireFormat::get());
  dumpForwardingEntry(forwardingEntry);
}

int main(int argc, char** argv)
{
  try {
    Interest interest;
    // Note, until the registration protocol is defined for the new TLV forwarder, we use the old BinaryXml format.
    interest.wireDecode(BinaryXmlInterest, sizeof(BinaryXmlInterest), *BinaryXmlWireFormat::get());
    cout << "Interest:" << endl;
    dumpInterestWithForwardingEntry(interest);
    
    Blob encoding = interest.wireEncode(*BinaryXmlWireFormat::get());
    cout << endl << "Re-encoded interest " << encoding.toHex() << endl;

    Interest reDecodedInterest;
    reDecodedInterest.wireDecode(*encoding, *BinaryXmlWireFormat::get());
    cout << endl << "Re-decoded Interest:" << endl;
    dumpInterestWithForwardingEntry(reDecodedInterest);
  } catch (exception& e) {
    cout << "exception: " << e.what() << endl;
  }
  return 0;
}
