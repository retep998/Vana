/*
Copyright (C) 2008-2013 Vana Development Team

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
#include "Connectable.h"
#include "PacketReader.h"
#include "TimeUtilities.h"
#include <cstring>
#include <ctime>

using TimeUtilities::getTickCount;

Connectable * Connectable::singleton = nullptr;

void Connectable::newPlayer(int32_t id, const Ip &ip, PacketReader &packet) {
	ConnectingPlayer player;
	player.connectIp = ip;
	player.connectTime = getTickCount();

	uint16_t pSize = packet.get<uint16_t>();
	if (pSize > 0) {
		unsigned char *buf = new unsigned char[pSize]; // Prevent the packet memory from being freed by external sources
		memcpy(buf, packet.getBuffer(), pSize);

		player.heldPacket.reset(new PacketReader(buf, pSize));
	}
	else {
		player.heldPacket.reset<PacketReader>(nullptr);
	}

	m_map[id] = player;
}

bool Connectable::checkPlayer(int32_t id, const Ip &ip) {
	bool correct = false;
	if (m_map.find(id) != m_map.end()) {
		ConnectingPlayer &t = m_map[id];
		if (t.connectIp == ip && (getTickCount() - t.connectTime) < MaxMilliseconds) {
			correct = true;
		}
	}
	return correct;
}

PacketReader * Connectable::getPacket(int32_t id) {
	return m_map[id].heldPacket.get();
}

void Connectable::playerEstablished(int32_t id) {
	m_map.erase(id);
}