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
#include "LevelsPacket.h"
#include "PacketCreator.h"
#include "Player.h"
#include "SendHeader.h"

void LevelsPacket::showEXP(Player* player, int exp, char type){
	Packet packet = Packet();
	packet.addHeader(SEND_NOTE);
	packet.addByte(3);
	packet.addByte(1);
	packet.addInt(exp);
	packet.addInt(type);
	packet.addInt(0);
	packet.addInt(0);
	packet.addByte(0);
	packet.packetSend(player);
}

void LevelsPacket::levelUP(Player* player, vector <Player*> players){
	Packet packet = Packet();
	packet.addHeader(SEND_UPDATE_STAT);
	packet.addShort(0);
	packet.addShort(0x7C10);
	packet.addShort(1);
	packet.addByte(player->getLevel());
	packet.addShort(player->getHP());
	packet.addShort(player->getRMHP());
	packet.addShort(player->getMP());
	packet.addShort(player->getRMMP());
	packet.addShort(player->getAp());
	packet.addInt(player->getExp());
	packet.packetSend(player); // TODO
	packet = Packet();
	packet.addHeader(SEND_SHOW_SKILL);
	packet.addInt(player->getPlayerid());
	packet.addByte(0);
	packet.sendTo<Player>(player, players, 0);

}

void LevelsPacket::statOK(Player* player){
	Packet packet = Packet();
	packet.addHeader(SEND_UPDATE_STAT);
	packet.addShort(1);
	packet.addInt(0);
	packet.packetSend(player);
}
void LevelsPacket::changeStat(Player* player, int nam, short val){
	Packet packet = Packet();
	packet.addHeader(SEND_UPDATE_STAT);
	packet.addShort(1);
	packet.addInt(nam);
	packet.addShort(val);
	packet.addShort(player->getAp());
	packet.packetSend(player);
}

void LevelsPacket::jobChange(Player *player, std::vector<Player*> players){
	Packet packet = Packet();
	packet.addHeader(SEND_UPDATE_STAT);
	packet.addShort(0);
	packet.addInt(0x20);
	packet.addShort(player->getJob());
	packet.packetSend(player);
	packet = Packet();
	packet.addHeader(SEND_SHOW_SKILL);
	packet.addInt(player->getPlayerid());
	packet.addByte(8);
	packet.sendTo<Player>(player, players, 0);
}