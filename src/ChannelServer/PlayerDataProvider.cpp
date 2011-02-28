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
#include "PlayerDataProvider.h"
#include "ChannelServer.h"
#include "Database.h"
#include "MapleSession.h"
#include "PacketCreator.h"
#include "PacketReader.h"
#include "Party.h"
#include "Player.h"
#include "WorldServerConnectPacket.h"
#include <boost/algorithm/string.hpp>
#include <cstring>

PlayerDataProvider * PlayerDataProvider::singleton = 0;

// Stored packets
void PlayerDataProvider::parseIncomingPacket(PacketReader &packet) {
	int32_t playerid = packet.get<int32_t>();

	size_t psize = packet.getBufferLength();
	unsigned char *buf = new unsigned char[psize]; // Prevent the packet memory from being freed by external sources
	memcpy(buf, packet.getBuffer(), psize);
	
	m_packets[playerid].reset(new PacketReader(buf, psize));
	WorldServerConnectPacket::playerBuffsTransferred(ChannelServer::Instance()->getWorldConnection(), playerid);
}

void PlayerDataProvider::removePacket(int32_t id) {
	if (m_packets.find(id) != m_packets.end()) {
		m_packets.erase(id);
	}
}

bool PlayerDataProvider::checkPlayer(int32_t id) {
	return m_packets.find(id) != m_packets.end();
}

PacketReader & PlayerDataProvider::getPacket(int32_t id) {
	return *(m_packets[id].get());
}

// Players
void PlayerDataProvider::addPlayer(Player *player) {
	m_players[player->getId()] = player;
	m_players_names[boost::to_upper_copy(player->getName())] = player; // Store in upper case for easy non-case-sensitive search
}

void PlayerDataProvider::removePlayer(Player *player) {
	m_players.erase(player->getId());
	m_players_names.erase(boost::to_upper_copy(player->getName()));
}

Player * PlayerDataProvider::getPlayer(int32_t id) {
	return (m_players.find(id) == m_players.end()) ? 0 : m_players[id];
}

Player * PlayerDataProvider::getPlayer(const string &name) {
	string upCaseName = boost::to_upper_copy(name);
	return (m_players_names.find(upCaseName) == m_players_names.end()) ? 0 : m_players_names[upCaseName];
}

void PlayerDataProvider::run(function<void (Player *)> func) {
	for (unordered_map<int32_t, Player *>::iterator iter = m_players.begin(); iter != m_players.end(); iter++) {
		func(iter->second);
	}
}

void PlayerDataProvider::sendPacket(PacketCreator &packet) {
	for (unordered_map<int32_t, Player *>::iterator iter = m_players.begin(); iter != m_players.end(); iter++) {
		iter->second->getSession()->send(packet);
	}
}

// Parties
Party * PlayerDataProvider::getParty(int32_t id) {
	return (m_parties.find(id) == m_parties.end() ? 0 : m_parties[id]);
}

void PlayerDataProvider::addParty(Party *party) {
	m_parties[party->getId()] = party;
}

void PlayerDataProvider::removeParty(int32_t id) {
	if (Party *party = getParty(id)) {
		m_parties.erase(id);
		delete party;
	}
}
