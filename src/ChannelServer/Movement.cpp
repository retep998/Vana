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
#include "MovableLife.h"
#include "Pos.h"
#include "ReadPacket.h"
#include <iomanip>
#include <iostream>

Pos Movement::parseMovement(MovableLife *life, ReadPacket *packet) {
	short foothold = 0;
	char stance = 0;
	short x = 0;
	short y = 0;

	unsigned char n = packet->getByte();
	for (unsigned char i = 0; i < n; i++) {
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
				y = packet->getShort();
				packet->skipBytes(4);
				foothold = packet->getShort();
				stance = packet->getByte();
				packet->skipBytes(2);
				break;
			case 1:
			case 2:
			case 6:
				x = packet->getShort();
				y = packet->getShort();
				stance = packet->getByte();
				packet->skipBytes(2);
				break;
			case 15:
				x = packet->getShort();
				y = packet->getShort();
				packet->skipBytes(6);
				foothold = packet->getShort();
				stance = packet->getByte();
				packet->skipBytes(2);
				break;
			case 11:
				x = packet->getShort();
				y = packet->getShort();
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
				y = packet->getShort();
				packet->skipBytes(4);
				stance = packet->getByte();
				break;
			default:
				std::cout << "New type of movement: 0x" << std::hex << (int) type << std::endl;
				break;
		}
	}

	Pos pos(x, y);

	life->setPos(pos);
	life->setFH(foothold);
	life->setStance(stance);

	return pos;
}