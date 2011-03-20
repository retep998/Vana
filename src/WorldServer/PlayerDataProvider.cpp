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
#include "Database.h"
#include "Channels.h"
#include "GameObjects.h"
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
	loadPlayers(worldid);
}

void PlayerDataProvider::loadPlayers(int16_t worldId) {
	std::cout << std::setw(outputWidth) << std::left << "Initializing Players... ";

	mysqlpp::Query query = Database::getCharDB().query();
	query << "SELECT c.character_id, c.`name` FROM characters c WHERE c.world_id = " << worldId;
	mysqlpp::UseQueryResult res = query.use();
	Player *p;

	enum TableColumns {
		CharacterId, Name
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		p = new Player(atoi(row[CharacterId]));
		p->setName(static_cast<string>(row[Name]));
		p->setJob(-1);
		p->setLevel(-1);
		p->setMap(-1);
		p->setChannel(0);

		registerPlayer(p, false);
	}

	std::cout << "DONE" << std::endl;
}

// Players
void PlayerDataProvider::registerPlayer(Player *player, bool online) {
	if (m_players.find(player->getId()) == m_players.end()) {
		m_players[player->getId()] = player;
	}
	if (online) {
		if (player->getParty() != nullptr) {
			SyncHandler::logInLogOut(player->getId());
		}
		Channels::Instance()->increasePopulation(player->getChannel());
	}
}

void PlayerDataProvider::remove(int32_t id, int16_t channel) {
	Player *player = m_players[id];
	if (channel == -1 || player->getChannel() == channel) {
		player->setOnline(false);
		if (player->getParty() != nullptr) {
			SyncHandler::logInLogOut(id);
		}
		Channels::Instance()->decreasePopulation(channel);
	}
}

Player * PlayerDataProvider::getPlayer(const string &name, bool includeOffline) {
	Player *player;
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
	if (m_channelSwitches.find(id) != m_channelSwitches.end())
		m_channelSwitches.erase(id);
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