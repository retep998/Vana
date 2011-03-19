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
#include "SyncPacket.h"
#include "Channel.h"
#include "Channels.h"
#include "InterHeader.h"
#include "InterHelper.h"
#include "MapConstants.h"
#include "MapleSession.h"
#include "PacketCreator.h"
#include "Party.h"
#include "Player.h"
#include "PlayerDataProvider.h"
#include "SendHeader.h"
#include "TimeUtilities.h"
#include "WorldServerAcceptConnection.h"

void SyncPacket::PartyPacket::giveLeader(uint16_t channel, int32_t playerid, int32_t target, bool is) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_FORWARD_TO);
	packet.add<int32_t>(playerid);
	packet.addHeader(SMSG_PARTY);
	packet.add<int8_t>(0x1B);
	packet.add<int32_t>(target);
	packet.addBool(is);
	Channels::Instance()->sendToChannel(channel, packet);
}

void SyncPacket::PartyPacket::invitePlayer(uint16_t channel, int32_t playerid, const string &inviter) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_FORWARD_TO);
	packet.add<int32_t>(playerid);
	packet.addHeader(SMSG_PARTY);
	packet.add<int8_t>(0x04);
	packet.add<int32_t>(PlayerDataProvider::Instance()->getPlayer(inviter)->getParty()->getId());
	packet.addString(inviter);
	packet.add<int8_t>(0);
	Channels::Instance()->sendToChannel(channel, packet);
}

void SyncPacket::PartyPacket::createParty(uint16_t channel, int32_t playerid) {
	Player *partyplayer = PlayerDataProvider::Instance()->getPlayer(playerid);
	PacketCreator packet;
	packet.add<int16_t>(IMSG_FORWARD_TO);
	packet.add<int32_t>(playerid);
	packet.addHeader(SMSG_PARTY);
	packet.add<int8_t>(0x08);
	packet.add<int32_t>(partyplayer->getParty()->getId());
	packet.add<int32_t>(Maps::NoMap);
	packet.add<int32_t>(Maps::NoMap);
	packet.add<int32_t>(0);
	Channels::Instance()->sendToChannel(channel, packet);

	packet = PacketCreator();
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Party);
	packet.add<int8_t>(PartyActions::Join);
	packet.add<int32_t>(playerid);
	packet.add<int32_t>(partyplayer->getParty()->getId());
	Channels::Instance()->sendToChannel(channel, packet);
}

void SyncPacket::PartyPacket::disbandParty(uint16_t channel, int32_t playerid) {
	Player *partyplayer = PlayerDataProvider::Instance()->getPlayer(playerid);
	PacketCreator packet;
	packet.add<int16_t>(IMSG_FORWARD_TO);
	packet.add<int32_t>(playerid);
	packet.addHeader(SMSG_PARTY);
	packet.add<int8_t>(0x0C);
	packet.add<int32_t>(partyplayer->getParty()->getId());
	packet.add<int32_t>(partyplayer->getParty()->getLeader());
	packet.add<int8_t>(0);
	packet.add<int32_t>(partyplayer->getParty()->getId());
	Channels::Instance()->sendToChannel(channel, packet);

	packet = PacketCreator();
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Party);
	packet.add<int8_t>(PartyActions::Leave);
	packet.add<int32_t>(playerid);
	packet.add<int32_t>(partyplayer->getParty()->getId());
	Channels::Instance()->sendToChannel(channel, packet);
}

