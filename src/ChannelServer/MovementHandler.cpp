/*
Copyright (C) 2008-2015 Vana Development Team

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
#include "Point.hpp"
#include "PacketReader.hpp"
#include <iomanip>
#include <iostream>

auto MovementHandler::parseMovement(MovableLife *life, PacketReader &reader) -> Point {
	foothold_id_t foothold = 0;
	int8_t stance = 0;
	coord_t x = 0;
	coord_t y = 0;
	uint8_t movementCount = reader.get<uint8_t>();

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

	for (uint8_t i = 0; i < movementCount; ++i) {
		int8_t type = reader.get<int8_t>();
		switch (type) {
			case Falling:
				reader.unk<uint8_t>();
				break;
			case Wings:
				reader.unk<uint8_t>();
				reader.unk<uint16_t>();
				reader.unk<uint32_t>();
				break;
			case WingsFalling:
				x = reader.get<coord_t>();
				y = reader.get<coord_t>();
				foothold = reader.get<foothold_id_t>();
				stance = reader.get<int8_t>();
				reader.unk<uint16_t>();
				reader.unk<uint16_t>();
				reader.unk<uint16_t>();
				break;
			case ExcessiveKb:
				reader.unk<uint8_t>();
				reader.unk<uint16_t>();
				reader.unk<uint32_t>();
				break;
			case Unk2:
				reader.unk<uint8_t>();
				reader.unk<uint32_t>();
				reader.unk<uint32_t>();
				break;
			case NormalMovement:
			case NormalMovement2:
				x = reader.get<coord_t>();
				y = reader.get<coord_t>();
				reader.unk<uint32_t>();
				foothold = reader.get<foothold_id_t>();
				stance = reader.get<int8_t>();
				reader.unk<uint16_t>();
				break;
			case Jump:
			case JumpKb:
			case FlashJump:
			case RecoilShot:
				x = reader.get<coord_t>();
				y = reader.get<coord_t>();
				stance = reader.get<int8_t>();
				foothold = reader.get<foothold_id_t>();
				break;
			case JumpDown:
				x = reader.get<coord_t>();
				y = reader.get<coord_t>();
				reader.unk<uint16_t>();
				reader.unk<uint32_t>();
				foothold = reader.get<foothold_id_t>();
				stance = reader.get<int8_t>();
				reader.unk<uint16_t>();
				break;
			case Chair:
				x = reader.get<coord_t>();
				y = reader.get<coord_t>();
				foothold = reader.get<foothold_id_t>();
				stance = reader.get<int8_t>();
				reader.unk<uint16_t>();
				break;
			case Unk1:
			case Teleport:
			case Assaulter:
			case Assassinate:
			case Rush:
				x = reader.get<coord_t>();
				y = reader.get<coord_t>();
				reader.unk<uint32_t>();
				stance = reader.get<int8_t>();
				break;
			default:
				std::cerr << "New type of movement: 0x" << std::hex << static_cast<int16_t>(type) << std::endl;
				break;
		}
	}

	Point pos(x, y);
	life->resetMovement(foothold, pos, stance);
	return pos;
}