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
#include "Drops.h"
#include "MapleSession.h"
#include "Maps.h"
#include "PacketCreator.h"
#include "Pets.h"
#include "Player.h"
#include "SendHeader.h"

void DropsPacket::showDrop(Player *player, Drop *drop, char type, bool newdrop, Pos origin) {
	PacketCreator packet;
	packet.addShort(SEND_DROP_ITEM);
	packet.addByte(type); // 2 = show existing, 1 then 0 = show new
	packet.addInt(drop->getID());
	packet.addByte(drop->isMesos());
	packet.addInt(drop->getObjectID());
	packet.addInt(drop->getOwner()); // Owner of drop
	packet.addByte(0);
	packet.addPos(drop->getPos());
	packet.addInt(drop->getTime());
	if (type == 1 || type == 0) { // Give the point of origin for things that are just being dropped in
		packet.addPos(origin);
		packet.addShort(0);
	}
	if (!drop->isMesos()) {
		packet.addBytes("008005BB46E61702");
	}
	packet.addByte(!drop->isplayerDrop()); // Determines whether pets can pick item up or not
	if (player != 0)
		player->getSession()->send(packet);
	else
		Maps::maps[drop->getMap()]->sendPacket(packet);

	if (newdrop)
		showDrop(player, drop, 0, false, origin);
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
	player->getSession()->send(packet);
}

void DropsPacket::takeDrop(Player *player, Drop *drop) {
	PacketCreator packet;
	packet.addShort(SEND_TAKE_DROP);
	packet.addByte(2);
	packet.addInt(drop->getID());
	packet.addInt(player->getId());
	if (!drop->isQuest()) {
		Maps::maps[player->getMap()]->sendPacket(packet);
	}
	else {
		player->getSession()->send(packet);
	}
}

void DropsPacket::dontTake(Player *player) {
	PacketCreator packet;
	packet.addShort(SEND_MOVE_ITEM);
	packet.addShort(1);
	player->getSession()->send(packet);
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

void DropsPacket::takeDropPet(Player *player, Drop *drop, Pet *pet) {
	PacketCreator packet;
	packet.addShort(SEND_TAKE_DROP);
	packet.addByte(5);
	packet.addInt(drop->getID());
	packet.addInt(player->getId());
	packet.addByte(pet->getIndex());
	if (!drop->isQuest()) {
		Maps::maps[player->getMap()]->sendPacket(packet);
	}
	player->getSession()->send(packet);
}
