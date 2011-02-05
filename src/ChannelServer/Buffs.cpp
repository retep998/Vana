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

int16_t Buffs::getValue(int8_t value, int32_t skillid, uint8_t level) {
	int16_t rvalue = 0;
	SkillLevelInfo *skill = SkillDataProvider::Instance()->getSkill(skillid, level);
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

int16_t Buffs::getMobSkillValue(int8_t value, uint8_t skillid, uint8_t level) {
	int16_t rvalue = 0;
	MobSkillLevelInfo *skill = SkillDataProvider::Instance()->getMobSkill(skillid, level);
	switch (value) {
		case SkillX: rvalue = static_cast<int16_t>(skill->x); break;
		case SkillY: rvalue = static_cast<int16_t>(skill->y); break;
		case SkillProp: rvalue = skill->prop; break;
		case SkillLv: rvalue = level; break;
	}
	return rvalue;
}

int32_t Buffs::parseMountInfo(Player *player, int32_t skillid, uint8_t level) {
	int32_t mountid = 0;
	switch (skillid) {
		case Jobs::Beginner::MonsterRider:
		case Jobs::Noblesse::MonsterRider:
			mountid = player->getInventory()->getEquippedId(EquipSlots::Mount);
			break;
		case Jobs::Corsair::Battleship:
			mountid = Items::BattleshipMount;
			break;
	}
	return mountid;
}

ActiveBuff Buffs::parseBuffInfo(Player *player, int32_t skillid, uint8_t level) {
	ActiveBuff playerskill;
	BuffInfo cur;
	SkillLevelInfo *skill = SkillDataProvider::Instance()->getSkill(skillid, level);
	SkillInfo *skillsinfo = BuffDataProvider::Instance()->getSkillInfo(skillid);

	for (size_t i = 0; i < skillsinfo->player.size(); i++) {
		cur = skillsinfo->player[i];
		int8_t val = cur.buff.value;
		if (GameLogicUtilities::isMaxDarkSight(skillid, level) && val == SkillSpeed) // Cancel speed change for maxed dark sight
			continue;
		playerskill.types[cur.buff.byte] += cur.buff.type;
		int16_t value = 0;
		if (cur.hasmapval)
			playerskill.hasmapbuff = true;
		if (val == SkillNone)
			value = cur.itemval;
		else {
			switch (skillid) {
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
					value = getValue(val, skillid, level);
					if (val == SkillMorph)
						value += (player->getGender() * 100); // Females are +100
					break;
				case Jobs::Marauder::EnergyCharge:
				case Jobs::ThunderBreaker::EnergyCharge:
					value = player->getActiveBuffs()->getEnergyChargeLevel();
					break;
				default:
					value = getValue(val, skillid, level);
					break;
			}
		}
		playerskill.vals.push_back(value);
	}
	return playerskill;
}

ActiveMapBuff Buffs::parseBuffMapInfo(Player *player, int32_t skillid, uint8_t level) {
	ActiveMapBuff mapskill;
	BuffInfo cur;
	BuffMapInfo map;
	int32_t maps = 0;
	SkillInfo *skillsinfo = BuffDataProvider::Instance()->getSkillInfo(skillid);

	for (size_t i = 0; i < skillsinfo->player.size(); i++) {
		cur = skillsinfo->player[i];
		if (!cur.hasmapval)
			continue;
		map = skillsinfo->map[maps++];
		int8_t val = map.buff.value;
		if (GameLogicUtilities::isMaxDarkSight(skillid, level) && val == SkillSpeed) { // Cancel speed update for maxed dark sight
			continue;
		}
		mapskill.bytes.push_back(map.buff.byte);
		mapskill.types.push_back(map.buff.type);
		mapskill.typelist[map.buff.byte] += map.buff.type;
		mapskill.usevals.push_back(map.useval);
		if (map.useval) {
			int16_t value = 0;
			if (val == SkillNone) {
				value = cur.itemval;
			}
			else {
				switch (skillid) {
					case Jobs::Crusader::ComboAttack:
					case Jobs::DawnWarrior::ComboAttack:
						value = player->getActiveBuffs()->getCombo() + 1;
						break;
					default:
						value = getValue(val, skillid, level);
						break;
				}
			}
			mapskill.values.push_back(value);
		}
	}
	return mapskill;
}

ActiveMapBuff Buffs::parseBuffMapEntryInfo(Player *player, int32_t skillid, uint8_t level) {
	ActiveMapBuff mapskill;
	BuffInfo cur;
	BuffMapInfo map;
	int8_t mapctr = 0;
	SkillInfo *skillsinfo = BuffDataProvider::Instance()->getSkillInfo(skillid);

	for (size_t i = 0; i < skillsinfo->player.size(); i++) {
		cur = skillsinfo->player[i];
		if (!cur.hasmapentry) {
			continue;
		}
		map = skillsinfo->map[mapctr++];
		int8_t val = map.buff.value;
		if (GameLogicUtilities::isMaxDarkSight(skillid, level) && val == SkillSpeed) { // Cancel speed update for maxed dark sight
			continue;
		}
		mapskill.bytes.push_back(map.buff.byte);
		mapskill.types.push_back(map.buff.type);
		mapskill.typelist[map.buff.byte] += map.buff.type;
		mapskill.usevals.push_back(map.useval);
		if (map.useval) {
			int16_t value = 0;
			if (val == SkillNone) {
				value = cur.itemval;
			}
			else {
				switch (skillid) {
					case Jobs::Crusader::ComboAttack:
					case Jobs::DawnWarrior::ComboAttack:
						value = player->getActiveBuffs()->getCombo() + 1;
						break;
					default:
						value = getValue(val, skillid, level);
						break;
				}
			}
			mapskill.values.push_back(value);
		}
	}
	return mapskill;
}

vector<Buff> Buffs::parseBuffs(int32_t skillid, uint8_t level) {
	vector<Buff> ret;
	SkillInfo *skillsinfo = BuffDataProvider::Instance()->getSkillInfo(skillid);

	for (size_t i = 0; i < skillsinfo->player.size(); i++) {
		BuffInfo cur = skillsinfo->player[i];
		if (GameLogicUtilities::isMaxDarkSight(skillid, level) && cur.buff.value == SkillSpeed) { // Cancel speed update for maxed dark sight
			continue;
		}
		ret.push_back(cur.buff);
	}
	return ret;
}

ActiveBuff Buffs::parseMobBuffInfo(Player *player, uint8_t skillid, uint8_t level) {
	ActiveBuff playerskill;
	BuffInfo cur;
	MobAilmentInfo *mobskillsinfo = BuffDataProvider::Instance()->getMobSkillInfo(skillid);

	for (size_t i = 0; i < mobskillsinfo->mob.size(); i++) {
		cur = mobskillsinfo->mob[i];
		int8_t val = cur.buff.value;
		playerskill.types[cur.buff.byte] += cur.buff.type;
		playerskill.hasmapbuff = true;
		int16_t value = (val == SkillNone ? 1 : getMobSkillValue(val, skillid, level));
		playerskill.vals.push_back(value);
	}
	return playerskill;
}

ActiveMapBuff Buffs::parseMobBuffMapInfo(Player *player, uint8_t skillid, uint8_t level) {
	ActiveMapBuff mapskill;
	BuffInfo cur;
	MobAilmentInfo *mobskillsinfo = BuffDataProvider::Instance()->getMobSkillInfo(skillid);

	for (size_t i = 0; i < mobskillsinfo->mob.size(); i++) {
		cur = mobskillsinfo->mob[i];
		int8_t val = cur.buff.value;
		mapskill.bytes.push_back(cur.buff.byte);
		mapskill.types.push_back(cur.buff.type);
		mapskill.typelist[cur.buff.byte] += cur.buff.type;
		mapskill.usevals.push_back(cur.useval);
		int16_t value = (val == SkillNone ? 1 : getMobSkillValue(val, skillid, level));
		mapskill.values.push_back(value);
	}
	return mapskill;
}

ActiveMapBuff Buffs::parseMobBuffMapEntryInfo(Player *player, uint8_t skillid, uint8_t level) {
	ActiveMapBuff mapskill;
	BuffInfo cur;
	MobAilmentInfo *mobskillsinfo = BuffDataProvider::Instance()->getMobSkillInfo(skillid);

	for (size_t i = 0; i < mobskillsinfo->mob.size(); i++) {
		cur = mobskillsinfo->mob[i];
		int8_t val = cur.buff.value;
		mapskill.bytes.push_back(cur.buff.byte);
		mapskill.types.push_back(cur.buff.type);
		mapskill.typelist[cur.buff.byte] += cur.buff.type;
		mapskill.usevals.push_back(true);
		mapskill.usevals.push_back(false);
		mapskill.values.push_back(skillid);
		mapskill.values.push_back(level);
	}
	mapskill.debuff = true;
	return mapskill;
}

vector<Buff> Buffs::parseMobBuffs(uint8_t skillid) {
	vector<Buff> ret;
	MobAilmentInfo mobskillsinfo = *BuffDataProvider::Instance()->getMobSkillInfo(skillid);

	for (size_t i = 0; i < mobskillsinfo.mob.size(); i++) {
		BuffInfo cur = mobskillsinfo.mob[i];
		ret.push_back(cur.buff);
	}
	return ret;
}

bool Buffs::addBuff(Player *player, int32_t skillid, uint8_t level, int16_t addedinfo, int32_t mapmobid) {
	if (!BuffDataProvider::Instance()->isBuff(skillid))
		return false; // Not a buff, so return false

	int32_t mountid = parseMountInfo(player, skillid, level);
	SkillLevelInfo *skill = SkillDataProvider::Instance()->getSkill(skillid, level);
	int32_t time = skill->time;
	switch (skillid) {
		case Jobs::DragonKnight::DragonRoar:
			time = skill->y;
			break;
		case Jobs::Beginner::MonsterRider:
		case Jobs::Noblesse::MonsterRider:
		case Jobs::Corsair::Battleship:
			if (mountid == 0) {
				// Hacking
				return true;
			}
			player->getActiveBuffs()->setMountInfo(skillid, mountid);
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
			player->getActiveBuffs()->setBooster(skillid); // Makes switching equips MUCH easier
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
			player->getActiveBuffs()->setCharge(skillid); // Makes switching equips/Charged Blow easier
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
	vector<Buff> buffs = parseBuffs(skillid, level);
	ActiveBuff playerskill = parseBuffInfo(player, skillid, level);
	ActiveMapBuff mapskill = parseBuffMapInfo(player, skillid, level);
	ActiveMapBuff enterskill = parseBuffMapEntryInfo(player, skillid, level);

	if (mountid > 0) {
		BuffsPacket::useMount(player, skillid, time, playerskill, mapskill, addedinfo, mountid);
	}
	else {
		switch (skillid) {
			case Jobs::Pirate::Dash:
			case Jobs::ThunderBreaker::Dash:
				BuffsPacket::usePirateBuff(player, skillid, time, playerskill, mapskill);
				break;
			case Jobs::Marauder::EnergyCharge:
			case Jobs::ThunderBreaker::EnergyCharge:
				BuffsPacket::usePirateBuff(player, 0, (player->getActiveBuffs()->getEnergyChargeLevel() == 10000 ? time : 0), playerskill, mapskill);
				break;
			case Jobs::Buccaneer::SpeedInfusion:
			case Jobs::ThunderBreaker::SpeedInfusion:
				BuffsPacket::useSpeedInfusion(player, skillid, time, playerskill, mapskill, addedinfo);
				break;
			case Jobs::Outlaw::HomingBeacon:
			case Jobs::Corsair::Bullseye:
				if (player->getActiveBuffs()->hasMarkedMonster()) // Otherwise the animation appears above numerous
					BuffsPacket::endSkill(player, playerskill);
				player->getActiveBuffs()->setMarkedMonster(mapmobid);
				BuffsPacket::useHomingBeacon(player, skillid, playerskill, mapmobid);
				break;
			default:
				BuffsPacket::useSkill(player, skillid, time, playerskill, mapskill, addedinfo);
		}
	}
	if (skillid != player->getSkills()->getEnergyCharge() || player->getActiveBuffs()->getEnergyChargeLevel() == 10000) {
		PlayerActiveBuffs *playerbuffs = player->getActiveBuffs();
		playerbuffs->addBuffInfo(skillid, buffs);
		playerbuffs->addMapEntryBuffInfo(enterskill);
		playerbuffs->setActiveSkillLevel(skillid, level);
		playerbuffs->removeBuff(skillid);
		playerbuffs->addBuff(skillid, time);
		doAct(player, skillid, level);
	}
	return true;
}

void Buffs::addBuff(Player *player, int32_t itemid, int32_t time) {
	itemid *= -1; // Make the Item ID negative for the packet and to discern from skill buffs
	vector<Buff> buffs = parseBuffs(itemid, 0);
	ActiveBuff pskill = parseBuffInfo(player, itemid, 0);
	ActiveMapBuff mskill = parseBuffMapInfo(player, itemid, 0);
	ActiveMapBuff meskill = parseBuffMapEntryInfo(player, itemid, 0);

	BuffsPacket::useSkill(player, itemid, time, pskill, mskill, 0);

	PlayerActiveBuffs *playerbuffs = player->getActiveBuffs();
	playerbuffs->removeBuff(itemid);
	playerbuffs->addBuffInfo(itemid, buffs);
	playerbuffs->addBuff(itemid, time);
	playerbuffs->addMapEntryBuffInfo(meskill);
	playerbuffs->setActiveSkillLevel(itemid, 1);
}

void Buffs::endBuff(Player *player, int32_t skill) {
	PlayerActiveBuffs *playerbuffs = player->getActiveBuffs();
	switch (skill) {
		case Jobs::Beginner::MonsterRider:
		case Jobs::Noblesse::MonsterRider:
		case Jobs::Corsair::Battleship:
			playerbuffs->setMountInfo(0, 0);
			break;
		case Jobs::Crusader::ComboAttack:
		case Jobs::DawnWarrior::ComboAttack:
			playerbuffs->setCombo(0, false);
			break;
		case Jobs::Spearman::HyperBody:
		case Jobs::SuperGm::HyperBody:
			player->getStats()->setHyperBody(0, 0);
			player->getStats()->setHp(player->getStats()->getHp());
			player->getStats()->setMp(player->getStats()->getMp());
			break;
		case Jobs::Marauder::EnergyCharge:
		case Jobs::ThunderBreaker::EnergyCharge:
			playerbuffs->resetEnergyChargeLevel();
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
			playerbuffs->setBooster(0);
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
			playerbuffs->setCharge(0);
			break;
		case Jobs::Outlaw::HomingBeacon:
		case Jobs::Corsair::Bullseye:
			playerbuffs->setMarkedMonster(0);
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
	uint8_t level = playerbuffs->getActiveSkillLevel(skill);
	vector<Buff> buffs = parseBuffs(skill, level);
	ActiveMapBuff meskill = parseBuffMapEntryInfo(player, skill, level);
	ActiveBuff pskill = playerbuffs->removeBuffInfo(skill, buffs);

	BuffsPacket::endSkill(player, pskill);

	playerbuffs->deleteMapEntryBuffInfo(meskill);
	playerbuffs->setActiveSkillLevel(skill, 0);
}

void Buffs::doAct(Player *player, int32_t skillid, uint8_t level) {
	SkillInfo *skillsinfo = BuffDataProvider::Instance()->getSkillInfo(skillid);

	if (skillsinfo->bact) {
		int16_t value = getValue(skillsinfo->act.value, skillid, level);
		player->getActiveBuffs()->addAct(skillid, skillsinfo->act.type, value, skillsinfo->act.time);
	}
}

void Buffs::addDebuff(Player *player, uint8_t skillid, uint8_t level) {
	if (!BuffDataProvider::Instance()->isDebuff(skillid))
		return;

	int16_t time = SkillDataProvider::Instance()->getMobSkill(skillid, level)->time;
	MobAilmentInfo *mobskillsinfo = BuffDataProvider::Instance()->getMobSkillInfo(skillid);

	vector<Buff> buffs = parseMobBuffs(skillid);
	ActiveBuff playerskill = parseMobBuffInfo(player, skillid, level);
	ActiveMapBuff mapskill = parseMobBuffMapInfo(player, skillid, level);
	ActiveMapBuff enterskill = parseMobBuffMapEntryInfo(player, skillid, level);

	BuffsPacket::giveDebuff(player, skillid, level, time, mobskillsinfo->delay, playerskill, mapskill);

	PlayerActiveBuffs *playerbuffs = player->getActiveBuffs();
	playerbuffs->setActiveSkillLevel(skillid, level);
	playerbuffs->addBuffInfo(skillid, buffs);
	playerbuffs->addMapEntryBuffInfo(enterskill);
	playerbuffs->addBuff(skillid, time);
}

void Buffs::endDebuff(Player *player, uint8_t skill) {
	PlayerActiveBuffs *playerbuffs = player->getActiveBuffs();
	vector<Buff> buffs = parseMobBuffs(skill);
	ActiveMapBuff meskill = parseMobBuffMapEntryInfo(player, skill, 1);
	ActiveBuff pskill = playerbuffs->removeBuffInfo(skill, buffs);

	BuffsPacket::endDebuff(player, pskill);

	playerbuffs->deleteMapEntryBuffInfo(meskill);
	playerbuffs->setActiveSkillLevel(skill, 0);
}