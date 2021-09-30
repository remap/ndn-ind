/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2020 Operant Networks, Incorporated.
 * @author: Jeff Thompson <jefft0@gmail.com>
 *
 * This works is based substantially on previous work as listed below:
 *
 * Original file: src/transport/udp-transport.cpp
 * Original repository: https://github.com/named-data/ndn-cpp
 *
 * Summary of Changes: Use ndn-ind includes. Add readRawPackets. Put element-listener.hpp in API. Support WinSock2.
 *
 * which was originally released under the LGPL license with the following rights:
 *
 * Copyright (C) 2013-2020 Regents of the University of California.
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

// Only compile if we have Unix or Windows socket support.
#include <ndn-ind/ndn-ind-config.h>
#if NDN_IND_HAVE_UNISTD_H || defined(_WIN32)

#include <stdexcept>
#include <stdlib.h>
#include "../c/transport/udp-transport.h"
#include "../c/encoding/element-reader.h"
#include "../util/dynamic-uint8-vector.hpp"
#include <ndn-ind/transport/udp-transport.hpp>

using namespace std;

namespace ndn {

UdpTransport::ConnectionInfo::~ConnectionInfo()
{
}

UdpTransport::UdpTransport(bool readRawPackets)
  : isConnected_(false), transport_(new struct ndn_UdpTransport),
    isBound_(false), boundPort_(0),
    elementBuffer_(new DynamicUInt8Vector(1000))
{
  ndn_UdpTransport_initialize
    (transport_.get(), elementBuffer_.get(), readRawPackets ? 1 : 0);
}

bool
UdpTransport::isLocal(const Transport::ConnectionInfo& connectionInfo)
{
  return ndn_UdpTransport_isLocal() != 0;
}

bool
UdpTransport::isAsync() { return false; }

void
UdpTransport::connect
  (const Transport::ConnectionInfo& connectionInfo,
   ElementListener& elementListener, const OnConnected& onConnected)
{
  const UdpTransport::ConnectionInfo& udpConnectionInfo =
    dynamic_cast<const UdpTransport::ConnectionInfo&>(connectionInfo);

  ndn_Error error;
  if ((error = ndn_UdpTransport_connect
       (transport_.get(), (char *)udpConnectionInfo.getHost().c_str(),
        udpConnectionInfo.getPort(), &elementListener)))
    throw runtime_error(ndn_getErrorString(error));

  isConnected_ = true;
  if (onConnected)
    onConnected();
}

void
UdpTransport::bind(const Transport::ConnectionInfo& connectionInfo,
    ElementListener& elementListener)
{
    const UdpTransport::ConnectionInfo& udpConnectionInfo =
        dynamic_cast<const UdpTransport::ConnectionInfo&>(connectionInfo);

    boundPort_ = udpConnectionInfo.getPort();
    ndn_Error error;
    if ((error = ndn_UdpTransport_bind
    (transport_.get(), (char*)udpConnectionInfo.getHost().c_str(),
        &boundPort_, &elementListener)))
        throw runtime_error(ndn_getErrorString(error));

    isBound_ = true;
}

void
UdpTransport::send(const uint8_t *data, size_t dataLength)
{
  ndn_Error error;
  if ((error = ndn_UdpTransport_send(transport_.get(), data, dataLength)))
    throw runtime_error(ndn_getErrorString(error));
}

void
UdpTransport::processEvents()
{
  uint8_t buffer[MAX_NDN_PACKET_SIZE];
  ndn_Error error;
  if ((error = ndn_UdpTransport_processEvents
       (transport_.get(), buffer, sizeof(buffer))))
    throw runtime_error(ndn_getErrorString(error));
}

bool
UdpTransport::getIsConnected()
{
  return isConnected_;
}

bool
UdpTransport::getIsBound() const
{
    return isBound_;
}

unsigned short
UdpTransport::getBoundPort() const
{ 
    return boundPort_; 
}

void
UdpTransport::close()
{
  ndn_Error error;
  if ((error = ndn_UdpTransport_close(transport_.get())))
    throw runtime_error(ndn_getErrorString(error));
}

}

#endif // NDN_IND_HAVE_UNISTD_H
