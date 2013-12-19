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
#include <memory>

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

	for (const auto &row : rs) {
		loadPlayer(row);
	}

	std::cout << "DONE" << std::endl;
}

void PlayerDataProvider::loadPlayer(int32_t playerId) {
	if (m_players.find(playerId) != m_players.end()) {
		return;
	}

	soci::rowset<> rs = (Database::getCharDb().prepare
		<< "SELECT c.character_id, c.name "
		<< "FROM characters c "
		<< "WHERE c.character_id = :char",
		soci::use(playerId, "char"));

	for (const auto &row : rs) {
		loadPlayer(row);
	}

	SyncPacket::PlayerPacket::characterCreated(playerId);
}

void PlayerDataProvider::loadPlayer(const soci::row &row) {
	Player *p = new Player(row.get<int32_t>("character_id"));
	p->setName(row.get<string>("name"));
	p->setJob(-1);
	p->setLevel(-1);
	p->setMap(-1);
	p->setChannel(0);
	playerConnect(p, false);
}

void PlayerDataProvider::getPlayerDataPacket(PacketCreator &packet, int32_t playerId) {
	generatePlayerDataPacket(packet, m_players[playerId].get());
}

void PlayerDataProvider::getChannelConnectPacket(PacketCreator &packet) {
	packet.add<uint32_t>(m_players.size());
	for (const auto &kvp : m_players) {
		Player *player = kvp.second.get();
		generatePlayerDataPacket(packet, player);
	}

	packet.add<uint32_t>(m_parties.size());
	Party *party;
	for (const auto &kvp : m_parties) {
		party = kvp.second.get();
		packet.add<int32_t>(party->getId());
		packet.add<int32_t>(party->getLeaderId());
		packet.add<int8_t>(party->getMemberCount());

		party->runFunction([&packet](Player *player) {
			packet.add<int32_t>(player->getId());
		});
	}
}

void PlayerDataProvider::generatePlayerDataPacket(PacketCreator &packet, Player *player) {
	packet.add<int32_t>(0);
	packet.add<bool>(false);
	packet.add<uint8_t>(player->getLevel());
	packet.add<int16_t>(player->getJob());
	packet.add<int16_t>(player->getChannel());
	packet.add<int32_t>(player->getMap());
	packet.add<int32_t>(player->getParty() != nullptr ? player->getParty()->getId() : 0);
	packet.add<int32_t>(player->getId());
}

// Players
void PlayerDataProvider::initialPlayerConnect(int32_t id, uint16_t channel, const Ip &ip) {
	std::shared_ptr<Player> player = m_players[id];
	player->setIp(ip);
}

void PlayerDataProvider::playerConnect(Player *player, bool online) {
	if (m_players.find(player->getId()) == m_players.end()) {
		m_players[player->getId()].reset(player);
	}
	if (online) {
		player->setOnline(true);
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
	for (const auto &kvp : m_players) {
		player = kvp.second.get();
		if ((player->isOnline() || includeOffline) && StringUtilities::noCaseCompare(player->getName(), name) == 0) {
			found = true;
			break;
		}
		player = nullptr;
	}
	if (!found) {
		/* TODO FIXME: Resource issue? */
		player = new Player;
		player->setChannel(-1);
	}
	return player;
}

int32_t PlayerDataProvider::getPlayerQuantity() {
	return m_players.size();
}

Player * PlayerDataProvider::getPlayer(int32_t id, bool includeOffline) {
	auto kvp = m_players.find(id);
	if (kvp != m_players.end()) {
		Player *player = kvp->second.get();
		if (player->isOnline() || includeOffline) {
			return player;
		}
	}
	return nullptr;
}

void PlayerDataProvider::removeChannelPlayers(uint16_t channel) {
	for (const auto &kvp : m_players) {
		Player *player = kvp.second.get();
		if (player->getChannel() == channel) {
			player->setOnline(false);
			removePendingPlayerEarly(player->getId());
		}
	}
}

// Channel changes
void PlayerDataProvider::addPendingPlayer(int32_t id, uint16_t channelId) {
	m_channelSwitches[id] = channelId;
}

void PlayerDataProvider::removePendingPlayer(int32_t id) {
	auto kvp = m_channelSwitches.find(id);
	if (kvp != m_channelSwitches.end()) {
		m_channelSwitches.erase(kvp);
	}
}

int16_t PlayerDataProvider::removePendingPlayerEarly(int32_t id) {
	int16_t channel = -1;
	auto kvp = m_channelSwitches.find(id);
	if (kvp != m_channelSwitches.end()) {
		channel = kvp->second;
		m_channelSwitches.erase(kvp);
	}
	return channel;
}

uint16_t PlayerDataProvider::getPendingPlayerChannel(int32_t id) {
	auto kvp = m_channelSwitches.find(id);
	return (kvp != m_channelSwitches.end() ? kvp->second : -1);
}

// Parties
int32_t PlayerDataProvider::getPartyId() {
	return m_partyIds.next();
}

Party * PlayerDataProvider::getParty(int32_t id) {
	auto kvp = m_parties.find(id);
	return (kvp != m_parties.end() ? kvp->second.get() : nullptr);
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