void SyncPacket::PartyPacket::updateParty(uint16_t channel, int8_t type, int32_t playerid, int32_t target) {
	Player *partyplayer = PlayerDataProvider::Instance()->getPlayer(playerid);
	PacketCreator packet;
	packet.add<int16_t>(IMSG_FORWARD_TO);
	packet.add<int32_t>(playerid);
	packet.addHeader(SMSG_PARTY);
	switch (type) {
		case PartyActions::Join: 
			packet.add<int8_t>(0x0F);
			packet.add<int32_t>(partyplayer->getParty()->getId());
			packet.addString(PlayerDataProvider::Instance()->getPlayer(target)->getName());
			break;
		case PartyActions::Leave:
		case PartyActions::Expel:
			packet.add<int8_t>(0x0C);
			packet.add<int32_t>(partyplayer->getParty()->getId());
			packet.add<int32_t>(target);
			packet.add<int8_t>(0x01);
			packet.add<int8_t>(type == PartyActions::Leave ? 0x00 : 0x01);
			packet.addString(PlayerDataProvider::Instance()->getPlayer(target, true)->getName());

		case PartyActions::SilentUpdate:
		case PartyActions::LogInOrOut:
			packet.add<int8_t>(0x07);
			packet.add<int32_t>(partyplayer->getParty()->getId());
	}
	addParty(packet, partyplayer->getParty(), PlayerDataProvider::Instance()->getPlayer(playerid)->getChannel());
	Channels::Instance()->sendToChannel(channel, packet);

	if (type == PartyActions::SilentUpdate)
		return;

	target = target == nullptr ? playerid : target;

	packet = PacketCreator();
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Party);
	packet.add<int8_t>(type);
	packet.add<int32_t>(target);
	packet.add<int32_t>(PlayerDataProvider::Instance()->getPlayer(target, true)->getParty()->getId());
	Channels::Instance()->sendToChannel(channel, packet);
}

void SyncPacket::PartyPacket::partyError(uint16_t channel, int32_t playerid, int8_t error) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_FORWARD_TO);
	packet.add<int32_t>(playerid);
	packet.addHeader(SMSG_PARTY);
	packet.add<int8_t>(error);
	Channels::Instance()->sendToChannel(channel, packet);
}

void SyncPacket::PartyPacket::addParty(PacketCreator &packet, Party *party, int32_t tochan) {
	size_t offset = 6 - party->members.size();
	size_t i = 0;
	map<int32_t, Player *>::iterator iter;

	// Add party member's ids to packet
	for (iter = party->members.begin(); iter != party->members.end(); iter++) {
		packet.add<int32_t>(iter->second->getId());
	}
	for (i = 0; i < offset; i++) {
		packet.add<int32_t>(0);
	}

	// Add party member's names to packet
	for (iter = party->members.begin(); iter != party->members.end(); iter++) {
		packet.addString(iter->second->getName(), 13);
	}
	for (i = 0; i < offset; i++) {
		packet.addString("", 13);
	}

	// Add party member's jobs to packet
	for (iter = party->members.begin(); iter != party->members.end(); iter++) {
		packet.add<int32_t>(iter->second->getJob());
	}
	for (i = 0; i < offset; i++) {
		packet.add<int32_t>(0);
	}

	// Add party member's levels to packet
	for (iter = party->members.begin(); iter != party->members.end(); iter++) {
		packet.add<int32_t>(iter->second->getLevel());
	}
	for (i = 0; i < offset; i++) {
		packet.add<int32_t>(0);
	}

	// Add party member's channels to packet
	for (iter = party->members.begin(); iter != party->members.end(); iter++) {
		if (iter->second->isOnline()) {

		}
		else {
			packet.add<int32_t>(-2);
		}
	}
	for (i = 0; i < offset; i++) {
		packet.add<int32_t>(-2);
	}

	packet.add<int32_t>(party->getLeader());

	// Add party member's maps to packet
	for (iter = party->members.begin(); iter != party->members.end(); iter++) {
		if (iter->second->getChannel() == tochan) {

		}
		else {
			packet.add<int32_t>(-2);
		}
	}
	for (i = 0; i < offset; i++) {
		packet.add<int32_t>(-2);
	}

	// Add some portal shit
	for (iter = party->members.begin(); iter != party->members.end(); iter++) {
		packet.add<int32_t>(Maps::NoMap);
		packet.add<int32_t>(Maps::NoMap);
		packet.add<int32_t>(-1);
		packet.add<int32_t>(-1);
	}
	for (i = 0; i < offset; i++) {
		packet.add<int32_t>(Maps::NoMap);
		packet.add<int32_t>(Maps::NoMap);
		packet.add<int32_t>(-1);
		packet.add<int32_t>(-1);
	}
}

void SyncPacket::PlayerPacket::newConnectable(uint16_t channel, int32_t playerid, uint32_t playerIp) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Player);
	packet.add<int8_t>(Sync::Player::NewConnectable);
	packet.add<int32_t>(playerid);
	packet.add<uint32_t>(playerIp);

	Channels::Instance()->sendToChannel(channel, packet);
}

