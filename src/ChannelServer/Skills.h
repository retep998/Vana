/*
Copyright (C) 2008-2009 Vana Development Team

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
#ifndef SKILLS_H
#define SKILLS_H

#include "Pos.h"
#include <boost/tr1/unordered_map.hpp>
#include <string>
#include <vector>

using std::string;
using std::tr1::unordered_map;
using std::vector;

class Party;
class Player;
class PacketReader;

struct SkillLevelInfo {
	int8_t mobcount;
	int8_t hitcount;
	int16_t mp;
	int16_t hp;
	int32_t item;
	int16_t itemcount;
	int16_t bulletcon;
	int16_t moneycon;
	int16_t x;
	int16_t y;
	int16_t speed;
	int16_t jump;
	int16_t watk;
	int16_t wdef;
	int16_t matk;
	int16_t mdef;
	int16_t acc;
	int16_t avo;
	int16_t cooltime;
	int16_t morph;
	int16_t damage;
	uint16_t hpP;
	uint16_t mpP;
	uint16_t prop;
	int32_t time;
	Pos lt;
	Pos rb;
};
typedef unordered_map<uint8_t, SkillLevelInfo> SkillsLevelInfo;

struct SpecialSkillInfo { // Hurricane, Big Bang, Monster Magnet, Pierce, etc.
	SpecialSkillInfo() : level(0), w_speed(0), direction(0),  skillid(0) { }
	uint8_t level;
	uint8_t w_speed;
	uint8_t direction;
	int32_t skillid;
};

struct PGMRInfo { // Power Guard/Mana Reflection
	PGMRInfo() : reduction(0), damage(0), mapmobid(0), isphysical(true), pos(0,0) { }
	uint8_t reduction;
	int32_t damage;
	int32_t mapmobid;
	bool isphysical;
	Pos pos;
};

struct MpEaterInfo { // MP Eater
	MpEaterInfo() : id(0), level(0), x(0), prop(0), onlyonce(false) { }
	int32_t id;
	uint8_t level;
	int16_t x;
	uint16_t prop;
	bool onlyonce;
};

struct MobSkillLevelInfo {
	int8_t summoneffect;
	uint8_t mp;
	uint8_t hp;
	uint8_t count;
	int16_t interval;
	int16_t prop;
	int16_t limit;
	int16_t time;
	int32_t x;
	int32_t y;
	Pos lt;
	Pos rb;
	vector<int32_t> summons;
};

typedef unordered_map<uint8_t, MobSkillLevelInfo> MobSkillsLevelInfo;

namespace Skills {
	extern unordered_map<uint8_t, MobSkillsLevelInfo> mobskills;
	extern unordered_map<int32_t, SkillsLevelInfo> skills;
	extern unordered_map<int32_t, uint8_t> maxlevels;
	void addSkillLevelInfo(int32_t skillid, uint8_t level, SkillLevelInfo levelinfo);
	void addMobSkillLevelInfo(uint8_t skillid, uint8_t level, MobSkillLevelInfo levelinfo);
	void addSkill(Player *player, PacketReader &packet);
	void cancelSkill(Player *player, PacketReader &packet);
	void useSkill(Player *player, PacketReader &packet);
	const vector<Player *> getAffectedPartyMembers(Party *party, int8_t affected, int8_t members);
	void applySkillCosts(Player *player, int32_t skillid, uint8_t level, bool elementalamp = false);
	void useAttackSkill(Player *player, int32_t skillid);
	void useAttackSkillRanged(Player *player, int32_t skillid, int16_t pos);
	void heal(Player *player, int16_t value, int32_t skillid);
	void hurt(Player *player, int16_t value, int32_t skillid);
	void stopSkill(Player *player, int32_t skillid, bool fromTimer = false);
	void startCooldown(Player *player, int32_t skillid, int16_t cooltime, bool initialload = false);
	void stopCooldown(Player *player, int32_t skillid);
	bool isCooling(Player *player, int32_t skillid);
	int16_t getCooldownTimeLeft(Player *player, int32_t skillid);
};

#endif
