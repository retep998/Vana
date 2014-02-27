/*
Copyright (C) 2008-2014 Vana Development Team

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
#include "MovementHandler.hpp"
#include "MovableLife.hpp"
#include "Pos.hpp"
#include "PacketReader.hpp"
#include <iomanip>
#include <iostream>

auto MovementHandler::parseMovement(MovableLife *life, PacketReader &reader) -> Pos {
	foothold_id_t foothold = 0;
	int8_t stance = 0;
	int16_t x = 0;
	int16_t y = 0;
	uint8_t n = reader.get<uint8_t>();

	enum MovementTypes {
		NormalMovement = 0,
		Jump = 1,
		JumpKb = 2,
		Unk1 = 3,
		Teleport = 4,
		NormalMovement2 = 5,
		FlashJump = 6,
		Assaulter = 7,
		Assassinate = 8,
		Rush = 9,
		Falling = 10,
		Chair = 11,
		ExcessiveKb = 12,
		RecoilShot = 13,
		Unk2 = 14,
		JumpDown = 15,
		Wings = 16,
		WingsFalling = 17
	};

	for (uint8_t i = 0; i < n; ++i) {
		int8_t type = reader.get<int8_t>();
		switch (type) {
			case Falling:
				reader.skipBytes(1);
				break;
			case Wings:
				reader.skipBytes(7);
				break;
			case WingsFalling:
				x = reader.get<int16_t>();
				y = reader.get<int16_t>();
				foothold = reader.get<int16_t>();
				stance = reader.get<int8_t>();
				reader.skipBytes(6);
				break;
			case ExcessiveKb:
				reader.skipBytes(7);
				break;
			case Unk2:
				reader.skipBytes(9);
				break;
			case NormalMovement:
			case NormalMovement2:
				x = reader.get<int16_t>();
				y = reader.get<int16_t>();
				reader.skipBytes(4);
				foothold = reader.get<int16_t>();
				stance = reader.get<int8_t>();
				reader.skipBytes(2);
				break;
			case Jump:
			case JumpKb:
			case FlashJump:
			case RecoilShot:
				x = reader.get<int16_t>();
				y = reader.get<int16_t>();
				stance = reader.get<int8_t>();
				foothold = reader.get<int16_t>();
				break;
			case JumpDown:
				x = reader.get<int16_t>();
				y = reader.get<int16_t>();
				reader.skipBytes(6);
				foothold = reader.get<int16_t>();
				stance = reader.get<int8_t>();
				reader.skipBytes(2);
				break;
			case Chair:
				x = reader.get<int16_t>();
				y = reader.get<int16_t>();
				foothold = reader.get<int16_t>();
				stance = reader.get<int8_t>();
				reader.skipBytes(2);
				break;
			case Unk1:
			case Teleport:
			case Assaulter:
			case Assassinate:
			case Rush:
				x = reader.get<int16_t>();
				y = reader.get<int16_t>();
				reader.skipBytes(4);
				stance = reader.get<int8_t>();
				break;
			default:
				std::cerr << "New type of movement: 0x" << std::hex << static_cast<int16_t>(type) << std::endl;
				break;
		}
	}

	Pos pos(x, y);
	life->resetMovement(foothold, pos, stance);
	return pos;
}