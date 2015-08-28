/*
Copyright (C) 2008-2015 Vana Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2
of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#include "AuthenticationPacket.hpp"
#include "AbstractServerConnection.hpp"
#include "InterHeader.hpp"
#include "PacketBuilder.hpp"
#include "Session.hpp"
#include <algorithm>

namespace Vana {
namespace Packets {

PACKET_IMPL(sendPassword, AbstractServerConnection *connection, const string_t &pass, const IpMatrix &extIp) {
	PacketBuilder builder;
	builder.add<header_t>(IMSG_PASSWORD);
	builder.add<string_t>(pass);
	builder.add<vector_t<ExternalIp>>(extIp);
	builder.add<server_type_t>(static_cast<server_type_t>(connection->getType()));
	return builder;
}

}
}