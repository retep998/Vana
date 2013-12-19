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
#include "SyncHandler.h"
#include "AbstractConnection.h"
#include "Channel.h"
#include "Channels.h"
#include "Database.h"
#include "GameObjects.h"
#include "InterHeader.h"
#include "InterHelper.h"
#include "PacketCreator.h"
#include "PacketReader.h"
#include "Party.h"
#include "Player.h"
#include "PlayerDataProvider.h"
#include "Session.h"
#include "SmsgHeader.h"
#include "SyncPacket.h"
#include "WorldServer.h"
#include "WorldServerAcceptConnection.h"
#include "WorldServerAcceptPacket.h"

void SyncHandler::handle(AbstractConnection *connection, PacketReader &packet) {
	switch (packet.get<int8_t>()) {
		case Sync::SyncTypes::Config: handleConfigSync(packet); break;
		case Sync::SyncTypes::Player: handlePlayerSync(connection, packet); break;
		case Sync::SyncTypes::Party: handlePartySync(packet); break;
		case Sync::SyncTypes::Buddy: handleBuddySync(packet); break;
	}
}

void SyncHandler::handleConfigSync(PacketReader &packet) {
	switch (packet.get<int8_t>()) {
		case Sync::Config::RateSet: handleSetRates(packet); break;
		case Sync::Config::RateReset: WorldServer::Instance()->resetRates(); break;
		case Sync::Config::ScrollingHeader: handleScrollingHeader(packet); break;
	}
}

void SyncHandler::handleSetRates(PacketReader &packet) {
	Rates rates = packet.getClass<Rates>();
	WorldServer::Instance()->setRates(rates);
}

void SyncHandler::handleScrollingHeader(PacketReader &packet) {
	const string &message = packet.getString();
	WorldServer::Instance()->setScrollingHeader(message);
}

void SyncHandler::handlePlayerSync(AbstractConnection *connection, PacketReader &packet) {
	switch (packet.get<int8_t>()) {
		case Sync::Player::ChangeChannelRequest: playerChangeChannel(connection, packet); break;
		case Sync::Player::ChangeChannelGo: handleChangeChannel(connection, packet); break;
		case Sync::Player::Connect: playerConnect(static_cast<WorldServerAcceptConnection *>(connection)->getChannel(), packet); break;
		case Sync::Player::Disconnect: playerDisconnect(static_cast<WorldServerAcceptConnection *>(connection)->getChannel(), packet); break;
		case Sync::Player::UpdatePlayer: handlePlayerUpdate(packet); break;
		case Sync::Player::CharacterCreated: handleCharacterCreated(packet); break;
	}
}

