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
#include "InterHeader.h"
#include "MapleSession.h"
#include "PacketCreator.h"
#include "Parties.h"
#include "PartyHandler.h"
#include "Players.h"
#include "SendHeader.h"
#include "WorldServerAcceptConnection.h"

void PartyPacket::giveLeader(WorldServerAcceptConnection *player, int32_t playerid, int32_t target, bool is) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_FORWARD_TO);
	packet.add<int32_t>(playerid);
	packet.add<int16_t>(SMSG_PARTY);
	packet.add<int8_t>(0x1A);
	packet.add<int32_t>(target);
	packet.add<int8_t>(is);
	player->getSession()->send(packet);
}

void PartyPacket::invitePlayer(WorldServerAcceptConnection *player, int32_t playerid, const string &inviter) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_FORWARD_TO);
	packet.add<int32_t>(playerid);
	packet.add<int16_t>(SMSG_PARTY);
	packet.add<int8_t>(0x04);
	packet.add<int32_t>(Players::Instance()->getPlayerFromName(inviter)->party);
	packet.addString(inviter);
	packet.add<int8_t>(0);
	player->getSession()->send(packet);
}

void PartyPacket::createParty(WorldServerAcceptConnection *player, int32_t playerid) {
	Player *partyplayer = Players::Instance()->getPlayer(playerid);
	PacketCreator packet;
	packet.add<int16_t>(INTER_FORWARD_TO);
	packet.add<int32_t>(playerid);
	packet.add<int16_t>(SMSG_PARTY);
	packet.add<int8_t>(0x08);
	packet.add<int32_t>(partyplayer->party);
	packet.add<int32_t>(999999999);
	packet.add<int32_t>(999999999);
	packet.add<int32_t>(0);
	player->getSession()->send(packet);

	packet = PacketCreator();
	packet.add<int16_t>(INTER_PARTY_OPERATION);
	packet.add<int8_t>(PARTY_JOIN);
	packet.add<int32_t>(playerid);
	packet.add<int32_t>(partyplayer->party);
	player->getSession()->send(packet);
}

void PartyPacket::disbandParty(WorldServerAcceptConnection *player, int32_t playerid) {
	Player *partyplayer = Players::Instance()->getPlayer(playerid);
	PacketCreator packet;
	packet.add<int16_t>(INTER_FORWARD_TO);
	packet.add<int32_t>(playerid);
	packet.add<int16_t>(SMSG_PARTY);
	packet.add<int8_t>(0x0C);
	packet.add<int32_t>(partyplayer->party);
	packet.add<int32_t>(Parties::Instance()->getParty(partyplayer->party)->getLeader());
	packet.add<int8_t>(0);
	packet.add<int32_t>(partyplayer->party);
	player->getSession()->send(packet);

	packet = PacketCreator();
	packet.add<int16_t>(INTER_PARTY_OPERATION);
	packet.add<int8_t>(PARTY_LEAVE);
	packet.add<int32_t>(playerid);
	packet.add<int32_t>(partyplayer->party);
	player->getSession()->send(packet);
}

void PartyPacket::updateParty(WorldServerAcceptConnection *player, int8_t type, int32_t playerid, int32_t target) {
	Player *partyplayer = Players::Instance()->getPlayer(playerid);
	PacketCreator packet;
	packet.add<int16_t>(INTER_FORWARD_TO);
	packet.add<int32_t>(playerid);
	packet.add<int16_t>(SMSG_PARTY);
	switch (type) {
		case PARTY_JOIN: 
			packet.add<int8_t>(0x0F);
			packet.add<int32_t>(partyplayer->party);
			packet.addString(Players::Instance()->getPlayer(target)->name);
			break;
		case PARTY_LEAVE:
		case PARTY_EXPEL:
			packet.add<int8_t>(0x0C);
			packet.add<int32_t>(partyplayer->party);
			packet.add<int32_t>(target);
			packet.add<int8_t>(0x01);
			packet.add<int8_t>(type == PARTY_LEAVE ? 0x00 : 0x01);
			packet.addString(Players::Instance()->getPlayer(target, true)->name);
			break;	
		case PARTY_SILENT_UPDATE:
		case PARTY_LOG_IN_OUT:
			packet.add<int8_t>(0x07);
			packet.add<int32_t>(partyplayer->party);
	}
	addParty(packet, Parties::Instance()->getParty(partyplayer->party), Players::Instance()->getPlayer(playerid)->channel);
	player->getSession()->send(packet);

	if (type == PARTY_SILENT_UPDATE)
		return;

	target = target == 0 ? playerid : target;

	packet = PacketCreator();
	packet.add<int16_t>(INTER_PARTY_OPERATION);
	packet.add<int8_t>(type);
	packet.add<int32_t>(target);
	packet.add<int32_t>(Players::Instance()->getPlayer(target, true)->party);
	player->getSession()->send(packet);
}

void PartyPacket::partyError(WorldServerAcceptConnection *player, int32_t playerid, int8_t error) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_FORWARD_TO);
	packet.add<int32_t>(playerid);
	packet.add<int16_t>(SMSG_PARTY);
	packet.add<int8_t>(error);
	player->getSession()->send(packet);
}

void PartyPacket::addParty(PacketCreator &packet, Party *party, int32_t tochan) {
	size_t offset = 6 - party->members.size();
	size_t i = 0;
	map<int32_t, Player *>::iterator iter;

	// Add party member's ids to packet
	for (iter = party->members.begin(); iter != party->members.end(); iter++) {
		packet.add<int32_t>(iter->second->id);
	}
	for (i = 0; i < offset; i++) {
		packet.add<int32_t>(0);
	}
	
	// Add party member's names to packet
	for (iter = party->members.begin(); iter != party->members.end(); iter++) {
		packet.addString(iter->second->name, 13);
	}
	for (i = 0; i < offset; i++) {
		packet.addString("", 13);
	}

	// Add party member's jobs to packet
	for (iter = party->members.begin(); iter != party->members.end(); iter++) {
		packet.add<int32_t>(iter->second->job);
	}
	for (i = 0; i < offset; i++) {
		packet.add<int32_t>(0);
	}

	// Add party member's levels to packet
	for (iter = party->members.begin(); iter != party->members.end(); iter++) {
		packet.add<int32_t>(iter->second->level);
	}
	for (i = 0; i < offset; i++) {
		packet.add<int32_t>(0);
	}

	// Add party member's channels to packet
	for (iter = party->members.begin(); iter != party->members.end(); iter++) {
		if (iter->second->online) {
			packet.add<int32_t>(iter->second->channel); 
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
		if (iter->second->channel == tochan) {
			packet.add<int32_t>(iter->second->map); 
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
		packet.add<int32_t>(999999999);
		packet.add<int32_t>(999999999);
		packet.add<int32_t>(-1);
		packet.add<int32_t>(-1);
	}
	for (i = 0; i < offset; i++) {
		packet.add<int32_t>(999999999);
		packet.add<int32_t>(999999999);
		packet.add<int32_t>(-1);
		packet.add<int32_t>(-1);
	}
}
