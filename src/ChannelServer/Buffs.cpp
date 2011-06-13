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
#include "Buffs.h"
#include "BuffDataProvider.h"
#include "BuffsPacket.h"
#include "GameLogicUtilities.h"
#include "Player.h"
#include "SkillConstants.h"
#include "SkillDataProvider.h"

int16_t Buffs::getValue(int8_t value, int32_t skillId, uint8_t level) {
	int16_t rvalue = 0;
	SkillLevelInfo *skill = SkillDataProvider::Instance()->getSkill(skillId, level);
	switch (value) {
		case SkillX: rvalue = skill->x; break;
		case SkillY: rvalue = skill->y; break;
		case SkillSpeed: rvalue = skill->speed; break;
		case SkillJump: rvalue = skill->jump; break;
		case SkillWatk: rvalue = skill->wAtk; break;
		case SkillWdef: rvalue = skill->wDef; break;
		case SkillMatk: rvalue = skill->mAtk; break;
		case SkillMdef: rvalue = skill->mDef; break;
		case SkillAcc: rvalue = skill->acc; break;
		case SkillAvo: rvalue = skill->avo; break;
		case SkillProp: rvalue = skill->prop; break;
		case SkillMorph: rvalue = skill->morph; break;
		case SkillLv: rvalue = level; break;
	}
	return rvalue;
}

int16_t Buffs::getMobSkillValue(int8_t value, uint8_t skillId, uint8_t level) {
	int16_t rvalue = 0;
	MobSkillLevelInfo *skill = SkillDataProvider::Instance()->getMobSkill(skillId, level);
	switch (value) {
		case SkillX: rvalue = static_cast<int16_t>(skill->x); break;
		case SkillY: rvalue = static_cast<int16_t>(skill->y); break;
		case SkillProp: rvalue = skill->prop; break;
		case SkillLv: rvalue = level; break;
	}
	return rvalue;
}

int32_t Buffs::parseMountInfo(Player *player, int32_t skillId, uint8_t level) {
	int32_t mountId = 0;
	switch (skillId) {
		case Jobs::Beginner::MonsterRider:
		case Jobs::Noblesse::MonsterRider:
			mountId = player->getInventory()->getEquippedId(EquipSlots::Mount);
			break;
		case Jobs::Corsair::Battleship:
			mountId = Items::BattleshipMount;
			break;
	}
	return mountId;
}

ActiveBuff Buffs::parseBuffInfo(Player *player, int32_t skillId, uint8_t level) {
	ActiveBuff playerSkill;
	BuffInfo cur;
	SkillLevelInfo *skill = SkillDataProvider::Instance()->getSkill(skillId, level);
	SkillInfo *skillsInfo = BuffDataProvider::Instance()->getSkillInfo(skillId);

	for (size_t i = 0; i < skillsInfo->player.size(); i++) {
		cur = skillsInfo->player[i];
		int8_t val = cur.buff.value;
		if (GameLogicUtilities::isMaxDarkSight(skillId, level) && val == SkillSpeed) {
			// Cancel speed change for maxed dark sight
			continue;
		}
		playerSkill.types[cur.buff.byte] += cur.buff.type;
		int16_t value = 0;
		if (cur.hasMapVal)
			playerSkill.hasMapBuff = true;
		if (val == SkillNone)
			value = cur.itemVal;
		else {
			switch (skillId) {
				case Jobs::Bowmaster::SharpEyes:
				case Jobs::Marksman::SharpEyes:
				case Jobs::Hermit::ShadowPartner:
				case Jobs::NightWalker::ShadowPartner:
					value = skill->x * 256 + skill->y;
					break;
				case Jobs::Crusader::ComboAttack:
				case Jobs::DawnWarrior::ComboAttack:
					value = player->getActiveBuffs()->getCombo() + 1;
					break;
				case Jobs::NightLord::ShadowStars:
					value = (player->getInventory()->doShadowStars() % 10000) + 1;
					break;
				case Jobs::Marauder::Transformation:
				case Jobs::Buccaneer::SuperTransformation:
				case Jobs::WindArcher::EagleEye:
				case Jobs::ThunderBreaker::Transformation:
					value = getValue(val, skillId, level);
					if (val == SkillMorph)
						value += (player->getGender() * 100); // Females are +100
					break;
				case Jobs::Marauder::EnergyCharge:
				case Jobs::ThunderBreaker::EnergyCharge:
					value = player->getActiveBuffs()->getEnergyChargeLevel();
					break;
				default:
					value = getValue(val, skillId, level);
					break;
			}
		}
		playerSkill.vals.push_back(value);
	}
	return playerSkill;
}

