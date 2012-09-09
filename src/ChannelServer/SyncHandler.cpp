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
#include "BuddyListPacket.h"
#include "ChannelServer.h"
#include "Configuration.h"
#include "Connectable.h"
#include "GameObjects.h"
#include "InterHeader.h"
#include "InterHelper.h"
#include "PacketCreator.h"
#include "PacketReader.h"
#include "Party.h"
#include "Player.h"
#include "PlayerBuddyList.h"
#include "PlayerDataProvider.h"
#include "PlayerPacket.h"
#include "SmsgHeader.h"
#include "SyncPacket.h"

void SyncHandler::handle(PacketReader &packet) {
	switch (packet.get<int8_t>()) {
		case Sync::SyncTypes::Config: handleConfigSync(packet); break;
		case Sync::SyncTypes::ChannelStart: handleChannelSync(packet); break;
		case Sync::SyncTypes::Player: handlePlayerSync(packet); break;
		case Sync::SyncTypes::Party: handlePartySync(packet); break;
		case Sync::SyncTypes::Buddy: handleBuddySync(packet); break;
	}
}

void SyncHandler::handleConfigSync(PacketReader &packet) {
	switch (packet.get<int8_t>()) {
		case Sync::Config::RateSet: setRates(packet); break;
		case Sync::Config::ScrollingHeader: ChannelServer::Instance()->setScrollingHeader(packet.getString()); break;
	}
}

void SyncHandler::handleChannelSync(PacketReader &packet) {
	PlayerDataProvider::Instance()->parseChannelConnectPacket(packet);
}

void SyncHandler::handlePlayerSync(PacketReader &packet) {
	switch (packet.get<int8_t>()) {
		case Sync::Player::NewConnectable: PlayerDataProvider::Instance()->newConnectable(packet); break;
		case Sync::Player::DeleteConnectable: PlayerDataProvider::Instance()->deleteConnectable(packet.get<int32_t>()); break;
		case Sync::Player::ChangeChannelGo: PlayerDataProvider::Instance()->changeChannel(packet); break;
		case Sync::Player::UpdatePlayer: PlayerDataProvider::Instance()->updatePlayer(packet); break;
		case Sync::Player::CharacterCreated: PlayerDataProvider::Instance()->newPlayer(packet); break;
	}
}

void SyncHandler::handlePartySync(PacketReader &packet) {
	int8_t type = packet.get<int8_t>();
	int32_t partyId = packet.get<int32_t>();
	switch (type) {
		case Sync::Party::Create: PlayerDataProvider::Instance()->newParty(partyId, packet.get<int32_t>()); break;
		case Sync::Party::Disband: PlayerDataProvider::Instance()->disbandParty(partyId); break;
		case Sync::Party::SwitchLeader: PlayerDataProvider::Instance()->switchPartyLeader(partyId, packet.get<int32_t>()); break;
		case Sync::Party::AddMember: PlayerDataProvider::Instance()->addPartyMember(partyId, packet.get<int32_t>()); break;
		case Sync::Party::RemoveMember: {
			int32_t playerId = packet.get<int32_t>();
			PlayerDataProvider::Instance()->removePartyMember(partyId, playerId, packet.getBool());
			break;
		}
	}
}

void SyncHandler::handleBuddySync(PacketReader &packet) {
	switch (packet.get<int8_t>()) {
		case Sync::Buddy::Invite: SyncHandler::buddyInvite(packet); break;
		case Sync::Buddy::OnlineOffline: SyncHandler::buddyOnlineOffline(packet); break;
	}
}

void SyncHandler::buddyInvite(PacketReader &packet) {
	int32_t inviterId = packet.get<int32_t>();
	int32_t inviteeId = packet.get<int32_t>();
	if (Player *invitee = PlayerDataProvider::Instance()->getPlayer(inviteeId)) {
		BuddyInvite invite;
		invite.id = inviterId;
		invite.name = packet.getString();
		invitee->getBuddyList()->addBuddyInvite(invite);
		invitee->getBuddyList()->checkForPendingBuddy();
	}
}

void SyncHandler::buddyOnlineOffline(PacketReader &packet) {
	int32_t playerId = packet.get<int32_t>(); // The id of the player coming online
	int32_t channel = packet.get<int32_t>();
	const vector<int32_t> &players = packet.getVector<int32_t>(); // Holds the buddy IDs

	for (size_t i = 0; i < players.size(); i++) {
		if (Player *player = PlayerDataProvider::Instance()->getPlayer(players[i])) {
			if (BuddyPtr ptr = player->getBuddyList()->getBuddy(playerId)) {
				ptr->channel = channel;
				BuddyListPacket::online(player, playerId, channel);
			}
		}
	}
}

void SyncHandler::setRates(PacketReader &packet) {
	Rates rates = packet.getClass<Rates>();
	ChannelServer::Instance()->setRates(rates);
}