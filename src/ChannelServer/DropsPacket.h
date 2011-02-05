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
#pragma once

#include "Types.h"

class Player;
class Drop;
class Pet;
struct Pos;

namespace DropsPacket {
	namespace DropTypes {
		enum DropTypes {
			ShowDrop = 0,
			DropAnimation = 1,
			ShowExisting = 2,
			DisappearDuringDrop = 3
		};
	}
	void showDrop(Player *player, Drop *drop, int8_t type, bool newdrop, const Pos &origin);
	void takeDrop(Player *player, Drop *drop, int8_t pet_index = -1);
	void dontTake(Player *player);
	void removeDrop(Drop *drop);
	void explodeDrop(Drop *drop);
	void dropNotAvailableForPickup(Player *player);
	void cantGetAnymoreItems(Player *player);
	void pickupDrop(Player *player, int32_t id, int32_t amount, bool isMesos = false, int16_t cafeBonus = 0);
	void pickupDropSpecial(Player *player, int32_t id);
};
