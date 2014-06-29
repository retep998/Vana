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
#pragma once

#include "PacketBuilder.hpp"
#include "Types.hpp"

class Drop;
class Pet;
class Player;
struct Point;

namespace DropsPacket {
	namespace DropTypes {
		enum DropTypes : int8_t {
			ShowDrop = 0,
			DropAnimation = 1,
			ShowExisting = 2,
			DisappearDuringDrop = 3
		};
	}

	PACKET(showDrop, Drop *drop, int8_t type, const Point &origin);
	PACKET(takeDrop, player_id_t playerId, map_object_t dropId, int8_t petIndex = -1);
	PACKET(dontTake);
	PACKET(removeDrop, map_object_t dropId);
	PACKET(explodeDrop, map_object_t dropId);
	PACKET(dropNotAvailableForPickup);
	PACKET(cantGetAnymoreItems);
	PACKET(pickupDrop, map_object_t id, int32_t amount, bool isMesos = false, int16_t cafeBonus = 0);
	PACKET(pickupDropSpecial, map_object_t id);
}