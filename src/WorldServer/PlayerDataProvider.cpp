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
#include "PacketWrapper.hpp"
#include "PlayerObjects.hpp"
#include "SmsgHeader.hpp"
#include "StringUtilities.hpp"
#include "SyncHandler.hpp"
#include "SyncPacket.hpp"
#include "WorldServer.hpp"
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

auto PlayerDataProvider::getChannelConnectPacket(PacketBuilder &packet) -> void {
	packet.add<uint32_t>(m_players.size());
	for (const auto &kvp : m_players) {
		packet.add<PlayerData>(kvp.second);
	}

	packet.add<uint32_t>(m_parties.size());
	for (const auto &kvp : m_parties) {
		packet.add<PartyData>(kvp.second);
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
}

auto PlayerDataProvider::addPlayer(const PlayerData &data) -> void {
	m_players[data.id] = data;
	auto &element = m_players[data.id];
	m_playersByName[data.name] = &element;
}

auto PlayerDataProvider::sendSync(const PacketBuilder &builder) const -> void {
	Channels::getInstance().send(builder);
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

auto PlayerDataProvider::send(int32_t playerId, const PacketBuilder &builder) -> void {
	auto &data = m_players[playerId];
	if (data.channel == -1) {
		return;
	}

	Channels::getInstance().send(data.channel, Packets::prepend(
		builder, [&](PacketBuilder &packet) {
			packet
				.add<header_t>(IMSG_TO_PLAYER)
				.add<int32_t>(playerId);
		}));
}

auto PlayerDataProvider::send(const vector_t<int32_t> &playerIds, const PacketBuilder &builder) -> void {
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

	for (const auto &kvp : sendTargets) {
		Channels::getInstance().send(kvp.first, Packets::prepend(
			builder, [&](PacketBuilder &packet) {
				packet
					.add<header_t>(IMSG_TO_PLAYER_LIST)
					.add<vector_t<int32_t>>(kvp.second);
			}));
	}
}

auto PlayerDataProvider::send(const PacketBuilder &builder) -> void {
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

	for (const auto &kvp : sendTargets) {
		Channels::getInstance().send(kvp.first, Packets::prepend(
			builder, [&](PacketBuilder &packet) {
				packet
					.add<header_t>(IMSG_TO_PLAYER_LIST)
					.add<vector_t<int32_t>>(kvp.second);
			}));
	}
}

// Handlers
auto PlayerDataProvider::handlePlayerSync(AbstractConnection *connection, PacketReader &reader) -> void {
	switch (reader.get<sync_t>()) {
		case Sync::Player::ChangeChannelRequest: handleChangeChannelRequest(connection, reader); break;
		case Sync::Player::ChangeChannelGo: handleChangeChannel(connection, reader); break;
		case Sync::Player::Connect: handlePlayerConnect(static_cast<WorldServerAcceptConnection *>(connection)->getChannel(), reader); break;
		case Sync::Player::Disconnect: handlePlayerDisconnect(static_cast<WorldServerAcceptConnection *>(connection)->getChannel(), reader); break;
		case Sync::Player::UpdatePlayer: handlePlayerUpdate(reader); break;
		case Sync::Player::CharacterCreated: handleCharacterCreated(reader); break;
		case Sync::Player::CharacterDeleted: handleCharacterDeleted(reader); break;
	}
}

auto PlayerDataProvider::handlePartySync(AbstractConnection *connection, PacketReader &reader) -> void {
	int8_t type = reader.get<sync_t>();
	int32_t playerId = reader.get<int32_t>();
	switch (type) {
		case PartyActions::Create: handleCreateParty(playerId); break;
		case PartyActions::Leave: handlePartyLeave(playerId); break;
		case PartyActions::Expel: handlePartyRemove(playerId, reader.get<int32_t>()); break;
		case PartyActions::Join: handlePartyAdd(playerId, reader.get<int32_t>()); break;
		case PartyActions::SetLeader: handlePartyTransfer(playerId, reader.get<int32_t>()); break;
	}
}

auto PlayerDataProvider::handleBuddySync(AbstractConnection *connection, PacketReader &reader) -> void {
	switch (reader.get<sync_t>()) {
		case Sync::Buddy::Invite: buddyInvite(reader); break;
		case Sync::Buddy::OnlineOffline: buddyOnline(reader); break;
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

auto PlayerDataProvider::handlePlayerUpdate(PacketReader &reader) -> void {
	update_bits_t flags = reader.get<update_bits_t>();
	int32_t playerId = reader.get<int32_t>();
	auto &player = m_players[playerId];

	if (flags & Sync::Player::UpdateBits::Full) {
		PlayerData data = reader.get<PlayerData>();
		player.copyFrom(data);
	}
	else{
		if (flags & Sync::Player::UpdateBits::Level) {
			player.level = reader.get<int16_t>();
		}
		if (flags & Sync::Player::UpdateBits::Job) {
			player.job = reader.get<int16_t>();
		}
		if (flags & Sync::Player::UpdateBits::Map) {
			player.map = reader.get<int32_t>();
		}
		if (flags & Sync::Player::UpdateBits::Channel) {
			player.channel = reader.get<channel_id_t>();
		}
		if (flags & Sync::Player::UpdateBits::Ip) {
			player.ip = reader.get<Ip>();
		}
		if (flags & Sync::Player::UpdateBits::Cash) {
			player.cashShop = reader.get<bool>();
		}
	}

	sendSync(SyncPacket::PlayerPacket::updatePlayer(player, flags));
}

auto PlayerDataProvider::handlePlayerConnect(channel_id_t channel, PacketReader &reader) -> void {
	bool firstConnect = reader.get<bool>();
	int32_t playerId = reader.get<int32_t>();
	auto &player = m_players[playerId];

	if (firstConnect) {
		PlayerData data = reader.get<PlayerData>();
		player.copyFrom(data);
		player.initialized = true;
		sendSync(SyncPacket::PlayerPacket::updatePlayer(player, Sync::Player::UpdateBits::Full));
	}
	else {
		// Only the map/channel are relevant
		player.map = reader.get<int32_t>();
		player.channel = reader.get<channel_id_t>();
		player.ip = reader.get<Ip>();

		sendSync(SyncPacket::PlayerPacket::updatePlayer(player, Sync::Player::UpdateBits::Map | Sync::Player::UpdateBits::Channel | Sync::Player::UpdateBits::Ip));
	}

	Channels::getInstance().increasePopulation(channel);
}

auto PlayerDataProvider::handlePlayerDisconnect(channel_id_t channel, PacketReader &reader) -> void {
	int32_t id = reader.get<int32_t>();

	auto &player = m_players.find(id)->second;
	if (channel == -1 || player.channel == channel) {
		player.channel = -1;
		if (player.party > 0) {
			sendSync(SyncPacket::PlayerPacket::updatePlayer(player, Sync::Player::UpdateBits::Channel));
		}
	}

	Channels::getInstance().decreasePopulation(channel);

	channel_id_t oldChannel = removePendingPlayer(id);
	if (oldChannel != -1) {
		Channels::getInstance().send(oldChannel, SyncPacket::PlayerPacket::deleteConnectable(id));
	}
}

auto PlayerDataProvider::handleCharacterCreated(PacketReader &reader) -> void {
	int32_t id = reader.get<int32_t>();
	loadPlayer(id);
	sendSync(SyncPacket::PlayerPacket::characterCreated(m_players[id]));
}

auto PlayerDataProvider::handleCharacterDeleted(PacketReader &reader) -> void {
	int32_t id = reader.get<int32_t>();
	// TODO FIXME interserver must handle this state when a character is deleted
	// To my knowledge, the player takes up space in buddies, parties, AND guilds until the player is kicked from those or the social grouping disappears
	// This means we can't delete the info when the character is deleted and must also take care to preserve it for buddies/guilds
	// This design is not in place yet
	sendSync(SyncPacket::PlayerPacket::characterDeleted(id));
}

auto PlayerDataProvider::handleChangeChannelRequest(AbstractConnection *connection, PacketReader &reader) -> void {
	int32_t playerId = reader.get<int32_t>();
	Channel *channel = Channels::getInstance().getChannel(reader.get<channel_id_t>());
	Ip ip(0);
	port_t port = -1;
	if (channel != nullptr) {
		m_channelSwitches[playerId] = channel->getId();

		ip = reader.get<Ip>();
		channel->send(SyncPacket::PlayerPacket::newConnectable(playerId, ip, reader));
	}
	else {
		connection->send(SyncPacket::PlayerPacket::playerChangeChannel(playerId, -1, ip, port));
	}
}

auto PlayerDataProvider::handleChangeChannel(AbstractConnection *connection, PacketReader &reader) -> void {
	// TODO FIXME
	// This request comes from the destination channel so I can't remove the ->getConnection() calls
	int32_t playerId = reader.get<int32_t>();

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

	currentChannel->send(SyncPacket::PlayerPacket::playerChangeChannel(playerId, channelId, ip, port));
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

	sendSync(SyncPacket::PartyPacket::createParty(party.id, playerId));
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
		sendSync(SyncPacket::PartyPacket::disbandParty(party.id));
		m_parties.erase(kvp);
	}
	else {
		ext::remove_element(party.members, playerId);
		sendSync(SyncPacket::PartyPacket::removePartyMember(party.id, playerId, false));
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
	sendSync(SyncPacket::PartyPacket::removePartyMember(party.id, targetId, true));
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
	sendSync(SyncPacket::PartyPacket::addPartyMember(party.id, player.id));
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
	sendSync(SyncPacket::PartyPacket::newPartyLeader(party.id, newLeaderId));
}

// Buddies
auto PlayerDataProvider::buddyInvite(PacketReader &reader) -> void {
	int32_t inviterId = reader.get<int32_t>();
	int32_t inviteeId = reader.get<int32_t>();
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
		Channels::getInstance().send(invitee.channel, SyncPacket::BuddyPacket::sendBuddyInvite(inviteeId, inviterId, inviter.name));
	}
}

auto PlayerDataProvider::buddyOnline(PacketReader &reader) -> void {
	int32_t playerId = reader.get<int32_t>();
	bool online = reader.get<bool>();
	vector_t<int32_t> tempIds = reader.get<vector_t<int32_t>>();

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
			channel->send(SyncPacket::BuddyPacket::sendBuddyOnlineOffline(kvp.second, playerId, (online ? player.channel : -1)));
		}
	}
}