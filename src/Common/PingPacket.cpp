/*
Copyright (C) 2008-2014 Vana Development Team

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
#include "PingPacket.hpp"
#include "CommonHeader.hpp"
#include "Session.hpp"

namespace PingPacket {

PACKET_IMPL(ping) {
	PacketBuilder builder;
	builder.add<header_t>(SMSG_PING);
	return builder;
}

PACKET_IMPL(pong) {
	PacketBuilder builder;
	builder.add<header_t>(CMSG_PONG);
	return builder;
}

}