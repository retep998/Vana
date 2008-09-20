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
#include "MapPacket.h"
#include "Inventory.h"
#include "MapleSession.h"
#include "Maps.h"
#include "PacketCreator.h"
#include "Pets.h"
#include "Player.h"
#include "PlayerPacketHelper.h"
#include "SendHeader.h"

PacketCreator MapPacket::playerPacket(Player *player) {
	PacketCreator packet;
	packet.addShort(SEND_SHOW_PLAYER);
	packet.addInt(player->getId());
	packet.addString(player->getName());
	packet.addString(""); // Guild
	packet.addShort(0); // Guild icon garbage
	packet.addByte(0); // Guild icon garbage
	packet.addShort(0); // Guild icon garbage
	packet.addByte(0); // Guild icon garbage

	packet.addInt(0);
	packet.addInt(1);
	packet.addShort(0);
	packet.addByte(0);
	packet.addByte(0xf8);

	packet.addByte(player->getActiveBuffs()->getSkillMapEnterInfo().types[0]);
	packet.addByte(player->getActiveBuffs()->getSkillMapEnterInfo().types[1]);
	packet.addByte(player->getActiveBuffs()->getSkillMapEnterInfo().types[2]);
	packet.addByte(player->getActiveBuffs()->getSkillMapEnterInfo().types[3]);
	packet.addByte(player->getActiveBuffs()->getSkillMapEnterInfo().types[4]);
	packet.addByte(player->getActiveBuffs()->getSkillMapEnterInfo().types[5]);
	packet.addByte(player->getActiveBuffs()->getSkillMapEnterInfo().types[6]);
	packet.addByte(player->getActiveBuffs()->getSkillMapEnterInfo().types[7]);
	if (player->getActiveBuffs()->getSkillMapEnterInfo().isval) {
		packet.addByte(player->getActiveBuffs()->getSkillMapEnterInfo().val);
	}

	packet.addInt(0);
	packet.addShort(0);
	for (int8_t i = 0; i < 3; i++) {
		packet.addShort(21613); // Unknown
		packet.addInt(10028); // Unknown
		packet.addInt(0);
		packet.addInt(0);
	}
	packet.addShort(21613);
	packet.addInt(10028);
	packet.addInt(0);
	packet.addShort(0);
	packet.addShort(21613);
	packet.addInt(10028);
	packet.addInt(0);
	packet.addInt(0);
	packet.addInt(0);
	packet.addShort(21613);
	packet.addInt(10028);
	packet.addShort(0);
	packet.addShort(320);

	PlayerPacketHelper::addPlayerDisplay(packet, player);

	packet.addInt(0);
	packet.addInt(player->getItemEffect());
	packet.addInt(player->getChair());
	packet.addPos(player->getPos());
	packet.addByte(player->getStance());
	packet.addShort(player->getFH());
	packet.addByte(0);
	for (int8_t i = 0; i < 3; i++) {
		if (player->getPets()->getSummoned(i) != 0) {
			packet.addByte(1);
			Pet *pet = player->getPets()->getPet(player->getPets()->getSummoned(i));
			packet.addInt(pet->getType());
			packet.addString(pet->getName());
			packet.addInt(pet->getId());
			packet.addInt(0);
			packet.addPos(pet->getPos());
			packet.addByte(pet->getStance());
			packet.addInt(pet->getFH());
		}
		else {
			packet.addByte(0);
			break;
		}
	}
	packet.addShort(1);
	packet.addInt(0);
	packet.addInt(0);
	packet.addInt(0);
	packet.addInt(0);
	packet.addInt(0);
	return packet;
}

void MapPacket::showPlayer(Player *player) {
	PacketCreator packet = playerPacket(player);
	Maps::maps[player->getMap()]->sendPacket(packet, player);
}

void MapPacket::removePlayer(Player *player) {
	PacketCreator packet;
	packet.addShort(SEND_REMOVE_PLAYER);
	packet.addInt(player->getId());
	Maps::maps[player->getMap()]->sendPacket(packet, player);
}

void MapPacket::changeMap(Player *player) {
	PacketCreator packet;
	packet.addShort(SEND_CHANGE_MAP);
	packet.addInt(ChannelServer::Instance()->getChannel()); // Channel
	packet.addShort(0); // 2?
	packet.addShort(0);
	packet.addInt(player->getMap());
	packet.addByte(player->getMappos());
	packet.addShort(player->getHP());
	packet.addByte(0);
	packet.addInt(-1);
	packet.addShort(-1);
	packet.addByte(-1);
	packet.addByte(1);
	player->getSession()->send(packet);
}

void MapPacket::portalBlocked(Player *player) {
	PacketCreator packet;
	packet.addShort(SEND_UPDATE_STAT);
	packet.addByte(1);
	packet.addInt(0);
	player->getSession()->send(packet);
}

void MapPacket::showClock(Player *player, uint8_t hour, uint8_t min, uint8_t sec) {
	PacketCreator packet;
	packet.addShort(SEND_TIME);
	packet.addByte(1);
	packet.addByte(hour);
	packet.addByte(min);
	packet.addByte(sec);
	player->getSession()->send(packet);
}

void MapPacket::showTimer(Player *player, int32_t sec) {
	PacketCreator packet;
	packet.addShort(SEND_TIME);
	packet.addByte(2);
	packet.addInt(sec);
	player->getSession()->send(packet);
}

void MapPacket::makeApple(Player *player) {
	PacketCreator packet;
	packet.addShort(SEND_MAKE_APPLE);
	player->getSession()->send(packet);
}

void MapPacket::changeMusic(int32_t mapid, const string &musicname) { // Change music
	PacketCreator packet;
	packet.addShort(SEND_MAP_EFFECT);
	packet.addByte(0x06);
	packet.addString(musicname);
	Maps::maps[mapid]->sendPacket(packet);
}

void MapPacket::sendSound(int32_t mapid, const string &soundname) { // Send Sound
	// Party1/Clear = Clear
	// Party1/Failed = Wrong
	// Cokeplay/Victory = Victory
	// Cokeplay/Failed = Lose
	// Coconut/Victory = Victory
	// Coconut/Failed = Lose 
	PacketCreator packet = PacketCreator();
	packet.addShort(SEND_MAP_EFFECT);
	packet.addByte(0x04);
	packet.addString(soundname);
	Maps::maps[mapid]->sendPacket(packet);
}
// Send Event
void MapPacket::sendEvent(int32_t mapid, const string &eventname) {
	// quest/party/clear = Clear
	// quest/party/wrong_kor = Wrong
	// quest/carnival/win = Win
	// quest/carnival/lose = Lose
	// event/coconut/victory = Victory
	// event/coconut/lose = Lose
	PacketCreator packet = PacketCreator();
	packet.addShort(SEND_MAP_EFFECT);
	packet.addByte(0x03);
	packet.addString(eventname);
	Maps::maps[mapid]->sendPacket(packet);
}

void MapPacket::showEventInstructions(int32_t mapid) { // Thanks to Snow/Raz who found this by playing around
	PacketCreator packet = PacketCreator();
	packet.addShort(SEND_GM_EVENT_INSTRUCTIONS);
	packet.addByte(0x00);
	Maps::maps[mapid]->sendPacket(packet);
}