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

Packet MapPacket::playerPacket(Player* player) {
	Packet packet;
	packet.addHeader(SEND_SHOW_PLAYER);
	packet.addInt(player->getPlayerid());
	packet.addShort(strlen(player->getName()));
	packet.addString(player->getName(), strlen(player->getName()));
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
	if(player->getSkill().isval)
		packet.addByte(player->getSkill().val);
	packet.addByte(player->getGender());
	packet.addByte(player->getSkin());
	packet.addInt(player->getEyes());
	packet.addByte(1);
	packet.addInt(player->getHair());
	int equips[35][2] = {0};
	for(int i=0; i<player->inv->getEquipNum(); i++){ //sort equips
		Equip* equip = player->inv->getEquip(i);
		if(equip->pos<0){
			if(equips[equip->type][0]>0){
				if(Inventory::isCash(equip->id)){
					equips[equip->type][1] = equips[equip->type][0];
					equips[equip->type][0] = equip->id;
				}
				else{
					equips[equip->type][1] = equip->id;
				}
			}
			else{
				equips[equip->type][0] = equip->id;
			}
		}
	}
	for(int i=0; i<35; i++){ //shown items
		if(equips[i][0]>0){
			packet.addByte(i);
			if(i == 11 && equips[i][1]>0) // normal weapons always here
				packet.addInt(equips[i][1]);
			else
				packet.addInt(equips[i][0]);
		}
	}
	packet.addByte(-1);
	for(int i=0; i<35; i++){ //covered items
		if(equips[i][1]>0 && i != 11){
			packet.addByte(i);
			packet.addInt(equips[i][1]);
		}
	}
	packet.addByte(-1);
	if(equips[11][1]>0) // cs weapon
		packet.addInt(equips[11][0]);
	else
		packet.addInt(0);
	packet.addInt(0);
	packet.addInt(0);
	packet.addInt(0);
	packet.addInt(0);
	packet.addInt(player->getItemEffect()); 
	packet.addInt(player->getChair());
	packet.addShort(player->getPos().x);
	packet.addShort(player->getPos().y);
	packet.addByte(player->getType());
	packet.addInt(0);
	packet.addShort(1);
	packet.addInt(0);
	packet.addInt(0);
	packet.addInt(0);
	packet.addInt(0);
	return packet;
}

void MapPacket::showPlayer(Player* player, vector <Player*> players){
	Packet packet = playerPacket(player);
	for(unsigned int i=0; i<players.size(); i++){
		packet.send(players[i]);
	}
}

void MapPacket::removePlayer(Player* player, vector <Player*> players){
	Packet packet;
	packet.addHeader(SEND_REMOVE_PLAYER);
	packet.addInt(player->getPlayerid());
	for(unsigned int i=0; i<players.size(); i++){
		if(player->getPlayerid() != players[i]->getPlayerid())
			packet.send(players[i]);
	}
}

void MapPacket::showPlayers(Player* player, vector <Player*> players){
	for (unsigned int i=0; i<players.size(); i++) {
		if (player->getPlayerid() != players[i]->getPlayerid() && players[i]->skills->getActiveSkillLevel(5101004) == 0) {
			Packet packet = playerPacket(players[i]);
			packet.send(player);
		}
	}
}

void MapPacket::changeMap(Player* player){
	Packet packet;
	packet.addHeader(SEND_CHANGE_MAP);
	packet.addInt(player->getChannel()); // Channel
	packet.addShort(0); // 2?
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

void MapPacket::portalBlocked(Player* player){
	Packet packet;
	packet.addHeader(SEND_UPDATE_STAT);
	packet.addShort(1);
	packet.addInt(0);
	packet.send(player);
}

void MapPacket::showClock(Player* player, unsigned char hour, unsigned char min, unsigned char sec){
	Packet packet;
	packet.addHeader(SEND_TIME);
	packet.addByte(1);
	packet.addByte(hour);
	packet.addByte(min);
	packet.addByte(sec);

	packet.send(player);
}

void MapPacket::showTimer(Player *player, int sec) {
	Packet packet;
	packet.addHeader(SEND_TIME);
	packet.addByte(2);
	packet.addInt(sec);

	packet.send(player);
}

void MapPacket::makeApple(Player* player){
	Packet packet;
	packet.addHeader(SEND_MAKE_APPLE);  
	packet.send(player);
}

// Change music
void MapPacket::changeMusic(vector <Player*> players, std::string musicname) {
	Packet packet;
	packet.addHeader(SEND_MAP_EFFECT);
	packet.addByte(0x06);
	packet.addString(musicname);
	for (unsigned int i=0; i < players.size(); i++) {
		packet.send(players[i]);
	}
}
// Send Sound
void MapPacket::sendSound(vector <Player*> players, std::string soundname) {
	// Party1/Clear = Clear
	// Party1/Failed = Wrong
	// Cokeplay/Victory = Victory
	// Cokeplay/Failed = Lose
	// Coconut/Victory = Victory
	// Coconut/Failed = Lose 
	Packet packet = Packet();
	packet.addHeader(SEND_MAP_EFFECT);
	packet.addByte(0x04);
	packet.addString(soundname);
	for (unsigned int i=0; i < players.size(); i++) {
		packet.send(players[i]);
	}
}
// Send Event
void MapPacket::sendEvent(vector <Player*> players, std::string eventname) {
	// quest/party/clear = Clear
	// quest/party/wrong_kor = Wrong
	// quest/carnival/win = Win
	// quest/carnival/lose = Lose
	// event/coconut/victory = Victory
	// event/coconut/lose = Lose
	Packet packet = Packet();
	packet.addHeader(SEND_MAP_EFFECT);
	packet.addByte(0x03);
	packet.addString(eventname);
	for (unsigned int i=0; i < players.size(); i++) {
		packet.send(players[i]);
	}
}
