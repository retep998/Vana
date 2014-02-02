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

auto Connectable::newPlayer(int32_t id, const Ip &ip, PacketReader &reader) -> void {
	ConnectingPlayer player;
	player.connectIp = ip;
	player.connectTime = TimeUtilities::getNow();
	uint16_t packetSize = reader.get<uint16_t>();
	player.packetSize = packetSize;
	if (packetSize > 0) {
		player.heldPacket.reset(new unsigned char[packetSize]);
		memcpy(player.heldPacket.get(), reader.getBuffer(), packetSize);
	}

	m_map[id] = player;
}

auto Connectable::checkPlayer(int32_t id, const Ip &ip, bool &hasPacket) const -> Result {
	Result result = Result::Failure;
	hasPacket = false;
	auto kvp = m_map.find(id);
	if (kvp != std::end(m_map)) {
		auto &test = kvp->second;
		if (test.connectIp == ip && duration_cast<milliseconds_t>(TimeUtilities::getNow() - test.connectTime).count() < MaxMilliseconds) {
			result = Result::Successful;
			if (test.packetSize > 0) {
				hasPacket = true;
			}
		}
	}
	return result;
}

auto Connectable::getPacket(int32_t id) const -> PacketReader {
	auto kvp = m_map.find(id);
	auto &player = kvp->second;
	return PacketReader(player.heldPacket.get(), player.packetSize);
}

auto Connectable::playerEstablished(int32_t id) -> void {
	m_map.erase(id);
}