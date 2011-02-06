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
#include "CashServer.h"
#include "Database.h"
#include "MapleSession.h"
#include "PacketCreator.h"
#include "PacketReader.h"
#include "Player.h"
#include "SyncPacket.h"
#include <boost/algorithm/string.hpp>
#include <cstring>

PlayerDataProvider * PlayerDataProvider::singleton = nullptr;

// Stored packets
void PlayerDataProvider::parseIncomingPacket(PacketReader &packet) {
	int32_t playerid = packet.get<int32_t>();

	size_t psize = packet.getBufferLength();
	unsigned char *buf = new unsigned char[psize]; // Prevent the packet memory from being freed by external sources
	memcpy(buf, packet.getBuffer(), psize);
	
	m_packets[playerid].reset(new PacketReader(buf, psize));
	SyncPacket::playerBuffsTransferred(CashServer::Instance()->getWorldConnection(), playerid);
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
	return (m_players.find(id) == m_players.end()) ? nullptr : m_players[id];
}

Player * PlayerDataProvider::getPlayer(const string &name) {
	string upCaseName = boost::to_upper_copy(name);
	return (m_players_names.find(upCaseName) == m_players_names.end()) ? nullptr : m_players_names[upCaseName];
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

// Guilds
void PlayerDataProvider::addGuild(int32_t id, Guild *gi) {
	m_guilds[id] = gi;
	m_guilds_names[gi->name] = gi;
}

Guild * PlayerDataProvider::getGuild(int32_t id) {
	return (m_guilds.find(id) == m_guilds.end() ? nullptr : m_guilds[id]);
}

Guild * PlayerDataProvider::getGuild(const string &name) {
	return (m_guilds_names.find(name) == m_guilds_names.end() ? nullptr : m_guilds_names[name]);
}

void PlayerDataProvider::addGuild(int32_t id, const string &name, const GuildLogo &logo, int32_t capacity, int32_t allianceid) {
	Guild *gi = new Guild(id, name, logo, capacity, allianceid);
	m_guilds[id] = gi;
}

void PlayerDataProvider::loadGuild(int32_t id) {
	mysqlpp::Query query = Database::getCharDB().query();
	query << "SELECT name, logo, logocolor, logobg, logobgcolor, capacity, allianceid FROM guilds WHERE id = " << id << " LIMIT 1";
	mysqlpp::UseQueryResult res = query.use();

	MYSQL_ROW guildRow = res.fetch_raw_row();
	GuildLogo logo;
	logo.logo = atoi(guildRow[1]);
	logo.color = atoi(guildRow[2]);
	logo.background = atoi(guildRow[3]);
	logo.backgroundColor = atoi(guildRow[4]);
	addGuild(id, 
		(string) guildRow[0],
		logo,
		atoi(guildRow[5]),
		atoi(guildRow[6]));
}

void PlayerDataProvider::unloadGuild(int32_t id) {
	if (m_guilds.find(id) == m_guilds.end())
		return;
	m_guilds_names.erase(m_guilds[id]->name);
	delete m_guilds[id];
	m_guilds.erase(id);
}

bool PlayerDataProvider::hasEmblem(int32_t id) {
	if (m_guilds.find(id) == m_guilds.end())
		return false;
	Guild *g = m_guilds[id];
	return (g->logo.logo != 0 || g->logo.color != 0 || g->logo.background != 0 || g->logo.backgroundColor != 0);
}

// Alliances
void PlayerDataProvider::addAlliance(int32_t id, Alliance *alliance) {
	m_alliances[id] = alliance;
}

Alliance * PlayerDataProvider::getAlliance(int32_t id) {
	return (m_alliances.find(id) == m_alliances.end() ? nullptr : m_alliances[id]);
}

void PlayerDataProvider::addAlliance(int32_t id, const string &name, int32_t capacity) {
	Alliance *alliance = new Alliance(id, name, capacity);

	m_alliances[id] = alliance;
}

void PlayerDataProvider::loadAlliance(int32_t id) {
	mysqlpp::Query query = Database::getCharDB().query();

	query << "SELECT name, capacity FROM alliances WHERE id = " << id << " LIMIT 1";
	mysqlpp::UseQueryResult res = query.use();

	MYSQL_ROW allianceRow = res.fetch_raw_row();
	addAlliance(id, (string) allianceRow[0], atoi(allianceRow[1]));
}

void PlayerDataProvider::unloadAlliance(int32_t id) {
	if (m_alliances.find(id) == m_alliances.end())
		return;

	delete m_alliances[id];
	m_alliances.erase(id);
}
