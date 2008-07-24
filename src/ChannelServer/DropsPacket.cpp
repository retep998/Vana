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
#include "DropsPacket.h"
#include "PacketCreator.h"
#include "Map.h"
#include "Maps.h"
#include "Player.h"
#include "Drops.h"
#include "SendHeader.h"

void DropsPacket::drop(Drop *drop, Pos origin) {
	PacketCreator packet;
	packet.addShort(SEND_DROP_ITEM);
	packet.addByte(1);
	packet.addInt(drop->getID());
	packet.addByte(drop->isMesos());
	packet.addInt(drop->getObjectID());
	packet.addInt(drop->getOwner());
	packet.addByte(0);
	packet.addPos(drop->getPos());
	packet.addInt(drop->getTime()); // Time till
	packet.addPos(origin);
	packet.addShort(0);
	packet.addByte(0);
	if (!drop->isMesos()) {
		packet.addBytes("8005BB46E6170200");
	}
	Maps::maps[drop->getMap()]->sendPacket(packet);
}

void DropsPacket::dropForPlayer(Player *player, Drop *drop, Pos origin) {
	PacketCreator packet;
	packet.addShort(SEND_DROP_ITEM);
	packet.addByte(1);
	packet.addInt(drop->getID());
	packet.addByte(drop->isMesos());
	packet.addInt(drop->getObjectID());
	packet.addInt(drop->getOwner());
	packet.addByte(0);
	packet.addPos(drop->getPos());
	packet.addInt(drop->getTime()); // Time till
	packet.addPos(origin);
	packet.addShort(0);
	packet.addByte(0);
	if (!drop->isMesos()) {
		packet.addBytes("8005BB46E6170200");
	}
	packet.send(player);
}

void DropsPacket::showDrop(Player *player, Drop *drop) {
	PacketCreator packet;
	packet.addShort(SEND_DROP_ITEM);
	packet.addByte(2);
	packet.addInt(drop->getID());
	packet.addByte(drop->isMesos());
	packet.addInt(drop->getObjectID());
	packet.addInt(drop->getOwner()); // Owner of drop
	packet.addByte(0);
	packet.addPos(drop->getPos());
	packet.addInt(drop->getTime());
	packet.addByte(0);
	if (!drop->isMesos()) {
		packet.addBytes("8005BB46E6170200");
	}
	packet.send(player);
}

void DropsPacket::takeNote(Player *player, int id, bool ismesos, short amount) {
	PacketCreator packet;
	packet.addShort(SEND_NOTE);
	packet.addByte(0);
	if (id == 0)
		packet.addByte(-1);
	else {
		packet.addByte(ismesos);
		packet.addInt(id);
		if (ismesos) {
			packet.addShort(0); // Internet Cafe Bonus
		}
		else if (id/1000000 != 1)
			packet.addShort(amount);
	}
	if (!ismesos) {
		packet.addInt(0);
		packet.addInt(0);
	}
	packet.send(player);
}

void DropsPacket::takeDrop(Player *player, Drop *drop) {
	PacketCreator packet;
	packet.addShort(SEND_TAKE_DROP);
	packet.addByte(2);
	packet.addInt(drop->getID());
	packet.addInt(player->getPlayerid());
	if (!drop->isQuest()) {
		Maps::maps[player->getMap()]->sendPacket(packet);
	}
	else {
		packet.send(player);
	}
}

void DropsPacket::dontTake(Player *player) {
	PacketCreator packet;
	packet.addShort(SEND_MOVE_ITEM);
	packet.addShort(1);
	packet.send(player);
}

void DropsPacket::removeDrop(Drop *drop) {
	PacketCreator packet;
	packet.addShort(SEND_TAKE_DROP);
	packet.addByte(0);
	packet.addInt(drop->getID());
	Maps::maps[drop->getMap()]->sendPacket(packet);
}

void DropsPacket::explodeDrop(Drop *drop) {
	PacketCreator packet;
	packet.addShort(SEND_TAKE_DROP);
	packet.addByte(4);
	packet.addInt(drop->getID());
	packet.addShort(655);
	Maps::maps[drop->getMap()]->sendPacket(packet);
}
