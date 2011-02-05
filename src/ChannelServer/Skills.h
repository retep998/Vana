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
#include <vector>

using std::vector;

class Party;
class Player;
class PacketReader;
struct SkillLevelInfo;
struct MobSkillLevelInfo;

namespace Skills {
	void addSkillLevelInfo(int32_t skillid, uint8_t level, const SkillLevelInfo &levelinfo);
	void addMobSkillLevelInfo(uint8_t skillid, uint8_t level, const MobSkillLevelInfo &levelinfo);
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
