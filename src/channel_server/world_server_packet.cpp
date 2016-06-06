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
#include "world_server_packet.hpp"
#include "common/inter_header.hpp"
#include "channel_server/smsg_header.hpp"

namespace vana {
namespace channel_server {
namespace packets {
namespace interserver {

PACKET_IMPL(ranking_calculation) {
	packet_builder builder;
	builder
		.add<packet_header>(IMSG_TO_LOGIN)
		.add<packet_header>(IMSG_CALCULATE_RANKING);
	return builder;
}

PACKET_IMPL(reload_mcdb, const string &type) {
	packet_builder builder;
	builder
		.add<int16_t>(IMSG_TO_ALL_CHANNELS)
		.add<packet_header>(IMSG_REFRESH_DATA)
		.add<string>(type);
	return builder;
}

PACKET_IMPL(rehash_config) {
	packet_builder builder;
	builder
		.add<packet_header>(IMSG_TO_LOGIN)
		.add<packet_header>(IMSG_REHASH_CONFIG);
	return builder;
}

}
}
}
}