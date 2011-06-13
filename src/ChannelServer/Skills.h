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

class PacketReader;
class Party;
class Player;
struct MobSkillLevelInfo;
struct SkillLevelInfo;

namespace Skills {
	void addSkillLevelInfo(int32_t skillId, uint8_t level, const SkillLevelInfo &levelInfo);
	void addMobSkillLevelInfo(uint8_t skillId, uint8_t level, const MobSkillLevelInfo &levelInfo);
	void addSkill(Player *player, PacketReader &packet);
	void cancelSkill(Player *player, PacketReader &packet);
	void useSkill(Player *player, PacketReader &packet);
	const vector<Player *> getAffectedPartyMembers(Party *party, int8_t affected, int8_t members);
	void applySkillCosts(Player *player, int32_t skillId, uint8_t level, bool elementalAmp = false);
	void useAttackSkill(Player *player, int32_t skillId);
	void useAttackSkillRanged(Player *player, int32_t skillId, int16_t pos);
	void heal(Player *player, int16_t value, int32_t skillId);
	void hurt(Player *player, int16_t value, int32_t skillId);
	void stopSkill(Player *player, int32_t skillId, bool fromTimer = false);
	void startCooldown(Player *player, int32_t skillId, int16_t coolTime, bool initialLoad = false);
	void stopCooldown(Player *player, int32_t skillId);
	bool isCooling(Player *player, int32_t skillId);
	int16_t getCooldownTimeLeft(Player *player, int32_t skillId);
}