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
#include "Alliance.h"
#include "AlliancePacket.h"
#include "Database.h"
#include "Channels.h"
#include "GameObjects.h"
#include "Guild.h"
#include "GuildPacket.h"
#include "InitializeCommon.h"
#include "PacketCreator.h"
#include "Party.h"
#include "Player.h"
#include "StringUtilities.h"
#include "SyncHandler.h"
#include "WorldServer.h"

using Initializing::outputWidth;

PlayerDataProvider * PlayerDataProvider::singleton = nullptr;

void PlayerDataProvider::loadData() {
	int16_t worldid = WorldServer::Instance()->getWorldId();
	loadAlliances(worldid);
	loadGuilds(worldid);
	loadPlayers(worldid);
}

void PlayerDataProvider::loadGuilds(int16_t worldId) {
	std::cout << std::setw(outputWidth) << std::left << "Initializing Guilds... ";

	mysqlpp::Query query = Database::getCharDB().query();
	query << "SELECT * FROM guilds WHERE worldid = " << worldId << " ORDER BY id ASC";
	mysqlpp::StoreQueryResult res = query.store();
	GuildLogo logo;
	GuildRanks ranks;
	int32_t alliance;
	int32_t guild;

	for (int32_t i = 0; i < (int32_t) res.num_rows(); i++) {
		logo.logo = static_cast<int16_t>(res[i]["logo"]);
		logo.color = static_cast<uint8_t>(res[i]["logocolor"]);
		logo.background = static_cast<int16_t>(res[i]["logobg"]);
		logo.backgroundColor = static_cast<uint8_t>(res[i]["logobgcolor"]);

		ranks[0] = static_cast<string>(res[i]["rank1title"]);
		ranks[1] = static_cast<string>(res[i]["rank2title"]);
		ranks[2] = static_cast<string>(res[i]["rank3title"]);
		ranks[3] = static_cast<string>(res[i]["rank4title"]);
		ranks[4] = static_cast<string>(res[i]["rank5title"]);

		guild = res[i]["id"];
		alliance = res[i]["allianceid"];
		Alliance *all = getAlliance(alliance);

		createGuild(static_cast<string>(res[i]["name"]),
			static_cast<string>(res[i]["notice"]),
			guild,
			res[i]["leaderid"],
			res[i]["capacity"],
			res[i]["points"],
			logo,
			ranks,
			all);

		if (all != nullptr)
			all->addGuild(getGuild(guild));
	}

	std::cout << "DONE" << std::endl;
}

void PlayerDataProvider::loadAlliances(int16_t worldId) {
	std::cout << std::setw(outputWidth) << std::left << "Initializing Alliances... ";

	mysqlpp::Query query = Database::getCharDB().query();
	query << "SELECT * FROM alliances WHERE worldid = " << worldId << " ORDER BY id ASC";
	mysqlpp::StoreQueryResult res = query.store();
	GuildRanks ranks;

	for (int32_t i = 0; i < (int32_t) res.num_rows(); i++) {
		ranks[0] = static_cast<string>(res[i]["rank1title"]);
		ranks[1] = static_cast<string>(res[i]["rank2title"]);
		ranks[2] = static_cast<string>(res[i]["rank3title"]);
		ranks[3] = static_cast<string>(res[i]["rank4title"]);
		ranks[4] = static_cast<string>(res[i]["rank5title"]);

		addAlliance(res[i]["id"],
			static_cast<string>(res[i]["name"]),
			static_cast<string>(res[i]["notice"]),
			ranks,
			res[i]["capacity"],
			res[i]["leader"]);
	}

	std::cout << "DONE" << std::endl;
}