ActiveMapBuff Buffs::parseBuffMapInfo(Player *player, int32_t skillId, uint8_t level) {
	ActiveMapBuff mapSkill;
	BuffInfo cur;
	BuffMapInfo map;
	int32_t maps = 0;
	SkillInfo *skillsInfo = BuffDataProvider::Instance()->getSkillInfo(skillId);

	for (size_t i = 0; i < skillsInfo->player.size(); i++) {
		cur = skillsInfo->player[i];
		if (!cur.hasMapVal)
			continue;
		map = skillsInfo->map[maps++];
		int8_t val = map.buff.value;
		if (GameLogicUtilities::isMaxDarkSight(skillId, level) && val == SkillSpeed) { // Cancel speed update for maxed dark sight
			continue;
		}
		mapSkill.bytes.push_back(map.buff.byte);
		mapSkill.types.push_back(map.buff.type);
		mapSkill.typeList[map.buff.byte] += map.buff.type;
		mapSkill.useVals.push_back(map.useVal);
		if (map.useVal) {
			int16_t value = 0;
			if (val == SkillNone) {
				value = cur.itemVal;
			}
			else {
				switch (skillId) {
					case Jobs::Crusader::ComboAttack:
					case Jobs::DawnWarrior::ComboAttack:
						value = player->getActiveBuffs()->getCombo() + 1;
						break;
					default:
						value = getValue(val, skillId, level);
						break;
				}
			}
			mapSkill.values.push_back(value);
		}
	}
	return mapSkill;
}

ActiveMapBuff Buffs::parseBuffMapEntryInfo(Player *player, int32_t skillId, uint8_t level) {
	ActiveMapBuff mapSkill;
	BuffInfo cur;
	BuffMapInfo map;
	int8_t mapCounter = 0;
	SkillInfo *skillsInfo = BuffDataProvider::Instance()->getSkillInfo(skillId);

	for (size_t i = 0; i < skillsInfo->player.size(); i++) {
		cur = skillsInfo->player[i];
		if (!cur.hasmapentry) {
			continue;
		}
		map = skillsInfo->map[mapCounter++];
		int8_t val = map.buff.value;
		if (GameLogicUtilities::isMaxDarkSight(skillId, level) && val == SkillSpeed) {
			// Cancel speed update for maxed dark sight
			continue;
		}
		mapSkill.bytes.push_back(map.buff.byte);
		mapSkill.types.push_back(map.buff.type);
		mapSkill.typeList[map.buff.byte] += map.buff.type;
		mapSkill.useVals.push_back(map.useVal);
		if (map.useVal) {
			int16_t value = 0;
			if (val == SkillNone) {
				value = cur.itemVal;
			}
			else {
				switch (skillId) {
					case Jobs::Crusader::ComboAttack:
					case Jobs::DawnWarrior::ComboAttack:
						value = player->getActiveBuffs()->getCombo() + 1;
						break;
					default:
						value = getValue(val, skillId, level);
						break;
				}
			}
			mapSkill.values.push_back(value);
		}
	}
	return mapSkill;
}

vector<Buff> Buffs::parseBuffs(int32_t skillId, uint8_t level) {
	vector<Buff> ret;
	SkillInfo *skillsInfo = BuffDataProvider::Instance()->getSkillInfo(skillId);

	for (size_t i = 0; i < skillsInfo->player.size(); i++) {
		BuffInfo cur = skillsInfo->player[i];
		if (GameLogicUtilities::isMaxDarkSight(skillId, level) && cur.buff.value == SkillSpeed) { // Cancel speed update for maxed dark sight
			continue;
		}
		ret.push_back(cur.buff);
	}
	return ret;
}

