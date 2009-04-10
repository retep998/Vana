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
#include "PlayerActiveBuffs.h"
#include "WorldServerConnectPacket.h"

BuffHolder * BuffHolder::singleton = 0;

void BuffHolder::parseIncomingBuffs(PacketReader &packet) {
	int32_t playerid = packet.get<int32_t>();
	PlayerActiveBuffs *playerbuffs = new PlayerActiveBuffs(0);

	playerbuffs->setCombo(packet.get<uint8_t>(), false);
	playerbuffs->setEnergyChargeLevel(packet.get<int16_t>());
	playerbuffs->setCharge(packet.get<int32_t>());
	playerbuffs->setBooster(packet.get<int32_t>());
	int32_t mountid = packet.get<int32_t>();
	int32_t mountskill = packet.get<int32_t>();
	playerbuffs->setMountInfo(mountskill, mountid);
	MapEntryBuffs enterbuffs;
	for (int8_t i = 0; i < 8; i++) {
		enterbuffs.types[i] = packet.get<uint8_t>();
		enterbuffs.val[i] = packet.get<uint8_t>() != 0;
		vector<int16_t> vals;
		uint8_t size = packet.get<uint8_t>();
		for (uint8_t f = 0; f < size; f++) {
			vals.push_back(packet.get<int16_t>());
		}
		enterbuffs.values[i] = vals;
	}
	playerbuffs->setMapEntryBuffs(enterbuffs);

	uint8_t nbuffs = packet.get<uint8_t>();
	vector<BuffStorage> holdbuffs;
	BuffStorage cbuff;
	for (uint8_t i = 0; i < nbuffs; i++) {
		int32_t buffid = packet.get<int32_t>();
		int32_t bufftimeleft = packet.get<int32_t>();
		uint8_t skilllevel = packet.get<uint8_t>();
		cbuff.skillid = buffid;
		cbuff.timeleft = bufftimeleft;
		cbuff.level = skilllevel;
		holdbuffs.push_back(cbuff);
	}
	m_buff_map[playerid] = holdbuffs;

	ActiveBuffsByType bufftypes;
	unordered_map<uint8_t, int32_t> currentbyte;
	for (int8_t i = 0; i < 8; i++) {
		uint8_t size = packet.get<uint8_t>();
		for (uint8_t f = 0; f < size; f++) {
			uint8_t key = packet.get<uint8_t>();
			int32_t value = packet.get<int32_t>();
			currentbyte[key] = value;
		}
		bufftypes[i] = currentbyte;
	}
	playerbuffs->setActiveBuffsByType(bufftypes);
	m_map[playerid] = playerbuffs;
	WorldServerConnectPacket::playerBuffsTransferred(ChannelServer::Instance()->getWorldPlayer(), playerid);
}

void BuffHolder::removeBuffs(int32_t playerid) {
	if (m_map.find(playerid) != m_map.end()) {
		PlayerActiveBuffs *buff = m_map[playerid];
		delete buff;
		m_map.erase(playerid);
		if (m_buff_map.find(playerid) != m_buff_map.end()) {
			m_buff_map.erase(playerid);
		}
	}
}

bool BuffHolder::checkPlayer(int32_t playerid) {
	return m_map.find(playerid) != m_map.end();
}

PlayerActiveBuffs * BuffHolder::getBuffs(int32_t playerid) {
	return (m_map.find(playerid) != m_map.end() ? m_map[playerid] : 0);
}

vector<BuffStorage> BuffHolder::getStoredBuffs(int32_t playerid) {
	vector<BuffStorage> ret;
	if (m_buff_map.find(playerid) != m_buff_map.end())
		ret = m_buff_map[playerid];
	return ret;
}