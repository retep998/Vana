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
#include "PlayersPacket.h"
#include "MapleSession.h"
#include "Maps.h"
#include "PacketCreator.h"
#include "Pets.h"
#include "Player.h"
#include "Players.h"
#include "InterHeader.h"
#include "SendHeader.h"

void PlayersPacket::showMoving(Player *player, unsigned char *buf, size_t size) {
	if (player->getActiveBuffs()->getActiveSkillLevel(9101004) > 0)
		return;
	PacketCreator packet;
	packet.addShort(SEND_MOVE_PLAYER);
	packet.addInt(player->getId());
	packet.addInt(0);
	packet.addBuffer(buf, size);
	Maps::maps[player->getMap()]->sendPacket(packet, player);
}

void PlayersPacket::faceExpression(Player *player, int32_t face) {
	if (player->getActiveBuffs()->getActiveSkillLevel(9101004) > 0)
		return;
	PacketCreator packet;
	packet.addShort(SEND_FACE_EXPRESSION);
	packet.addInt(player->getId());
	packet.addInt(face);
	Maps::maps[player->getMap()]->sendPacket(packet, player);
}

void PlayersPacket::showChat(Player *player, const string &msg, int8_t bubbleOnly) {
	PacketCreator packet;
	packet.addShort(SEND_CHAT);
	packet.addInt(player->getId());
	packet.addByte(player->isGM());
	packet.addString(msg);
	packet.addByte(bubbleOnly);
	Maps::maps[player->getMap()]->sendPacket(packet);
}

void PlayersPacket::damagePlayer(Player *player, int32_t dmg, int32_t mob, uint8_t hit, uint8_t type, uint8_t stance, int32_t nodamageskill, PGMRInfo pgmr) {
	if (player->getActiveBuffs()->getActiveSkillLevel(9101004) > 0)
		return;
	PacketCreator packet;
	packet.addShort(SEND_DAMAGE_PLAYER);
	packet.addInt(player->getId());
	packet.addByte(type);
	switch (type) {
		case 0xFE:
			packet.addInt(dmg);
			packet.addInt(dmg);
			break;
		default:
			packet.addInt((pgmr.reduction > 0 ? pgmr.damage : dmg));
			packet.addInt(mob);
			packet.addByte(hit);
			if (pgmr.reduction > 0) {
				packet.addByte(pgmr.reduction);
				packet.addByte(pgmr.isphysical); // Maybe? No Mana Reflection on global to test with
				packet.addInt(pgmr.mapmobid);
				packet.addByte(6);
				packet.addPos(pgmr.pos);
			}
			else
				packet.addByte(0);
			packet.addByte(stance);
			packet.addInt(dmg);
			if (nodamageskill > 0)
				packet.addInt(nodamageskill);
			break;
	}
	Maps::maps[player->getMap()]->sendPacket(packet);
}

void PlayersPacket::showMessage(const string &msg, int8_t type) {
	PacketCreator packet;
	packet.addShort(SEND_NOTICE); 
	packet.addByte(type);
	packet.addString(msg);
	Players::Instance()->sendPacket(packet);
}

void PlayersPacket::showMessageWorld(const string &msg, int8_t type) {
	PacketCreator packet;
	packet.addShort(INTER_TO_PLAYERS);
	packet.addShort(SEND_NOTICE);
	packet.addByte(type);
	packet.addString(msg);
	ChannelServer::Instance()->sendToWorld(packet);
}

void PlayersPacket::showInfo(Player *player, Player *getinfo, uint8_t isself) {
	PacketCreator packet;
	packet.addShort(SEND_PLAYER_INFO);
	packet.addInt(getinfo->getId());
	packet.addByte(getinfo->getLevel());
	packet.addShort(getinfo->getJob());
	packet.addShort(getinfo->getFame());
	packet.addByte(0); // Married
	packet.addString("-"); // Guild
	packet.addString(""); // Guild Alliance
	packet.addByte(isself); // Is 1 when the character is clicking themselves
	for (int8_t i = 0; i < 3; i++) {
		if (Pet *pet = getinfo->getPets()->getSummoned(i)) {
			packet.addByte(1);
			packet.addInt(pet->getType());
			packet.addString(pet->getName());
			packet.addByte(pet->getLevel());
			packet.addShort(pet->getCloseness());
			packet.addByte(pet->getFullness());
			packet.addShort(0);
			packet.addInt(getinfo->getInventory()->getItem(1,  -114 - (i == 1 ? 16 : (i == 2 ? 24 : 0))) != 0 ? getinfo->getInventory()->getItem(1, -114 - (i == 1 ? 16 : (i == 2 ? 24 : 0)))->id : 0);
		}
	}
	packet.addByte(0); // End of pets / start of taming mob
	packet.addByte(0); // End of taming mob / start of wish list
	packet.addByte(0); // Wish list count
	packet.addInt(1);
	packet.addInt(0);
	packet.addInt(0);
	packet.addInt(0);
	packet.addInt(0);
	player->getSession()->send(packet);
}

void PlayersPacket::whisperPlayer(Player *target, const string &whisperer_name, uint16_t channel, const string &message) {
	PacketCreator packet;
	packet.addShort(SEND_COMMAND_RESPOND);
	packet.addByte(0x12);
	packet.addString(whisperer_name);
	packet.addShort(channel);
	packet.addString(message);
	target->getSession()->send(packet);
}

void PlayersPacket::findPlayer(Player *player, const string &name, int32_t map, uint8_t is, bool is_channel) {
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

	player->getSession()->send(packet);
}

void PlayersPacket::sendToPlayers(unsigned char *data, int32_t len) {
	PacketCreator packet;
	packet.addBuffer(data, len);
	Players::Instance()->sendPacket(packet);
}