void SyncPacket::PlayerPacket::sendPacketToChannelForHolding(uint16_t channel, int32_t playerid, PacketReader &buffer) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Player);
	packet.add<int8_t>(Sync::Player::PacketTransfer);
	packet.add<int32_t>(playerid);
	packet.addBuffer(buffer);
	Channels::Instance()->sendToChannel(channel, packet);
}

void SyncPacket::PlayerPacket::sendHeldPacketRemoval(uint16_t channel, int32_t playerid) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Player);
	packet.add<int8_t>(Sync::Player::RemovePacketTransfer);
	packet.add<int32_t>(playerid);
	Channels::Instance()->sendToChannel(channel, packet);
}

void SyncPacket::PlayerPacket::playerChangeChannel(WorldServerAcceptConnection *player, int32_t playerid, uint32_t ip, uint16_t port) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Player);
	packet.add<int8_t>(Sync::Player::ChangeChannelGo);
	packet.add<int32_t>(playerid);
	packet.add<uint32_t>(ip);
	packet.add<uint16_t>(port);
	player->getSession()->send(packet);
}

void SyncPacket::PlayerPacket::sendParties(WorldServerAcceptConnection *player) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::ChannelStart);

	unordered_map<int32_t, Party *> parties = PlayerDataProvider::Instance()->getParties();
	map<int32_t, Player *> players;

	packet.add<int32_t>((int32_t)(parties.size()));
	for (unordered_map<int32_t, Party *>::iterator iter = parties.begin(); iter != parties.end(); iter++) {
		packet.add<int32_t>(iter->first);
		players = iter->second->members;
		packet.add<int8_t>((int8_t)(players.size()));
		for (map<int32_t, Player *>::iterator playeriter = players.begin(); playeriter != players.end(); playeriter++) {
			packet.add<int32_t>(playeriter->first);
		}
		packet.add<int32_t>(iter->second->getLeader());
	}

	player->getSession()->send(packet);
}

void SyncPacket::PlayerPacket::sendRemovePartyPlayer(int32_t playerid, int32_t partyid) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Party);
	packet.add<int8_t>(Sync::Party::RemoveMember);
	packet.add<int32_t>(partyid);
	packet.add<int32_t>(playerid);
	Channels::Instance()->sendToAll(packet);
}

void SyncPacket::PlayerPacket::sendAddPartyPlayer(int32_t playerid, int32_t partyid) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Party);
	packet.add<int8_t>(Sync::Party::AddMember);
	packet.add<int32_t>(partyid);
	packet.add<int32_t>(playerid);
	Channels::Instance()->sendToAll(packet);
}

void SyncPacket::PlayerPacket::sendSwitchPartyLeader(int32_t playerid, int32_t partyid) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Party);
	packet.add<int8_t>(Sync::Party::SwitchLeader);
	packet.add<int32_t>(partyid);
	packet.add<int32_t>(playerid);
	Channels::Instance()->sendToAll(packet);
}

void SyncPacket::PlayerPacket::sendCreateParty(int32_t playerid, int32_t partyid) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Party);
	packet.add<int8_t>(Sync::Party::Create);
	packet.add<int32_t>(partyid);
	packet.add<int32_t>(playerid);
	Channels::Instance()->sendToAll(packet);
}

void SyncPacket::PlayerPacket::sendDisbandParty(int32_t partyid) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Party);
	packet.add<int8_t>(Sync::Party::Disband);
	packet.add<int32_t>(partyid);
	Channels::Instance()->sendToAll(packet);
}

void SyncPacket::BuddyPacket::sendBuddyInvite(WorldServerAcceptConnection *channel, int32_t inviteeid, int32_t inviterid, const string &name) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Buddy);
	packet.add<int8_t>(Sync::Buddy::Invite);
	packet.add<int32_t>(inviteeid);
	packet.add<int32_t>(inviterid);
	packet.addString(name);
	channel->getSession()->send(packet);
}

void SyncPacket::BuddyPacket::sendBuddyOnlineOffline(WorldServerAcceptConnection *channel, const vector<int32_t> &players, int32_t playerid, int32_t channelid) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Buddy);
	packet.add<int8_t>(Sync::Buddy::OnlineOffline);
	packet.add<int32_t>(playerid);
	packet.add<int32_t>(channelid); // I need to get FF FF FF FF, not FF FF 00 00
	packet.addVector(players);
	channel->getSession()->send(packet);
}