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
#include "ChannelServer.h"
#include "Connectable.h"
#include "Database.h"
#include "InterHelper.h"
#include "PacketCreator.h"
#include "PacketReader.h"
#include "Party.h"
#include "Player.h"
#include "PlayerPacket.h"
#include "Session.h"
#include "StringUtilities.h"
#include "SyncPacket.h"
#include <algorithm>
#include <cstring>

PlayerDataProvider * PlayerDataProvider::singleton = nullptr;

void PlayerDataProvider::parseChannelConnectPacket(PacketReader &packet) {
	// Players
	int32_t quantity = packet.get<int32_t>();
	int32_t i;
	for (i = 0; i < quantity; i++) {
		parsePlayer(packet);
	}

	// Parties
	quantity = packet.get<int32_t>();
	int8_t members, j;
	std::shared_ptr<Party> party;
	for (i = 0; i < quantity; i++) {
		party = std::make_shared<Party>(packet.get<int32_t>());
		party->setLeader(packet.get<int32_t>());
		members = packet.get<int8_t>();

		for (j = 0; j < members; j++) {
			party->addMember(packet.get<int32_t>());
		}

		m_parties[party->getId()] = party;
	}
}

void PlayerDataProvider::parsePlayer(PacketReader &packet) {
	std::shared_ptr<PlayerData> player = std::make_shared<PlayerData>();
	player->gmLevel = packet.get<int32_t>();
	player->admin = packet.get<bool>();
	player->level = packet.get<uint8_t>();
	player->job = packet.get<int16_t>();
	player->channel = packet.get<int16_t>();
	player->map = packet.get<int32_t>();
	player->party = packet.get<int32_t>();

	m_playerData[packet.get<int32_t>()] = player;
}

// Players
void PlayerDataProvider::addPlayer(Player *player) {
	m_players[player->getId()] = player;
	string upper = StringUtilities::toUpper(player->getName());
	m_playersByName[upper] = player;
}

void PlayerDataProvider::newPlayer(PacketReader &packet) {
	parsePlayer(packet);
}

void PlayerDataProvider::removePlayer(Player *player) {
	m_players.erase(player->getId());
	string upper = StringUtilities::toUpper(player->getName());
	m_playersByName.erase(upper);
}

void PlayerDataProvider::changeChannel(PacketReader &packet) {
	int32_t playerId = packet.get<int32_t>();
	const Ip &ip = packet.getClass<Ip>();
	port_t port = packet.get<port_t>();

	if (Player *player = getPlayer(playerId)) {
		if (!ip.isInitialized()) {
			PlayerPacket::sendBlockedMessage(player, PlayerPacket::BlockMessages::CannotGo);
		}
		else {
			player->setOnline(false); // Set online to false BEFORE CC packet is sent to player
			PlayerPacket::changeChannel(player, ip, port);
			player->saveAll(true);
			player->setSaveOnDc(false);
		}
	}
}

void PlayerDataProvider::newConnectable(PacketReader &packet) {
	int32_t playerId = packet.get<int32_t>();
	const Ip &ip = packet.getClass<Ip>();
	Connectable::Instance()->newPlayer(playerId, ip, packet);
	SyncPacket::PlayerPacket::connectableEstablished(playerId);
}

void PlayerDataProvider::deleteConnectable(int32_t id) {
	Connectable::Instance()->playerEstablished(id);
}

Player * PlayerDataProvider::getPlayer(int32_t id) {
	auto kvp = m_players.find(id);
	return kvp != m_players.end() ? kvp->second : nullptr;
}

Player * PlayerDataProvider::getPlayer(const string &name) {
	string upper = StringUtilities::toUpper(name);
	auto kvp = m_playersByName.find(upper);
	return kvp != m_playersByName.end() ? kvp->second : nullptr;
}

void PlayerDataProvider::run(function<void (Player *)> func) {
	for (const auto &kvp : m_players) {
		func(kvp.second);
	}
}

void PlayerDataProvider::sendPacket(PacketCreator &packet, int32_t minGmLevel) {
	for (const auto &kvp : m_players) {
		Player *player = kvp.second;
		if (player->getGmLevel() >= minGmLevel) {
			player->getSession()->send(packet);
		}
	}
}

void PlayerDataProvider::updatePlayer(PacketReader &packet) {
	int32_t playerId = packet.get<int32_t>();
	if (PlayerData *player = getPlayerData(playerId)) {
		int8_t updateBits = packet.get<int8_t>();
		bool updateParty = false;
		bool updateGuild = false;
		bool updateAlliance = false;
		if (updateBits & Sync::Player::UpdateBits::Job) {
			player->job = packet.get<int16_t>();
			updateParty = true;
			updateGuild = true;
			updateAlliance = true;
		}
		if (updateBits & Sync::Player::UpdateBits::Level) {
			player->level = packet.get<uint8_t>();
			updateParty = true;
			updateGuild = true;
			updateAlliance = true;
		}
		if (updateBits & Sync::Player::UpdateBits::Map) {
			player->map = packet.get<int32_t>();
			updateParty = true;
		}

		if (updateParty && player->party != 0) {
			getParty(player->party)->silentUpdate();
		}
	}
}

PlayerData * PlayerDataProvider::getPlayerData(int32_t id) {
	auto kvp = m_playerData.find(id);
	return kvp != m_playerData.end() ? kvp->second.get() : nullptr;
}

// Parties
Party * PlayerDataProvider::getParty(int32_t id) {
	return (m_parties.find(id) == m_parties.end() ? nullptr : m_parties[id].get());
}

void PlayerDataProvider::newParty(int32_t id, int32_t leaderId) {
	std::shared_ptr<Party> p = std::make_shared<Party>(id);
	Player *leader = getPlayer(leaderId);
	if (leader == 0) {
		p->addMember(leaderId);
	}
	else {
		p->addMember(leader);
	}
	p->setLeader(leaderId);
	m_parties[id] = p;
}

void PlayerDataProvider::disbandParty(int32_t id) {
	if (Party *party = getParty(id)) {
		party->disband();
		m_parties.erase(id);
	}
}

void PlayerDataProvider::switchPartyLeader(int32_t id, int32_t leaderId) {
	if (Party *party = getParty(id)) {
		party->setLeader(leaderId);
	}
}

void PlayerDataProvider::removePartyMember(int32_t id, int32_t playerId, bool kicked) {
	if (Party *party = getParty(id)) {
		if (Player *member = getPlayer(playerId)) {
			party->deleteMember(member, kicked);
		}
		else {
			party->deleteMember(playerId, kicked);
		}
	}
}

void PlayerDataProvider::addPartyMember(int32_t id, int32_t playerId) {
	if (Party *party = getParty(id)) {
		if (Player *member = getPlayer(playerId)) {
			party->addMember(member);
		}
		else {
			party->addMember(playerId);
		}
	}
}