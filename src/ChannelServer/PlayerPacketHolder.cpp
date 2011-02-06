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
#include "PlayerPacketHolder.h"
#include "ChannelServer.h"
#include "PacketReader.h"
#include "WorldServerConnectPacket.h"
#include <cstring>

PlayerPacketHolder * PlayerPacketHolder::singleton = 0;

void PlayerPacketHolder::parseIncomingPacket(PacketReader &packet) {
	int32_t playerid = packet.get<int32_t>();

	size_t psize = packet.getBufferLength();
	unsigned char *buf = new unsigned char[psize]; // Prevent the packet memory from being freed by external sources
	memcpy(buf, packet.getBuffer(), psize);
	
	m_map[playerid].reset(new PacketReader(buf, psize));
	WorldServerConnectPacket::playerBuffsTransferred(ChannelServer::Instance()->getWorldConnection(), playerid);
}

void PlayerPacketHolder::removePacket(int32_t playerid) {
	if (m_map.find(playerid) != m_map.end()) {
		m_map.erase(playerid);
	}
}

bool PlayerPacketHolder::checkPlayer(int32_t playerid) {
	return m_map.find(playerid) != m_map.end();
}

PacketReader & PlayerPacketHolder::getPacket(int32_t playerid) {
	return *(m_map[playerid].get());
}
