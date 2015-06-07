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
#pragma once

#include "Types.hpp"
#include <unordered_map>

class PacketBuilder;
class Pet;
class Player;

class PlayerPets {
	NONCOPYABLE(PlayerPets);
	NO_DEFAULT_CONSTRUCTOR(PlayerPets);
public:
	PlayerPets(Player *player) : m_player(player) { }

	auto save() -> void;
	auto petInfoPacket(PacketBuilder &packet) -> void;
	auto connectData(PacketBuilder &packet) -> void;

	auto getPet(pet_id_t petId) -> Pet *;
	auto getSummoned(int8_t index) -> Pet *;

	auto addPet(Pet *pet) -> void;
	auto setSummoned(int8_t index, pet_id_t petId) -> void;
private:
	Player *m_player = nullptr;
	hash_map_t<pet_id_t, Pet *> m_pets;
	hash_map_t<int8_t, pet_id_t> m_summoned;
};