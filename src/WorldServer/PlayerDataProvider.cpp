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
#include "Algorithm.hpp"
#include "Database.hpp"
#include "Channel.hpp"
#include "Channels.hpp"
#include "GameObjects.hpp"
#include "InitializeCommon.hpp"
#include "InterHeader.hpp"
#include "InterHelper.hpp"
#include "PacketCreator.hpp"
#include "PlayerObjects.hpp"
#include "SmsgHeader.hpp"
#include "StringUtilities.hpp"
#include "SyncHandler.hpp"
#include "SyncPacket.hpp"
#include "WorldServer.hpp"
#include "WorldServerAcceptHandler.hpp"
#include "WorldServerAcceptConnection.hpp"
#include "WorldServerAcceptPacket.hpp"
#include <iomanip>
#include <iostream>
#include <memory>

PlayerDataProvider::PlayerDataProvider() :
	m_partyIds(1, 100000)
{
}

auto PlayerDataProvider::loadData() -> void {
	world_id_t worldId = WorldServer::getInstance().getWorldId();
	loadPlayers(worldId);
}

auto PlayerDataProvider::getChannelConnectPacket(PacketCreator &packet) -> void {
	packet.add<uint32_t>(m_players.size());
	for (const auto &kvp : m_players) {
		packet.addClass<PlayerData>(kvp.second);
	}

	packet.add<uint32_t>(m_parties.size());
	for (const auto &kvp : m_parties) {
		packet.addClass<PartyData>(kvp.second);
	}
}

