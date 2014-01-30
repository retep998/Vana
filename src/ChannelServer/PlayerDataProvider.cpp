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
#include "BuddyListPacket.hpp"
#include "ChannelServer.hpp"
#include "Connectable.hpp"
#include "Database.hpp"
#include "InterHeader.hpp"
#include "InterHelper.hpp"
#include "PacketCreator.hpp"
#include "PacketReader.hpp"
#include "Party.hpp"
#include "PartyPacket.hpp"
#include "Player.hpp"
#include "PlayerPacket.hpp"
#include "PlayersPacket.hpp"
#include "Session.hpp"
#include "SmsgHeader.hpp"
#include "StringUtilities.hpp"
#include "SyncPacket.hpp"
#include <algorithm>
#include <cstring>

auto PlayerDataProvider::parseChannelConnectPacket(PacketReader &packet) -> void {
	// Players
	uint32_t quantity = packet.get<uint32_t>();
	for (uint32_t i = 0; i < quantity; i++) {
		PlayerData player = packet.getClass<PlayerData>();
		addPlayerData(player);
	}

	// Parties
	quantity = packet.get<uint32_t>();
	for (uint32_t i = 0; i < quantity; i++) {
		PartyData data = packet.getClass<PartyData>();
		ref_ptr_t<Party> party = make_ref_ptr<Party>(data.id);
		party->setLeader(data.leader);

		for (const auto &member : data.members) {
			auto &player = m_playerData[member];
			player.party = data.id;
			party->addMember(member, player.name, true);
		}

		m_parties[data.id] = party;
	}
}

// Players
auto PlayerDataProvider::addPlayerData(const PlayerData &data) -> void {
	m_playerData[data.id] = data;
	auto &player = m_playerData[data.id];
	m_playerDataByName[data.name] = &player;

	if (data.gmLevel > 0 || data.admin) {
		m_gmList.insert(data.id);
	}
}

auto PlayerDataProvider::addPlayer(Player *player) -> void {
	m_players[player->getId()] = player;
	m_playersByName[player->getName()] = player;
	auto &playerData = m_playerData[player->getId()];
	if (playerData.party > 0) {
		Party *party = getParty(playerData.party);
		player->setParty(party);
		party->setMember(playerData.id, player);
	}
}

auto PlayerDataProvider::removePlayer(Player *player) -> void {
	m_players.erase(player->getId());
	m_playersByName.erase(player->getName());

	if (Party *party = player->getParty()) {
		party->setMember(player->getId(), nullptr);
	}

	if (Player *followed = player->getFollow()) {
		stopFollowing(followed);
	}

	auto kvp = m_followers.find(player->getId());
	if (kvp != std::end(m_followers)) {
		for (auto follower : kvp->second) {
			PlayerPacket::showMessage(follower, "Player " + player->getName() + " has disconnected", PlayerPacket::NoticeTypes::Red);
			follower->setFollow(nullptr);
		}
		m_followers.erase(kvp);
	}
}

auto PlayerDataProvider::updatePlayerLevel(Player *player) -> void {
	auto &data = m_playerData[player->getId()];
	data.level = player->getStats()->getLevel();
	SyncPacket::PlayerPacket::updatePlayer(data, Sync::Player::UpdateBits::Level);
	if (data.party != 0) {
		getParty(data.party)->silentUpdate();
	}
}

auto PlayerDataProvider::updatePlayerMap(Player *player) -> void {
	auto &data = m_playerData[player->getId()];
	data.map = player->getMapId();
	SyncPacket::PlayerPacket::updatePlayer(data, Sync::Player::UpdateBits::Map);
	if (data.party != 0) {
		getParty(data.party)->silentUpdate();
	}

	auto kvp = m_followers.find(data.id);
	if (kvp != std::end(m_followers)) {
		for (auto player : kvp->second) {
			player->setMap(data.map);
		}
	}
}

