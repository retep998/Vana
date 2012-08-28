/*
Copyright (C) 2008-2012 Vana Development Team

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
#include "InterHelper.h"
#include "PacketCreator.h"
#include "Party.h"
#include "Player.h"
#include "PlayerObjects.h"
#include "StringUtilities.h"
#include "SyncHandler.h"
#include "SyncPacket.h"
#include "WorldServer.h"
#include <iomanip>
#include <iostream>

using Initializing::OutputWidth;

PlayerDataProvider * PlayerDataProvider::singleton = nullptr;

PlayerDataProvider::PlayerDataProvider() :
	m_partyIds(1, 100000)
{
}

void PlayerDataProvider::loadData() {
	int16_t worldId = WorldServer::Instance()->getWorldId();
	loadPlayers(worldId);
}

void PlayerDataProvider::loadPlayers(int16_t worldId) {
	std::cout << std::setw(OutputWidth) << std::left << "Initializing Players... ";

	soci::rowset<> rs = (Database::getCharDb().prepare
		<< "SELECT c.character_id, c.name "
		<< "FROM characters c "
		<< "WHERE c.world_id = :world",
		soci::use(worldId, "world"));

	Player *p;

	enum TableColumns : int32_t {
		CharacterId = 0,
		Name
	};
	for (soci::rowset<>::const_iterator i = rs.begin(); i != rs.end(); ++i) {
		soci::row const &row = *i;

		p = new Player(row.get<int32_t>(CharacterId));
		p->setName(row.get<string>(Name));
		p->setJob(-1);
		p->setLevel(-1);
		p->setMap(-1);
		p->setChannel(0);

		playerConnect(p, false);
	}

	std::cout << "DONE" << std::endl;
}

void PlayerDataProvider::getChannelConnectPacket(PacketCreator &packet) {
	packet.add<uint32_t>(m_players.size());
	Player *player;
	for (PlayerMap::iterator iter = m_players.begin(); iter != m_players.end(); ++iter) {
		player = iter->second.get();
		packet.addBool(false);
		packet.addBool(false);
		packet.add<int16_t>(player->getLevel());
		packet.add<int16_t>(player->getChannel());
		packet.add<int32_t>(player->getMap());
		packet.add<int32_t>(player->getParty() != nullptr ? player->getParty()->getId() : 0);
		packet.add<int32_t>(0);
	}

	packet.add<uint32_t>(m_parties.size());
	Party *party;
	for (PartyMap::iterator iter = m_parties.begin(); iter != m_parties.end(); ++iter) {
		party = iter->second.get();
		packet.add<int32_t>(party->getId());
		packet.add<int32_t>(party->getLeaderId());
		packet.add<int8_t>(party->getMemberCount());

		party->runFunction([&packet](Player *player) {
			packet.add<int32_t>(player->getId());
		});
	}
}

// Players
void PlayerDataProvider::playerConnect(Player *player, bool online) {
	if (m_players.find(player->getId()) == m_players.end()) {
		m_players[player->getId()].reset(player);
	}
	if (online) {
		if (player->getParty() != nullptr) {
			//SyncHandler::logInLogOut(player->getId());
		}
		Channels::Instance()->increasePopulation(player->getChannel());
	}
}

void PlayerDataProvider::playerDisconnect(int32_t id, int16_t channel) {
	Player *player = m_players[id].get();
	if (channel == -1 || player->getChannel() == channel) {
		player->setOnline(false);
		if (player->getParty() != nullptr) {
			//SyncHandler::logInLogOut(id);
		}
		Channels::Instance()->decreasePopulation(channel);
	}
}

Player * PlayerDataProvider::getPlayer(const string &name, bool includeOffline) {
	Player *player = nullptr;
	bool found = false;
	for (PlayerMap::iterator iter = m_players.begin(); iter != m_players.end(); ++iter) {
		if ((iter->second->isOnline() || includeOffline) && StringUtilities::noCaseCompare(iter->second->getName(), name) == 0) {
			player = iter->second.get();
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
		Player *player = m_players[id].get();
		if (player->isOnline() || includeOffline) {
			return player;
		}
	}
	return nullptr;
}

void PlayerDataProvider::removeChannelPlayers(uint16_t channel) {
	for (PlayerMap::iterator iter = m_players.begin(); iter != m_players.end(); ++iter) {
		if (iter->second->getChannel() == channel) {
			iter->second->setOnline(false);
			removePendingPlayerEarly(iter->second->getId());
		}
	}
}

// Channel changes
void PlayerDataProvider::addPendingPlayer(int32_t id, uint16_t channelId) {
	m_channelSwitches[id] = channelId;
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

// Parties
int32_t PlayerDataProvider::getPartyId() {
	return m_partyIds.next();
}

Party * PlayerDataProvider::getParty(int32_t id) {
	return (m_parties.find(id) != m_parties.end() ? m_parties[id].get() : nullptr);
}

void PlayerDataProvider::createParty(int32_t playerId) {
	Player *player = getPlayer(playerId);
	if (player->getParty() != nullptr) {
		// Hacking
		return;
	}
	Party *party = new Party(getPartyId(), player->getId());
	party->addMember(player, true);

	m_parties[party->getId()].reset(party);
}

void PlayerDataProvider::removePartyMember(int32_t playerId) {
	Player *player = getPlayer(playerId);
	Party *party = player->getParty();
	if (party == nullptr) {
		// Hacking
		return;
	}

	if (party->isLeader(playerId)) {
		int32_t id = party->getId();
		party->disband();
		m_parties.erase(id);
	}
	else {
		party->deleteMember(player, false);
	}
}

void PlayerDataProvider::removePartyMember(int32_t playerId, int32_t target) {
	Player *player = PlayerDataProvider::Instance()->getPlayer(playerId);
	Party *party = player->getParty();
	if (party == nullptr || !party->isLeader(playerId)) {
		// Hacking
		return;
	}

	Player *targetPlayer = PlayerDataProvider::Instance()->getPlayer(target, true);
	party->deleteMember(targetPlayer, true);
}

void PlayerDataProvider::addPartyMember(int32_t playerId) {
	Player *player = PlayerDataProvider::Instance()->getPlayer(playerId);
	Party *party = player->getParty();
	if (party != nullptr) {
		// Hacking
		return;
	}
	if (party->getMemberCount() < Parties::MaxMembers) {
		// Silent failure otherwise
		party->addMember(player);
	}
}

void PlayerDataProvider::setPartyLeader(int32_t playerId, int32_t leaderId) {
	Player *player = PlayerDataProvider::Instance()->getPlayer(playerId);
	Party *party = player->getParty();
	if (party == nullptr || !party->isLeader(playerId)) {
		// Hacking
		return;
	}

	party->setLeader(player);
}