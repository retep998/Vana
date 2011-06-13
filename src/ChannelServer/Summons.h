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

#include "Player.h"
#include <string>

using std::string;

class PacketReader;
class Summon;
template <typename T> class LoopingId;

namespace Summons {
	extern LoopingId<int32_t> summonIds;
	void useSummon(Player *player, int32_t skillId, uint8_t level);
	void removeSummon(Player *player, bool puppet, bool packetOnly, int8_t showMessage, bool fromTimer = false);
	void showSummon(Player *player);
	void showSummons(Player *fromPlayer, Player *toPlayer);
	void moveSummon(Player *Player, PacketReader &packet);
	void damageSummon(Player *player, PacketReader &packet);
	string getSummonName(int32_t summonId);
	int32_t loopId();
}

namespace SummonMessages {
	enum Messages : int8_t {
		OutOfTime = 0x01,
		Disappearing = 0x03,
		None = 0x04
	};
}

class Summon : public MovableLife {
public:
	enum MovementPatterns : int8_t {
		Static = 0,
		Follow = 1,
		Flying = 3
	};

	Summon() { }
	Summon(int32_t id, int32_t summonId, uint8_t level);

	int32_t getId() { return m_id; }
	int32_t getSummonId() { return m_summonId; }
	uint8_t getLevel() { return m_level; }
	uint8_t getType() { return m_type; }
	int32_t getHP() { return m_hp; }
	void doDamage(int32_t damage) { m_hp -= damage; }
private:
	int32_t m_id;
	int32_t m_summonId;
	uint8_t m_level;
	uint8_t m_type;
	int32_t m_hp; // For puppet
};