void PlayerDataProvider::loadPlayers(int16_t worldId) {
	std::cout << std::setw(outputWidth) << std::left << "Initializing Players... ";

	mysqlpp::Query query = Database::getCharDB().query();
	query << "SELECT id, `name`, guildid, guildrank, allianceid, alliancerank FROM characters WHERE world_id = " << worldId << " AND guildid <> 0 ORDER BY guildrank ASC";
	mysqlpp::UseQueryResult res = query.use();
	Player *p;
	int32_t guildid;
	Guild *guild;

	enum TableColumns {
		Id, Name, GuildId, GuildRank, AllianceId, AllianceRank
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		guildid = atoi(row[GuildId]);
		guild = getGuild(guildid);
		if (guildid != 0 && guild == nullptr) {
			std::stringstream x;
			x << (string) row[Name] << " has an invalid guild ID (guild doesn't exist)";
			WorldServer::Instance()->log(LogTypes::Warning, x.str());
			continue;
		}

		p = new Player(atoi(row[Id]));
		p->setName(static_cast<string>(row[Name]));
		p->setJob(-1);
		p->setLevel(-1);
		p->setMap(-1);
		p->setChannel(0);
		p->setGuild(guild);
		p->setGuildRank(static_cast<uint8_t>(atoi(row[GuildRank])));
		p->setAlliance(getAlliance(atoi(row[AllianceId])));
		p->setAllianceRank(static_cast<uint8_t>(atoi(row[AllianceRank])));

		registerPlayer(p, false);
		if (guildid != 0) {
			guild->addPlayer(p);
		}
	}

	std::cout << "DONE" << std::endl;
}

// Players
void PlayerDataProvider::registerPlayer(Player *player, bool online) {
	if (m_players.find(player->getId()) == m_players.end()) {
		m_players[player->getId()] = player;
	}
	if (online && !player->isInCashShop()) {
		if (player->getParty() != nullptr) {
			SyncHandler::logInLogOut(player->getId());
		}
		if (player->getGuild() != nullptr) {
			GuildPacket::sendPlayerUpdate(player->getGuild(), player, 3, false);
			if (player->getAlliance() != nullptr)
				AlliancePacket::sendUpdatePlayer(player->getAlliance(), player, 2);
		}
		Channels::Instance()->increasePopulation(player->getChannel());
	}
}

void PlayerDataProvider::removePlayer(int32_t id, int16_t channel) {
	Player *player = m_players[id];
	if (channel == -1 || player->getChannel() == channel) {
		player->setOnline(false);
		if (player->getParty() != nullptr) {
			SyncHandler::logInLogOut(id);
		}
		if (player->getGuild() != nullptr) {
			GuildPacket::sendPlayerUpdate(player->getGuild(), player, 3, false);
			if (player->getAlliance() != nullptr)
				AlliancePacket::sendUpdatePlayer(player->getAlliance(), player, 2);
		}
		Channels::Instance()->decreasePopulation(channel);
	}
}

Player * PlayerDataProvider::getPlayer(const string &name, bool includeOffline) {
	Player *player = nullptr;
	bool found = false;
	for (unordered_map<int32_t, Player *>::iterator iter = m_players.begin(); iter != m_players.end(); iter++) {
		if ((iter->second->isOnline() || includeOffline) && StringUtilities::noCaseCompare(iter->second->getName(), name) == 0) {
			player = iter->second;
			found = true;
			break;
		}
	}
	if (!found) {
		player = new Player;
		player->setChannel(-1);
	}
	return player;
}

int32_t PlayerDataProvider::getPlayerQuantity() {
	return m_players.size();
}

Player * PlayerDataProvider::getPlayer(int32_t id, bool includeOffline) {
	if (m_players.find(id) != m_players.end()) {
		Player *player = m_players[id];
		if (player->isOnline() || includeOffline)
			return player;
	}
	return nullptr;
}

void PlayerDataProvider::removeChannelPlayers(uint16_t channel) {
	for (unordered_map<int32_t, Player *>::iterator iter = m_players.begin(); iter != m_players.end(); iter++) {
		if (iter->second->getChannel() == channel) {
			iter->second->setOnline(false);
			removePendingPlayerEarly(iter->second->getId());
		}
	}
}

// Channel changes
void PlayerDataProvider::addPendingPlayer(int32_t id, uint16_t channelid) {
	m_channelSwitches[id] = channelid;
}

void PlayerDataProvider::removePendingPlayer(int32_t id) {
	if (m_channelSwitches.find(id) != m_channelSwitches.end()) {
		m_channelSwitches.erase(id);
	}
}

int16_t PlayerDataProvider::removePendingPlayerEarly(int32_t id) {
	int16_t channel = -1;
	if (m_channelSwitches.find(id) != m_channelSwitches.end()) {
		channel = m_channelSwitches[id];
		m_channelSwitches.erase(id);
	}
	return channel;
}

uint16_t PlayerDataProvider::getPendingPlayerChannel(int32_t id) {
	return (m_channelSwitches.find(id) != m_channelSwitches.end() ? m_channelSwitches[id] : -1);
}

