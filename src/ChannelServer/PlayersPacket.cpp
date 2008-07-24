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
#include "PacketCreator.h"
#include "Player.h"
#include "Players.h"
#include "PlayersPacket.h"
#include "SendHeader.h"
#include "Map.h"
#include "Maps.h"

void PlayersPacket::showMoving(Player *player, unsigned char *buf, size_t size) {
	PacketCreator packet;
	packet.addShort(SEND_MOVE_PLAYER);
	packet.addInt(player->getPlayerid());
	packet.addInt(0);
	packet.addBuffer(buf, size);
	Maps::maps[player->getMap()]->sendPacket(packet, player);
}

void PlayersPacket::faceExperiment(Player *player, int face) {
	PacketCreator packet;
	packet.addShort(SEND_FACE_EXPERIMENT);
	packet.addInt(player->getPlayerid());
	packet.addInt(face);
	Maps::maps[player->getMap()]->sendPacket(packet, player);
}

void PlayersPacket::showChat(Player *player, const string &msg, char bubbleOnly) {
	PacketCreator packet;
	packet.addShort(SEND_CHAT);
	packet.addInt(player->getPlayerid());
	packet.addByte(player->isGM());
	packet.addString(msg);
	packet.addByte(bubbleOnly);
	Maps::maps[player->getMap()]->sendPacket(packet);
}

void PlayersPacket::damagePlayer(Player *player, int dmg, int mob, unsigned char hit, unsigned char type, int fake, PGMRInfo pgmr) {
	PacketCreator packet;
	packet.addShort(SEND_DAMAGE_PLAYER);
	packet.addInt(player->getPlayerid());
	packet.addByte(type);
	if (pgmr.reduction)
		packet.addInt(pgmr.damage);
	else
		packet.addInt(dmg);
	packet.addInt(mob);
	packet.addByte(hit);
	if (pgmr.reduction) {
		packet.addByte(pgmr.reduction);
		packet.addByte(pgmr.isphysical); // Maybe? No Mana Reflection on global to test with
		packet.addInt(pgmr.mapmobid);
		packet.addByte(6);
		packet.addShort(pgmr.pos_x);
		packet.addShort(pgmr.pos_y);
		packet.addByte(0);
	}
	else
		packet.addShort(0);
	packet.addInt(dmg);
	if (fake > 0)
		packet.addInt(fake);
	Maps::maps[player->getMap()]->sendPacket(packet);
}

void PlayersPacket::showMessage(const string &msg, char type) {
	PacketCreator packet;
	packet.addShort(SEND_NOTICE); 
	packet.addByte(type);
	packet.addString(msg);
	for (hash_map<int,Player*>::iterator iter = Players::players.begin();
		 iter != Players::players.end(); iter++) {
			 packet.send(iter->second);
	}
}

void PlayersPacket::showInfo(Player *player, Player *getinfo) {
	PacketCreator packet;
	packet.addShort(SEND_PLAYER_INFO);
	packet.addInt(getinfo->getPlayerid());
	packet.addByte(getinfo->getLevel());
	packet.addShort(getinfo->getJob());
	packet.addShort(getinfo->getFame());
	packet.addByte(0); // Married
	packet.addShort(1); // Guild Name Len
	packet.addByte(0x2D); // Guild Name
	packet.addShort(0); // Guide Alliance Name Len ?
	packet.addInt(0);
	packet.addInt(1);
	packet.addInt(0);
	packet.addInt(0);
	packet.addInt(0);
	packet.addInt(0);
	packet.send(player);
}

void PlayersPacket::whisperPlayer(Player *target, const string &whisperer_name, int channel, const string &message) {
	PacketCreator packet;
	packet.addShort(SEND_COMMAND_RESPOND);
	packet.addByte(0x12);
	packet.addString(whisperer_name);
	packet.addShort(channel);
	packet.addString(message);
	packet.send(target);
}

void PlayersPacket::findPlayer(Player *player, const string &name, int map, unsigned char is, bool is_channel) {
	PacketCreator packet;
	packet.addShort(SEND_COMMAND_RESPOND);
	if (map != -1) {
		packet.addByte(0x09);
		packet.addString(name);
		if (is_channel)
			packet.addByte(0x03);
		else
			packet.addByte(0x01);
		packet.addInt(map);
		packet.addInt(0);
		packet.addInt(0);
	}
	else {	
		packet.addByte(0x0A);
		packet.addString(name);
		packet.addByte(is);
	}

	packet.send(player);
}

void PlayersPacket::sendToPlayers(unsigned char *data, int len) {
	PacketCreator packet;
	packet.addBuffer(data, len);
	for (hash_map<int,Player*>::iterator iter = Players::players.begin(); iter != Players::players.end(); iter++) {
			packet.send(iter->second);
	}
}
