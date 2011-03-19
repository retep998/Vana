/*
Copyright (C) 2008-2011 Vana Development Team

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
#include "Connectable.h"
#include "GameObjects.h"
#include "InterHeader.h"
#include "InterHelper.h"
#include "PacketCreator.h"
#include "PacketReader.h"
#include "Party.h"
#include "Player.h"
#include "PlayerDataProvider.h"
#include "PlayerPacket.h"
#include "SendHeader.h"
#include "SyncPacket.h"

void SyncHandler::handle(PacketReader &packet) {
	switch (packet.get<int8_t>()) {
		case Sync::SyncTypes::ChannelStart: handleChannelStart(packet); break;
		case Sync::SyncTypes::Party: handlePartyResponse(packet); break;
		case Sync::SyncTypes::Player: handlePlayerSync(packet); break;
		case Sync::SyncTypes::Data: handleDataSync(packet); break;
		case Sync::SyncTypes::Buddy: handleBuddy(packet); break;
	}
}

void SyncHandler::handlePlayerSync(PacketReader &packet) {
	switch (packet.get<int8_t>()) {
		case Sync::Player::NewConnectable: newConnectable(packet); break;
		case Sync::Player::ChangeChannelGo: playerChangeChannel(packet); break;
		case Sync::Player::PacketTransfer: PlayerDataProvider::Instance()->parseIncomingPacket(packet); break;
		case Sync::Player::RemovePacketTransfer: PlayerDataProvider::Instance()->removePacket(packet.get<int32_t>()); break;
	}
}

void SyncHandler::playerChangeChannel(PacketReader &packet) {
	int32_t playerid = packet.get<int32_t>();
	uint32_t ip = packet.get<uint32_t>();
	uint16_t port = packet.get<uint16_t>();

	Player *ccPlayer = PlayerDataProvider::Instance()->getPlayer(playerid);
	if (!ccPlayer) {
		return;
	}
	if (ip == 0) {
		PlayerPacket::sendBlockedMessage(ccPlayer, PlayerPacket::BlockMessages::CannotGo);
	}
	else {
		ccPlayer->setOnline(false); // Set online to 0 BEFORE CC packet is sent to player
		ccPlayer->setChangingChannel(true);
		PlayerPacket::changeChannel(ccPlayer, ip, port);
		ccPlayer->saveAll(true);
		ccPlayer->setSaveOnDc(false);
	}
}

void SyncHandler::newConnectable(PacketReader &packet) {
	int32_t playerId = packet.get<int32_t>();
	uint32_t playerIp = packet.get<uint32_t>();
	Connectable::Instance()->newPlayer(playerId, playerIp);
}

void SyncHandler::handleDataSync(PacketReader &packet) {
	switch (packet.get<int8_t>()) {
		case Sync::SyncTypes::Party: {
			int8_t type = packet.get<int8_t>();
			switch (type) {
				case Sync::Party::Disband:
					disbandParty(packet);
					break;
				case Sync::Party::Create: {
					int32_t partyid = packet.get<int32_t>();
					int32_t leaderid = packet.get<int32_t>();
					Party *party = new Party(partyid);
					Player *leader = PlayerDataProvider::Instance()->getPlayer(leaderid);
					if (leader == nullptr) {
						party->addMember(leaderid);
					}
					else {
						party->addMember(leader);
					}
					party->setLeader(leaderid, true);
					PlayerDataProvider::Instance()->addParty(party);
					break;
				}
				case Sync::Party::SwitchLeader: {
					int32_t partyid = packet.get<int32_t>();
					if (Party *party = PlayerDataProvider::Instance()->getParty(partyid)) {
						int32_t newleader = packet.get<int32_t>();
						party->setLeader(newleader);
					}
					break;
				}
				case Sync::Party::RemoveMember: {
					int32_t partyid = packet.get<int32_t>();
					if (Party *party = PlayerDataProvider::Instance()->getParty(partyid)) {
						int32_t playerid = packet.get<int32_t>();
						Player *member = PlayerDataProvider::Instance()->getPlayer(playerid);
						if (member == nullptr) {
							party->deleteMember(playerid);
						}
						else {
							party->deleteMember(member);
						}
					}
					break;
				}
				case Sync::Party::AddMember: {
					int32_t partyid = packet.get<int32_t>();
					if (Party *party = PlayerDataProvider::Instance()->getParty(partyid)) {
						int32_t playerid = packet.get<int32_t>();
						Player *member = PlayerDataProvider::Instance()->getPlayer(playerid);
						if (member == nullptr) {
							party->addMember(playerid);
						}
						else {
							party->addMember(member);
						}
					}
					break;
				}
			}
			break;
		}
	}
}

void SyncHandler::handleChannelStart(PacketReader &packet) {
	int32_t numberparties = packet.get<int32_t>();
	for (int32_t i = 0; i < numberparties; i++) {
		int32_t partyid = packet.get<int32_t>();
		int8_t membersnum = packet.get<int8_t>();
		Party *party = new Party(partyid);
		for (int8_t j = 0; j < membersnum; j++) {
			int32_t memberid = packet.get<int32_t>();
			party->addMember(memberid);
		}
		int32_t leaderid = packet.get<int32_t>();
		party->setLeader(leaderid, true);
		PlayerDataProvider::Instance()->addParty(party);
	}
}

void SyncHandler::disbandParty(PacketReader &packet) {
	int32_t partyid = packet.get<int32_t>();
	if (Party *party = PlayerDataProvider::Instance()->getParty(partyid)) {
		party->disband();
		PlayerDataProvider::Instance()->removeParty(party->getId());
	}
}

void SyncHandler::handlePartyResponse(PacketReader &packet) {
	int8_t type = packet.get<int8_t>();
	int32_t playerid = packet.get<int32_t>();
	int32_t partyid = packet.get<int32_t>();
	Player *player = PlayerDataProvider::Instance()->getPlayer(playerid);
	Party *party = PlayerDataProvider::Instance()->getParty(partyid);
	if (player == nullptr || party == nullptr)
		return;
	switch (type) {
		case PartyActions::Leave:
		case PartyActions::Expel:
			party->deleteMember(player);
			break;
		case PartyActions::Join:
			player->setParty(party);
			party->addMember(player);
			party->showHpBar(player);
			party->receiveHpBar(player);
			break;
		case PartyActions::LogInOrOut:
			player->setParty(party);
			party->setMember(player->getId(), player);
			party->showHpBar(player);
			party->receiveHpBar(player);
			break;
	}
}

void SyncHandler::handleBuddy(PacketReader &packet) {
	switch (packet.get<int8_t>()) {
		case Sync::Buddy::Invite: SyncHandler::buddyInvite(packet); break;
		case Sync::Buddy::OnlineOffline: SyncHandler::buddyOnlineOffline(packet); break;
	}
}

void SyncHandler::buddyInvite(PacketReader &packet) {
	int32_t playerid = packet.get<int32_t>();
	if (Player *player = PlayerDataProvider::Instance()->getPlayer(playerid)) {
		BuddyInvite invite;
		invite.id = packet.get<int32_t>();
		invite.name = packet.getString();
		player->getBuddyList()->addBuddyInvite(invite);
		player->getBuddyList()->checkForPendingBuddy();
	}
}

void SyncHandler::buddyOnlineOffline(PacketReader &packet) {
	int32_t playerid = packet.get<int32_t>(); // The id of the player coming online
	int32_t channel = packet.get<int32_t>();
	vector<int32_t> players = packet.getVector<int32_t>(); // Holds the buddyids

	for (size_t i = 0; i < players.size(); i++) {
		if (Player *player = PlayerDataProvider::Instance()->getPlayer(players[i])) {
			if (BuddyPtr ptr = player->getBuddyList()->getBuddy(playerid)) {
				ptr->channel = channel;
				BuddyListPacket::online(player, playerid, channel);
			}
		}
	}
}