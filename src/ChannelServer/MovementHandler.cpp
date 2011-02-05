/*
Copyright (C) 2008-2011 Vana Development Team

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
#include "MovementHandler.h"
#include "MovableLife.h"
#include "Pos.h"
#include "PacketReader.h"
#include <iomanip>
#include <iostream>

Pos MovementHandler::parseMovement(MovableLife *life, PacketReader &packet) {
	int16_t foothold = 0;
	int8_t stance = 0;
	int16_t x = 0;
	int16_t y = 0;
	uint8_t n = packet.get<uint8_t>();
	for (uint8_t i = 0; i < n; i++) {
		int8_t type = packet.get<int8_t>();
		switch (type) {
			case 10: // Falling of some kind
				packet.skipBytes(1);
				break;
			case 16: // Wings
				packet.skipBytes(7);
				break;
			case 17: // Part of Wings, the falling, I believe
				x = packet.get<int16_t>();
				y = packet.get<int16_t>();
				foothold = packet.get<int16_t>();
				stance = packet.get<int8_t>();
				packet.skipBytes(6);
				break;
			case 12: // Horntail knockback
				packet.skipBytes(7);
				break;
			case 14:
				packet.skipBytes(9);
				break;
			case 0: // Normal up/down/left/right movement
			case 5:
				x = packet.get<int16_t>();
				y = packet.get<int16_t>();
				packet.skipBytes(4);
				foothold = packet.get<int16_t>();
				stance = packet.get<int8_t>();
				packet.skipBytes(2);
				break;
			case 1: // Jumping
			case 2: // Jumping/knockback?
			case 6: // Flash Jump
			case 13: // Recoil Shot
				x = packet.get<int16_t>();
				y = packet.get<int16_t>();
				stance = packet.get<int8_t>();
				foothold = packet.get<int16_t>();
				break;
			case 15: // Jump down
				x = packet.get<int16_t>();
				y = packet.get<int16_t>();
				packet.skipBytes(6);
				foothold = packet.get<int16_t>();
				stance = packet.get<int8_t>();
				packet.skipBytes(2);
				break;
			case 11: // Chair
				x = packet.get<int16_t>();
				y = packet.get<int16_t>();
				foothold = packet.get<int16_t>();
				stance = packet.get<int8_t>();
				packet.skipBytes(2);
				break;
			case 3:
			case 4: // Teleport
			case 7: // Assaulter
			case 8: // Assassinate
			case 9: // Rush
				x = packet.get<int16_t>();
				y = packet.get<int16_t>();
				packet.skipBytes(4);
				stance = packet.get<int8_t>();
				break;
			default:
				std::cout << "New type of movement: 0x" << std::hex << (int16_t) type << std::endl;
				break;
		}
	}
	Pos pos(x, y);
	life->setPos(pos);
	life->setFh(foothold);
	life->setStance(stance);
	return pos;
}