auto PlayerDataProvider::updatePlayerJob(Player *player) -> void {
	auto &data = m_playerData[player->getId()];
	data.job = player->getStats()->getJob();
	SyncPacket::PlayerPacket::updatePlayer(data, Sync::Player::UpdateBits::Job);
	if (data.party != 0) {
		getParty(data.party)->silentUpdate();
	}
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

auto PlayerDataProvider::getParty(int32_t id) -> Party * {
	return m_parties.find(id) == std::end(m_parties) ? nullptr : m_parties[id].get();
}

auto PlayerDataProvider::getPlayerData(int32_t id) const -> const PlayerData * const {
	return &m_playerData.find(id)->second;
}

auto PlayerDataProvider::getPlayerDataByName(const string_t &name) const -> const PlayerData * const {
	auto kvp = m_playerDataByName.find(name);
	if (kvp == std::end(m_playerDataByName)) {
		return nullptr;
	}
	return kvp->second;
}

auto PlayerDataProvider::sendPacket(PacketCreator &packet, int32_t minGmLevel) -> void {
	for (const auto &kvp : m_players) {
		Player *player = kvp.second;
		if (player->getGmLevel() >= minGmLevel) {
			player->getSession()->send(packet);
		}
	}
}

auto PlayerDataProvider::sendPacketToList(const vector_t<int32_t> &playerIds, PacketCreator &packet) -> void {
	for (const auto &playerId : playerIds) {
		auto kvp = m_players.find(playerId);
		if (kvp != std::end(m_players)) {
			kvp->second->getSession()->send(packet);
		}
	}
}

auto PlayerDataProvider::addFollower(Player *follower, Player *target) -> void {
	auto kvp = m_followers.find(target->getId());
	if (kvp == std::end(m_followers)) {
		kvp = m_followers.emplace(target->getId(), vector_t<Player *>()).first;
	}
	kvp->second.push_back(follower);
	follower->setFollow(target);
}

auto PlayerDataProvider::stopFollowing(Player *follower) -> void {
	auto kvp = m_followers.find(follower->getFollow()->getId());
	ext::remove_element(kvp->second, follower);
	if (kvp->second.size() == 0) {
		m_followers.erase(kvp);
	}
	follower->setFollow(nullptr);
}

auto PlayerDataProvider::handleGroupChat(int8_t chatType, int32_t playerId, const vector_t<int32_t> &receivers, const string_t &chat) -> void {
	// TODO FIXME API
	PacketCreator groupMessagePacket;
	groupMessagePacket.add<header_t>(SMSG_MESSAGE_GROUP);
	groupMessagePacket.add<int8_t>(chatType);
	groupMessagePacket.addString(m_playerData[playerId].name);
	groupMessagePacket.addString(chat);

	vector_t<int32_t> nonPresentReceivers;
	for (const auto &playerId : receivers) {
		if (Player *player = getPlayer(playerId)) {
			player->getSession()->send(groupMessagePacket);
		}
		else {
			nonPresentReceivers.push_back(playerId);
		}
	}

	if (nonPresentReceivers.size() > 0) {
		PacketCreator wrapped;
		wrapped.add<header_t>(IMSG_TO_PLAYER_LIST);
		wrapped.addVector<int32_t>(nonPresentReceivers);
		wrapped.addBuffer(groupMessagePacket);
		ChannelServer::getInstance().sendPacketToWorld(wrapped);
	}
}

auto PlayerDataProvider::handleGmChat(Player *player, const string_t &chat) -> void {
	PacketCreator messagePacket;
	out_stream_t message;
	message << player->getName() << " [ch"
		<< static_cast<int32_t>(ChannelServer::getInstance().getChannelId() + 1)
		<< "] : " << chat;

	PlayerPacket::showMessagePacket(messagePacket, message.str(), PlayerPacket::NoticeTypes::Blue);

	vector_t<int32_t> nonPresentReceivers;
	for (const auto &playerId : m_gmList) {
		if (Player *player = getPlayer(playerId)) {
			player->getSession()->send(messagePacket);
		}
		else {
			nonPresentReceivers.push_back(playerId);
		}
	}

	if (nonPresentReceivers.size() > 0) {
		PacketCreator wrapped;
		wrapped.add<header_t>(IMSG_TO_PLAYER_LIST);
		wrapped.addVector<int32_t>(nonPresentReceivers);
		wrapped.addBuffer(messagePacket);
		ChannelServer::getInstance().sendPacketToWorld(wrapped);
	}
}

auto PlayerDataProvider::handlePlayerSync(PacketReader &packet) -> void {
	switch (packet.get<sync_t>()) {
		case Sync::Player::NewConnectable: handleNewConnectable(packet); break;
		case Sync::Player::DeleteConnectable: handleDeleteConnectable(packet.get<int32_t>()); break;
		case Sync::Player::ChangeChannelGo: handleChangeChannel(packet); break;
		case Sync::Player::UpdatePlayer: handleUpdatePlayer(packet); break;
		case Sync::Player::CharacterCreated: handleCharacterCreated(packet); break;
		case Sync::Player::CharacterDeleted: handleCharacterDeleted(packet); break;
	}
}

auto PlayerDataProvider::handlePartySync(PacketReader &packet) -> void {
	int8_t type = packet.get<sync_t>();
	int32_t partyId = packet.get<int32_t>();
	switch (type) {
		case Sync::Party::Create: handleCreateParty(partyId, packet.get<int32_t>()); break;
		case Sync::Party::Disband: handleDisbandParty(partyId); break;
		case Sync::Party::SwitchLeader: handlePartyTransfer(partyId, packet.get<int32_t>()); break;
		case Sync::Party::AddMember: handlePartyAdd(partyId, packet.get<int32_t>()); break;
		case Sync::Party::RemoveMember: {
			int32_t playerId = packet.get<int32_t>();
			handlePartyRemove(partyId, playerId, packet.get<bool>());
			break;
		}
	}
}

auto PlayerDataProvider::handleBuddySync(PacketReader &packet) -> void {
	switch (packet.get<sync_t>()) {
		case Sync::Buddy::Invite: PlayerDataProvider::buddyInvite(packet); break;
		case Sync::Buddy::OnlineOffline: PlayerDataProvider::buddyOnlineOffline(packet); break;
	}
}

auto PlayerDataProvider::handleChangeChannel(PacketReader &packet) -> void {
	int32_t playerId = packet.get<int32_t>();
	channel_id_t channelId = packet.get<channel_id_t>();
	const Ip &ip = packet.getClass<Ip>();
	port_t port = packet.get<port_t>();

	if (Player *player = getPlayer(playerId)) {
		if (!ip.isInitialized()) {
			PlayerPacket::sendBlockedMessage(player, PlayerPacket::BlockMessages::CannotGo);
		}
		else {
			auto kvp = m_followers.find(playerId);
			if (kvp != std::end(m_followers)) {
				for (auto follower : kvp->second) {
					follower->changeChannel(channelId);
					follower->setFollow(nullptr);
				}

				m_followers.erase(kvp);
			}

			player->setOnline(false); // Set online to false BEFORE CC packet is sent to player
			PlayerPacket::changeChannel(player, ip, port);
			player->saveAll(true);
			player->setSaveOnDc(false);
		}
	}
}

auto PlayerDataProvider::handleNewConnectable(PacketReader &packet) -> void {
	int32_t playerId = packet.get<int32_t>();
	const Ip &ip = packet.getClass<Ip>();
	Connectable::getInstance().newPlayer(playerId, ip, packet);
	SyncPacket::PlayerPacket::connectableEstablished(playerId);
}

auto PlayerDataProvider::handleDeleteConnectable(int32_t id) -> void {
	Connectable::getInstance().playerEstablished(id);
}

auto PlayerDataProvider::handleUpdatePlayer(PacketReader &packet) -> void {
	int32_t playerId = packet.get<int32_t>();
	auto &player = m_playerData[playerId];

	update_bits_t flags = packet.get<update_bits_t>();
	bool updateParty = false;
	bool updateGuild = false;
	bool updateAlliance = false;
	int16_t oldJob = player.job;
	int16_t oldLevel = player.level;
	int32_t oldMap = player.map;
	channel_id_t oldChannel = player.channel;

	if (flags & Sync::Player::UpdateBits::Full) {
		updateParty = true;
		updateGuild = true;
		updateAlliance = true;
		PlayerData data = packet.getClass<PlayerData>();
		player.copyFrom(data);
		if (data.gmLevel > 0 || data.admin) {
			m_gmList.insert(data.id);
		}
	}
	else {
		if (flags & Sync::Player::UpdateBits::Job) {
			player.job = packet.get<int16_t>();
			updateParty = true;
			updateGuild = true;
			updateAlliance = true;
		}
		if (flags & Sync::Player::UpdateBits::Level) {
			player.level = packet.get<int16_t>();
			updateParty = true;
			updateGuild = true;
			updateAlliance = true;
		}
		if (flags & Sync::Player::UpdateBits::Map) {
			player.map = packet.get<int32_t>();
			updateParty = true;
		}
		if (flags & Sync::Player::UpdateBits::Channel) {
			player.channel = packet.get<channel_id_t>();
			updateParty = true;
		}
	}

	bool actuallyUpdated = oldJob != player.job ||
		oldLevel != player.level ||
		oldMap != player.map ||
		oldChannel != player.channel;

	if (actuallyUpdated && updateParty && player.party != 0) {
		getParty(player.party)->silentUpdate();
	}
}

auto PlayerDataProvider::handleCharacterCreated(PacketReader &packet) -> void {
	PlayerData data = packet.getClass<PlayerData>();
	addPlayerData(data);
}

auto PlayerDataProvider::handleCharacterDeleted(PacketReader &packet) -> void {
	int32_t id = packet.get<int32_t>();
	// Intentionally blank for now
}

// Parties
auto PlayerDataProvider::handleCreateParty(int32_t id, int32_t leaderId) -> void {
	ref_ptr_t<Party> p = make_ref_ptr<Party>(id);
	Player *leader = getPlayer(leaderId);
	auto &data = m_playerData[leaderId];
	data.party = id;

	if (leader == nullptr) {
		p->addMember(leaderId, data.name, true);
	}
	else {
		p->addMember(leader, true);
		PartyPacket::createParty(leader, p.get());
	}
	
	p->setLeader(leaderId);
	m_parties[id] = p;
}

auto PlayerDataProvider::handleDisbandParty(int32_t id) -> void {
	if (Party *party = getParty(id)) {
		auto &members = party->getMembers();
		for (const auto &kvp : members) {
			auto &member = m_playerData[kvp.first];
			member.party = 0;
		}

		party->disband();
		m_parties.erase(id);
	}
}

auto PlayerDataProvider::handlePartyTransfer(int32_t id, int32_t leaderId) -> void {
	if (Party *party = getParty(id)) {
		party->setLeader(leaderId, true);
	}
}

auto PlayerDataProvider::handlePartyRemove(int32_t id, int32_t playerId, bool kicked) -> void {
	if (Party *party = getParty(id)) {
		auto &data = m_playerData[playerId];
		data.party = 0;
		if (Player *member = getPlayer(playerId)) {
			party->deleteMember(member, kicked);
		}
		else {
			party->deleteMember(playerId, data.name, kicked);
		}
	}
}

auto PlayerDataProvider::handlePartyAdd(int32_t id, int32_t playerId) -> void {
	if (Party *party = getParty(id)) {
		auto &data = m_playerData[playerId];
		data.party = id;
		if (Player *member = getPlayer(playerId)) {
			party->addMember(member);
		}
		else {
			party->addMember(playerId, data.name);
		}
	}
}

// Buddies
auto PlayerDataProvider::buddyInvite(PacketReader &packet) -> void {
	int32_t inviterId = packet.get<int32_t>();
	int32_t inviteeId = packet.get<int32_t>();
	if (Player *invitee = getPlayer(inviteeId)) {
		BuddyInvite invite;
		invite.id = inviterId;
		invite.name = packet.getString();
		invitee->getBuddyList()->addBuddyInvite(invite);
		invitee->getBuddyList()->checkForPendingBuddy();
	}
}

auto PlayerDataProvider::buddyOnlineOffline(PacketReader &packet) -> void {
	int32_t playerId = packet.get<int32_t>(); // The id of the player coming online
	channel_id_t channel = packet.get<channel_id_t>();
	vector_t<int32_t> players = packet.getVector<int32_t>(); // Holds the buddy IDs

	for (size_t i = 0; i < players.size(); i++) {
		if (Player *player = getPlayer(players[i])) {
			if (ref_ptr_t<Buddy> ptr = player->getBuddyList()->getBuddy(playerId)) {
				ptr->channel = channel;
				BuddyListPacket::online(player, playerId, channel);
			}
		}
	}
}