ActiveBuff Buffs::parseMobBuffInfo(Player *player, uint8_t skillId, uint8_t level) {
	ActiveBuff playerSkill;
	BuffInfo cur;
	MobAilmentInfo *mobSkillsInfo = BuffDataProvider::Instance()->getMobSkillInfo(skillId);

	for (size_t i = 0; i < mobSkillsInfo->mob.size(); i++) {
		cur = mobSkillsInfo->mob[i];
		int8_t val = cur.buff.value;
		playerSkill.types[cur.buff.byte] += cur.buff.type;
		playerSkill.hasMapBuff = true;
		int16_t value = (val == SkillNone ? 1 : getMobSkillValue(val, skillId, level));
		playerSkill.vals.push_back(value);
	}
	return playerSkill;
}

ActiveMapBuff Buffs::parseMobBuffMapInfo(Player *player, uint8_t skillId, uint8_t level) {
	ActiveMapBuff mapSkill;
	BuffInfo cur;
	MobAilmentInfo *mobSkillsInfo = BuffDataProvider::Instance()->getMobSkillInfo(skillId);

	for (size_t i = 0; i < mobSkillsInfo->mob.size(); i++) {
		cur = mobSkillsInfo->mob[i];
		int8_t val = cur.buff.value;
		mapSkill.bytes.push_back(cur.buff.byte);
		mapSkill.types.push_back(cur.buff.type);
		mapSkill.typeList[cur.buff.byte] += cur.buff.type;
		mapSkill.useVals.push_back(cur.useVal);
		int16_t value = (val == SkillNone ? 1 : getMobSkillValue(val, skillId, level));
		mapSkill.values.push_back(value);
	}
	return mapSkill;
}

ActiveMapBuff Buffs::parseMobBuffMapEntryInfo(Player *player, uint8_t skillId, uint8_t level) {
	ActiveMapBuff mapSkill;
	BuffInfo cur;
	MobAilmentInfo *mobSkillsInfo = BuffDataProvider::Instance()->getMobSkillInfo(skillId);

	for (size_t i = 0; i < mobSkillsInfo->mob.size(); i++) {
		cur = mobSkillsInfo->mob[i];
		int8_t val = cur.buff.value;
		mapSkill.bytes.push_back(cur.buff.byte);
		mapSkill.types.push_back(cur.buff.type);
		mapSkill.typeList[cur.buff.byte] += cur.buff.type;
		mapSkill.useVals.push_back(true);
		mapSkill.useVals.push_back(false);
		mapSkill.values.push_back(skillId);
		mapSkill.values.push_back(level);
	}
	mapSkill.debuff = true;
	return mapSkill;
}

vector<Buff> Buffs::parseMobBuffs(uint8_t skillId) {
	vector<Buff> ret;
	MobAilmentInfo mobSkillsInfo = *BuffDataProvider::Instance()->getMobSkillInfo(skillId);

	for (size_t i = 0; i < mobSkillsInfo.mob.size(); i++) {
		BuffInfo cur = mobSkillsInfo.mob[i];
		ret.push_back(cur.buff);
	}
	return ret;
}

