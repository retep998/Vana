/*
Copyright (C) 2008-2016 Vana Development Team

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

namespace vana {
namespace packets {

PACKET_IMPL(ping) {
	packet_builder builder;
	builder.add<packet_header>(SMSG_PING);
	return builder;
}

PACKET_IMPL(pong) {
	packet_builder builder;
	builder.add<packet_header>(CMSG_PONG);
	return builder;
}

PACKET_IMPL(connect, const string &subversion, crypto_iv recv, crypto_iv send) {
	packet_builder builder;
	builder
		.defer<packet_header>()
		.add<game_version>(maple_version::version)
		.add<string>(subversion)
		.add<crypto_iv>(recv)
		.add<crypto_iv>(send)
		.add<game_locale>(maple_version::locale);

	builder.set<packet_header>(
		static_cast<packet_header>(builder.get_size() - sizeof(packet_header)),
		0);
	return builder;
}

}
}