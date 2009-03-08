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
#include "MapPacket.h"
#include "Buffs.h"
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
	packet.add<int16_t>(SEND_SHOW_PLAYER);
	packet.add<int32_t>(player->getId());
	packet.addString(player->getName());
	packet.addString(""); // Guild
	packet.add<int16_t>(0); // Guild icon garbage
	packet.add<int8_t>(0); // Guild icon garbage
	packet.add<int16_t>(0); // Guild icon garbage
	packet.add<int8_t>(0); // Guild icon garbage

	packet.add<int32_t>(0);
	packet.add<int32_t>(1);
	packet.add<int16_t>(0);
	packet.add<int8_t>(0);
	packet.add<int8_t>(0xf8);

	/* Code that needs to be modified
	SkillMapEnterActiveInfo enter = player->getActiveBuffs()->getSkillMapEnterInfo();
	packet.add<int8_t>(enter.types[TYPE_1]);
	packet.add<int8_t>(enter.types[TYPE_2]);
	packet.add<int8_t>(enter.types[TYPE_3]);
	packet.add<int8_t>(enter.types[TYPE_4]);
	packet.add<int8_t>(enter.types[TYPE_5]);
	packet.add<int8_t>(enter.types[TYPE_6]);
	packet.add<int8_t>(enter.types[TYPE_7]);
	packet.add<int8_t>(enter.types[TYPE_8]);
	if (enter.isval) {
		packet.add<int8_t>(enter.val);
	}
	*/
	packet.add<int64_t>(0); // Temporary addition until all the buffs are working properly with no disconnections
	packet.add<int32_t>(0);
	packet.add<int16_t>(0);
	packet.add<int32_t>(1065638850); // Unknown
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int16_t>(0);
	packet.add<int32_t>(1065638850);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int16_t>(0);
	packet.add<int32_t>(1065638850);
	packet.add<int16_t>(0);

	int32_t mount = player->getInventory()->getEquippedID(18);
	if (mount != 0 && player->getActiveBuffs()->getActiveSkillLevel(1004) > 0) {
		packet.add<int32_t>(mount);
		packet.add<int32_t>(1004);
		packet.add<int32_t>(1066113350);
	}
	else if (player->getActiveBuffs()->getActiveSkillLevel(5221006) > 0) {
		packet.add<int32_t>(1932000);
		packet.add<int32_t>(5221006);
		packet.add<int32_t>(1066113350);
	}
	else {
		packet.add<int32_t>(0);
		packet.add<int32_t>(0);
		packet.add<int32_t>(1065638850);
	}

	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(1065638850);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int16_t>(0);
	packet.add<int32_t>(1065638850);
	packet.add<int32_t>(0);
	packet.add<int16_t>(player->getJob());

	PlayerPacketHelper::addPlayerDisplay(packet, player);

	packet.add<int32_t>(0);
	packet.add<int32_t>(player->getItemEffect());
	packet.add<int32_t>(player->getChair());
	packet.addPos(player->getPos());
	packet.add<int8_t>(player->getStance());
	packet.add<int16_t>(player->getFH());
	packet.add<int8_t>(0);
	for (int8_t i = 0; i < 3; i++) {
		if (Pet *pet = player->getPets()->getSummoned(i)) {
			packet.add<int8_t>(1);
			packet.add<int32_t>(pet->getType());
			packet.addString(pet->getName());
			packet.add<int32_t>(pet->getId());
			packet.add<int32_t>(0);
			packet.addPos(pet->getPos());
			packet.add<int8_t>(pet->getStance());
			packet.add<int32_t>(pet->getFH());
		}
	}
	packet.add<int8_t>(0);
	packet.add<int16_t>(1);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	return packet;
}

void MapPacket::showPlayer(Player *player) {
	PacketCreator packet = playerPacket(player);
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void MapPacket::removePlayer(Player *player) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_REMOVE_PLAYER);
	packet.add<int32_t>(player->getId());
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void MapPacket::changeMap(Player *player) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_CHANGE_MAP);
	packet.add<int32_t>(ChannelServer::Instance()->getChannel()); // Channel
	packet.add<int16_t>(0); // 2?
	packet.add<int16_t>(0);
	packet.add<int32_t>(player->getMap());
	packet.add<int8_t>(player->getMappos());
	packet.add<int16_t>(player->getHP());
	packet.add<int8_t>(0);
	packet.add<int32_t>(-1);
	packet.add<int16_t>(-1);
	packet.add<int8_t>(-1);
	packet.add<int8_t>(1);
	player->getSession()->send(packet);
}

void MapPacket::portalBlocked(Player *player) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_UPDATE_STAT);
	packet.add<int8_t>(1);
	packet.add<int32_t>(0);
	player->getSession()->send(packet);
}

void MapPacket::showClock(Player *player, uint8_t hour, uint8_t min, uint8_t sec) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_TIME);
	packet.add<int8_t>(1);
	packet.add<int8_t>(hour);
	packet.add<int8_t>(min);
	packet.add<int8_t>(sec);
	player->getSession()->send(packet);
}

void MapPacket::showTimer(Player *player, int32_t sec) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_TIME);
	packet.add<int8_t>(2);
	packet.add<int32_t>(sec);
	player->getSession()->send(packet);
}

void MapPacket::forceMapEquip(Player *player) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_FORCE_MAP_EQUIP);
	player->getSession()->send(packet);
}

void MapPacket::setMusic(int32_t mapid, const string &musicname) { // Set music
	PacketCreator packet;
	packet.add<int16_t>(SEND_MAP_EFFECT);
	packet.add<int8_t>(0x06);
	packet.addString(musicname);
	Maps::getMap(mapid)->sendPacket(packet);
}

void MapPacket::sendSound(int32_t mapid, const string &soundname) { // Send Sound
	// Party1/Clear = Clear
	// Party1/Failed = Wrong
	// Cokeplay/Victory = Victory
	// Cokeplay/Failed = Lose
	// Coconut/Victory = Victory
	// Coconut/Failed = Lose 
	PacketCreator packet = PacketCreator();
	packet.add<int16_t>(SEND_MAP_EFFECT);
	packet.add<int8_t>(0x04);
	packet.addString(soundname);
	Maps::getMap(mapid)->sendPacket(packet);
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
	packet.add<int16_t>(SEND_MAP_EFFECT);
	packet.add<int8_t>(0x03);
	packet.addString(eventname);
	Maps::getMap(mapid)->sendPacket(packet);
}

void MapPacket::showEventInstructions(int32_t mapid) { // Thanks to Snow/Raz who found this by playing around
	PacketCreator packet = PacketCreator();
	packet.add<int16_t>(SEND_GM_EVENT_INSTRUCTIONS);
	packet.add<int8_t>(0x00);
	Maps::getMap(mapid)->sendPacket(packet);
}