bool Buffs::addBuff(Player *player, int32_t skillId, uint8_t level, int16_t addedInfo, int32_t mapMobId) {
	if (!BuffDataProvider::Instance()->isBuff(skillId))
		return false; // Not a buff, so return false

	int32_t mountId = parseMountInfo(player, skillId, level);
	SkillLevelInfo *skill = SkillDataProvider::Instance()->getSkill(skillId, level);
	int32_t time = skill->time;
	switch (skillId) {
		case Jobs::DragonKnight::DragonRoar:
			time = skill->y;
			break;
		case Jobs::Beginner::MonsterRider:
		case Jobs::Noblesse::MonsterRider:
		case Jobs::Corsair::Battleship:
			if (mountId == 0) {
				// Hacking
				return true;
			}
			player->getActiveBuffs()->setMountInfo(skillId, mountId);
			break;
		case Jobs::SuperGm::Hide:
			time = 2100000; // Make sure that it doesn't end any time soon
			break;
		case Jobs::Spearman::HyperBody:
		case Jobs::SuperGm::HyperBody:
			player->getStats()->setHyperBody(skill->x, skill->y);
			break;
		case Jobs::Crusader::ComboAttack:
		case Jobs::DawnWarrior::ComboAttack:
			player->getActiveBuffs()->setCombo(0, false);
			break;
		case Jobs::Hero::Enrage:
			if (player->getActiveBuffs()->getCombo() != 10)
				return true;
			player->getActiveBuffs()->setCombo(0, true);
			break;
		case Jobs::Fighter::SwordBooster:
		case Jobs::Fighter::AxeBooster:
		case Jobs::Page::SwordBooster:
		case Jobs::Page::BwBooster:
		case Jobs::Spearman::SpearBooster:
		case Jobs::Spearman::PolearmBooster:
		case Jobs::FPMage::SpellBooster:
		case Jobs::ILMage::SpellBooster:
		case Jobs::Hunter::BowBooster:
		case Jobs::Crossbowman::CrossbowBooster:
		case Jobs::Assassin::ClawBooster:
		case Jobs::Bandit::DaggerBooster:
		case Jobs::Brawler::KnucklerBooster:
		case Jobs::Gunslinger::GunBooster:
		case Jobs::DawnWarrior::SwordBooster:
		case Jobs::BlazeWizard::SpellBooster:
		case Jobs::WindArcher::BowBooster:
		case Jobs::NightWalker::ClawBooster:
		case Jobs::ThunderBreaker::KnucklerBooster:
			player->getActiveBuffs()->setBooster(skillId); // Makes switching equips MUCH easier
			break;
		case Jobs::WhiteKnight::BwFireCharge:
		case Jobs::WhiteKnight::BwIceCharge:
		case Jobs::WhiteKnight::BwLitCharge:
		case Jobs::WhiteKnight::SwordFireCharge:
		case Jobs::WhiteKnight::SwordIceCharge:
		case Jobs::WhiteKnight::SwordLitCharge:
		case Jobs::Paladin::BwHolyCharge:
		case Jobs::Paladin::SwordHolyCharge:
		case Jobs::DawnWarrior::SoulCharge:
		case Jobs::ThunderBreaker::LightningCharge:
			player->getActiveBuffs()->setCharge(skillId); // Makes switching equips/Charged Blow easier
			break;
		case Jobs::Hero::MapleWarrior:
		case Jobs::Paladin::MapleWarrior:
		case Jobs::DarkKnight::MapleWarrior:
		case Jobs::FPArchMage::MapleWarrior:
		case Jobs::ILArchMage::MapleWarrior:
		case Jobs::Bishop::MapleWarrior:
		case Jobs::Bowmaster::MapleWarrior:
		case Jobs::Marksman::MapleWarrior:
		case Jobs::NightLord::MapleWarrior:
		case Jobs::Shadower::MapleWarrior:
		case Jobs::Buccaneer::MapleWarrior:
		case Jobs::Corsair::MapleWarrior:
			player->getStats()->setMapleWarrior(skill->x); // Take into account Maple Warrior for tracking stats if things are equippable, damage calculations, or w/e else
			break;
	}
	vector<Buff> buffs = parseBuffs(skillId, level);
	ActiveBuff playerSkill = parseBuffInfo(player, skillId, level);
	ActiveMapBuff mapSkill = parseBuffMapInfo(player, skillId, level);
	ActiveMapBuff enterskill = parseBuffMapEntryInfo(player, skillId, level);

	if (mountId > 0) {
		BuffsPacket::useMount(player, skillId, time, playerSkill, mapSkill, addedInfo, mountId);
	}
	else {
		switch (skillId) {
			case Jobs::Pirate::Dash:
			case Jobs::ThunderBreaker::Dash:
				BuffsPacket::usePirateBuff(player, skillId, time, playerSkill, mapSkill);
				break;
			case Jobs::Marauder::EnergyCharge:
			case Jobs::ThunderBreaker::EnergyCharge:
				BuffsPacket::usePirateBuff(player, 0, (player->getActiveBuffs()->getEnergyChargeLevel() == Stats::MaxEnergyChargeLevel ? time : 0), playerSkill, mapSkill);
				break;
			case Jobs::Buccaneer::SpeedInfusion:
			case Jobs::ThunderBreaker::SpeedInfusion:
				BuffsPacket::useSpeedInfusion(player, skillId, time, playerSkill, mapSkill, addedInfo);
				break;
			case Jobs::Outlaw::HomingBeacon:
			case Jobs::Corsair::Bullseye:
				if (player->getActiveBuffs()->hasMarkedMonster()) // Otherwise the animation appears above numerous
					BuffsPacket::endSkill(player, playerSkill);
				player->getActiveBuffs()->setMarkedMonster(mapMobId);
				BuffsPacket::useHomingBeacon(player, skillId, playerSkill, mapMobId);
				break;
			default:
				BuffsPacket::useSkill(player, skillId, time, playerSkill, mapSkill, addedInfo);
		}
	}
	if (skillId != player->getSkills()->getEnergyCharge() || player->getActiveBuffs()->getEnergyChargeLevel() == Stats::MaxEnergyChargeLevel) {
		PlayerActiveBuffs *playerBuffs = player->getActiveBuffs();
		playerBuffs->addBuffInfo(skillId, buffs);
		playerBuffs->addMapEntryBuffInfo(enterskill);
		playerBuffs->setActiveSkillLevel(skillId, level);
		playerBuffs->removeBuff(skillId);
		playerBuffs->addBuff(skillId, time);
		doAct(player, skillId, level);
	}
	return true;
}

