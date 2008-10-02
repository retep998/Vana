/*
Copyright (C) 2008 Vana Development Team

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
#ifndef SUMMONS_H
#define SUMMONS_H

#define ISSUMMON(skillid) (skillid == 3111002 || skillid == 3211002 || skillid == 2311006 || skillid == 3111005 || skillid == 3211005 || skillid == 1321007 || skillid == 2121005 || skillid == 2221005 || skillid == 2321003 || skillid == 3121006 || skillid == 3221005 || skillid == 5211001 || skillid == 5211002)
#define ISPUPPET(skillid) (skillid == 3111002 || skillid == 3211002)

#include "Player.h"
class Summon;
class ReadPacket;
class LoopingId;

namespace Summons {
	extern LoopingId summonids;
	void useSummon(Player *player, int32_t skillid, uint8_t level);
	void removeSummon(Player *player, bool puppet, bool animated, bool packetOnly, bool fromTimer, bool showMessage = false);
	void showSummon(Player *player);
	void showSummons(Player *ofplayer, Player *toplayer);
	void moveSummon(Player *Player, ReadPacket *packet);
	void damageSummon(Player *player, ReadPacket *packet);
};

class Summon : public MovableLife {
public:
	Summon() { }
	Summon(int32_t id, int32_t summonid, uint8_t level);
	int32_t getID() {
		return id;
	}
	int32_t getSummonID() {
		return summonid;
	}
	uint8_t getLevel() {
		return level;
	}
	uint8_t getType() {
		return type;
	}
	int32_t getHP() {
		return hp;
	}
	void doDamage(int32_t damage) {
		hp -= damage;
	}
private:
	int32_t id;
	int32_t summonid;
	uint8_t level;
	uint8_t type;
	int32_t hp; // For puppet
};

#endif
