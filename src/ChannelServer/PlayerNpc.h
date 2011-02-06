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

#include "ItemConstants.h"
#include "PacketCreator.h"
#include "Player.h"
#include "Types.h"
#include <boost/array.hpp>
#include <string>

using std::string;

class PacketCreator;
class Player;

class PlayerNpc {
public:
	PlayerNpc(int32_t npcId, int32_t charid, uint8_t level, int8_t gender, int8_t skin, int32_t eyes, int32_t hair);

	void makePacket(PacketCreator &packet);
	void makePacket(Player *player);

	string getName() const { return name; }
	uint8_t getLevel() const { return level; }
	int32_t getCharacterId() const { return characterId; }

	void renewData(Player *player);
	void save();
private:
	void loadEquips();

	boost::array<boost::array<int32_t, 2>, Inventories::EquippedSlots> m_equipped;

	int8_t level;
	int8_t skin;
	int8_t gender;
	int32_t eyes;
	int32_t hair;
	int32_t characterId;
	int32_t npcId;
	string name;
};