void Buffs::addBuff(Player *player, int32_t itemId, int32_t time) {
	itemId *= -1; // Make the Item ID negative for the packet and to discern from skill buffs
	vector<Buff> buffs = parseBuffs(itemId, 0);
	ActiveBuff playerSkill = parseBuffInfo(player, itemId, 0);
	ActiveMapBuff mapSkill = parseBuffMapInfo(player, itemId, 0);
	ActiveMapBuff meskill = parseBuffMapEntryInfo(player, itemId, 0);

	BuffsPacket::useSkill(player, itemId, time, playerSkill, mapSkill, 0);

	PlayerActiveBuffs *playerBuffs = player->getActiveBuffs();
	playerBuffs->removeBuff(itemId);
	playerBuffs->addBuffInfo(itemId, buffs);
	playerBuffs->addBuff(itemId, time);
	playerBuffs->addMapEntryBuffInfo(meskill);
	playerBuffs->setActiveSkillLevel(itemId, 1);
}

void Buffs::endBuff(Player *player, int32_t skill) {
	PlayerActiveBuffs *playerBuffs = player->getActiveBuffs();
	switch (skill) {
		case Jobs::Beginner::MonsterRider:
		case Jobs::Noblesse::MonsterRider:
		case Jobs::Corsair::Battleship:
			playerBuffs->setMountInfo(0, 0);
			break;
		case Jobs::Crusader::ComboAttack:
		case Jobs::DawnWarrior::ComboAttack:
			playerBuffs->setCombo(0, false);
			break;
		case Jobs::Spearman::HyperBody:
		case Jobs::SuperGm::HyperBody:
			player->getStats()->setHyperBody(0, 0);
			player->getStats()->setHp(player->getStats()->getHp());
			player->getStats()->setMp(player->getStats()->getMp());
			break;
		case Jobs::Marauder::EnergyCharge:
		case Jobs::ThunderBreaker::EnergyCharge:
			playerBuffs->resetEnergyChargeLevel();
			break;
		case Jobs::Fighter::SwordBooster:
		case Jobs::Fighter::AxeBooster:
		case Jobs::Page::SwordBooster:
		case Jobs::Page::BwBooster:
		case Jobs::Spearman::SpearBooster:
		case Jobs::Spearman::PolearmBooster:
		case Jobs::FPMage::SpellBooster:
		case Jobs::ILMage::SpellBooster:
		case Jobs::Hunter::BowBooster:
		case Jobs::Crossbowman::CrossbowBooster:
		case Jobs::Assassin::ClawBooster:
		case Jobs::Bandit::DaggerBooster:
		case Jobs::Brawler::KnucklerBooster:
		case Jobs::Gunslinger::GunBooster:
		case Jobs::DawnWarrior::SwordBooster:
		case Jobs::BlazeWizard::SpellBooster:
		case Jobs::WindArcher::BowBooster:
		case Jobs::NightWalker::ClawBooster:
		case Jobs::ThunderBreaker::KnucklerBooster:
			playerBuffs->setBooster(0);
			break;
		case Jobs::WhiteKnight::BwFireCharge:
		case Jobs::WhiteKnight::BwIceCharge:
		case Jobs::WhiteKnight::BwLitCharge:
		case Jobs::WhiteKnight::SwordFireCharge:
		case Jobs::WhiteKnight::SwordIceCharge:
		case Jobs::WhiteKnight::SwordLitCharge:
		case Jobs::Paladin::BwHolyCharge:
		case Jobs::Paladin::SwordHolyCharge:
		case Jobs::DawnWarrior::SoulCharge:
		case Jobs::ThunderBreaker::LightningCharge:
			playerBuffs->setCharge(0);
			break;
		case Jobs::Outlaw::HomingBeacon:
		case Jobs::Corsair::Bullseye:
			playerBuffs->setMarkedMonster(0);
			break;
		case Jobs::Hero::MapleWarrior:
		case Jobs::Paladin::MapleWarrior:
		case Jobs::DarkKnight::MapleWarrior:
		case Jobs::FPArchMage::MapleWarrior:
		case Jobs::ILArchMage::MapleWarrior:
		case Jobs::Bishop::MapleWarrior:
		case Jobs::Bowmaster::MapleWarrior:
		case Jobs::Marksman::MapleWarrior:
		case Jobs::NightLord::MapleWarrior:
		case Jobs::Shadower::MapleWarrior:
		case Jobs::Buccaneer::MapleWarrior:
		case Jobs::Corsair::MapleWarrior:
			player->getStats()->setMapleWarrior(0);
			break;
	}
	uint8_t level = playerBuffs->getActiveSkillLevel(skill);
	vector<Buff> buffs = parseBuffs(skill, level);
	ActiveMapBuff meskill = parseBuffMapEntryInfo(player, skill, level);
	ActiveBuff playerSkill = playerBuffs->removeBuffInfo(skill, buffs);

	BuffsPacket::endSkill(player, playerSkill);

	playerBuffs->deleteMapEntryBuffInfo(meskill);
	playerBuffs->setActiveSkillLevel(skill, 0);
}

