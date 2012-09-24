/*
Copyright (C) 2008-2012 Vana Development Team

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
	int16_t rValue = 0;
	SkillLevelInfo *skill = SkillDataProvider::Instance()->getSkill(skillId, level);
	switch (value) {
		case SkillX: rValue = skill->x; break;
		case SkillY: rValue = skill->y; break;
		case SkillSpeed: rValue = skill->speed; break;
		case SkillJump: rValue = skill->jump; break;
		case SkillWatk: rValue = skill->wAtk; break;
		case SkillWdef: rValue = skill->wDef; break;
		case SkillMatk: rValue = skill->mAtk; break;
		case SkillMdef: rValue = skill->mDef; break;
		case SkillAcc: rValue = skill->acc; break;
		case SkillAvo: rValue = skill->avo; break;
		case SkillProp: rValue = skill->prop; break;
		case SkillMorph: rValue = skill->morph; break;
		case SkillLv: rValue = level; break;
	}
	return rValue;
}

int16_t Buffs::getMobSkillValue(int8_t value, uint8_t skillId, uint8_t level) {
	int16_t rValue = 0;
	MobSkillLevelInfo *skill = SkillDataProvider::Instance()->getMobSkill(skillId, level);
	switch (value) {
		case SkillX: rValue = static_cast<int16_t>(skill->x); break;
		case SkillY: rValue = static_cast<int16_t>(skill->y); break;
		case SkillProp: rValue = skill->prop; break;
		case SkillLv: rValue = level; break;
	}
	return rValue;
}

int32_t Buffs::parseMountInfo(Player *player, int32_t skillId, uint8_t level) {
	int32_t mountId = 0;
	switch (skillId) {
		case Skills::Beginner::MonsterRider:
		case Skills::Noblesse::MonsterRider:
			mountId = player->getInventory()->getEquippedId(EquipSlots::Mount);
			break;
		case Skills::Corsair::Battleship:
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
		if (!Buffs::buffMayApply(skillId, level, val)) {
			continue;
		}
		playerSkill.types[cur.buff.byte] += cur.buff.type;
		int16_t value = 0;
		if (cur.hasMapVal) {
			playerSkill.hasMapBuff = true;
		}
		if (val == SkillNone) {
			value = cur.itemVal;
		}
		else {
			switch (skillId) {
				case Skills::Bowmaster::SharpEyes:
				case Skills::Marksman::SharpEyes:
				case Skills::Hermit::ShadowPartner:
				case Skills::NightWalker::ShadowPartner:
					value = skill->x * 256 + skill->y;
					break;
				case Skills::Crusader::ComboAttack:
				case Skills::DawnWarrior::ComboAttack:
					value = player->getActiveBuffs()->getCombo() + 1;
					break;
				case Skills::NightLord::ShadowStars:
					value = (player->getInventory()->doShadowStars() % 10000) + 1;
					break;
				case Skills::Marauder::Transformation:
				case Skills::Buccaneer::SuperTransformation:
				case Skills::WindArcher::EagleEye:
				case Skills::ThunderBreaker::Transformation:
					value = getValue(val, skillId, level);
					if (val == SkillMorph) {
						value += (player->getGender() * 100); // Females are +100
					}
					break;
				case Skills::Marauder::EnergyCharge:
				case Skills::ThunderBreaker::EnergyCharge:
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
		if (!cur.hasMapVal) {
			continue;
		}
		map = skillsInfo->map[maps++];
		int8_t val = map.buff.value;
		if (!Buffs::buffMayApply(skillId, level, val)) {
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
					case Skills::Crusader::ComboAttack:
					case Skills::DawnWarrior::ComboAttack:
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
		if (!Buffs::buffMayApply(skillId, level, val)) {
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
					case Skills::Crusader::ComboAttack:
					case Skills::DawnWarrior::ComboAttack:
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
		if (!Buffs::buffMayApply(skillId, level, cur.buff.value)) {
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
		int16_t value = (val == SkillNone ? (skillId == MobSkills::Seal ? 2 : 1) : getMobSkillValue(val, skillId, level));
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
	if (!BuffDataProvider::Instance()->isBuff(skillId)) {
		return false;
	}

	int32_t mountId = parseMountInfo(player, skillId, level);
	SkillLevelInfo *skill = SkillDataProvider::Instance()->getSkill(skillId, level);
	int32_t time = skill->time;
	switch (skillId) {
		case Skills::DragonKnight::DragonRoar:
			time = skill->y;
			break;
		case Skills::Beginner::MonsterRider:
		case Skills::Noblesse::MonsterRider:
		case Skills::Corsair::Battleship:
			if (mountId == 0) {
				// Hacking
				return true;
			}
			player->getActiveBuffs()->setMountInfo(skillId, mountId);
			break;
		case Skills::SuperGm::Hide:
			time = 2100000; // Make sure that it doesn't end any time soon
			break;
		case Skills::Spearman::HyperBody:
		case Skills::SuperGm::HyperBody:
			player->getStats()->setHyperBody(skill->x, skill->y);
			break;
		case Skills::Crusader::ComboAttack:
		case Skills::DawnWarrior::ComboAttack:
			player->getActiveBuffs()->setCombo(0, false);
			break;
		case Skills::Hero::Enrage:
			if (player->getActiveBuffs()->getCombo() != 10) {
				return true;
			}
			player->getActiveBuffs()->setCombo(0, true);
			break;
		case Skills::Fighter::SwordBooster:
		case Skills::Fighter::AxeBooster:
		case Skills::Page::SwordBooster:
		case Skills::Page::BwBooster:
		case Skills::Spearman::SpearBooster:
		case Skills::Spearman::PolearmBooster:
		case Skills::FpMage::SpellBooster:
		case Skills::IlMage::SpellBooster:
		case Skills::Hunter::BowBooster:
		case Skills::Crossbowman::CrossbowBooster:
		case Skills::Assassin::ClawBooster:
		case Skills::Bandit::DaggerBooster:
		case Skills::Brawler::KnucklerBooster:
		case Skills::Gunslinger::GunBooster:
		case Skills::DawnWarrior::SwordBooster:
		case Skills::BlazeWizard::SpellBooster:
		case Skills::WindArcher::BowBooster:
		case Skills::NightWalker::ClawBooster:
		case Skills::ThunderBreaker::KnucklerBooster:
			player->getActiveBuffs()->setBooster(skillId); // Makes switching equips MUCH easier
			break;
		case Skills::WhiteKnight::BwFireCharge:
		case Skills::WhiteKnight::BwIceCharge:
		case Skills::WhiteKnight::BwLitCharge:
		case Skills::WhiteKnight::SwordFireCharge:
		case Skills::WhiteKnight::SwordIceCharge:
		case Skills::WhiteKnight::SwordLitCharge:
		case Skills::Paladin::BwHolyCharge:
		case Skills::Paladin::SwordHolyCharge:
		case Skills::DawnWarrior::SoulCharge:
		case Skills::ThunderBreaker::LightningCharge:
			player->getActiveBuffs()->setCharge(skillId); // Makes switching equips/Charged Blow easier
			break;
		case Skills::Hero::MapleWarrior:
		case Skills::Paladin::MapleWarrior:
		case Skills::DarkKnight::MapleWarrior:
		case Skills::FpArchMage::MapleWarrior:
		case Skills::IlArchMage::MapleWarrior:
		case Skills::Bishop::MapleWarrior:
		case Skills::Bowmaster::MapleWarrior:
		case Skills::Marksman::MapleWarrior:
		case Skills::NightLord::MapleWarrior:
		case Skills::Shadower::MapleWarrior:
		case Skills::Buccaneer::MapleWarrior:
		case Skills::Corsair::MapleWarrior:
			player->getStats()->setMapleWarrior(skill->x); // Take into account Maple Warrior for tracking stats if things are equippable, damage calculations, or w/e else
			break;
	}
	const vector<Buff> &buffs = parseBuffs(skillId, level);
	const ActiveBuff &playerSkill = parseBuffInfo(player, skillId, level);
	const ActiveMapBuff &mapSkill = parseBuffMapInfo(player, skillId, level);
	const ActiveMapBuff &enterSkill = parseBuffMapEntryInfo(player, skillId, level);

	if (mountId > 0) {
		BuffsPacket::useMount(player, skillId, time, playerSkill, mapSkill, addedInfo, mountId);
	}
	else {
		switch (skillId) {
			case Skills::Pirate::Dash:
			case Skills::ThunderBreaker::Dash:
				BuffsPacket::usePirateBuff(player, skillId, time, playerSkill, mapSkill);
				break;
			case Skills::Marauder::EnergyCharge:
			case Skills::ThunderBreaker::EnergyCharge:
				BuffsPacket::usePirateBuff(player, 0, (player->getActiveBuffs()->getEnergyChargeLevel() == Stats::MaxEnergyChargeLevel ? time : 0), playerSkill, mapSkill);
				break;
			case Skills::Buccaneer::SpeedInfusion:
			case Skills::ThunderBreaker::SpeedInfusion:
				BuffsPacket::useSpeedInfusion(player, skillId, time, playerSkill, mapSkill, addedInfo);
				break;
			case Skills::Outlaw::HomingBeacon:
			case Skills::Corsair::Bullseye:
				if (player->getActiveBuffs()->hasMarkedMonster()) {
					// Otherwise the animation appears above numerous
					BuffsPacket::endSkill(player, playerSkill);
				}
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
		playerBuffs->addMapEntryBuffInfo(enterSkill);
		playerBuffs->setActiveSkillLevel(skillId, level);
		playerBuffs->removeBuff(skillId);
		playerBuffs->addBuff(skillId, time);
		doAct(player, skillId, level);
	}
	return true;
}

void Buffs::addBuff(Player *player, int32_t itemId, int32_t time) {
	itemId *= -1; // Make the Item ID negative for the packet and to discern from skill buffs
	const vector<Buff> &buffs = parseBuffs(itemId, 0);
	const ActiveBuff &playerSkill = parseBuffInfo(player, itemId, 0);
	const ActiveMapBuff &mapSkill = parseBuffMapInfo(player, itemId, 0);
	const ActiveMapBuff &enterSkill = parseBuffMapEntryInfo(player, itemId, 0);

	BuffsPacket::useSkill(player, itemId, time, playerSkill, mapSkill, 0);

	PlayerActiveBuffs *playerBuffs = player->getActiveBuffs();
	playerBuffs->removeBuff(itemId);
	playerBuffs->addBuffInfo(itemId, buffs);
	playerBuffs->addBuff(itemId, time);
	playerBuffs->addMapEntryBuffInfo(enterSkill);
	playerBuffs->setActiveSkillLevel(itemId, 1);
}

void Buffs::endBuff(Player *player, int32_t skill) {
	PlayerActiveBuffs *playerBuffs = player->getActiveBuffs();
	switch (skill) {
		case Skills::Beginner::MonsterRider:
		case Skills::Noblesse::MonsterRider:
		case Skills::Corsair::Battleship:
			playerBuffs->setMountInfo(0, 0);
			break;
		case Skills::Crusader::ComboAttack:
		case Skills::DawnWarrior::ComboAttack:
			playerBuffs->setCombo(0, false);
			break;
		case Skills::Spearman::HyperBody:
		case Skills::SuperGm::HyperBody:
			player->getStats()->setHyperBody(0, 0);
			player->getStats()->setHp(player->getStats()->getHp());
			player->getStats()->setMp(player->getStats()->getMp());
			break;
		case Skills::Marauder::EnergyCharge:
		case Skills::ThunderBreaker::EnergyCharge:
			playerBuffs->resetEnergyChargeLevel();
			break;
		case Skills::Fighter::SwordBooster:
		case Skills::Fighter::AxeBooster:
		case Skills::Page::SwordBooster:
		case Skills::Page::BwBooster:
		case Skills::Spearman::SpearBooster:
		case Skills::Spearman::PolearmBooster:
		case Skills::FpMage::SpellBooster:
		case Skills::IlMage::SpellBooster:
		case Skills::Hunter::BowBooster:
		case Skills::Crossbowman::CrossbowBooster:
		case Skills::Assassin::ClawBooster:
		case Skills::Bandit::DaggerBooster:
		case Skills::Brawler::KnucklerBooster:
		case Skills::Gunslinger::GunBooster:
		case Skills::DawnWarrior::SwordBooster:
		case Skills::BlazeWizard::SpellBooster:
		case Skills::WindArcher::BowBooster:
		case Skills::NightWalker::ClawBooster:
		case Skills::ThunderBreaker::KnucklerBooster:
			playerBuffs->setBooster(0);
			break;
		case Skills::WhiteKnight::BwFireCharge:
		case Skills::WhiteKnight::BwIceCharge:
		case Skills::WhiteKnight::BwLitCharge:
		case Skills::WhiteKnight::SwordFireCharge:
		case Skills::WhiteKnight::SwordIceCharge:
		case Skills::WhiteKnight::SwordLitCharge:
		case Skills::Paladin::BwHolyCharge:
		case Skills::Paladin::SwordHolyCharge:
		case Skills::DawnWarrior::SoulCharge:
		case Skills::ThunderBreaker::LightningCharge:
			playerBuffs->setCharge(0);
			break;
		case Skills::Outlaw::HomingBeacon:
		case Skills::Corsair::Bullseye:
			playerBuffs->setMarkedMonster(0);
			break;
		case Skills::Hero::MapleWarrior:
		case Skills::Paladin::MapleWarrior:
		case Skills::DarkKnight::MapleWarrior:
		case Skills::FpArchMage::MapleWarrior:
		case Skills::IlArchMage::MapleWarrior:
		case Skills::Bishop::MapleWarrior:
		case Skills::Bowmaster::MapleWarrior:
		case Skills::Marksman::MapleWarrior:
		case Skills::NightLord::MapleWarrior:
		case Skills::Shadower::MapleWarrior:
		case Skills::Buccaneer::MapleWarrior:
		case Skills::Corsair::MapleWarrior:
			player->getStats()->setMapleWarrior(0);
			break;
	}
	uint8_t level = playerBuffs->getActiveSkillLevel(skill);
	const vector<Buff> &buffs = parseBuffs(skill, level);
	const ActiveMapBuff &enterSkill = parseBuffMapEntryInfo(player, skill, level);
	const ActiveBuff &playerSkill = playerBuffs->removeBuffInfo(skill, buffs);

	BuffsPacket::endSkill(player, playerSkill);

	playerBuffs->deleteMapEntryBuffInfo(enterSkill);
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
	if (!BuffDataProvider::Instance()->isDebuff(skillId)) {
		return;
	}

	int16_t time = SkillDataProvider::Instance()->getMobSkill(skillId, level)->time;
	MobAilmentInfo *mobSkillsInfo = BuffDataProvider::Instance()->getMobSkillInfo(skillId);

	const vector<Buff> &buffs = parseMobBuffs(skillId);
	const ActiveBuff &playerSkill = parseMobBuffInfo(player, skillId, level);
	const ActiveMapBuff &mapSkill = parseMobBuffMapInfo(player, skillId, level);
	const ActiveMapBuff &enterSkill = parseMobBuffMapEntryInfo(player, skillId, level);

	BuffsPacket::giveDebuff(player, skillId, level, time, mobSkillsInfo->delay, playerSkill, mapSkill);

	PlayerActiveBuffs *playerBuffs = player->getActiveBuffs();
	playerBuffs->setActiveSkillLevel(skillId, level);
	playerBuffs->addBuffInfo(skillId, buffs);
	playerBuffs->addMapEntryBuffInfo(enterSkill);
	playerBuffs->addBuff(skillId, time);
}

void Buffs::endDebuff(Player *player, uint8_t skill) {
	PlayerActiveBuffs *playerBuffs = player->getActiveBuffs();
	const vector<Buff> &buffs = parseMobBuffs(skill);
	const ActiveMapBuff &enterSkill = parseMobBuffMapEntryInfo(player, skill, 1);
	const ActiveBuff &playerSkill = playerBuffs->removeBuffInfo(skill, buffs);

	BuffsPacket::endDebuff(player, playerSkill);

	playerBuffs->deleteMapEntryBuffInfo(enterSkill);
	playerBuffs->setActiveSkillLevel(skill, 0);
}

bool Buffs::buffMayApply(int32_t skillId, uint8_t level, int8_t buffValue) {
	if (GameLogicUtilities::isDarkSight(skillId)) {
		return buffValue != SkillSpeed || level != SkillDataProvider::Instance()->getMaxLevel(skillId);
	}
	return true;
}