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

class Summon;
class PacketReader;
class LoopingId;

namespace Summons {
	extern LoopingId summonids;
	void useSummon(Player *player, int32_t skillid, uint8_t level);
	void removeSummon(Player *player, bool puppet, bool packetOnly, int8_t showMessage, bool fromTimer = false);
	void showSummon(Player *player);
	void showSummons(Player *ofplayer, Player *toplayer);
	void moveSummon(Player *Player, PacketReader &packet);
	void damageSummon(Player *player, PacketReader &packet);
	int32_t loopId();
};

namespace SummonMessages {
	enum Messages {
		OutOfTime = 0x00,
		Disappearing = 0x03,
		None = 0x04
	};
}

class Summon : public MovableLife {
public:
	enum MovementPatterns {
		Static = 0,
		Follow = 1,
		Flying = 3
	};

	Summon() { }
	Summon(int32_t id, int32_t summonid, uint8_t level);

	int32_t getId() { return id; }
	int32_t getSummonId() { return summonid; }
	uint8_t getLevel() { return level; }
	uint8_t getType() { return type; }
	int32_t getHP() { return hp; }
	void doDamage(int32_t damage) { hp -= damage; }
private:
	int32_t id;
	int32_t summonid;
	uint8_t level;
	uint8_t type;
	int32_t hp; // For puppet
};
