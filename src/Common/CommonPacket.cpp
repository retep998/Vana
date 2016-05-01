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
#include "CommonPacket.hpp"
#include "Common/CommonHeader.hpp"
#include "Common/MapleVersion.hpp"
#include "Common/Session.hpp"

namespace Vana {
namespace Packets {

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

PACKET_IMPL(connect, const string_t &subversion, iv_t recvIv, iv_t sendIv) {
	PacketBuilder builder;
	builder
		.defer<header_t>()
		.add<version_t>(MapleVersion::Version)
		.add<string_t>(subversion)
		.add<iv_t>(recvIv)
		.add<iv_t>(sendIv)
		.add<game_locale_t>(MapleVersion::Locale);

	builder.set<header_t>(static_cast<header_t>(builder.getSize() - sizeof(header_t)), 0);
	return builder;
}

}
}