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

#include "Types.hpp"
#include <string>

class Item;
class PacketCreator;
class Pet;
class Player;

namespace PetsPacket {
	auto petSummoned(Player *player, Pet *pet, bool kick = false, bool onlyPlayer = false, int8_t index = -1) -> void;
	auto showChat(Player *player, Pet *pet, const string_t &message, int8_t act) -> void;
	auto showMovement(Player *player, Pet *pet, unsigned char *buf, int32_t bufLen) -> void;
	auto showAnimation(Player *player, Pet *pet, int8_t animation) -> void;
	auto updatePet(Player *player, Pet *pet, Item *petItem) -> void;
	auto levelUp(Player *player, Pet *pet) -> void;
	auto changeName(Player *player, Pet *pet) -> void;
	auto showPet(Player *player, Pet *pet) -> void;
	auto updateSummonedPets(Player *player) -> void;
	auto blankUpdate(Player *player) -> void;
	auto addInfo(PacketCreator &packet, Pet *pet, Item *petItem) -> void;
}