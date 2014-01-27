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
#include "SyncHandler.hpp"
#include "AbstractConnection.hpp"
#include "Channel.hpp"
#include "Channels.hpp"
#include "Database.hpp"
#include "GameObjects.hpp"
#include "InterHeader.hpp"
#include "InterHelper.hpp"
#include "PacketCreator.hpp"
#include "PacketReader.hpp"
#include "Party.hpp"
#include "Player.hpp"
#include "PlayerDataProvider.hpp"
#include "Session.hpp"
#include "SmsgHeader.hpp"
#include "SyncPacket.hpp"
#include "WorldServer.hpp"
#include "WorldServerAcceptConnection.hpp"
#include "WorldServerAcceptPacket.hpp"

auto SyncHandler::handle(AbstractConnection *connection, PacketReader &packet) -> void {
	switch (packet.get<int8_t>()) {
		case Sync::SyncTypes::Config: handleConfigSync(packet); break;
		case Sync::SyncTypes::Player: handlePlayerSync(connection, packet); break;
		case Sync::SyncTypes::Party: handlePartySync(packet); break;
		case Sync::SyncTypes::Buddy: handleBuddySync(packet); break;
	}
}

auto SyncHandler::handleConfigSync(PacketReader &packet) -> void {
	switch (packet.get<int8_t>()) {
		case Sync::Config::RateSet: handleSetRates(packet); break;
		case Sync::Config::RateReset: WorldServer::getInstance().resetRates(); break;
		case Sync::Config::ScrollingHeader: handleScrollingHeader(packet); break;
	}
}

auto SyncHandler::handleSetRates(PacketReader &packet) -> void {
	Rates rates = packet.getClass<Rates>();
	WorldServer::getInstance().setRates(rates);
}

auto SyncHandler::handleScrollingHeader(PacketReader &packet) -> void {
	const string_t &message = packet.getString();
	WorldServer::getInstance().setScrollingHeader(message);
}

auto SyncHandler::handlePlayerSync(AbstractConnection *connection, PacketReader &packet) -> void {
	switch (packet.get<int8_t>()) {
		case Sync::Player::ChangeChannelRequest: playerChangeChannel(connection, packet); break;
		case Sync::Player::ChangeChannelGo: handleChangeChannel(connection, packet); break;
		case Sync::Player::Connect: playerConnect(static_cast<WorldServerAcceptConnection *>(connection)->getChannel(), packet); break;
		case Sync::Player::Disconnect: playerDisconnect(static_cast<WorldServerAcceptConnection *>(connection)->getChannel(), packet); break;
		case Sync::Player::UpdatePlayer: handlePlayerUpdate(packet); break;
		case Sync::Player::CharacterCreated: handleCharacterCreated(packet); break;
	}
}

auto SyncHandler::handlePlayerUpdate(PacketReader &packet) -> void {
	int8_t bits = packet.get<int8_t>();
	int32_t playerId = packet.get<int32_t>();
	Player *player = PlayerDataProvider::getInstance().getPlayer(playerId);

	if (bits & Sync::Player::UpdateBits::Level) {
		player->setLevel(packet.get<uint8_t>());
	}
	if (bits & Sync::Player::UpdateBits::Job) {
		player->setJob(packet.get<int16_t>());
	}
	if (bits & Sync::Player::UpdateBits::Map) {
		player->setMap(packet.get<int32_t>());
	}
}

auto SyncHandler::playerConnect(int16_t channel, PacketReader &packet) -> void {
	int32_t id = packet.get<int32_t>();
	int32_t map = packet.get<int32_t>();

	Player *p = PlayerDataProvider::getInstance().getPlayer(id, true);
	if (p == nullptr) {
		p = new Player(id);
	}
	p->setMap(map);
	p->setChannel(channel);
	p->setOnline(true);
	PlayerDataProvider::getInstance().playerConnect(p);
}

auto SyncHandler::playerDisconnect(int16_t channel, PacketReader &packet) -> void {
	int32_t id = packet.get<int32_t>();
	PlayerDataProvider::getInstance().playerDisconnect(id, channel);
	int16_t chan = PlayerDataProvider::getInstance().removePendingPlayerEarly(id);
	if (chan != -1) {
		SyncPacket::PlayerPacket::deleteConnectable(chan, id);
	}
}

auto SyncHandler::handleCharacterCreated(PacketReader &packet) -> void {
	int32_t id = packet.get<int32_t>();
	PlayerDataProvider::getInstance().loadPlayer(id);
}

