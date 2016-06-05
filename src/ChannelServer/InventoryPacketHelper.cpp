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
#include "InventoryPacketHelper.hpp"
#include "Common/MapConstants.hpp"

namespace vana {
namespace channel_server {
namespace packets {
namespace helpers {

PACKET_IMPL(fill_rock_packet, const vector<game_map_id> &vec, size_t max_size) {
	packet_builder builder;
	size_t remaining = 1;
	while (remaining <= vec.size()) {
		builder.add<game_map_id>(vec[remaining - 1]);
		remaining++;
	}
	while (remaining <= max_size) {
		builder.add<game_map_id>(vana::maps::no_map);
		remaining++;
	}
	return builder;
}

}
}
}
}