auto PlayerDataProvider::loadPlayers(world_id_t worldId) -> void {
	std::cout << std::setw(Initializing::OutputWidth) << std::left << "Initializing Players... ";

	soci::rowset<> rs = (Database::getCharDb().prepare
		<< "SELECT c.character_id, c.name "
		<< "FROM characters c "
		<< "WHERE c.world_id = :world",
		soci::use(worldId, "world"));

	for (const auto &row : rs) {
		PlayerData data;
		data.id = row.get<int32_t>("character_id");
		data.name = row.get<string_t>("name");
		addPlayer(data);
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

	const auto &row = *rs.begin();
	PlayerData data;
	data.id = row.get<int32_t>("character_id");
	data.name = row.get<string_t>("name");
	addPlayer(data);
	SyncPacket::PlayerPacket::characterCreated(data);
}

auto PlayerDataProvider::addPlayer(const PlayerData &data) -> void {
	m_players[data.id] = data;
	auto &element = m_players[data.id];
	m_playersByName[data.name] = &element;
}

auto PlayerDataProvider::channelDisconnect(channel_id_t channel) -> void {
	for (auto &kvp : m_players) {
		auto &player = kvp.second;
		if (player.channel == channel) {
			player.channel = -1;
			removePendingPlayer(player.id);
		}
	}
}

auto PlayerDataProvider::forwardPacketToPlayer(PacketReader &packet) -> void {
	int32_t playerId = packet.get<int32_t>();
	auto &data = m_players[playerId];
	if (data.channel == -1) {
		return;
	}

	// TODO FIXME interserver
	// TODO FIXME API
	// This is ugly and API redesign is sorely needed
	PacketCreator send;
	send.add<channel_id_t>(data.channel);
	send.add<header_t>(IMSG_TO_PLAYER);
	send.add<int32_t>(playerId);
	send.addBuffer(packet);
	PacketReader uglyHacks(const_cast<unsigned char *>(send.getBuffer()), send.getSize());
	WorldServerAcceptHandler::sendPacketToChannel(uglyHacks);
}

auto PlayerDataProvider::forwardPacketToPlayerList(PacketReader &packet) -> void {
	vector_t<int32_t> playerIds = packet.getVector<int32_t>();
	hash_map_t<channel_id_t, vector_t<int32_t>> sendTargets;

	for (const auto &playerId : playerIds) {
		auto &data = m_players[playerId];
		if (data.channel == -1) {
			continue;
		}

		auto kvp = sendTargets.find(data.channel);
		if (kvp == std::end(sendTargets)) {
			kvp = sendTargets.emplace(data.channel, vector_t<int32_t>()).first;
		}

		kvp->second.push_back(data.id);
	}

	// This is ugly and API redesign is sorely needed
	for (const auto &kvp : sendTargets) {
		PacketCreator send;
		send.add<channel_id_t>(kvp.first);
		send.add<header_t>(IMSG_TO_PLAYER_LIST);
		send.addVector<int32_t>(kvp.second);
		send.addBuffer(packet);
		PacketReader uglyHacks(const_cast<unsigned char *>(send.getBuffer()), send.getSize());
		WorldServerAcceptHandler::sendPacketToChannel(uglyHacks);
	}
}

auto PlayerDataProvider::forwardPacketToAllPlayers(PacketReader &packet) -> void {
	hash_map_t<channel_id_t, vector_t<int32_t>> sendTargets;

	for (const auto &iter : m_players) {
		auto &data = iter.second;
		if (data.channel == -1) {
			continue;
		}
		auto kvp = sendTargets.find(data.channel);
		if (kvp == std::end(sendTargets)) {
			kvp = sendTargets.emplace(data.channel, vector_t<int32_t>()).first;
		}

		kvp->second.push_back(data.id);
	}

	// TODO FIXME API
	// This is ugly and API redesign is sorely needed
	for (const auto &kvp : sendTargets) {
		PacketCreator send;
		send.add<channel_id_t>(kvp.first);
		send.add<header_t>(IMSG_TO_PLAYER_LIST);
		send.addVector<int32_t>(kvp.second);
		send.addBuffer(packet);
		PacketReader uglyHacks(const_cast<unsigned char *>(send.getBuffer()), send.getSize());
		WorldServerAcceptHandler::sendPacketToChannel(uglyHacks);
	}
}

// Handlers
auto PlayerDataProvider::handlePlayerSync(AbstractConnection *connection, PacketReader &packet) -> void {
	switch (packet.get<sync_t>()) {
		case Sync::Player::ChangeChannelRequest: handleChangeChannelRequest(connection, packet); break;
		case Sync::Player::ChangeChannelGo: handleChangeChannel(connection, packet); break;
		case Sync::Player::Connect: handlePlayerConnect(static_cast<WorldServerAcceptConnection *>(connection)->getChannel(), packet); break;
		case Sync::Player::Disconnect: handlePlayerDisconnect(static_cast<WorldServerAcceptConnection *>(connection)->getChannel(), packet); break;
		case Sync::Player::UpdatePlayer: handlePlayerUpdate(packet); break;
		case Sync::Player::CharacterCreated: handleCharacterCreated(packet); break;
		case Sync::Player::CharacterDeleted: handleCharacterDeleted(packet); break;
	}
}

auto PlayerDataProvider::handlePartySync(AbstractConnection *connection, PacketReader &packet) -> void {
	int8_t type = packet.get<sync_t>();
	int32_t playerId = packet.get<int32_t>();
	switch (type) {
		case PartyActions::Create: handleCreateParty(playerId); break;
		case PartyActions::Leave: handlePartyLeave(playerId); break;
		case PartyActions::Expel: handlePartyRemove(playerId, packet.get<int32_t>()); break;
		case PartyActions::Join: handlePartyAdd(playerId, packet.get<int32_t>()); break;
		case PartyActions::SetLeader: handlePartyTransfer(playerId, packet.get<int32_t>()); break;
	}
}

auto PlayerDataProvider::handleBuddySync(AbstractConnection *connection, PacketReader &packet) -> void {
	switch (packet.get<sync_t>()) {
		case Sync::Buddy::Invite: buddyInvite(packet); break;
		case Sync::Buddy::OnlineOffline: buddyOnline(packet); break;
	}
}

// Players
auto PlayerDataProvider::removePendingPlayer(int32_t id) -> channel_id_t {
	channel_id_t channel = -1;
	auto kvp = m_channelSwitches.find(id);
	if (kvp != std::end(m_channelSwitches)) {
		channel = kvp->second;
		m_channelSwitches.erase(kvp);
	}
	return channel;
}

auto PlayerDataProvider::handlePlayerUpdate(PacketReader &packet) -> void {
	update_bits_t flags = packet.get<update_bits_t>();
	int32_t playerId = packet.get<int32_t>();
	auto &player = m_players[playerId];

	if (flags & Sync::Player::UpdateBits::Full) {
		PlayerData data = packet.getClass<PlayerData>();
		player.copyFrom(data);
	}
	else{
		if (flags & Sync::Player::UpdateBits::Level) {
			player.level = packet.get<int16_t>();
		}
		if (flags & Sync::Player::UpdateBits::Job) {
			player.job = packet.get<int16_t>();
		}
		if (flags & Sync::Player::UpdateBits::Map) {
			player.map = packet.get<int32_t>();
		}
		if (flags & Sync::Player::UpdateBits::Channel) {
			player.channel = packet.get<channel_id_t>();
		}
	}

	SyncPacket::PlayerPacket::updatePlayer(player, flags);
}

auto PlayerDataProvider::handlePlayerConnect(channel_id_t channel, PacketReader &packet) -> void {
	PlayerData data = packet.getClass<PlayerData>();
	auto &player = m_players[data.id];
	player.copyFrom(data);

	SyncPacket::PlayerPacket::updatePlayer(player, Sync::Player::UpdateBits::Full);
	Channels::getInstance().increasePopulation(channel);
}

auto PlayerDataProvider::handlePlayerDisconnect(channel_id_t channel, PacketReader &packet) -> void {
	int32_t id = packet.get<int32_t>();

	auto &player = m_players.find(id)->second;
	if (channel == -1 || player.channel == channel) {
		player.channel = -1;
		if (player.party > 0) {
			SyncPacket::PlayerPacket::updatePlayer(player, Sync::Player::UpdateBits::Channel);
		}
	}

	Channels::getInstance().decreasePopulation(channel);

	channel_id_t oldChannel = removePendingPlayer(id);
	if (oldChannel != -1) {
		SyncPacket::PlayerPacket::deleteConnectable(oldChannel, id);
	}
}

auto PlayerDataProvider::handleCharacterCreated(PacketReader &packet) -> void {
	int32_t id = packet.get<int32_t>();
	loadPlayer(id);
	SyncPacket::PlayerPacket::characterCreated(m_players[id]);
}

auto PlayerDataProvider::handleCharacterDeleted(PacketReader &packet) -> void {
	int32_t id = packet.get<int32_t>();
	// TODO FIXME interserver must handle this state when a character is deleted
	// To my knowledge, the player takes up space in buddies, parties, AND guilds until the player is kicked from those or the social grouping disappears
	// This means we can't delete the info when the character is deleted and must also take care to preserve it for buddies/guilds
	// This design is not in place yet
	SyncPacket::PlayerPacket::characterDeleted(id);
}

auto PlayerDataProvider::handleChangeChannelRequest(AbstractConnection *connection, PacketReader &packet) -> void {
	int32_t playerId = packet.get<int32_t>();
	Channel *channel = Channels::getInstance().getChannel(packet.get<channel_id_t>());
	Ip ip(0);
	port_t port = -1;
	if (channel != nullptr) {
		m_channelSwitches[playerId] = channel->getId();

		ip = packet.getClass<Ip>();
		SyncPacket::PlayerPacket::newConnectable(channel->getId(), playerId, ip, packet);
	}
	else {
		SyncPacket::PlayerPacket::playerChangeChannel(connection, playerId, -1, ip, port);
	}
}

auto PlayerDataProvider::handleChangeChannel(AbstractConnection *connection, PacketReader &packet) -> void {
	// TODO FIXME
	// This request comes from the destination channel so I can't remove the ->getConnection() calls
	int32_t playerId = packet.get<int32_t>();

	auto &player = m_players.find(playerId)->second;
	Channel *currentChannel = Channels::getInstance().getChannel(player.channel);
	if (currentChannel == nullptr) {
		return;
	}

	channel_id_t channelId = m_channelSwitches[playerId];
	Channel *destinationChannel = Channels::getInstance().getChannel(channelId);
	Ip ip(0);
	port_t port = -1;
	if (destinationChannel != nullptr) {
		ip = destinationChannel->matchIpToSubnet(player.ip);
		port = destinationChannel->getPort();
	}

	SyncPacket::PlayerPacket::playerChangeChannel(currentChannel->getConnection(), playerId, channelId, ip, port);
	removePendingPlayer(playerId);
}

// Parties
auto PlayerDataProvider::handleCreateParty(int32_t playerId) -> void {
	auto &player = m_players[playerId];
	if (player.party > 0) {
		// Hacking
		return;
	}

	PartyData party;
	party.id = m_partyIds.next();
	party.leader = player.id;
	party.members.push_back(player.id);
	m_parties[party.id] = party;

	player.party = party.id;

	SyncPacket::PartyPacket::createParty(party.id, playerId);
}

auto PlayerDataProvider::handlePartyLeave(int32_t playerId) -> void {
	auto &player = m_players[playerId];
	if (player.party == 0) {
		// Hacking
		return;
	}

	auto kvp = m_parties.find(player.party);
	if (kvp == std::end(m_parties)) {
		// Lag or something
		return;
	}

	auto &party = kvp->second;
	player.party = 0;

	if (party.leader == playerId) {
		for (const auto &memberId : party.members) {
			if (memberId != playerId) {
				auto &member = m_players[memberId];
				member.party = 0;
			}
		}
		SyncPacket::PartyPacket::disbandParty(party.id);
		m_parties.erase(kvp);
	}
	else {
		ext::remove_element(party.members, playerId);
		SyncPacket::PartyPacket::removePartyMember(party.id, playerId, false);
	}
}

auto PlayerDataProvider::handlePartyRemove(int32_t playerId, int32_t targetId) -> void {
	auto &player = m_players[playerId];
	if (player.party == 0) {
		// Hacking
		return;
	}

	auto kvp = m_parties.find(player.party);
	if (kvp == std::end(m_parties)) {
		// Lag or something
		return;
	}

	auto &party = kvp->second;
	if (party.leader != playerId) {
		// Hacking
		return;
	}

	auto &target = m_players[targetId];
	target.party = 0;
	ext::remove_element(party.members, targetId);
	SyncPacket::PartyPacket::removePartyMember(party.id, targetId, true);
}

auto PlayerDataProvider::handlePartyAdd(int32_t playerId, int32_t partyId) -> void {
	auto &player = m_players[playerId];
	if (player.party != 0) {
		// Hacking
		return;
	}

	auto kvp = m_parties.find(partyId);
	if (kvp == std::end(m_parties)) {
		// Lag or hacking
		return;
	}

	auto &party = kvp->second;
	if (party.members.size() >= Parties::MaxMembers) {
		return;
	}

	player.party = party.id;
	party.members.push_back(player.id);
	SyncPacket::PartyPacket::addPartyMember(party.id, player.id);
}

auto PlayerDataProvider::handlePartyTransfer(int32_t playerId, int32_t newLeaderId) -> void {
	auto &player = m_players[playerId];
	if (player.party == 0) {
		// Hacking
		return;
	}

	auto kvp = m_parties.find(player.party);
	if (kvp == std::end(m_parties)) {
		// Lag or something
		return;
	}

	auto &party = kvp->second;
	if (party.leader != playerId) {
		// Hacking
		return;
	}

	auto &target = m_players[newLeaderId];
	if (target.party != player.party) {
		// ???
		return;
	}

	party.leader = newLeaderId;
	SyncPacket::PartyPacket::newPartyLeader(party.id, newLeaderId);
}

// Buddies
auto PlayerDataProvider::buddyInvite(PacketReader &packet) -> void {
	int32_t inviterId = packet.get<int32_t>();
	int32_t inviteeId = packet.get<int32_t>();
	auto &inviter = m_players[inviterId];
	auto &invitee = m_players[inviteeId];
	if (invitee.channel == -1) {
		// Make new pending buddy in the database
		Database::getCharDb().once
			<< "INSERT INTO buddylist_pending "
			<< "VALUES (:invitee, :name, :inviter)",
			soci::use(inviteeId, "invitee"),
			soci::use(inviter.name, "name"),
			soci::use(inviterId, "inviter");
	}
	else {
		SyncPacket::BuddyPacket::sendBuddyInvite(Channels::getInstance().getChannel(invitee.channel), inviteeId, inviterId, inviter.name);
	}
}

auto PlayerDataProvider::buddyOnline(PacketReader &packet) -> void {
	int32_t playerId = packet.get<int32_t>();
	bool online = packet.get<bool>();
	vector_t<int32_t> tempIds = packet.getVector<int32_t>();

	auto &player = m_players[playerId];
	hash_map_t<channel_id_t, vector_t<int32_t>> ids; // <channel, <ids>>, for sending less packets for a buddylist of 100 people

	for (const auto &id : tempIds) {
		auto &buddy = m_players[id];
		if (buddy.channel != -1) {
			ids[buddy.channel].push_back(id);
		}
	}

	for (const auto &kvp : ids) {
		if (Channel *channel = Channels::getInstance().getChannel(kvp.first)) {
			SyncPacket::BuddyPacket::sendBuddyOnlineOffline(channel, kvp.second, playerId, (online ? player.channel : -1));
		}
	}
}