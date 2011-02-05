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

class Player;
class PacketCreator;
struct SkillLevelInfo;

struct PlayerSkillInfo {
	PlayerSkillInfo() : level(0), maxlevel(0) {}
	uint8_t level;
	uint8_t maxlevel;
};

class PlayerSkills {
public:
	PlayerSkills(Player *player) : player(player) { load(); }

	void load();
	void save(bool savecooldowns = false);
	void connectData(PacketCreator &packet);

	bool addSkillLevel(int32_t skillid, uint8_t amount, bool sendpacket = true);
	uint8_t getSkillLevel(int32_t skillid);
	uint8_t getMaxSkillLevel(int32_t skillid);
	void setMaxSkillLevel(int32_t skillid, uint8_t maxlevel, bool sendpacket = true);
	SkillLevelInfo * getSkillInfo(int32_t skillid);

	bool hasElementalAmp();
	bool hasEnergyCharge();
	bool hasHpIncrease();
	bool hasMpIncrease();
	bool hasVenomousWeapon();
	bool hasAchilles();
	bool hasNoDamageSkill();
	int32_t getElementalAmp();
	int32_t getEnergyCharge();
	int32_t getComboAttack();
	int32_t getAdvancedCombo();
	int32_t getAlchemist();
	int32_t getHpIncrease();
	int32_t getMpIncrease();
	int32_t getMastery();
	int32_t getMpEater();
	int32_t getVenomousWeapon();
	int32_t getAchilles();
	int32_t getNoDamageSkill();
	int16_t getRechargeableBonus();

	void addCooldown(int32_t skillid, int16_t time);
	void removeCooldown(int32_t skillid);
	void removeAllCooldowns();
private:
	unordered_map<int32_t, PlayerSkillInfo> playerskills;
	unordered_map<int32_t, int16_t> cooldowns;
	Player *player;
};
