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
#include "Maps.h"
#include "MapPacket.h"
#include "Inventory.h"
#include "SendHeader.h"

PacketCreator MapPacket::playerPacket(Player *player) {
	PacketCreator packet;
	packet.addShort(SEND_SHOW_PLAYER);
	packet.addInt(player->getPlayerid());
	packet.addString(player->getName());
	packet.addInt(0);
	packet.addInt(0);
	packet.addByte(player->getSkill().types[0]);
	packet.addByte(player->getSkill().types[1]);
	packet.addByte(player->getSkill().types[2]);
	packet.addByte(player->getSkill().types[3]);
	packet.addByte(player->getSkill().types[4]);
	packet.addByte(player->getSkill().types[5]);
	packet.addByte(player->getSkill().types[6]);
	packet.addByte(player->getSkill().types[7]);
	if (player->getSkill().isval) {
		packet.addByte(player->getSkill().val);
	}

	packet.addShort(0);
	packet.addByte(0);
	packet.addInt(0x7E);
	packet.addShort(0);
	packet.addByte(0);
	packet.addInt(0);
	packet.addInt(0);
	for (size_t i = 0; i < 3; i++) {
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
	packet.addInt(0);

	packet.addByte(player->getGender());
	packet.addByte(player->getSkin());
	packet.addInt(player->getEyes());
	packet.addByte(1);
	packet.addInt(player->getHair());

	int equips[35][2] = {0};
	equipinventory *playerequips = player->inv->getEquips();
	for (equipinventory::iterator iter = playerequips->begin(); iter != playerequips->end(); iter++) { //sort equips
		Equip *equip = iter->second;
		if (iter->first < 0) {
			if (equips[equip->type][0] > 0) {
				if (Inventory::isCash(equip->id)) {
					equips[equip->type][1] = equips[equip->type][0];
					equips[equip->type][0] = equip->id;
				}
				else {
					equips[equip->type][1] = equip->id;
				}
			}
			else {
				equips[equip->type][0] = equip->id;
			}
		}
	}
	for (int i = 0; i < 35; i++) { //shown items
		if (equips[i][0] > 0) {
			packet.addByte(i);
			if (i == 11 && equips[i][1] > 0) // normal weapons always here
				packet.addInt(equips[i][1]);
			else
				packet.addInt(equips[i][0]);
		}
	}
	packet.addByte(-1);
	for (int i = 0; i < 35; i++) { //covered items
		if (equips[i][1] > 0 && i != 11) {
			packet.addByte(i);
			packet.addInt(equips[i][1]);
		}
	}
	packet.addByte(-1);
	if (equips[11][1] > 0) // cs weapon
		packet.addInt(equips[11][0]);
	else
		packet.addInt(0);
	packet.addInt(0);
	packet.addInt(0);
	packet.addInt(0);

	packet.addInt(0);
	packet.addInt(player->getItemEffect()); 
	packet.addInt(player->getChair());
	packet.addPos(player->getPos());
	packet.addByte(player->getType());
	packet.addInt(0);
	packet.addShort(1);
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
	packet.addInt(player->getPlayerid());
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
	packet.send(player);
}

void MapPacket::portalBlocked(Player *player) {
	PacketCreator packet;
	packet.addShort(SEND_UPDATE_STAT);
	packet.addByte(1);
	packet.addInt(0);
	packet.send(player);
}

void MapPacket::showClock(Player *player, unsigned char hour, unsigned char min, unsigned char sec) {
	PacketCreator packet;
	packet.addShort(SEND_TIME);
	packet.addByte(1);
	packet.addByte(hour);
	packet.addByte(min);
	packet.addByte(sec);

	packet.send(player);
}

void MapPacket::showTimer(Player *player, int sec) {
	PacketCreator packet;
	packet.addShort(SEND_TIME);
	packet.addByte(2);
	packet.addInt(sec);

	packet.send(player);
}

void MapPacket::makeApple(Player *player) {
	PacketCreator packet;
	packet.addShort(SEND_MAKE_APPLE);  
	packet.send(player);
}

// Change music
void MapPacket::changeMusic(int mapid, const string &musicname) {
	PacketCreator packet;
	packet.addShort(SEND_MAP_EFFECT);
	packet.addByte(0x06);
	packet.addString(musicname);
	Maps::maps[mapid]->sendPacket(packet);
}
// Send Sound
void MapPacket::sendSound(int mapid, const string &soundname) {
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
void MapPacket::sendEvent(int mapid, const string &eventname) {
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
