/*
Copyright (C) 2008-2015 Vana Development Team

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
#include "Channel.hpp"
#include "Channels.hpp"
#include "Database.hpp"
#include "GameObjects.hpp"
#include "InitializeCommon.hpp"
#include "InterHeader.hpp"
#include "InterHelper.hpp"
#include "PacketWrapper.hpp"
#include "PlayerObjects.hpp"
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
	m_partyIds{1, 100000}
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
		<< "FROM " << Database::makeCharTable("characters") << " c "
		<< "WHERE c.world_id = :world",
		soci::use(worldId, "world"));

	for (const auto &row : rs) {
		PlayerData data;
		data.id = row.get<player_id_t>("character_id");
		data.name = row.get<string_t>("name");
		addPlayer(data);
	}

	std::cout << "DONE" << std::endl;
}

auto PlayerDataProvider::loadPlayer(player_id_t playerId) -> void {
	if (m_players.find(playerId) != std::end(m_players)) {
		return;
	}

	soci::rowset<> rs = (Database::getCharDb().prepare
		<< "SELECT c.character_id, c.name "
		<< "FROM " << Database::makeCharTable("characters") << " c "
		<< "WHERE c.character_id = :char",
		soci::use(playerId, "char"));

	const auto &row = *rs.begin();
	PlayerData data;
	data.id = row.get<player_id_t>("character_id");
	data.name = row.get<string_t>("name");
	addPlayer(data);
}

auto PlayerDataProvider::addPlayer(const PlayerData &data) -> void {
	m_players[data.id] = data;
	auto &element = m_players[data.id];
	m_playersByName[data.name] = &element;
}

auto PlayerDataProvider::sendSync(const PacketBuilder &builder) const -> void {
	WorldServer::getInstance().getChannels().send(builder);
}

auto PlayerDataProvider::channelDisconnect(channel_id_t channel) -> void {
	for (auto &kvp : m_players) {
		auto &player = kvp.second;
		if (player.channel == channel) {
			player.channel.reset();
			removePendingPlayer(player.id);
		}
	}
}

auto PlayerDataProvider::send(player_id_t playerId, const PacketBuilder &builder) -> void {
	auto &data = m_players[playerId];
	if (!data.channel.is_initialized()) {
		return;
	}

	WorldServer::getInstance().getChannels().send(data.channel.get(), Packets::prepend(
		builder, [&](PacketBuilder &packet) {
			packet
				.add<header_t>(IMSG_TO_PLAYER)
				.add<player_id_t>(playerId);
		}));
}

auto PlayerDataProvider::send(const vector_t<player_id_t> &playerIds, const PacketBuilder &builder) -> void {
	hash_map_t<channel_id_t, vector_t<player_id_t>> sendTargets;

	for (const auto &playerId : playerIds) {
		auto &data = m_players[playerId];
		if (!data.channel.is_initialized()) {
			continue;
		}

		auto kvp = sendTargets.find(data.channel.get());
		if (kvp == std::end(sendTargets)) {
			kvp = sendTargets.emplace(data.channel.get(), vector_t<player_id_t>{}).first;
		}

		kvp->second.push_back(data.id);
	}

	for (const auto &kvp : sendTargets) {
		WorldServer::getInstance().getChannels().send(kvp.first, Packets::prepend(
			builder, [&](PacketBuilder &packet) {
				packet
					.add<header_t>(IMSG_TO_PLAYER_LIST)
					.add<vector_t<player_id_t>>(kvp.second);
			}));
	}
}

auto PlayerDataProvider::send(const PacketBuilder &builder) -> void {
	hash_map_t<channel_id_t, vector_t<player_id_t>> sendTargets;

	for (const auto &iter : m_players) {
		auto &data = iter.second;
		if (!data.channel.is_initialized()) {
			continue;
		}
		auto kvp = sendTargets.find(data.channel.get());
		if (kvp == std::end(sendTargets)) {
			kvp = sendTargets.emplace(data.channel.get(), vector_t<player_id_t>{}).first;
		}

		kvp->second.push_back(data.id);
	}

	for (const auto &kvp : sendTargets) {
		WorldServer::getInstance().getChannels().send(kvp.first, Packets::prepend(
			builder, [&](PacketBuilder &packet) {
				packet
					.add<header_t>(IMSG_TO_PLAYER_LIST)
					.add<vector_t<player_id_t>>(kvp.second);
			}));
	}
}

// Handlers
auto PlayerDataProvider::handleSync(AbstractConnection *connection, sync_t type, PacketReader &reader) -> void {
	switch (type) {
		case Sync::SyncTypes::Player: handlePlayerSync(connection, reader); break;
		case Sync::SyncTypes::Party: handlePartySync(connection, reader); break;
		case Sync::SyncTypes::Buddy: handleBuddySync(connection, reader); break;
		default: throw NotImplementedException{"Sync type"};
	}
}

auto PlayerDataProvider::handlePlayerSync(AbstractConnection *connection, PacketReader &reader) -> void {
	switch (reader.get<sync_t>()) {
		case Sync::Player::ChangeChannelRequest: handleChangeChannelRequest(connection, reader); break;
		case Sync::Player::ChangeChannelGo: handleChangeChannel(connection, reader); break;
		case Sync::Player::Connect: handlePlayerConnect(static_cast<WorldServerAcceptConnection *>(connection)->getChannel(), reader); break;
		case Sync::Player::Disconnect: handlePlayerDisconnect(static_cast<WorldServerAcceptConnection *>(connection)->getChannel(), reader); break;
		case Sync::Player::UpdatePlayer: handlePlayerUpdate(reader); break;
		case Sync::Player::CharacterCreated: handleCharacterCreated(reader); break;
		case Sync::Player::CharacterDeleted: handleCharacterDeleted(reader); break;
		default: throw NotImplementedException{"PlayerSync type"};
	}
}

auto PlayerDataProvider::handlePartySync(AbstractConnection *connection, PacketReader &reader) -> void {
	sync_t type = reader.get<sync_t>();
	player_id_t playerId = reader.get<player_id_t>();
	switch (type) {
		case PartyActions::Create: handleCreateParty(playerId); break;
		case PartyActions::Leave: handlePartyLeave(playerId); break;
		case PartyActions::Expel: handlePartyRemove(playerId, reader.get<player_id_t>()); break;
		case PartyActions::Join: handlePartyAdd(playerId, reader.get<party_id_t>()); break;
		case PartyActions::SetLeader: handlePartyTransfer(playerId, reader.get<player_id_t>()); break;
		default: throw NotImplementedException{"PartySync type"};
	}
}

auto PlayerDataProvider::handleBuddySync(AbstractConnection *connection, PacketReader &reader) -> void {
	switch (reader.get<sync_t>()) {
		case Sync::Buddy::Invite: buddyInvite(reader); break;
		case Sync::Buddy::AcceptInvite: acceptBuddyInvite(reader); break;
		case Sync::Buddy::RemoveBuddy: removeBuddy(reader); break;
		case Sync::Buddy::ReaddBuddy: readdBuddy(reader); break;
		default: throw NotImplementedException{"BuddySync type"};
	}
}

// Players
auto PlayerDataProvider::removePendingPlayer(player_id_t id) -> channel_id_t {
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
	player_id_t playerId = reader.get<player_id_t>();
	auto &player = m_players[playerId];

	if (flags & Sync::Player::UpdateBits::Full) {
		PlayerData data = reader.get<PlayerData>();
		player.copyFrom(data);
	}
	else{
		if (flags & Sync::Player::UpdateBits::Level) {
			player.level = reader.get<player_level_t>();
		}
		if (flags & Sync::Player::UpdateBits::Job) {
			player.job = reader.get<job_id_t>();
		}
		if (flags & Sync::Player::UpdateBits::Map) {
			player.map = reader.get<map_id_t>();
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
		if (flags & Sync::Player::UpdateBits::Mts) {
			player.mts = reader.get<bool>();
		}
	}

	sendSync(SyncPacket::PlayerPacket::updatePlayer(player, flags));
}

auto PlayerDataProvider::handlePlayerConnect(channel_id_t channel, PacketReader &reader) -> void {
	bool firstConnect = reader.get<bool>();
	player_id_t playerId = reader.get<player_id_t>();
	auto &player = m_players[playerId];

	if (firstConnect) {
		PlayerData data = reader.get<PlayerData>();
		player.copyFrom(data);
		player.initialized = true;
		player.transferring = false;
		sendSync(SyncPacket::PlayerPacket::updatePlayer(player, Sync::Player::UpdateBits::Full));
	}
	else {
		// Only the map/channel are relevant
		player.map = reader.get<map_id_t>();
		player.channel = reader.get<channel_id_t>();
		player.ip = reader.get<Ip>();
		player.transferring = false;

		sendSync(SyncPacket::PlayerPacket::updatePlayer(player, Sync::Player::UpdateBits::Map | Sync::Player::UpdateBits::Channel | Sync::Player::UpdateBits::Transfer | Sync::Player::UpdateBits::Ip));
	}

	WorldServer::getInstance().getChannels().increasePopulation(channel);
}

auto PlayerDataProvider::handlePlayerDisconnect(channel_id_t channel, PacketReader &reader) -> void {
	player_id_t id = reader.get<player_id_t>();

	auto &player = m_players.find(id)->second;
	if (channel == -1 || player.channel == channel) {
		player.channel.reset();
		sendSync(SyncPacket::PlayerPacket::updatePlayer(player, Sync::Player::UpdateBits::Channel));
	}

	WorldServer::getInstance().getChannels().decreasePopulation(channel);

	channel_id_t oldChannel = removePendingPlayer(id);
	if (oldChannel != -1) {
		WorldServer::getInstance().getChannels().send(oldChannel, SyncPacket::PlayerPacket::deleteConnectable(id));
		player.transferring = false;
		sendSync(SyncPacket::PlayerPacket::updatePlayer(player, Sync::Player::UpdateBits::Transfer));
	}
}

auto PlayerDataProvider::handleCharacterCreated(PacketReader &reader) -> void {
	player_id_t id = reader.get<player_id_t>();
	loadPlayer(id);
	sendSync(SyncPacket::PlayerPacket::characterCreated(m_players[id]));
}

auto PlayerDataProvider::handleCharacterDeleted(PacketReader &reader) -> void {
	player_id_t id = reader.get<player_id_t>();
	// TODO FIXME interserver must handle this state when a character is deleted
	// To my knowledge, the player takes up space in buddies, parties, AND guilds until the player is kicked from those or the social grouping disappears
	// This means we can't delete the info when the character is deleted and must also take care to preserve it for buddies/guilds
	// This design is not in place yet
	sendSync(SyncPacket::PlayerPacket::characterDeleted(id));
}

auto PlayerDataProvider::handleChangeChannelRequest(AbstractConnection *connection, PacketReader &reader) -> void {
	player_id_t playerId = reader.get<player_id_t>();
	Channel *channel = WorldServer::getInstance().getChannels().getChannel(reader.get<channel_id_t>());
	Ip ip{0};
	port_t port = -1;
	if (channel != nullptr) {
		m_channelSwitches[playerId] = channel->getId();

		auto &player = m_players[playerId];
		player.transferring = true;
		sendSync(SyncPacket::PlayerPacket::updatePlayer(player, Sync::Player::UpdateBits::Transfer));

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
	player_id_t playerId = reader.get<player_id_t>();

	auto &player = m_players.find(playerId)->second;
	Channel *currentChannel = WorldServer::getInstance().getChannels().getChannel(player.channel.get());
	if (currentChannel == nullptr) {
		return;
	}

	channel_id_t channelId = m_channelSwitches[playerId];
	Channel *destinationChannel = WorldServer::getInstance().getChannels().getChannel(channelId);
	Ip ip{0};
	port_t port = -1;
	if (destinationChannel != nullptr) {
		ip = destinationChannel->matchIpToSubnet(player.ip);
		port = destinationChannel->getPort();
	}

	currentChannel->send(SyncPacket::PlayerPacket::playerChangeChannel(playerId, channelId, ip, port));
	removePendingPlayer(playerId);
}

// Parties
auto PlayerDataProvider::handleCreateParty(player_id_t playerId) -> void {
	auto &player = m_players[playerId];
	if (player.party > 0) {
		// Hacking
		return;
	}

	PartyData party;
	party.id = m_partyIds.lease();
	party.leader = player.id;
	party.members.push_back(player.id);
	m_parties[party.id] = party;

	player.party = party.id;

	sendSync(SyncPacket::PartyPacket::createParty(party.id, playerId));
}

auto PlayerDataProvider::handlePartyLeave(player_id_t playerId) -> void {
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
		m_partyIds.release(party.id);
		m_parties.erase(kvp);
	}
	else {
		ext::remove_element(party.members, playerId);
		sendSync(SyncPacket::PartyPacket::removePartyMember(party.id, playerId, false));
	}
}

auto PlayerDataProvider::handlePartyRemove(player_id_t playerId, player_id_t targetId) -> void {
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

auto PlayerDataProvider::handlePartyAdd(player_id_t playerId, party_id_t partyId) -> void {
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

auto PlayerDataProvider::handlePartyTransfer(player_id_t playerId, player_id_t newLeaderId) -> void {
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
	player_id_t inviterId = reader.get<player_id_t>();
	player_id_t inviteeId = reader.get<player_id_t>();
	auto &inviter = m_players[inviterId];
	auto &invitee = m_players[inviteeId];

	if (!invitee.channel.is_initialized()) {
		// Make new pending buddy in the database
		Database::getCharDb().once
			<< "INSERT INTO " << Database::makeCharTable("buddylist_pending") << " "
			<< "VALUES (:invitee, :name, :inviter)",
			soci::use(inviteeId, "invitee"),
			soci::use(inviter.name, "name"),
			soci::use(inviterId, "inviter");
	}
	else {
		WorldServer::getInstance().getChannels().send(invitee.channel.get(), SyncPacket::BuddyPacket::sendBuddyInvite(inviteeId, inviterId, inviter.name));
	}
}

auto PlayerDataProvider::acceptBuddyInvite(PacketReader &reader) -> void {
	player_id_t inviteeId = reader.get<player_id_t>();
	player_id_t inviterId = reader.get<player_id_t>();
	auto &invitee = m_players[inviteeId];
	auto &inviter = m_players[inviterId];

	invitee.mutualBuddies.push_back(inviterId);
	inviter.mutualBuddies.push_back(inviteeId);

	sendSync(SyncPacket::BuddyPacket::sendAcceptBuddyInvite(inviteeId, inviterId));
}

auto PlayerDataProvider::removeBuddy(PacketReader &reader) -> void {
	player_id_t listOwnerId = reader.get<player_id_t>();
	player_id_t removalId = reader.get<player_id_t>();
	auto &listOwner = m_players[listOwnerId];
	auto &removal = m_players[removalId];

	ext::remove_element(listOwner.mutualBuddies, removalId);
	ext::remove_element(removal.mutualBuddies, listOwnerId);

	sendSync(SyncPacket::BuddyPacket::sendBuddyRemoval(listOwnerId, removalId));
}

auto PlayerDataProvider::readdBuddy(PacketReader &reader) -> void {
	player_id_t listOwnerId = reader.get<player_id_t>();
	player_id_t buddyId = reader.get<player_id_t>();
	auto &listOwner = m_players[listOwnerId];
	auto &buddy = m_players[buddyId];

	listOwner.mutualBuddies.push_back(buddyId);
	buddy.mutualBuddies.push_back(listOwnerId);

	sendSync(SyncPacket::BuddyPacket::sendReaddBuddy(listOwnerId, buddyId));
}