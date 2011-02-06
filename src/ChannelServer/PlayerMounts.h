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
#include <boost/tr1/unordered_map.hpp>

using std::tr1::unordered_map;

class PacketCreator;
class Player;

struct MountData {
	int16_t exp;
	int8_t tiredness;
	int8_t level;
};

class PlayerMounts {
public:
	PlayerMounts(Player *p);

	void save();
	void load();

	void mountInfoPacket(PacketCreator &packet);
	int32_t getCurrentMount() const { return m_currentmount; }
	int16_t getCurrentExp();
	int8_t getCurrentLevel();
	int8_t getCurrentTiredness();
	void setCurrentMount(int32_t id) { m_currentmount = id; }
	void setCurrentExp(int16_t exp);
	void setCurrentLevel(int8_t level);
	void setCurrentTiredness(int8_t tiredness);

	void addMount(int32_t id);

	int16_t getMountExp(int32_t id);
	int8_t getMountLevel(int32_t id);
	int8_t getMountTiredness(int32_t id);
private:
	Player *m_player;
	unordered_map<int32_t, MountData> m_mounts;
	int32_t m_currentmount;
};
