/*
Copyright (C) 2008-2009 Vana Development Team

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
#include "BuffHolder.h"
#include "ChannelServer.h"
#include "PacketReader.h"
#include "WorldServerConnectPacket.h"

BuffHolder * BuffHolder::singleton = 0;

void BuffHolder::parseIncomingBuffs(PacketReader &packet) {
	int32_t playerid = packet.get<int32_t>();
	m_map[playerid] = packet;
	WorldServerConnectPacket::playerBuffsTransferred(ChannelServer::Instance()->getWorldPlayer(), playerid);
}

void BuffHolder::removePacket(int32_t playerid) {
	if (m_map.find(playerid) != m_map.end()) {
		m_map.erase(playerid);
	}
}

bool BuffHolder::checkPlayer(int32_t playerid) {
	return m_map.find(playerid) != m_map.end();
}

PacketReader BuffHolder::getPacket(int32_t playerid) {
	return m_map[playerid];
}