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
#include "InventoryPacketHelper.hpp"
#include "MapConstants.hpp"

namespace InventoryPacketHelper {

PACKET_IMPL(fillRockPacket, const vector_t<map_id_t> &vec, size_t maxSize) {
	PacketBuilder builder;
	size_t remaining = 1;
	while (remaining <= vec.size()) {
		builder.add<map_id_t>(vec[remaining - 1]);
		remaining++;
	}
	while (remaining <= maxSize) {
		builder.add<map_id_t>(Maps::NoMap);
		remaining++;
	}
	return builder;
}

}