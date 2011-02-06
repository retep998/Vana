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
#include "GuildPacket.h"
#include "InterHeader.h"
#include "InterHelper.h"
#include "PacketCreator.h"
#include "PacketReader.h"
#include "Party.h"
#include "Player.h"
#include "PlayerDataProvider.h"
#include "PlayerPacket.h"
#include "PlayerBuddyList.h"
#include "SendHeader.h"
#include "SyncPacket.h"

void SyncHandler::handle(PacketReader &packet) {
	switch (packet.get<int8_t>()) {
		case Sync::SyncTypes::ChannelStart: handleChannelStart(packet); break;
		case Sync::SyncTypes::Party: handlePartyResponse(packet); break;
		case Sync::SyncTypes::Guild: guildPacketHandlerWorld(packet); break;
		case Sync::SyncTypes::Alliance: alliancePacketHandlerWorld(packet); break;
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
		case Sync::Player::CannotChangeServer: cannotGo(packet); break;
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
		PlayerPacket::sendBlockedMessage(ccPlayer, Sync::Player::BlockMessages::CannotGo);
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
	int32_t playerid = packet.get<int32_t>();
	uint32_t playerip = packet.get<uint32_t>();
	Connectable::Instance()->newPlayer(playerid, playerip);
}

void SyncHandler::cannotGo(PacketReader &packet) {
	if (Player *ccPlayer = PlayerDataProvider::Instance()->getPlayer(packet.get<int32_t>())) {	
		PlayerPacket::sendBlockedMessage(ccPlayer, packet.get<int8_t>());
	}
}

void SyncHandler::guildPacketHandlerWorld(PacketReader &packet) {
	switch (packet.get<int8_t>()) {
		case Sync::Guild::Unload: PlayerDataProvider::Instance()->unloadGuild(packet.get<int32_t>()); break;
		case Sync::Guild::Load: PlayerDataProvider::Instance()->loadGuild(packet.get<int32_t>()); break;
		case Sync::Guild::ChangeEmblem: GuildPacket::handleEmblemChange(packet); break;
		case Sync::Guild::RemovePlayer: {
			Player *player = PlayerDataProvider::Instance()->getPlayer(packet.get<int32_t>());
			if (player == nullptr)
				return;

			player->setGuildId(0);
			player->setGuildRank(5);
			break;
		}
		case Sync::Guild::AddPlayer: {
			Player *player = PlayerDataProvider::Instance()->getPlayer(packet.get<int32_t>());
			if (player == nullptr)
				return;

			player->setGuildId(packet.get<int32_t>());
			player->setGuildRank(packet.get<uint8_t>());
			player->setAllianceId(packet.get<int32_t>());
			player->setAllianceRank(packet.get<uint8_t>());
			break;
		}
		case Sync::Guild::ChangePlayer: {
			Player *player = PlayerDataProvider::Instance()->getPlayer(packet.get<int32_t>());
			if (player == nullptr)
				return;

			player->setGuildRank(packet.get<uint8_t>());
			player->setAllianceRank(packet.get<uint8_t>());
			break;
		}
		case Sync::Guild::ChangePlayerMoney: {
			Player *player = PlayerDataProvider::Instance()->getPlayer(packet.get<int32_t>());
			if (player == nullptr)
				return;

			Quests::giveMesos(player, packet.get<int32_t>());
			break;
		}
		case Sync::Guild::ChangeCapacity: {
			Guild *gi = PlayerDataProvider::Instance()->getGuild(packet.get<int32_t>());
			if (gi == nullptr)
				return;
			gi->capacity = packet.get<int32_t>();
			break;
		}
		case Sync::Guild::ChangePlayerGuildName: GuildPacket::handleNameChange(packet); break;
		case Sync::Guild::ChannelConnect: {
			int32_t guilds = packet.get<int32_t>();
			int32_t id, capacity, alliance;
			string name;
			GuildLogo logo;
			for (int32_t i = 0; i < guilds; i++) {
				id = packet.get<int32_t>();
				name = packet.getString();
				logo.logo = packet.get<int16_t>();
				logo.color = packet.get<uint8_t>();
				logo.background = packet.get<int16_t>();
				logo.backgroundColor = packet.get<uint8_t>();
				capacity = packet.get<int32_t>();
				alliance = packet.get<int32_t>();

				PlayerDataProvider::Instance()->addGuild(id, name, logo, capacity, alliance);
			}
			break;
		}
	}
}

void SyncHandler::alliancePacketHandlerWorld(PacketReader &packet) {
	switch (packet.get<int8_t>()) {
		case Sync::Alliance::ChangeAlliance: { // (un)Load Alliance and set the allianceids and ranks
			uint8_t option = packet.get<uint8_t>();
			int32_t allianceid = packet.get<int32_t>();
			if (option == 0)
				PlayerDataProvider::Instance()->unloadAlliance(allianceid);
			else
				PlayerDataProvider::Instance()->loadAlliance(allianceid);
			uint8_t guilds = packet.get<uint8_t>();
			for (uint8_t i = 0; i < guilds; i++) {
				Guild *guild = PlayerDataProvider::Instance()->getGuild(packet.get<int32_t>());
				guild->allianceid = allianceid;

				int32_t players = packet.get<int32_t>();
				for (int32_t j = 0; j < players; j++) {
					int32_t playerid = packet.get<int32_t>();
					uint8_t rank = packet.get<uint8_t>();
					if (Player *player = PlayerDataProvider::Instance()->getPlayer(playerid)) {
						player->setAllianceId(allianceid);
						player->setAllianceRank(rank);
					}
				}
			}
			break;
		}
		case Sync::Alliance::ChangeLeader: { // Changing the leader
			int32_t allianceid = packet.get<int32_t>();
			Player *to = PlayerDataProvider::Instance()->getPlayer(packet.get<int32_t>());
			Player *from = PlayerDataProvider::Instance()->getPlayer(packet.get<int32_t>());
			if ((to != nullptr && to->getAllianceId() != allianceid) || (from != nullptr && from->getAllianceId() != allianceid))
				return;
			if (to != nullptr)
				to->setAllianceRank(2);
			if (from != nullptr)
				from->setAllianceRank(1);
			break;
		}
		case Sync::Alliance::ChangeCapacity: { // Changing the Capacity
			if (Alliance *alliance = PlayerDataProvider::Instance()->getAlliance(packet.get<int32_t>()))
				alliance->capacity = packet.get<int32_t>();
			break;
		}
		case Sync::Alliance::ChangeGuild: { // Changing the id and rank of multiple players
			int32_t allianceid = packet.get<int32_t>();
			Guild *guild = PlayerDataProvider::Instance()->getGuild(packet.get<int32_t>());
			guild->allianceid = allianceid;
			int32_t players = packet.get<int32_t>();
			int32_t playerid = 0;
			uint8_t rank = 0;
			for (int32_t i = 0; i < players; i++) {
				playerid = packet.get<int32_t>();
				rank = packet.get<uint8_t>();
				if (Player *player = PlayerDataProvider::Instance()->getPlayer(playerid)) {
					player->setAllianceId(allianceid);
					player->setAllianceRank(rank);
				}
			}
			break;
		}
		case Sync::Alliance::ChangeRank: { // Changing the rank of someone
			int32_t allianceid = packet.get<int32_t>();
			Player *victim = PlayerDataProvider::Instance()->getPlayer(packet.get<int32_t>());
			if (victim != nullptr || victim->getAllianceId() != allianceid || victim->getGuildId() == 0)
				return;
			victim->setAllianceRank(packet.get<uint8_t>());
			break;
		}
		case Sync::Alliance::ChannelConnect: { // Channel Server Alliance Data Packet
			int32_t alliances = packet.get<int32_t>();
			int32_t id, capacity;
			string name;
			for (int32_t i = 0; i < alliances; i++) {
				id = packet.get<int32_t>();
				name = packet.getString();
				capacity = packet.get<int32_t>();
				PlayerDataProvider::Instance()->addAlliance(id, name, capacity);
			}
			break;
		}
	}
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
		case PartyActions::Leave: // Leave / Disband
		case PartyActions::Expel: // Expel
			party->deleteMember(player);
			break;
		case PartyActions::Join: // Create / Join
			player->setParty(party);
			party->addMember(player);
			party->showHpBar(player);
			party->receiveHpBar(player);
			break;
		case PartyActions::LogInOrOut: // LogInLogOut
			player->setParty(party);
			party->setMember(player->getId(), player);
			party->showHpBar(player);
			party->receiveHpBar(player);
			break;
	}
}

void SyncHandler::handleBuddy(PacketReader &packet) {
	switch (packet.get<int8_t>()) {
		case Sync::Buddies::Invite: SyncHandler::buddyInvite(packet); break;
		case Sync::Buddies::OnlineOffline: SyncHandler::buddyOnlineOffline(packet); break;
	}
}

void SyncHandler::buddyInvite(PacketReader &packet) {
	int32_t playerid = packet.get<int32_t>();
	if (Player *player = PlayerDataProvider::Instance()->getPlayer(playerid)) {
		PlayerBuddyList::BuddyInvite invite;
		invite.m_id = packet.get<int32_t>();
		invite.m_name = packet.getString();
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
			if (PlayerBuddyList::BuddyPtr ptr = player->getBuddyList()->getBuddy(playerid)) {
				ptr->m_channel = channel;
				BuddyListPacket::online(player, playerid, channel);
			}
		}
	}
}
