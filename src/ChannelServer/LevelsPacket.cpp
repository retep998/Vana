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
#include "MapleSession.h"
#include "Maps.h"
#include "PacketCreator.h"
#include "Player.h"
#include "SendHeader.h"

void LevelsPacket::showEXP(Player *player, int32_t exp, int8_t type) {
	PacketCreator packet;
	packet.addShort(SEND_NOTE);
	packet.addByte(3);
	packet.addByte(1);
	packet.addInt(exp);
	packet.addInt(type);
	packet.addInt(0);
	packet.addInt(0);
	packet.addByte(0);
	player->getSession()->send(packet);
}

void LevelsPacket::levelUp(Player *player) {
	PacketCreator packet;
	packet.addShort(SEND_SHOW_SKILL);
	packet.addInt(player->getId());
	packet.addByte(0);
	Maps::maps[player->getMap()]->sendPacket(packet, player);
}

void LevelsPacket::statOK(Player *player) {
	PacketCreator packet;
	packet.addShort(SEND_UPDATE_STAT);
	packet.addShort(1);
	packet.addInt(0);
	player->getSession()->send(packet);
}

void LevelsPacket::jobChange(Player *player) {
	PacketCreator packet;
	packet.addShort(SEND_SHOW_SKILL);
	packet.addInt(player->getId());
	packet.addByte(8);
	Maps::maps[player->getMap()]->sendPacket(packet, player);
}
