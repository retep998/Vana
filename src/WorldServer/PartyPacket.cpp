/*
Copyright (C) 2008-2009 Vana Development Team

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
#include "PartyPacket.h"
#include "Channels.h"
#include "GameConstants.h"
#include "InterHeader.h"
#include "MapleSession.h"
#include "PacketCreator.h"
#include "Party.h"
#include "PartyHelper.h"
#include "Player.h"
#include "PlayerDataProvider.h"
#include "SendHeader.h"

void PartyPacket::giveLeader(uint16_t channel, int32_t playerid, int32_t target, bool is) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_FORWARD_TO);
	packet.add<int32_t>(playerid);
	packet.add<int16_t>(SMSG_PARTY);
	packet.add<int8_t>(0x1A);
	packet.add<int32_t>(target);
	packet.add<int8_t>(is);
	Channels::Instance()->sendToChannel(channel, packet);
}

void PartyPacket::invitePlayer(uint16_t channel, int32_t playerid, const string &inviter) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_FORWARD_TO);
	packet.add<int32_t>(playerid);
	packet.add<int16_t>(SMSG_PARTY);
	packet.add<int8_t>(0x04);
	packet.add<int32_t>(PlayerDataProvider::Instance()->getPlayer(inviter)->getParty()->getId());
	packet.addString(inviter);
	packet.add<int8_t>(0);
	Channels::Instance()->sendToChannel(channel, packet);
}

void PartyPacket::createParty(uint16_t channel, int32_t playerid) {
	Player *partyplayer = PlayerDataProvider::Instance()->getPlayer(playerid);
	PacketCreator packet;
	packet.add<int16_t>(INTER_FORWARD_TO);
	packet.add<int32_t>(playerid);
	packet.add<int16_t>(SMSG_PARTY);
	packet.add<int8_t>(0x08);
	packet.add<int32_t>(partyplayer->getParty()->getId());
	packet.add<int32_t>(Maps::NoMap);
	packet.add<int32_t>(Maps::NoMap);
	packet.add<int32_t>(0);
	Channels::Instance()->sendToChannel(channel, packet);

	packet = PacketCreator();
	packet.add<int16_t>(INTER_PARTY_OPERATION);
	packet.add<int8_t>(PartyActions::Join);
	packet.add<int32_t>(playerid);
	packet.add<int32_t>(partyplayer->getParty()->getId());
	Channels::Instance()->sendToChannel(channel, packet);
}

void PartyPacket::disbandParty(uint16_t channel, int32_t playerid) {
	Player *partyplayer = PlayerDataProvider::Instance()->getPlayer(playerid);
	PacketCreator packet;
	packet.add<int16_t>(INTER_FORWARD_TO);
	packet.add<int32_t>(playerid);
	packet.add<int16_t>(SMSG_PARTY);
	packet.add<int8_t>(0x0C);
	packet.add<int32_t>(partyplayer->getParty()->getId());
	packet.add<int32_t>(partyplayer->getParty()->getLeader());
	packet.add<int8_t>(0);
	packet.add<int32_t>(partyplayer->getParty()->getId());
	Channels::Instance()->sendToChannel(channel, packet);

	packet = PacketCreator();
	packet.add<int16_t>(INTER_PARTY_OPERATION);
	packet.add<int8_t>(PartyActions::Leave);
	packet.add<int32_t>(playerid);
	packet.add<int32_t>(partyplayer->getParty()->getId());
	Channels::Instance()->sendToChannel(channel, packet);
}

void PartyPacket::updateParty(uint16_t channel, int8_t type, int32_t playerid, int32_t target) {
	Player *partyplayer = PlayerDataProvider::Instance()->getPlayer(playerid);
	PacketCreator packet;
	packet.add<int16_t>(INTER_FORWARD_TO);
	packet.add<int32_t>(playerid);
	packet.add<int16_t>(SMSG_PARTY);
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
			break;	
		case PartyActions::SilentUpdate:
		case PartyActions::LogInOrOut:
			packet.add<int8_t>(0x07);
			packet.add<int32_t>(partyplayer->getParty()->getId());
	}
	addParty(packet, partyplayer->getParty(), PlayerDataProvider::Instance()->getPlayer(playerid)->getChannel());
	Channels::Instance()->sendToChannel(channel, packet);

	if (type == PartyActions::SilentUpdate)
		return;

	target = target == 0 ? playerid : target;

	packet = PacketCreator();
	packet.add<int16_t>(INTER_PARTY_OPERATION);
	packet.add<int8_t>(type);
	packet.add<int32_t>(target);
	packet.add<int32_t>(PlayerDataProvider::Instance()->getPlayer(target, true)->getParty()->getId());
	Channels::Instance()->sendToChannel(channel, packet);
}

void PartyPacket::partyError(uint16_t channel, int32_t playerid, int8_t error) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_FORWARD_TO);
	packet.add<int32_t>(playerid);
	packet.add<int16_t>(SMSG_PARTY);
	packet.add<int8_t>(error);
	Channels::Instance()->sendToChannel(channel, packet);
}

void PartyPacket::addParty(PacketCreator &packet, Party *party, int32_t tochan) {
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
			packet.add<int32_t>(iter->second->getChannel()); 
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
			packet.add<int32_t>(iter->second->getMap()); 
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
