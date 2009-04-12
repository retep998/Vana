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
#include "WorldServerAcceptPlayer.h"

void PartyPacket::giveLeader(WorldServerAcceptPlayer *player, int32_t playerid, int32_t target, bool is) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_FORWARD_TO);
	packet.add<int32_t>(playerid);
	packet.add<int16_t>(SEND_PARTY_ACTION);
	packet.add<int8_t>(0x1A);
	packet.add<int32_t>(target);
	packet.add<int8_t>(is);
	player->getSession()->send(packet);
}

void PartyPacket::invitePlayer(WorldServerAcceptPlayer *player, int32_t playerid, const string &inviter) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_FORWARD_TO);
	packet.add<int32_t>(playerid);
	packet.add<int16_t>(SEND_PARTY_ACTION);
	packet.add<int8_t>(0x04);
	packet.add<int32_t>(Players::Instance()->getPlayerFromName(inviter)->party);
	packet.addString(inviter);
	packet.add<int8_t>(0);
	player->getSession()->send(packet);
}

void PartyPacket::createParty(WorldServerAcceptPlayer *player, int32_t playerid) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_FORWARD_TO);
	packet.add<int32_t>(playerid);
	packet.add<int16_t>(SEND_PARTY_ACTION);
	packet.add<int8_t>(0x08);
	packet.add<int32_t>(Players::Instance()->getPlayer(playerid)->party);
	packet.addBytes("FFC99A3BFFC99A3B00000000");
	player->getSession()->send(packet);

	packet = PacketCreator();
	packet.add<int16_t>(INTER_PARTY_OPERATION);
	packet.add<int8_t>(PARTY_JOIN);
	packet.add<int32_t>(playerid);
	packet.add<int32_t>(Players::Instance()->getPlayer(playerid)->party);
	player->getSession()->send(packet);
}

void PartyPacket::disbandParty(WorldServerAcceptPlayer *player, int32_t playerid) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_FORWARD_TO);
	packet.add<int32_t>(playerid);
	packet.add<int16_t>(SEND_PARTY_ACTION);
	packet.add<int8_t>(0x0C);
	packet.add<int16_t>(0x8B);
	packet.add<int16_t>(0x2);
	packet.add<int32_t>(Parties::Instance()->getParty(Players::Instance()->getPlayer(playerid)->party)->getLeader());
	packet.add<int8_t>(0);
	packet.add<int32_t>(Players::Instance()->getPlayer(playerid)->party);
	player->getSession()->send(packet);

	packet = PacketCreator();
	packet.add<int16_t>(INTER_PARTY_OPERATION);
	packet.add<int8_t>(PARTY_LEAVE);
	packet.add<int32_t>(playerid);
	packet.add<int32_t>(Players::Instance()->getPlayer(playerid)->party);
	player->getSession()->send(packet);
}

void PartyPacket::updateParty(WorldServerAcceptPlayer *player, int8_t type, int32_t playerid, int32_t target) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_FORWARD_TO);
	packet.add<int32_t>(playerid);
	packet.add<int16_t>(SEND_PARTY_ACTION);
	switch (type) {
		case PARTY_JOIN: 
			packet.add<int8_t>(0x0F);
			packet.add<int16_t>(0x8B);
			packet.add<int16_t>(0x2);
			packet.addString(Players::Instance()->getPlayer(target)->name);
			break;
		case PARTY_LEAVE:
		case PARTY_EXPEL:
			packet.add<int8_t>(0x0C);
			packet.add<int16_t>(0x8B);
			packet.add<int16_t>(0x2);
			packet.add<int32_t>(target);
			packet.add<int8_t>(0x01);
			packet.add<int8_t>(type == PARTY_LEAVE ? 0x00 : 0x01);
			packet.addString(Players::Instance()->getPlayer(target)->name);
			break;	
		case PARTY_SILENT_UPDATE:
		case PARTY_LOG_IN_OUT:
			packet.add<int8_t>(0x07);
			packet.add<uint8_t>(0xDD);
			packet.add<int8_t>(0x14);
			packet.add<int16_t>(0);
	}
	addParty(packet, Parties::Instance()->getParty(Players::Instance()->getPlayer(playerid)->party), Players::Instance()->getPlayer(playerid)->channel);
	player->getSession()->send(packet);

	if (type == PARTY_SILENT_UPDATE)
		return;

	target = target == 0 ? playerid : target;

	packet = PacketCreator();
	packet.add<int16_t>(INTER_PARTY_OPERATION);
	packet.add<int8_t>(type);
	packet.add<int32_t>(target);
	packet.add<int32_t>(Players::Instance()->getPlayer(target)->party);
	player->getSession()->send(packet);
}

void PartyPacket::partyError(WorldServerAcceptPlayer *player, int32_t playerid, int8_t error) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_FORWARD_TO);
	packet.add<int32_t>(playerid);
	packet.add<int16_t>(SEND_PARTY_ACTION);
	packet.add<int8_t>(error);
	player->getSession()->send(packet);
}

void PartyPacket::addParty(PacketCreator &packet, Party *party, int32_t tochan) {
	size_t offset = 6 - party->members.size();
	
	// Add party member's ids to packet
	for (map<int32_t, Player *>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
		packet.add<int32_t>(iter->second->id);
	}
	for (size_t i = 0; i < offset; i++) {
		packet.add<int32_t>(0);
	}
	
	// Add party member's names to packet
	for (map<int32_t, Player *>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
		packet.addString(iter->second->name, 13);
	}
	for (size_t i = 0; i < offset; i++) {
		packet.addString("", 13);
	}

	// Add party member's jobs to packet
	for (map<int32_t, Player *>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
		packet.add<int32_t>(iter->second->job);
	}
	for (size_t i = 0; i < offset; i++) {
		packet.add<int32_t>(0);
	}

	// Add party member's levels to packet
	for (map<int32_t, Player *>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
		packet.add<int32_t>(iter->second->level);
	}
	for (size_t i = 0; i < offset; i++) {
		packet.add<int32_t>(0);
	}

	// Add party member's channels to packet
	for (map<int32_t, Player *>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
		if (iter->second->online) {
			packet.add<int32_t>(iter->second->channel); 
		}
		else {
			packet.add<int32_t>(-2);
		}
	}
	for (size_t i = 0; i < offset; i++) {
		packet.add<int32_t>(-2);
	}
	
	packet.add<int32_t>(party->getLeader());

	// Add party member's maps to packet
	for (map<int32_t, Player *>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
		if (iter->second->channel == tochan) {
			packet.add<int32_t>(iter->second->map); 
		}
		else {
			packet.add<int32_t>(-2);
		}
	}
	for (size_t i = 0; i < offset; i++) {
		packet.add<int32_t>(-2);
	}

	// Add some portal shit
	for (map<int32_t, Player *>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
		packet.add<int32_t>(999999999);
		packet.add<int32_t>(999999999);
		packet.add<int32_t>(-1);
		packet.add<int32_t>(-1);
	}
	for (size_t i = 0; i < offset; i++) {
		packet.add<int32_t>(999999999);
		packet.add<int32_t>(999999999);
		packet.add<int32_t>(-1);
		packet.add<int32_t>(-1);
	}
}