// Guilds
Guild * PlayerDataProvider::addGuild(Guild *guild) {
	m_guilds[guild->getId()] = guild;
	m_guildsName[guild->getName()] = guild;
	return m_guilds[guild->getId()];
}

Guild * PlayerDataProvider::getGuild(const string &name) {
	return (m_guildsName.find(name) == m_guildsName.end() ? nullptr : m_guildsName[name]);
}

Guild * PlayerDataProvider::getGuild(int32_t id) {
	return (m_guilds.find(id) == m_guilds.end() ? nullptr : m_guilds[id]);
}

void PlayerDataProvider::createGuild(const string &name, const string &notice, int32_t id, int32_t leaderid, int32_t capacity, int32_t gp, const GuildLogo &logo, const GuildRanks &ranks, Alliance *alliance) {
	Guild *guild = getGuild(id);
	if (guild == nullptr) {
		guild = new Guild(name, notice, id, leaderid, capacity, gp, logo, ranks, alliance);
		addGuild(guild);
	}
	else {
		guild->setNotice(notice);
		guild->setGuildPoints(gp);
		guild->setAlliance(alliance);
		guild->setLogo(logo);
		guild->setRanks(ranks);
	}
}

void PlayerDataProvider::removeGuild(Guild *guild) {
	for (unordered_map<int32_t, Player *>::iterator iter = guild->m_players.begin(); iter != guild->m_players.end(); iter++) {
		iter->second->setGuild(nullptr);
		iter->second->setGuildRank(5);
		iter->second->setAlliance(nullptr);
		iter->second->setAllianceRank(5);
	}

	m_guilds.erase(guild->getId());
	m_guildsName.erase(guild->getName());
	delete guild;
}

void PlayerDataProvider::getChannelConnectPacketGuild(PacketCreator &packet) {
	packet.add<int32_t>(m_guilds.size());
	Guild *guild;
	GuildLogo logo;
	for (unordered_map<int32_t, Guild *>::iterator iter = m_guilds.begin(); iter != m_guilds.end(); iter++) {
		guild = iter->second;
		packet.add<int32_t>(guild->getId());
		packet.addString(guild->getName());
		logo = guild->getLogo();
		packet.add<int16_t>(logo.logo);
		packet.add<uint8_t>(logo.color);
		packet.add<int16_t>(logo.background);
		packet.add<uint8_t>(logo.backgroundColor);
		packet.add<int32_t>(guild->getCapacity());
		packet.add<int32_t>(guild->getAlliance() ? guild->getAlliance()->getId() : 0);
	}
}

// Alliances
Alliance * PlayerDataProvider::getAlliance(int32_t id) {
	return (m_alliances.find(id) == m_alliances.end() ? nullptr : m_alliances[id]);
}

void PlayerDataProvider::removeAlliance(int32_t id) {
	delete m_alliances[id];
	m_alliances.erase(id);
}

void PlayerDataProvider::addAlliance(int32_t id, const string &name, const string &notice, const GuildRanks &ranks, int32_t capacity, int32_t leader) {
	Alliance *alliance = getAlliance(id);
	if (alliance == nullptr) {
		alliance = new Alliance(id, name, notice, ranks, capacity, leader);
		m_alliances[id] = alliance;
	}
	else {
		alliance->setCapacity(capacity);
		alliance->setLeaderId(leader);
		alliance->setNotice(notice);
		alliance->setRanks(ranks);
	}
}

void PlayerDataProvider::getChannelConnectPacketAlliance(PacketCreator &packet) {
	packet.add<int32_t>(m_alliances.size());
	for (unordered_map<int32_t, Alliance *>::iterator iter = m_alliances.begin(); iter != m_alliances.end(); iter++) {
		packet.add<int32_t>(iter->second->getId());
		packet.addString(iter->second->getName());
		packet.add<int32_t>(iter->second->getCapacity());
	}
}

// Parties
int32_t PlayerDataProvider::getPartyId() {
	return ++pid;
}

void PlayerDataProvider::addParty(Party *party) {
	m_parties[party->getId()] = party;
}

void PlayerDataProvider::removeParty(int32_t id) {
	if (m_parties.find(id) != m_parties.end()) {
		Party *party = m_parties[id];
		delete party;
		m_parties.erase(id);
	}
}

Party * PlayerDataProvider::getParty(int32_t id) {
	return (m_parties.find(id) != m_parties.end() ? m_parties[id] : nullptr);
}

unordered_map<int32_t, Party *> PlayerDataProvider::getParties() {
	return m_parties;
}