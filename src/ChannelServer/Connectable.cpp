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
#include "Connectable.hpp"
#include "PacketReader.hpp"
#include "TimeUtilities.hpp"
#include <cstring>
#include <ctime>

auto Connectable::newPlayer(int32_t id, const Ip &ip, PacketReader &packet) -> void {
	ConnectingPlayer player;
	player.connectIp = ip;
	player.connectTime = TimeUtilities::getNow();

	uint16_t pSize = packet.get<uint16_t>();
	if (pSize > 0) {
		unsigned char *buf = new unsigned char[pSize]; // Prevent the packet memory from being freed by external sources
		memcpy(buf, packet.getBuffer(), pSize);

		player.heldPacket = make_ref_ptr<PacketReader>(buf, pSize);
	}
	else {
		player.heldPacket.reset<PacketReader>(nullptr);
	}

	m_map[id] = player;
}

auto Connectable::checkPlayer(int32_t id, const Ip &ip) -> bool {
	bool correct = false;
	auto kvp = m_map.find(id);
	if (kvp != std::end(m_map)) {
		const ConnectingPlayer &test = kvp->second;
		if (test.connectIp == ip && duration_cast<milliseconds_t>(TimeUtilities::getNow() - test.connectTime).count() < MaxMilliseconds) {
			correct = true;
		}
	}
	return correct;
}

auto Connectable::getPacket(int32_t id) -> PacketReader * {
	return m_map[id].heldPacket.get();
}

auto Connectable::playerEstablished(int32_t id) -> void {
	m_map.erase(id);
}