void SyncHandler::handlePlayerUpdate(PacketReader &packet) {
	int8_t bits = packet.get<int8_t>();
	int32_t playerId = packet.get<int32_t>();
	Player *player = PlayerDataProvider::Instance()->getPlayer(playerId);

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

void SyncHandler::playerConnect(int16_t channel, PacketReader &packet) {
	int32_t id = packet.get<int32_t>();
	int32_t map = packet.get<int32_t>();

	Player *p = PlayerDataProvider::Instance()->getPlayer(id, true);
	if (p == nullptr) {
		p = new Player(id);
	}
	p->setMap(map);
	p->setChannel(channel);
	p->setOnline(true);
	PlayerDataProvider::Instance()->playerConnect(p);
}

void SyncHandler::playerDisconnect(int16_t channel, PacketReader &packet) {
	int32_t id = packet.get<int32_t>();
	PlayerDataProvider::Instance()->playerDisconnect(id, channel);
	int16_t chan = PlayerDataProvider::Instance()->removePendingPlayerEarly(id);
	if (chan != -1) {
		SyncPacket::PlayerPacket::deleteConnectable(chan, id);
	}
}

void SyncHandler::handleCharacterCreated(PacketReader &packet) {
	int32_t id = packet.get<int32_t>();
	PlayerDataProvider::Instance()->loadPlayer(id);
}

void SyncHandler::playerChangeChannel(AbstractConnection *connection, PacketReader &packet) {
	int32_t playerId = packet.get<int32_t>();
	Channel *chan = Channels::Instance()->getChannel(packet.get<uint16_t>());
	Ip ip(0);
	port_t port = -1;
	if (chan) {
		PlayerDataProvider::Instance()->addPendingPlayer(playerId, chan->getId());
		ip = packet.getClass<Ip>();
		SyncPacket::PlayerPacket::newConnectable(chan->getId(), playerId, ip, packet);
	}
	else {
		SyncPacket::PlayerPacket::playerChangeChannel(connection, playerId, ip, port);
	}
}

void SyncHandler::handleChangeChannel(AbstractConnection *connection, PacketReader &packet) {
	// TODO FIXME
	// This request comes from the destination channel so I can't remove the ->getConnection() calls
	int32_t playerId = packet.get<int32_t>();
	Player *gamePlayer = PlayerDataProvider::Instance()->getPlayer(playerId);
	if (gamePlayer) {
		uint16_t chanId = PlayerDataProvider::Instance()->getPendingPlayerChannel(playerId);
		Channel *destinationChannel = Channels::Instance()->getChannel(chanId);
		Channel *currentChannel = Channels::Instance()->getChannel(gamePlayer->getChannel());
		Ip ip(0);
		port_t port = -1;
		if (destinationChannel) {
			ip = destinationChannel->matchIpToSubnet(gamePlayer->getIp());
			port = destinationChannel->getPort();
		}

		SyncPacket::PlayerPacket::playerChangeChannel(currentChannel->getConnection(), playerId, ip, port);
		PlayerDataProvider::Instance()->removePendingPlayer(playerId);
	}
}

void SyncHandler::handlePartySync(PacketReader &packet) {
	int8_t type = packet.get<int8_t>();
	int32_t playerId = packet.get<int32_t>();
	switch (type) {
		case PartyActions::Create: PlayerDataProvider::Instance()->createParty(playerId); break;
		case PartyActions::Leave: PlayerDataProvider::Instance()->removePartyMember(playerId); break;
		case PartyActions::Expel: PlayerDataProvider::Instance()->removePartyMember(playerId, packet.get<int32_t>()); break;
		case PartyActions::Join: PlayerDataProvider::Instance()->addPartyMember(playerId); break;
		case PartyActions::SetLeader: PlayerDataProvider::Instance()->setPartyLeader(playerId, packet.get<int32_t>()); break;
	}
}

void SyncHandler::handleBuddySync(PacketReader &packet) {
	switch (packet.get<int8_t>()) {
		case Sync::Buddy::Invite: buddyInvite(packet); break;
		case Sync::Buddy::OnlineOffline: buddyOnline(packet); break;
	}
}

void SyncHandler::buddyInvite(PacketReader &packet) {
	int32_t inviterId = packet.get<int32_t>();
	Player *inviter = PlayerDataProvider::Instance()->getPlayer(inviterId);
	if (inviter == nullptr) {
		// No idea how this would happen... take no risk and just return
		return;
	}

	int32_t inviteeId = packet.get<int32_t>();
	if (Player *invitee = PlayerDataProvider::Instance()->getPlayer(inviteeId)) {
		SyncPacket::BuddyPacket::sendBuddyInvite(Channels::Instance()->getChannel(invitee->getChannel()), inviteeId, inviterId, inviter->getName());
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

void SyncHandler::buddyOnline(PacketReader &packet) {
	int32_t playerId = packet.get<int32_t>();
	Player *player = PlayerDataProvider::Instance()->getPlayer(playerId);
	if (player == nullptr) {
		// No idea how this would happen... take no risk and just return
		return;
	}

	bool online = packet.get<bool>();

	vector<int32_t> tempIds = packet.getVector<int32_t>();
	unordered_map<int16_t, vector<int32_t>> ids; // <channel, <ids>>, for sending less packets for a buddylist of 100 people

	int32_t id = 0;
	for (size_t i = 0; i < tempIds.size(); i++) {
		id = tempIds[i];
		if (Player *player = PlayerDataProvider::Instance()->getPlayer(id)) {
			if (player->isOnline()) {
				ids[player->getChannel()].push_back(id);
			}
		}
	}

	for (const auto &kvp : ids) {
		if (Channel *channel = Channels::Instance()->getChannel(kvp.first)) {
			SyncPacket::BuddyPacket::sendBuddyOnlineOffline(channel, kvp.second, playerId, (online ? player->getChannel() : -1));
		}
	}
}