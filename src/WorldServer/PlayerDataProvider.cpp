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
#include "PlayerDataProvider.hpp"
#include "Database.hpp"
#include "Channels.hpp"
#include "GameObjects.hpp"
#include "InitializeCommon.hpp"
#include "InterHelper.hpp"
#include "PacketCreator.hpp"
#include "Party.hpp"
#include "Player.hpp"
#include "PlayerObjects.hpp"
#include "StringUtilities.hpp"
#include "SyncHandler.hpp"
#include "SyncPacket.hpp"
#include "WorldServer.hpp"
#include <iomanip>
#include <iostream>
#include <memory>

PlayerDataProvider::PlayerDataProvider() :
	m_partyIds(1, 100000)
{
}

auto PlayerDataProvider::loadData() -> void {
	int16_t worldId = WorldServer::getInstance().getWorldId();
	loadPlayers(worldId);
}

auto PlayerDataProvider::loadPlayers(int16_t worldId) -> void {
	std::cout << std::setw(Initializing::OutputWidth) << std::left << "Initializing Players... ";

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

auto PlayerDataProvider::loadPlayer(int32_t playerId) -> void {
	if (m_players.find(playerId) != std::end(m_players)) {
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

auto PlayerDataProvider::loadPlayer(const soci::row &row) -> void {
	Player *p = new Player(row.get<int32_t>("character_id"));
	p->setName(row.get<string_t>("name"));
	p->setJob(-1);
	p->setLevel(-1);
	p->setMap(-1);
	p->setChannel(0);
	playerConnect(p, false);
}

auto PlayerDataProvider::getPlayerDataPacket(PacketCreator &packet, int32_t playerId) -> void {
	generatePlayerDataPacket(packet, m_players[playerId].get());
}

auto PlayerDataProvider::getChannelConnectPacket(PacketCreator &packet) -> void {
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

auto PlayerDataProvider::generatePlayerDataPacket(PacketCreator &packet, Player *player) -> void {
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
auto PlayerDataProvider::initialPlayerConnect(int32_t id, uint16_t channel, const Ip &ip) -> void {
	ref_ptr_t<Player> player = m_players[id];
	player->setIp(ip);
}

auto PlayerDataProvider::playerConnect(Player *player, bool online) -> void {
	if (m_players.find(player->getId()) == std::end(m_players)) {
		m_players[player->getId()].reset(player);
	}
	if (online) {
		player->setOnline(true);
		if (player->getParty() != nullptr) {
			//SyncHandler::logInLogOut(player->getId());
		}
		Channels::getInstance().increasePopulation(player->getChannel());
	}
}

auto PlayerDataProvider::playerDisconnect(int32_t id, int16_t channel) -> void {
	Player *player = m_players[id].get();
	if (channel == -1 || player->getChannel() == channel) {
		player->setOnline(false);
		if (player->getParty() != nullptr) {
			//SyncHandler::logInLogOut(id);
		}
		Channels::getInstance().decreasePopulation(channel);
	}
}

auto PlayerDataProvider::getPlayer(const string_t &name, bool includeOffline) -> Player * {
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

auto PlayerDataProvider::getPlayerQuantity() -> int32_t {
	return m_players.size();
}

auto PlayerDataProvider::getPlayer(int32_t id, bool includeOffline) -> Player * {
	auto kvp = m_players.find(id);
	if (kvp != std::end(m_players)) {
		Player *player = kvp->second.get();
		if (player->isOnline() || includeOffline) {
			return player;
		}
	}
	return nullptr;
}

auto PlayerDataProvider::removeChannelPlayers(uint16_t channel) -> void {
	for (const auto &kvp : m_players) {
		Player *player = kvp.second.get();
		if (player->getChannel() == channel) {
			player->setOnline(false);
			removePendingPlayerEarly(player->getId());
		}
	}
}

// Channel changes
auto PlayerDataProvider::addPendingPlayer(int32_t id, uint16_t channelId) -> void {
	m_channelSwitches[id] = channelId;
}

auto PlayerDataProvider::removePendingPlayer(int32_t id) -> void {
	auto kvp = m_channelSwitches.find(id);
	if (kvp != std::end(m_channelSwitches)) {
		m_channelSwitches.erase(kvp);
	}
}

auto PlayerDataProvider::removePendingPlayerEarly(int32_t id) -> int16_t {
	int16_t channel = -1;
	auto kvp = m_channelSwitches.find(id);
	if (kvp != std::end(m_channelSwitches)) {
		channel = kvp->second;
		m_channelSwitches.erase(kvp);
	}
	return channel;
}

auto PlayerDataProvider::getPendingPlayerChannel(int32_t id) -> uint16_t {
	auto kvp = m_channelSwitches.find(id);
	return (kvp != std::end(m_channelSwitches) ? kvp->second : -1);
}

// Parties
auto PlayerDataProvider::getPartyId() -> int32_t {
	return m_partyIds.next();
}

auto PlayerDataProvider::getParty(int32_t id) -> Party * {
	auto kvp = m_parties.find(id);
	return (kvp != std::end(m_parties) ? kvp->second.get() : nullptr);
}

auto PlayerDataProvider::createParty(int32_t playerId) -> void {
	Player *player = getPlayer(playerId);
	if (player->getParty() != nullptr) {
		// Hacking
		return;
	}
	Party *party = new Party(getPartyId(), player->getId());
	party->addMember(player, true);

	m_parties[party->getId()].reset(party);
}

auto PlayerDataProvider::removePartyMember(int32_t playerId) -> void {
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

auto PlayerDataProvider::removePartyMember(int32_t playerId, int32_t target) -> void {
	Player *player = PlayerDataProvider::getInstance().getPlayer(playerId);
	Party *party = player->getParty();
	if (party == nullptr || !party->isLeader(playerId)) {
		// Hacking
		return;
	}

	Player *targetPlayer = PlayerDataProvider::getInstance().getPlayer(target, true);
	party->deleteMember(targetPlayer, true);
}

auto PlayerDataProvider::addPartyMember(int32_t playerId) -> void {
	Player *player = PlayerDataProvider::getInstance().getPlayer(playerId);
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

auto PlayerDataProvider::setPartyLeader(int32_t playerId, int32_t leaderId) -> void {
	Player *player = PlayerDataProvider::getInstance().getPlayer(playerId);
	Party *party = player->getParty();
	if (party == nullptr || !party->isLeader(playerId)) {
		// Hacking
		return;
	}

	party->setLeader(player);
}