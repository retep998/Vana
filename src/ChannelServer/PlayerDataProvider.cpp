/*
Copyright (C) 2008-2014 Vana Development Team

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

auto PlayerDataProvider::parseChannelConnectPacket(PacketReader &packet) -> void {
	// Players
	int32_t quantity = packet.get<int32_t>();
	int32_t i;
	for (i = 0; i < quantity; i++) {
		parsePlayer(packet);
	}

	// Parties
	quantity = packet.get<int32_t>();
	int8_t members, j;
	ref_ptr_t<Party> party;
	for (i = 0; i < quantity; i++) {
		party = make_ref_ptr<Party>(packet.get<int32_t>());
		party->setLeader(packet.get<int32_t>());
		members = packet.get<int8_t>();

		for (j = 0; j < members; j++) {
			party->addMember(packet.get<int32_t>());
		}

		m_parties[party->getId()] = party;
	}
}

auto PlayerDataProvider::parsePlayer(PacketReader &packet) -> void {
	ref_ptr_t<PlayerData> player = make_ref_ptr<PlayerData>();
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
auto PlayerDataProvider::addPlayer(Player *player) -> void {
	m_players[player->getId()] = player;
	m_playersByName[player->getName()] = player;
}

auto PlayerDataProvider::newPlayer(PacketReader &packet) -> void {
	parsePlayer(packet);
}

auto PlayerDataProvider::removePlayer(Player *player) -> void {
	m_players.erase(player->getId());
	m_playersByName.erase(player->getName());
}

auto PlayerDataProvider::changeChannel(PacketReader &packet) -> void {
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

auto PlayerDataProvider::newConnectable(PacketReader &packet) -> void {
	int32_t playerId = packet.get<int32_t>();
	const Ip &ip = packet.getClass<Ip>();
	Connectable::getInstance().newPlayer(playerId, ip, packet);
	SyncPacket::PlayerPacket::connectableEstablished(playerId);
}

auto PlayerDataProvider::deleteConnectable(int32_t id) -> void {
	Connectable::getInstance().playerEstablished(id);
}

auto PlayerDataProvider::getPlayer(int32_t id) -> Player * {
	auto kvp = m_players.find(id);
	return kvp != std::end(m_players) ? kvp->second : nullptr;
}

auto PlayerDataProvider::getPlayer(const string_t &name) -> Player * {
	auto kvp = m_playersByName.find(name);
	return kvp != std::end(m_playersByName) ? kvp->second : nullptr;
}

auto PlayerDataProvider::run(function_t<void(Player *)> func) -> void {
	for (const auto &kvp : m_players) {
		func(kvp.second);
	}
}

auto PlayerDataProvider::sendPacket(PacketCreator &packet, int32_t minGmLevel) -> void {
	for (const auto &kvp : m_players) {
		Player *player = kvp.second;
		if (player->getGmLevel() >= minGmLevel) {
			player->getSession()->send(packet);
		}
	}
}

auto PlayerDataProvider::updatePlayer(PacketReader &packet) -> void {
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

auto PlayerDataProvider::getPlayerData(int32_t id) -> PlayerData * {
	auto kvp = m_playerData.find(id);
	return kvp != std::end(m_playerData) ? kvp->second.get() : nullptr;
}

// Parties
auto PlayerDataProvider::getParty(int32_t id) -> Party * {
	return m_parties.find(id) == std::end(m_parties) ? nullptr : m_parties[id].get();
}

auto PlayerDataProvider::newParty(int32_t id, int32_t leaderId) -> void {
	ref_ptr_t<Party> p = make_ref_ptr<Party>(id);
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

auto PlayerDataProvider::disbandParty(int32_t id) -> void {
	if (Party *party = getParty(id)) {
		party->disband();
		m_parties.erase(id);
	}
}

auto PlayerDataProvider::switchPartyLeader(int32_t id, int32_t leaderId) -> void {
	if (Party *party = getParty(id)) {
		party->setLeader(leaderId);
	}
}

auto PlayerDataProvider::removePartyMember(int32_t id, int32_t playerId, bool kicked) -> void {
	if (Party *party = getParty(id)) {
		if (Player *member = getPlayer(playerId)) {
			party->deleteMember(member, kicked);
		}
		else {
			party->deleteMember(playerId, kicked);
		}
	}
}

auto PlayerDataProvider::addPartyMember(int32_t id, int32_t playerId) -> void {
	if (Party *party = getParty(id)) {
		if (Player *member = getPlayer(playerId)) {
			party->addMember(member);
		}
		else {
			party->addMember(playerId);
		}
	}
}