void Buffs::doAct(Player *player, int32_t skillId, uint8_t level) {
	SkillInfo *skillsInfo = BuffDataProvider::Instance()->getSkillInfo(skillId);

	if (skillsInfo->bact) {
		int16_t value = getValue(skillsInfo->act.value, skillId, level);
		player->getActiveBuffs()->addAct(skillId, skillsInfo->act.type, value, skillsInfo->act.time);
	}
}

void Buffs::addDebuff(Player *player, uint8_t skillId, uint8_t level) {
	if (!BuffDataProvider::Instance()->isDebuff(skillId))
		return;

	int16_t time = SkillDataProvider::Instance()->getMobSkill(skillId, level)->time;
	MobAilmentInfo *mobSkillsInfo = BuffDataProvider::Instance()->getMobSkillInfo(skillId);

	vector<Buff> buffs = parseMobBuffs(skillId);
	ActiveBuff playerSkill = parseMobBuffInfo(player, skillId, level);
	ActiveMapBuff mapSkill = parseMobBuffMapInfo(player, skillId, level);
	ActiveMapBuff enterskill = parseMobBuffMapEntryInfo(player, skillId, level);

	BuffsPacket::giveDebuff(player, skillId, level, time, mobSkillsInfo->delay, playerSkill, mapSkill);

	PlayerActiveBuffs *playerBuffs = player->getActiveBuffs();
	playerBuffs->setActiveSkillLevel(skillId, level);
	playerBuffs->addBuffInfo(skillId, buffs);
	playerBuffs->addMapEntryBuffInfo(enterskill);
	playerBuffs->addBuff(skillId, time);
}

void Buffs::endDebuff(Player *player, uint8_t skill) {
	PlayerActiveBuffs *playerBuffs = player->getActiveBuffs();
	vector<Buff> buffs = parseMobBuffs(skill);
	ActiveMapBuff meskill = parseMobBuffMapEntryInfo(player, skill, 1);
	ActiveBuff playerSkill = playerBuffs->removeBuffInfo(skill, buffs);

	BuffsPacket::endDebuff(player, playerSkill);

	playerBuffs->deleteMapEntryBuffInfo(meskill);
	playerBuffs->setActiveSkillLevel(skill, 0);
}