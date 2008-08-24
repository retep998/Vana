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

#include "Movement.h"
#include "Mobs.h"
#include "Player.h"
#include "Pos.h"
#include "ReadPacket.h"
#include <iostream>

void Movement::parseMovement(Player *player, ReadPacket *packet) {
	short foothold = 0;
	char stance = 0;
	char n = packet->getByte();
	short x = 0;
	short y = 0;
	for (char i = 0; i < n; i++) {
		char type = packet->getByte();
		switch (type) {
			case 10:
				packet->skipBytes(1);
				break;
			case 14:
				packet->skipBytes(9);
				break;
			case 0:
			case 5:
				x = packet->getShort();
				y = packet->getShort() - 1;
				packet->skipBytes(4);
				foothold = packet->getShort();
				stance = packet->getByte();
				packet->skipBytes(2);
				break;
			case 1:
			case 2:
			case 6:
				x = packet->getShort();
				y = packet->getShort() - 1;
				stance = packet->getByte();
				packet->skipBytes(2);
				break;
			case 15:
				x = packet->getShort();
				y = packet->getShort() - 1;
				packet->skipBytes(6);
				foothold = packet->getShort();
				stance = packet->getByte();
				packet->skipBytes(2);
				break;
			case 11:
				x = packet->getShort();
				y = packet->getShort() - 1;
				foothold = packet->getShort();
				stance = packet->getByte();
				packet->skipBytes(2);
				break;
			case 3:
			case 4:
			case 7:
			case 8:
			case 9:
				x = packet->getShort();
				y = packet->getShort() - 1;
				packet->skipBytes(4);
				stance = packet->getByte();
				break;
			default:
				std::cout << "New type of movement: " << type << std::endl;
				break;
		}
	}
	player->setPos(Pos(x, y));
	player->setFH(foothold);
	player->setType(stance);
}

Pos Movement::parseMovement(Mob *mob, ReadPacket *packet) {
	short foothold = 0;
	char stance = 0;
	char n = packet->getByte();
	short x = 0;
	short y = 0;
	for (char i = 0; i < n; i++) {
		char type = packet->getByte();
		switch (type) {
			case 10:
				packet->skipBytes(1);
				break;
			case 14:
				packet->skipBytes(9);
				break;
			case 0:
			case 5:
				x = packet->getShort();
				y = packet->getShort() - 1;
				packet->skipBytes(4);
				foothold = packet->getShort();
				stance = packet->getByte();
				packet->skipBytes(2);
				break;
			case 1:
			case 2:
			case 6:
				x = packet->getShort();
				y = packet->getShort() - 1;
				stance = packet->getByte();
				packet->skipBytes(2);
				break;
			case 15:
				x = packet->getShort();
				y = packet->getShort() - 1;
				packet->skipBytes(6);
				foothold = packet->getShort();
				stance = packet->getByte();
				packet->skipBytes(2);
				break;
			case 11:
				x = packet->getShort();
				y = packet->getShort() - 1;
				foothold = packet->getShort();
				stance = packet->getByte();
				packet->skipBytes(2);
				break;
			case 3:
			case 4:
			case 7:
			case 8:
			case 9:
				x = packet->getShort();
				y = packet->getShort() - 1;
				packet->skipBytes(4);
				stance = packet->getByte();
				break;
			default:
				std::cout << "New type of movement: " << type << std::endl;
				break;
		}
	}
	mob->setPos(Pos(x, y));
	mob->setFH(foothold);
	mob->setType(stance);
	return Pos(x, y);
}