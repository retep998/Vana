/*
Copyright (C) 2008-2012 Vana Development Team

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
#include "Channel.h"
#include "Channels.h"
#include "Database.h"
#include "GameObjects.h"
#include "InterHeader.h"
#include "InterHelper.h"
#include "IpUtilities.h"
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

void SyncHandler::handle(WorldServerAcceptConnection *connection, PacketReader &packet) {
	switch (packet.get<int8_t>()) {
		case Sync::SyncTypes::Player: handlePlayerPacket(connection, packet); break;
		case Sync::SyncTypes::Party: handlePartySync(packet); break;
		case Sync::SyncTypes::Buddy: handleBuddyPacket(packet); break;
	}
}

void SyncHandler::handlePlayerPacket(WorldServerAcceptConnection *connection, PacketReader &packet) {
	switch (packet.get<int8_t>()) {
		case Sync::Player::ChangeChannelRequest: playerChangeChannel(connection, packet); break;
		case Sync::Player::ChangeChannelGo: handleChangeChannel(connection, packet); break;
		case Sync::Player::Connect: playerConnect(connection->getChannel(), packet); break;
		case Sync::Player::Disconnect: playerDisconnect(connection->getChannel(), packet); break;
		case Sync::Player::UpdatePlayer: handlePlayerUpdate(packet); break;
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

void SyncHandler::playerChangeChannel(WorldServerAcceptConnection *connection, PacketReader &packet) {
	int32_t playerId = packet.get<int32_t>();
	Channel *chan = Channels::Instance()->getChannel(packet.get<uint16_t>());
	if (chan) {
		PlayerDataProvider::Instance()->addPendingPlayer(playerId, chan->getId());
		SyncPacket::PlayerPacket::newConnectable(chan->getId(), playerId, packet.get<ip_t>(), packet);
	}
	else {
		// Channel doesn't exist (offline)
		SyncPacket::PlayerPacket::playerChangeChannel(connection, playerId, 0, -1);
	}
}

void SyncHandler::handleChangeChannel(WorldServerAcceptConnection *connection, PacketReader &packet) {
	int32_t playerId = packet.get<int32_t>();
	Player *gamePlayer = PlayerDataProvider::Instance()->getPlayer(playerId);
	if (gamePlayer) {
		uint16_t chanId = PlayerDataProvider::Instance()->getPendingPlayerChannel(playerId);
		Channel *chan = Channels::Instance()->getChannel(chanId);
		Channel *curchan = Channels::Instance()->getChannel(gamePlayer->getChannel());
		if (chan) {
			ip_t chanIp = IpUtilities::matchIpSubnet(gamePlayer->getIp(), chan->getExternalIps(), chan->getIp());
			SyncPacket::PlayerPacket::playerChangeChannel(curchan->getConnection(), playerId, chanIp, chan->getPort());
		}
		else {
			SyncPacket::PlayerPacket::playerChangeChannel(curchan->getConnection(), playerId, 0, -1);
		}
		PlayerDataProvider::Instance()->removePendingPlayer(playerId);
	}
}

void SyncHandler::handleBuddyPacket(PacketReader &packet) {
	switch (packet.get<int8_t>()) {
		case Sync::Buddy::Invite: buddyInvite(packet); break;
		case Sync::Buddy::OnlineOffline: buddyOnline(packet); break;
	}
}

void SyncHandler::buddyInvite(PacketReader &packet) {
	int32_t playerId = packet.get<int32_t>();
	Player *inviter = PlayerDataProvider::Instance()->getPlayer(playerId);
	if (inviter == nullptr) {
		// No idea how this would happen... take no risk and just return
		return;
	}

	int32_t inviteeId = packet.get<int32_t>();
	if (Player *invitee = PlayerDataProvider::Instance()->getPlayer(inviteeId)) {
		SyncPacket::BuddyPacket::sendBuddyInvite(Channels::Instance()->getChannel(invitee->getChannel())->getConnection(), inviteeId, playerId, inviter->getName());
	}
	else {
		// Make new pending buddy in the database
		Database::getCharDb().once << "INSERT INTO buddylist_pending " <<
										"VALUES (:invitee, :name, :player)",
										soci::use(inviteeId, "invitee"),
										soci::use(inviter->getName(), "inviter"),
										soci::use(playerId, "player");
	}
}

void SyncHandler::buddyOnline(PacketReader &packet) {
	int32_t playerId = packet.get<int32_t>();
	Player *player = PlayerDataProvider::Instance()->getPlayer(playerId);
	if (player == nullptr) {
		// No idea how this would happen... take no risk and just return
		return;
	}

	bool online = packet.getBool();

	vector<int32_t> &tempIds = packet.getVector<int32_t>();
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

	for (unordered_map<int16_t, vector<int32_t>>::iterator iter = ids.begin(); iter != ids.end(); ++iter) {
		if (Channel *channel = Channels::Instance()->getChannel(iter->first)) {
			SyncPacket::BuddyPacket::sendBuddyOnlineOffline(channel->getConnection(), iter->second, playerId, (online ? player->getChannel() : -1));
		}
	}
}