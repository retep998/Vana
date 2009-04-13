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
#include <boost/tr1/unordered_map.hpp>
#include <utility>

using std::pair;
using std::tr1::unordered_map;

PacketCreator MapPacket::playerPacket(Player *player) {
	PacketCreator packet;
	MapEntryBuffs enter = player->getActiveBuffs()->getMapEntryBuffs();
	packet.add<int16_t>(SEND_SHOW_PLAYER);
	packet.add<int32_t>(player->getId());
	packet.addString(player->getName());
	packet.addString(""); // Guild
	packet.add<int16_t>(0); // Guild icon garbage
	packet.add<int8_t>(0); // Guild icon garbage
	packet.add<int16_t>(0); // Guild icon garbage
	packet.add<int8_t>(0); // Guild icon garbage

	packet.add<int32_t>(0);
	packet.add<uint8_t>(0xf8);
	packet.add<int8_t>(3);
	packet.add<int16_t>(0);
	packet.add<int8_t>(enter.types[Byte5]);
	packet.add<int8_t>(enter.types[Byte6]);
	packet.add<int8_t>(enter.types[Byte7]);
	packet.add<int8_t>(enter.types[Byte8]);
	packet.add<int8_t>(enter.types[Byte1]);
	packet.add<int8_t>(enter.types[Byte2]);
	packet.add<int8_t>(enter.types[Byte3]);
	packet.add<int8_t>(enter.types[Byte4]);
	const int8_t byteorder[8] = { Byte1, Byte2, Byte3, Byte4, Byte5, Byte6, Byte7, Byte8 };

	for (int8_t i = 0; i < 8; i++) {
		int8_t cbyte = byteorder[i]; // Values are sorted by lower bytes first
		if (enter.types[cbyte] != 0) {
			for (unordered_map<uint8_t, pair<bool, int16_t> >::iterator iter = enter.values[cbyte].begin(); iter != enter.values[cbyte].end(); iter++) {
				if (iter->second.first) {
					int16_t value = iter->second.second;
					if (cbyte == Byte3) {
						if (iter->first == 0x20) {
							packet.add<int8_t>(player->getActiveBuffs()->getCombo() + 1);
						}
						if (iter->first == 0x40) {
							packet.add<int32_t>(player->getActiveBuffs()->getCharge());
						}
					}
					else if (cbyte == Byte5) {
						packet.add<int16_t>(value);
					}
					else {
						packet.add<int8_t>(static_cast<int8_t>(value));
					}
				}
			}
		}
	}

	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int16_t>(0);
	packet.add<int32_t>(1065638850); // Unknown
	packet.add<int16_t>(0);
	packet.add<int8_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(1065638850);
	packet.add<int16_t>(0);
	packet.add<int8_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(1065638850);
	packet.add<int16_t>(0);
	packet.add<int8_t>(0);

	if (enter.mountid > 0) {
		packet.add<int32_t>(enter.mountid);
		packet.add<int32_t>(enter.mountskill);
	}
	else {
		packet.add<int32_t>(0);
		packet.add<int32_t>(0);
	}

	packet.add<int32_t>(1065638850);
	packet.add<int8_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(1065638850);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(1065638850);
	packet.add<int8_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(1065638850);
	packet.add<int16_t>(0);
	packet.add<int8_t>(0);
	packet.add<int16_t>(player->getJob());

	PlayerPacketHelper::addPlayerDisplay(packet, player);

	packet.add<int32_t>(0);
	packet.add<int32_t>(player->getItemEffect());
	packet.add<int32_t>(player->getChair());
	packet.addPos(player->getPos());
	packet.add<int8_t>(player->getStance());
	packet.add<int16_t>(player->getFh());
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
			packet.add<int32_t>(pet->getFh());
		}
	}
	packet.add<int8_t>(0);
	packet.add<int16_t>(1);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int16_t>(0);
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
	packet.add<int8_t>(0x01);
	packet.add<int8_t>(hour);
	packet.add<int8_t>(min);
	packet.add<int8_t>(sec);
	player->getSession()->send(packet);
}

void MapPacket::showTimer(int32_t mapid, int32_t sec) {
	PacketCreator packet;
	if (sec > 0) {
		packet.add<int16_t>(SEND_TIME);
		packet.add<int8_t>(0x02);
		packet.add<int32_t>(sec);
	}
	else {
		packet.add<int16_t>(SEND_STOP_TIME);
	}
	Maps::getMap(mapid)->sendPacket(packet);
}

void MapPacket::showTimer(Player *player, int32_t sec) {
	PacketCreator packet;
	if (sec > 0) {
		packet.add<int16_t>(SEND_TIME);
		packet.add<int8_t>(0x02);
		packet.add<int32_t>(sec);
	}
	else {
		packet.add<int16_t>(SEND_STOP_TIME);
	}
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