auto SyncHandler::playerChangeChannel(AbstractConnection *connection, PacketReader &packet) -> void {
	int32_t playerId = packet.get<int32_t>();
	Channel *chan = Channels::getInstance().getChannel(packet.get<uint16_t>());
	Ip ip(0);
	port_t port = -1;
	if (chan) {
		PlayerDataProvider::getInstance().addPendingPlayer(playerId, chan->getId());
		ip = packet.getClass<Ip>();
		SyncPacket::PlayerPacket::newConnectable(chan->getId(), playerId, ip, packet);
	}
	else {
		SyncPacket::PlayerPacket::playerChangeChannel(connection, playerId, ip, port);
	}
}

auto SyncHandler::handleChangeChannel(AbstractConnection *connection, PacketReader &packet) -> void {
	// TODO FIXME
	// This request comes from the destination channel so I can't remove the ->getConnection() calls
	int32_t playerId = packet.get<int32_t>();
	Player *gamePlayer = PlayerDataProvider::getInstance().getPlayer(playerId);
	if (gamePlayer) {
		uint16_t chanId = PlayerDataProvider::getInstance().getPendingPlayerChannel(playerId);
		Channel *destinationChannel = Channels::getInstance().getChannel(chanId);
		Channel *currentChannel = Channels::getInstance().getChannel(gamePlayer->getChannel());
		Ip ip(0);
		port_t port = -1;
		if (destinationChannel) {
			ip = destinationChannel->matchIpToSubnet(gamePlayer->getIp());
			port = destinationChannel->getPort();
		}

		SyncPacket::PlayerPacket::playerChangeChannel(currentChannel->getConnection(), playerId, ip, port);
		PlayerDataProvider::getInstance().removePendingPlayer(playerId);
	}
}

auto SyncHandler::handlePartySync(PacketReader &packet) -> void {
	int8_t type = packet.get<int8_t>();
	int32_t playerId = packet.get<int32_t>();
	switch (type) {
		case PartyActions::Create: PlayerDataProvider::getInstance().createParty(playerId); break;
		case PartyActions::Leave: PlayerDataProvider::getInstance().removePartyMember(playerId); break;
		case PartyActions::Expel: PlayerDataProvider::getInstance().removePartyMember(playerId, packet.get<int32_t>()); break;
		case PartyActions::Join: PlayerDataProvider::getInstance().addPartyMember(playerId); break;
		case PartyActions::SetLeader: PlayerDataProvider::getInstance().setPartyLeader(playerId, packet.get<int32_t>()); break;
	}
}

auto SyncHandler::handleBuddySync(PacketReader &packet) -> void {
	switch (packet.get<int8_t>()) {
		case Sync::Buddy::Invite: buddyInvite(packet); break;
		case Sync::Buddy::OnlineOffline: buddyOnline(packet); break;
	}
}

auto SyncHandler::buddyInvite(PacketReader &packet) -> void {
	int32_t inviterId = packet.get<int32_t>();
	Player *inviter = PlayerDataProvider::getInstance().getPlayer(inviterId);
	if (inviter == nullptr) {
		// No idea how this would happen... take no risk and just return
		return;
	}

	int32_t inviteeId = packet.get<int32_t>();
	if (Player *invitee = PlayerDataProvider::getInstance().getPlayer(inviteeId)) {
		SyncPacket::BuddyPacket::sendBuddyInvite(Channels::getInstance().getChannel(invitee->getChannel()), inviteeId, inviterId, inviter->getName());
	}
	else {
		// Make new pending buddy in the database
		Database::getCharDb().once
			<< "INSERT INTO buddylist_pending "
			<< "VALUES (:invitee, :name, :inviter)",
			soci::use(inviteeId, "invitee"),
			soci::use(inviter->getName(), "name"),
			soci::use(inviterId, "inviter");
	}
}

auto SyncHandler::buddyOnline(PacketReader &packet) -> void {
	int32_t playerId = packet.get<int32_t>();
	Player *player = PlayerDataProvider::getInstance().getPlayer(playerId);
	if (player == nullptr) {
		// No idea how this would happen... take no risk and just return
		return;
	}

	bool online = packet.get<bool>();

	vector_t<int32_t> tempIds = packet.getVector<int32_t>();
	hash_map_t<int16_t, vector_t<int32_t>> ids; // <channel, <ids>>, for sending less packets for a buddylist of 100 people

	int32_t id = 0;
	for (size_t i = 0; i < tempIds.size(); i++) {
		id = tempIds[i];
		if (Player *player = PlayerDataProvider::getInstance().getPlayer(id)) {
			if (player->isOnline()) {
				ids[player->getChannel()].push_back(id);
			}
		}
	}

	for (const auto &kvp : ids) {
		if (Channel *channel = Channels::getInstance().getChannel(kvp.first)) {
			SyncPacket::BuddyPacket::sendBuddyOnlineOffline(channel, kvp.second, playerId, (online ? player->getChannel() : -1));
		}
	}
}