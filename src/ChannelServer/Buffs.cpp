/*
Copyright (C) 2008-2014 Vana Development Team

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
#include "Buffs.hpp"
#include "BuffDataProvider.hpp"
#include "BuffsPacket.hpp"
#include "GameLogicUtilities.hpp"
#include "Map.hpp"
#include "Player.hpp"
#include "SkillConstants.hpp"
#include "SkillDataProvider.hpp"
#include "SummonHandler.hpp"

auto Buffs::getValue(int8_t value, skill_id_t skillId, skill_level_t level) -> int16_t {
	int16_t rValue = 0;
	auto skill = SkillDataProvider::getInstance().getSkill(skillId, level);
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

auto Buffs::getMobSkillValue(int8_t value, mob_skill_id_t skillId, mob_skill_level_t level) -> int16_t {
	int16_t rValue = 0;
	auto skill = SkillDataProvider::getInstance().getMobSkill(skillId, level);
	switch (value) {
		case SkillX: rValue = static_cast<int16_t>(skill->x); break;
		case SkillY: rValue = static_cast<int16_t>(skill->y); break;
		case SkillProp: rValue = skill->prop; break;
		case SkillLv: rValue = level; break;
	}
	return rValue;
}

auto Buffs::parseMountInfo(Player *player, skill_id_t skillId, skill_level_t level) -> skill_id_t {
	skill_id_t mountId = 0;
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

auto Buffs::parseBuffInfo(Player *player, skill_id_t skillId, skill_level_t level) -> ActiveBuff {
	ActiveBuff playerSkill;
	auto skill = SkillDataProvider::getInstance().getSkill(skillId, level);
	auto &skillsInfo = BuffDataProvider::getInstance().getSkillInfo(skillId);

	for (const auto &buffInfo : skillsInfo.player) {
		const auto &buff = buffInfo.buff;
		int8_t val = buff.value;
		if (!Buffs::buffMayApply(skillId, level, val)) {
			continue;
		}
		playerSkill.types[buff.byte] += buff.type;
		int16_t value = 0;
		if (buffInfo.hasMapVal) {
			playerSkill.hasMapBuff = true;
		}
		if (val == SkillNone) {
			value = buffInfo.itemVal;
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
						// Females are +100
						value += player->getGender() * 100;
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

auto Buffs::parseBuffMapInfo(Player *player, skill_id_t skillId, skill_level_t level) -> ActiveMapBuff {
	ActiveMapBuff mapSkill;
	int32_t maps = 0;
	auto &skillsInfo = BuffDataProvider::getInstance().getSkillInfo(skillId);

	for (const auto &buffInfo : skillsInfo.player) {
		if (!buffInfo.hasMapVal) {
			continue;
		}
		const auto &map = skillsInfo.map[maps++];
		const auto &buff = map.buff;
		int8_t val = buff.value;
		if (!Buffs::buffMayApply(skillId, level, val)) {
			continue;
		}
		mapSkill.bytes.push_back(buff.byte);
		mapSkill.types.push_back(buff.type);
		mapSkill.typeList[buff.byte] += buff.type;
		mapSkill.useVals.push_back(map.useVal);
		if (map.useVal) {
			int16_t value = 0;
			if (val == SkillNone) {
				value = buffInfo.itemVal;
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

auto Buffs::parseBuffMapEntryInfo(Player *player, skill_id_t skillId, skill_level_t level) -> ActiveMapBuff {
	ActiveMapBuff mapSkill;
	int8_t mapCounter = 0;
	auto &skillsInfo = BuffDataProvider::getInstance().getSkillInfo(skillId);

	for (const auto &buffInfo : skillsInfo.player) {
		if (!buffInfo.hasMapEntry) {
			continue;
		}

		const auto &map = skillsInfo.map[mapCounter++];
		const auto &buff = map.buff;
		int8_t val = buff.value;
		if (!Buffs::buffMayApply(skillId, level, val)) {
			continue;
		}
		mapSkill.bytes.push_back(buff.byte);
		mapSkill.types.push_back(buff.type);
		mapSkill.typeList[buff.byte] += buff.type;
		mapSkill.useVals.push_back(map.useVal);
		if (map.useVal) {
			int16_t value = 0;
			if (val == SkillNone) {
				value = buffInfo.itemVal;
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

auto Buffs::parseBuffs(skill_id_t skillId, skill_level_t level) -> vector_t<Buff> {
	vector_t<Buff> ret;
	auto &skillsInfo = BuffDataProvider::getInstance().getSkillInfo(skillId);

	for (const auto &buffInfo : skillsInfo.player) {
		if (!Buffs::buffMayApply(skillId, level, buffInfo.buff.value)) {
			continue;
		}
		ret.push_back(buffInfo.buff);
	}
	return ret;
}

auto Buffs::parseMobBuffInfo(Player *player, mob_skill_id_t skillId, mob_skill_level_t level) -> ActiveBuff {
	ActiveBuff playerSkill;
	auto &mobSkillsInfo = BuffDataProvider::getInstance().getMobSkillInfo(skillId);

	for (const auto &buffInfo : mobSkillsInfo.mob) {
		const auto &buff = buffInfo.buff;
		int8_t val = buff.value;
		playerSkill.types[buff.byte] += buff.type;
		playerSkill.hasMapBuff = true;
		int16_t value = (val == SkillNone ? (skillId == MobSkills::Seal ? 2 : 1) : getMobSkillValue(val, skillId, level));
		playerSkill.vals.push_back(value);
	}
	return playerSkill;
}

auto Buffs::parseMobBuffMapInfo(Player *player, mob_skill_id_t skillId, mob_skill_level_t level) -> ActiveMapBuff {
	ActiveMapBuff mapSkill;
	auto &mobSkillsInfo = BuffDataProvider::getInstance().getMobSkillInfo(skillId);

	for (const auto &buffInfo : mobSkillsInfo.mob) {
		const auto &buff = buffInfo.buff;
		int8_t val = buff.value;
		mapSkill.bytes.push_back(buff.byte);
		mapSkill.types.push_back(buff.type);
		mapSkill.typeList[buff.byte] += buff.type;
		mapSkill.useVals.push_back(buffInfo.useVal);
		int16_t value = (val == SkillNone ? 1 : getMobSkillValue(val, skillId, level));
		mapSkill.values.push_back(value);
	}
	return mapSkill;
}

auto Buffs::parseMobBuffMapEntryInfo(Player *player, mob_skill_id_t skillId, mob_skill_level_t level) -> ActiveMapBuff {
	ActiveMapBuff mapSkill;
	auto &mobSkillsInfo = BuffDataProvider::getInstance().getMobSkillInfo(skillId);

	for (const auto &buffInfo : mobSkillsInfo.mob) {
		const auto &buff = buffInfo.buff;
		int8_t val = buff.value;
		mapSkill.bytes.push_back(buff.byte);
		mapSkill.types.push_back(buff.type);
		mapSkill.typeList[buff.byte] += buff.type;
		mapSkill.useVals.push_back(true);
		mapSkill.useVals.push_back(false);
		mapSkill.values.push_back(skillId);
		mapSkill.values.push_back(level);
	}
	mapSkill.debuff = true;
	return mapSkill;
}

auto Buffs::parseMobBuffs(mob_skill_id_t skillId) -> vector_t<Buff> {
	vector_t<Buff> ret;
	auto &mobSkillsInfo = BuffDataProvider::getInstance().getMobSkillInfo(skillId);

	for (const auto &buffInfo : mobSkillsInfo.mob) {
		ret.push_back(buffInfo.buff);
	}
	return ret;
}

auto Buffs::addBuff(Player *player, skill_id_t skillId, skill_level_t level, int16_t addedInfo, map_object_t mapMobId) -> Result {
	if (!BuffDataProvider::getInstance().isBuff(skillId)) {
		return Result::Failure;
	}

	skill_id_t mountId = parseMountInfo(player, skillId, level);
	auto skill = SkillDataProvider::getInstance().getSkill(skillId, level);
	seconds_t time(skill->time);

	switch (skillId) {
		case Skills::DragonKnight::DragonRoar:
			time = seconds_t(skill->y);
			break;
		case Skills::Beginner::MonsterRider:
		case Skills::Noblesse::MonsterRider:
		case Skills::Corsair::Battleship:
			if (mountId == 0) {
				// Hacking
				return Result::Failure;
			}
			player->getActiveBuffs()->setMountInfo(skillId, mountId);
			break;
		case Skills::SuperGm::Hide:
			time = seconds_t(2100000); // Make sure that it doesn't end any time soon
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
				return Result::Failure;
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

	vector_t<Buff> buffs = parseBuffs(skillId, level);
	ActiveBuff playerSkill = parseBuffInfo(player, skillId, level);
	ActiveMapBuff mapSkill = parseBuffMapInfo(player, skillId, level);
	ActiveMapBuff enterSkill = parseBuffMapEntryInfo(player, skillId, level);

	if (mountId > 0) {
		player->sendMap(BuffsPacket::useMount(player->getId(), skillId, time, playerSkill, mapSkill, addedInfo, mountId));
	}
	else {
		switch (skillId) {
			case Skills::Pirate::Dash:
			case Skills::ThunderBreaker::Dash:
				player->sendMap(BuffsPacket::usePirateBuff(player->getId(), skillId, time, playerSkill, mapSkill));
				break;
			case Skills::Marauder::EnergyCharge:
			case Skills::ThunderBreaker::EnergyCharge:
				player->sendMap(BuffsPacket::usePirateBuff(player->getId(), 0, (player->getActiveBuffs()->getEnergyChargeLevel() == Stats::MaxEnergyChargeLevel ? time : seconds_t(0)), playerSkill, mapSkill));
				break;
			case Skills::Buccaneer::SpeedInfusion:
			case Skills::ThunderBreaker::SpeedInfusion:
				player->sendMap(BuffsPacket::useSpeedInfusion(player->getId(), skillId, time, playerSkill, mapSkill, addedInfo));
				break;
			case Skills::Outlaw::HomingBeacon:
			case Skills::Corsair::Bullseye:
				if (player->getActiveBuffs()->hasMarkedMonster()) {
					// Otherwise the animation appears above numerous
					player->sendMap(BuffsPacket::endSkill(player->getId(), playerSkill));
				}
				player->getActiveBuffs()->setMarkedMonster(mapMobId);
				player->send(BuffsPacket::useHomingBeacon(skillId, playerSkill, mapMobId));
				break;
			default:
				player->sendMap(BuffsPacket::useSkill(player->getId(), skillId, time, playerSkill, mapSkill, addedInfo));
		}
	}

	if (skillId != player->getSkills()->getEnergyCharge() || player->getActiveBuffs()->getEnergyChargeLevel() == Stats::MaxEnergyChargeLevel) {
		PlayerActiveBuffs *playerBuffs = player->getActiveBuffs();
		playerBuffs->addBuffInfo(skillId, buffs);
		playerBuffs->addMapEntryBuffInfo(enterSkill);
		playerBuffs->setActiveSkillLevel(skillId, level);
		playerBuffs->removeBuff(skillId);
		playerBuffs->addBuff(skillId, time);
		doAction(player, skillId, level);
	}

	return Result::Successful;
}

auto Buffs::addBuff(Player *player, item_id_t itemId, const seconds_t &time) -> void {
	itemId *= -1; // Make the Item ID negative for the packet and to discern from skill buffs
	auto buffs = parseBuffs(itemId, 0);
	auto playerSkill = parseBuffInfo(player, itemId, 0);
	auto mapSkill = parseBuffMapInfo(player, itemId, 0);
	auto enterSkill = parseBuffMapEntryInfo(player, itemId, 0);

	player->sendMap(BuffsPacket::useSkill(player->getId(), itemId, time, playerSkill, mapSkill, 0));

	PlayerActiveBuffs *playerBuffs = player->getActiveBuffs();
	playerBuffs->removeBuff(itemId);
	playerBuffs->addBuffInfo(itemId, buffs);
	playerBuffs->addBuff(itemId, time);
	playerBuffs->addMapEntryBuffInfo(enterSkill);
	playerBuffs->setActiveSkillLevel(itemId, 1);
}

auto Buffs::endBuff(Player *player, skill_id_t skill) -> void {
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

	if (skill < 0) {
		// Items
		item_id_t itemId = -skill;
		switch (itemId) {
			case Items::BeholderHexWdef:
			case Items::BeholderHexMdef:
			case Items::BeholderHexAcc:
			case Items::BeholderHexAvo:
			case Items::BeholderHexWatk:
				// Handled specially
				Buff data = SummonHandler::makeBuff(player, itemId);
				vector_t<Buff> parsed{data};
				auto playerSkill = playerBuffs->removeBuffInfo(skill, parsed);
				player->sendMap(BuffsPacket::endSkill(player->getId(), playerSkill));
				playerBuffs->setActiveSkillLevel(skill, 0);
				return;
		}
	}

	skill_level_t level = playerBuffs->getActiveSkillLevel(skill);
	auto buffs = parseBuffs(skill, level);
	auto enterSkill = parseBuffMapEntryInfo(player, skill, level);
	auto playerSkill = playerBuffs->removeBuffInfo(skill, buffs);

	player->sendMap(BuffsPacket::endSkill(player->getId(), playerSkill));

	playerBuffs->deleteMapEntryBuffInfo(enterSkill);
	playerBuffs->setActiveSkillLevel(skill, 0);
}

auto Buffs::doAction(Player *player, skill_id_t skillId, skill_level_t level) -> void {
	auto &skillsInfo = BuffDataProvider::getInstance().getSkillInfo(skillId);

	if (skillsInfo.hasAction) {
		int16_t value = getValue(skillsInfo.act.value, skillId, level);
		player->getActiveBuffs()->addAction(skillId, skillsInfo.act.type, value, milliseconds_t(skillsInfo.act.time));
	}
}

auto Buffs::addDebuff(Player *player, mob_skill_id_t skillId, mob_skill_level_t level) -> void {
	if (!BuffDataProvider::getInstance().isDebuff(skillId)) {
		return;
	}

	seconds_t time(SkillDataProvider::getInstance().getMobSkill(skillId, level)->time);
	auto &mobSkillsInfo = BuffDataProvider::getInstance().getMobSkillInfo(skillId);

	auto buffs = parseMobBuffs(skillId);
	auto playerSkill = parseMobBuffInfo(player, skillId, level);
	auto mapSkill = parseMobBuffMapInfo(player, skillId, level);
	auto enterSkill = parseMobBuffMapEntryInfo(player, skillId, level);

	player->sendMap(BuffsPacket::giveDebuff(player->getId(), skillId, level, time, mobSkillsInfo.delay, playerSkill, mapSkill));

	PlayerActiveBuffs *playerBuffs = player->getActiveBuffs();
	playerBuffs->setActiveSkillLevel(skillId, level);
	playerBuffs->addBuffInfo(skillId, buffs);
	playerBuffs->addMapEntryBuffInfo(enterSkill);
	playerBuffs->addBuff(skillId, time);
}

auto Buffs::endDebuff(Player *player, mob_skill_id_t skill) -> void {
	PlayerActiveBuffs *playerBuffs = player->getActiveBuffs();
	auto buffs = parseMobBuffs(skill);
	auto enterSkill = parseMobBuffMapEntryInfo(player, skill, 1);
	auto playerSkill = playerBuffs->removeBuffInfo(skill, buffs);

	player->sendMap(BuffsPacket::endSkill(player->getId(), playerSkill));

	playerBuffs->deleteMapEntryBuffInfo(enterSkill);
	playerBuffs->setActiveSkillLevel(skill, 0);
}

auto Buffs::buffMayApply(skill_id_t skillId, skill_level_t level, int8_t buffValue) -> bool {
	if (GameLogicUtilities::isDarkSight(skillId)) {
		return buffValue != SkillSpeed || level != SkillDataProvider::getInstance().getMaxLevel(skillId);
	}
	return true;
}