/*
Copyright (C) 2008-2011 Vana Development Team

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
#include "InventoryPacketHelper.h"
#include "GameConstants.h"
#include "MapConstants.h"
#include "PacketCreator.h"

void InventoryPacketHelper::fillRockPacket(PacketCreator &packet, const vector<int32_t> &vec, size_t maxSize) {
	size_t remaining = 1;
	while (remaining <= vec.size()) {
		packet.add<int32_t>(vec[remaining - 1]);
		remaining++;
	}
	while (remaining <= maxSize) {
		packet.add<int32_t>(Maps::NoMap);
		remaining++;
	}
}
