/*
Copyright (C) 2008 Vana Development Team

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
#include "WorldServerAcceptPlayer.h"
#include "PartyHandler.h"
#include "PacketCreator.h"
#include "InterHeader.h"
#include "Players.h"
#include "Channels.h"

void PartyPacket::giveLeader(WorldServerAcceptPlayer *player, int playerid, int target, bool is) {
	Packet packet;
	packet.addHeader(INTER_FORWARD_TO);
	packet.addInt(playerid);
	packet.addHeader(SEND_PARTY_ACTION);
	packet.addByte(0x1A);
	packet.addInt(target);
	packet.addByte(is);
	packet.send(player);
}

void PartyPacket::invitePlayer(WorldServerAcceptPlayer *player, int playerid, const string &inviter) {
	Packet packet;
	packet.addHeader(INTER_FORWARD_TO);
	packet.addInt(playerid);
	packet.addHeader(SEND_PARTY_ACTION);
	packet.addByte(0x04);
	packet.addInt(Players::Instance()->getPlayerFromName(inviter)->party);
	packet.addString(inviter);
	packet.addByte(0);

	packet.send(player);
}

void PartyPacket::createParty(WorldServerAcceptPlayer *player, int playerid) {
	Packet packet;
	packet.addHeader(INTER_FORWARD_TO);
	packet.addInt(playerid);
	packet.addShort(SEND_PARTY_ACTION);
	packet.addByte(0x08);
	packet.addInt(Players::Instance()->getPlayer(playerid)->party);
	packet.addBytes("FFC99A3BFFC99A3B00000000");

	packet.send(player);
}

void PartyPacket::disbandParty(WorldServerAcceptPlayer *player, int playerid) {
	Packet packet;
	packet.addHeader(INTER_FORWARD_TO);
	packet.addInt(playerid);
	packet.addShort(SEND_PARTY_ACTION);
	packet.addByte(0x0C);
	packet.addShort(0x8B);
	packet.addShort(0x2);
	packet.addInt(PartyHandler::parties[Players::Instance()->getPlayer(playerid)->party]->getLeader());
	packet.addByte(0);
	packet.addInt(Players::Instance()->getPlayer(playerid)->party);

	packet.send(player);
}

void PartyPacket::updateParty(WorldServerAcceptPlayer *player, char type, int playerid, int target) {
	Packet packet;
	packet.addHeader(INTER_FORWARD_TO);
	packet.addInt(playerid);
	packet.addShort(SEND_PARTY_ACTION);
	switch(type){
		case PARTY_JOIN: 
			packet.addByte(0x0F);
			packet.addShort(0x8B);
			packet.addShort(0x2);
			packet.addString(Players::Instance()->getPlayer(target)->name);
			break;
		case PARTY_LEAVE:
		case PARTY_EXPEL:
			packet.addByte(0x0C);
			packet.addShort(0x8B);
			packet.addShort(0x2);
			packet.addInt(target);
			packet.addByte(0x01);
			if (type == PARTY_LEAVE) {
				packet.addByte(0x00);
			}
			else {
				packet.addByte(0x01);
			}
			packet.addString(Players::Instance()->getPlayer(target)->name);
			break;	
		case PARTY_SILENT_UPDATE:
		case PARTY_LOG_IN_OUT:
			packet.addByte(0x07);
			packet.addByte(0xDD);
			packet.addByte(0x14);
			packet.addShort(0);
	}
	addParty(packet, PartyHandler::parties[Players::Instance()->getPlayer(playerid)->party], Players::Instance()->getPlayer(playerid)->channel);

	packet.send(player);
}

void PartyPacket::partyError(WorldServerAcceptPlayer *player, int playerid, char error) {
	Packet packet;
	packet.addHeader(INTER_FORWARD_TO);
	packet.addInt(playerid);
	packet.addShort(SEND_PARTY_ACTION);
	packet.addByte(error);

	packet.send(player);
}

void PartyPacket::addParty(Packet &packet, Party *party, int tochan) {
	size_t offset = 6-party->members.size();
	
	//Add party member's ids to packet
	for (hash_map<int, Player *>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
		packet.addInt(iter->second->id);
	}
	for (size_t i = 0; i < offset; i++) {
		packet.addInt(0);
	}
	
	//Add party member's names to packet
	for (hash_map<int, Player *>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
		packet.addString(iter->second->name, 13);
	}
	for (size_t i = 0; i < offset; i++) {
		packet.addString("", 13);
	}

	//Add party member's jobs to packet
	for (hash_map<int, Player *>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
		packet.addInt(iter->second->job);
	}
	for (size_t i = 0; i < offset; i++) {
		packet.addInt(0);
	}

	//Add party member's levels to packet
	for (hash_map<int, Player *>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
		packet.addInt(iter->second->level);
	}
	for (size_t i = 0; i < offset; i++) {
		packet.addInt(0);
	}

	//Add party member's channels to packet
	for (hash_map<int, Player *>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
		if (iter->second->online) {
			packet.addInt(iter->second->channel); 
		}
		else {
			packet.addInt(-2);
		}
	}
	for (size_t i = 0; i < offset; i++) {
		packet.addInt(-2);
	}
	
	packet.addInt(party->getLeader());

	//Add party member's maps to packet
	for (hash_map<int, Player *>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
		if (iter->second->channel == tochan) {
			packet.addInt(iter->second->map); 
		}
		else {
			packet.addInt(-2);
		}
	}
	for (size_t i = 0; i < offset; i++) {
		packet.addInt(-2);
	}

	//Add some portal shit
	for (hash_map<int, Player *>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
		packet.addInt(999999999);
		packet.addInt(999999999);
		packet.addInt(-1);
		packet.addInt(-1);
	}
	for (size_t i = 0; i < offset; i++) {
		packet.addInt(999999999);
		packet.addInt(999999999);
		packet.addInt(-1);
		packet.addInt(-1);
	}
}
