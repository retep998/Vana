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
#include "Buffs.h"
#include "BuffsPacket.h"
#include "GameConstants.h"
#include "Inventory.h"
#include "Player.h"
#include "Skills.h"

Buffs *Buffs::singleton = 0;

Buffs::Buffs() {
	BuffInfo player;
	SkillAct act;
	// Boosters
	player.type = 0x08;
	player.byte = Type2;
	player.value = SkillX;
	skillsinfo[Jobs::Fighter::SwordBooster].player.push_back(player); // Sword Booster
	skillsinfo[Jobs::Fighter::AxeBooster].player.push_back(player); // Axe Booster
	skillsinfo[Jobs::Page::SwordBooster].player.push_back(player); // Sword Booster
	skillsinfo[Jobs::Page::BwBooster].player.push_back(player); // Blunt Booster
	skillsinfo[Jobs::Spearman::SpearBooster].player.push_back(player); // Spear Booster
	skillsinfo[Jobs::Spearman::PolearmBooster].player.push_back(player); // Polearm Booster
	skillsinfo[Jobs::FPMage::SpellBooster].player.push_back(player); // Spell Booster
	skillsinfo[Jobs::ILMage::SpellBooster].player.push_back(player); // Spell Booster
	skillsinfo[Jobs::Hunter::BowBooster].player.push_back(player); // Bow Booster
	skillsinfo[Jobs::Crossbowman::CrossbowBooster].player.push_back(player); // Crossbow Booster
	skillsinfo[Jobs::Assassin::ClawBooster].player.push_back(player); // Claw Booster
	skillsinfo[Jobs::Bandit::DaggerBooster].player.push_back(player); // Dagger Booster
	skillsinfo[Jobs::Infighter::KnucklerBooster].player.push_back(player); // Knuckle Booster
	skillsinfo[Jobs::Gunslinger::GunBooster].player.push_back(player); // Gun Booster
	// Speed Infusion
	player.type = 0x80;
	player.byte = Type8;
	player.value = SkillX;
	skillsinfo[Jobs::Buccaneer::SpeedInfusion].player.push_back(player);
	// Maple Warriors
	player.type = 0x8;
	player.byte = Type5;
	player.value = SkillX;
	skillsinfo[Jobs::Hero::MapleWarrior].player.push_back(player); // Hero
	skillsinfo[Jobs::Paladin::MapleWarrior].player.push_back(player); // Paladin
	skillsinfo[Jobs::DarkKnight::MapleWarrior].player.push_back(player); // Dark Knight
	skillsinfo[Jobs::FPArchMage::MapleWarrior].player.push_back(player); // FP Arch Mage
	skillsinfo[Jobs::ILArchMage::MapleWarrior].player.push_back(player); // IL Arch Mage
	skillsinfo[Jobs::Bishop::MapleWarrior].player.push_back(player); // Bishop
	skillsinfo[Jobs::Bowmaster::MapleWarrior].player.push_back(player); // Bowmaster
	skillsinfo[Jobs::Marksman::MapleWarrior].player.push_back(player); // Marksman
	skillsinfo[Jobs::NightLord::MapleWarrior].player.push_back(player); // Night Lord
	skillsinfo[Jobs::Shadower::MapleWarrior].player.push_back(player); // Shadower
	skillsinfo[Jobs::Buccaneer::MapleWarrior].player.push_back(player); // Buccaneer
	skillsinfo[Jobs::Corsair::MapleWarrior].player.push_back(player); // Corsair
	// 1002 - Nimble Feet
	player.type = 0x80;
	player.byte = Type1;
	player.value = SkillSpeed;
	skillsinfo[Jobs::Beginner::NimbleFeet].player.push_back(player);
	// 1001003 - Iron Body
	player.type = 0x02;
	player.byte = Type1;
	player.value = SkillWdef;
	skillsinfo[Jobs::Swordsman::IronBody].player.push_back(player);
	// 2001002 - Magic Guard
	player.type = 0x02;
	player.byte = Type2;
	player.value = SkillX;
	skillsinfo[Jobs::Magician::MagicGuard].player.push_back(player);
	// 2001003 - Magic Armor
	player.type = 0x02;
	player.byte = Type1;
	player.value = SkillWdef;
	skillsinfo[Jobs::Magician::MagicArmor].player.push_back(player);
	// 3001003 - Focus
	player.type = 0x10;
	player.byte = Type1;
	player.value = SkillAcc;
	skillsinfo[Jobs::Archer::Focus].player.push_back(player);
	player.type = 0x20;
	player.byte = Type1;
	player.value = SkillAvo;
	skillsinfo[Jobs::Archer::Focus].player.push_back(player);
	// 1101006 - Rage
	player.type = 0x1;
	player.byte = Type1;
	player.value = SkillWatk;
	skillsinfo[Jobs::Fighter::Rage].player.push_back(player);
	player.type = 0x2;
	player.byte = Type1;
	player.value = SkillWdef;
	skillsinfo[Jobs::Fighter::Rage].player.push_back(player);
	// 1101007 & 1201007 - Power Guard
	player.type = 0x10;
	player.byte = Type2;
	player.value = SkillX;
	skillsinfo[Jobs::Fighter::PowerGuard].player.push_back(player);
	skillsinfo[Jobs::Page::PowerGuard].player.push_back(player);
	// 1301006 - Iron Will
	player.type = 0x2;
	player.byte = Type1;
	player.value = SkillWdef;
	skillsinfo[Jobs::Spearman::IronWill].player.push_back(player);
	player.type = 0x8;
	player.byte = Type1;
	player.value = SkillMdef;
	skillsinfo[Jobs::Spearman::IronWill].player.push_back(player);
	// 1301007 & 9101008 - Hyper Body, GM Hyper Body
	player.type = 0x20;
	player.byte = Type2;
	player.value = SkillX;
	skillsinfo[Jobs::Spearman::HyperBody].player.push_back(player);
	skillsinfo[Jobs::SuperGM::HyperBody].player.push_back(player);
	player.type = 0x40;
	player.byte = Type2;
	player.value = SkillY;
	skillsinfo[Jobs::Spearman::HyperBody].player.push_back(player);
	skillsinfo[Jobs::SuperGM::HyperBody].player.push_back(player);
	// 2101001 & 2201001 - Meditation
	player.type = 0x04;
	player.byte = Type1;
	player.value = SkillMatk;
	skillsinfo[Jobs::FPWizard::Meditation].player.push_back(player);
	skillsinfo[Jobs::ILWizard::Meditation].player.push_back(player);
	// 2301003 - Invincible
	player.type = 0x80;
	player.byte = Type2;
	player.value = SkillX;
	skillsinfo[Jobs::Cleric::Invincible].player.push_back(player);
	// 2301004 - Bless
	player.type = 0x2;
	player.byte = Type1;
	player.value = SkillWdef;
	skillsinfo[Jobs::Cleric::Bless].player.push_back(player);
	player.type = 0x8;
	player.byte = Type1;
	player.value = SkillMdef;
	skillsinfo[Jobs::Cleric::Bless].player.push_back(player);
	player.type = 0x10;
	player.byte = Type1;
	player.value = SkillAcc;
	skillsinfo[Jobs::Cleric::Bless].player.push_back(player);
	player.type = 0x20;
	player.byte = Type1;
	player.value = SkillAvo;
	skillsinfo[Jobs::Cleric::Bless].player.push_back(player);
	// 9101003 - GM Bless
	player.type = 0x1;
	player.byte = Type1;
	player.value = SkillWatk;
	skillsinfo[Jobs::SuperGM::Bless].player.push_back(player);
	player.type = 0x2;
	player.byte = Type1;
	player.value = SkillWdef;
	skillsinfo[Jobs::SuperGM::Bless].player.push_back(player);
	player.type = 0x4;
	player.byte = Type1;
	player.value = SkillMatk;
	skillsinfo[Jobs::SuperGM::Bless].player.push_back(player);
	player.type = 0x8;
	player.byte = Type1;
	player.value = SkillMdef;
	skillsinfo[Jobs::SuperGM::Bless].player.push_back(player);
	player.type = 0x10;
	player.byte = Type1;
	player.value = SkillAcc;
	skillsinfo[Jobs::SuperGM::Bless].player.push_back(player);
	player.type = 0x20;
	player.byte = Type1;
	player.value = SkillAvo;
	skillsinfo[Jobs::SuperGM::Bless].player.push_back(player);
	// 9101004 - GM Hide
	player.type = 0x01;
	player.byte = Type8;
	player.value = 0;
	skillsinfo[Jobs::SuperGM::Hide].player.push_back(player);
	// 4101004, 4201003, 9001000, & 9101001 - Haste
	player.type = 0x80;
	player.byte = Type1;
	player.value = SkillSpeed;
	skillsinfo[Jobs::Assassin::Haste].player.push_back(player);
	skillsinfo[Jobs::Bandit::Haste].player.push_back(player);
	skillsinfo[Jobs::GM::Haste].player.push_back(player);
	skillsinfo[Jobs::SuperGM::Haste].player.push_back(player);
	player.type = 0x1;
	player.byte = Type2;
	player.value = SkillJump;
	skillsinfo[Jobs::Assassin::Haste].player.push_back(player);
	skillsinfo[Jobs::Bandit::Haste].player.push_back(player);
	skillsinfo[Jobs::GM::Haste].player.push_back(player);
	skillsinfo[Jobs::SuperGM::Haste].player.push_back(player);
	// 4211005 - Meso Guard
	player.type = 0x10;
	player.byte = Type4;
	player.value = SkillX;
	skillsinfo[Jobs::ChiefBandit::MesoGuard].player.push_back(player);
	// 3121002 & 3221002 - Sharp Eyes
	player.type = 0x20;
	player.byte = Type5;
	player.value = SkillX;
	skillsinfo[Jobs::Bowmaster::SharpEyes].player.push_back(player);
	skillsinfo[Jobs::Marksman::SharpEyes].player.push_back(player);
	// 1211003, 1211004, 1211005, 1211006, 1211007, 1211008, 1221003, & 1221004 - WK/Paladin Charges
	player.type = 0x4;
	player.byte = Type1;
	player.value = SkillMatk;
	skillsinfo[Jobs::WhiteKnight::SwordFireCharge].player.push_back(player);
	skillsinfo[Jobs::WhiteKnight::BwFireCharge].player.push_back(player);
	skillsinfo[Jobs::WhiteKnight::SwordIceCharge].player.push_back(player);
	skillsinfo[Jobs::WhiteKnight::BwIceCharge].player.push_back(player);
	skillsinfo[Jobs::WhiteKnight::SwordLitCharge].player.push_back(player);
	skillsinfo[Jobs::WhiteKnight::BwLitCharge].player.push_back(player);
	skillsinfo[Jobs::Paladin::SwordHolyCharge].player.push_back(player);
	skillsinfo[Jobs::Paladin::BwHolyCharge].player.push_back(player);
	player.type = 0x40;
	player.byte = Type3;
	player.value = SkillMatk;
	skillsinfo[Jobs::WhiteKnight::SwordFireCharge].player.push_back(player);
	skillsinfo[Jobs::WhiteKnight::BwFireCharge].player.push_back(player);
	skillsinfo[Jobs::WhiteKnight::SwordIceCharge].player.push_back(player);
	skillsinfo[Jobs::WhiteKnight::BwIceCharge].player.push_back(player);
	skillsinfo[Jobs::WhiteKnight::SwordLitCharge].player.push_back(player);
	skillsinfo[Jobs::WhiteKnight::BwLitCharge].player.push_back(player);
	skillsinfo[Jobs::Paladin::SwordHolyCharge].player.push_back(player);
	skillsinfo[Jobs::Paladin::BwHolyCharge].player.push_back(player);
	// 2311003 & 9101002 - Holy Symbol
	player.type = 0x1;
	player.byte = Type4;
	player.value = SkillX;
	skillsinfo[Jobs::Priest::HolySymbol].player.push_back(player); // Priest
	skillsinfo[Jobs::SuperGM::HolySymbol].player.push_back(player); // Super GM
	// 1121010 - Enrage
	player.type = 0x1;
	player.byte = Type1;
	player.value = SkillWatk;
	skillsinfo[Jobs::Hero::Enrage].player.push_back(player);
	// 3121008 - Concentrate
	player.type = 0x1;
	player.byte = Type1;
	player.value = SkillWatk;
	skillsinfo[Jobs::Bowmaster::Concentrate].player.push_back(player);
	// 4211003 - Pickpocket
	player.type = 0x8;
	player.byte = Type4;
	player.value = SkillX;
	skillsinfo[Jobs::ChiefBandit::Pickpocket].player.push_back(player);
	// 4111001 - Meso Up
	player.type = 0x8;
	player.byte = Type4;
	player.value = SkillX;
	skillsinfo[Jobs::Hermit::MesoUp].player.push_back(player);
	// 4121006 - Shadow Claw
	player.type = 0x1;
	player.byte = Type6;
	player.value = SkillX;
	skillsinfo[Jobs::NightLord::ShadowClaw].player.push_back(player);
	// 2121004, 2221004, & 2321004 - Infinity
	player.type = 0x2;
	player.byte = Type6;
	player.value = SkillX;
	skillsinfo[Jobs::FPArchMage::Infinity].player.push_back(player);
	skillsinfo[Jobs::ILArchMage::Infinity].player.push_back(player);
	skillsinfo[Jobs::Bishop::Infinity].player.push_back(player);
	// 1005 - Echo of Hero
	player.type = 0x80;
	player.byte = Type6;
	player.value = SkillX;
	skillsinfo[Jobs::Beginner::EchoOfHero].player.push_back(player);
	// 1121002, 1221002, & 1321002 - Power Stance
	player.type = 0x10;
	player.byte = Type5;
	player.value = SkillProp;
	skillsinfo[Jobs::Hero::PowerStance].player.push_back(player);
	skillsinfo[Jobs::Paladin::PowerStance].player.push_back(player);
	skillsinfo[Jobs::DarkKnight::PowerStance].player.push_back(player);
	// 2121002, 2221002, & 2321002 - Mana Reflection
	player.type = 0x40;
	player.byte = Type5;
	player.value = SkillLv;
	skillsinfo[Jobs::FPArchMage::ManaReflection].player.push_back(player);
	skillsinfo[Jobs::ILArchMage::ManaReflection].player.push_back(player);
	skillsinfo[Jobs::Bishop::ManaReflection].player.push_back(player);
	// 5001005 - Dash
	player.type = 0x10;
	player.byte = Type8;
	player.value = SkillX;
	skillsinfo[Jobs::Pirate::Dash].player.push_back(player);
	player.type = 0x20;
	player.byte = Type8;
	player.value = SkillY;
	skillsinfo[Jobs::Pirate::Dash].player.push_back(player);
	// 5110001 - Energy Charge
	player.type = 0x8;
	player.byte = Type8;
	player.value = SkillX;
	skillsinfo[Jobs::Marauder::EnergyCharge].player.push_back(player);
	// End regular buffs

	// Begin act buffs
	// 1001 - Recovery
	player.type = 0x4;
	player.byte = Type5;
	player.value = SkillX;
	act.type = ActHeal;
	act.time = 4900;
	act.value = SkillX;
	skillsinfo[Jobs::Beginner::Recovery].player.push_back(player);
	skillsinfo[Jobs::Beginner::Recovery].act = act;
	skillsinfo[Jobs::Beginner::Recovery].bact.push_back(true);
	// 1311008 - Dragon Blood
	player.type = 0x1;
	player.byte = Type1;
	player.value = SkillWatk;
	skillsinfo[Jobs::DragonKnight::DragonBlood].player.push_back(player);
	player.type = 0x80;
	player.byte = Type3;
	player.value = SkillLv;
	skillsinfo[Jobs::DragonKnight::DragonBlood].player.push_back(player);
	act.type = ActHurt;
	act.time = 4000;
	act.value = SkillX;
	skillsinfo[Jobs::DragonKnight::DragonBlood].act = act;
	skillsinfo[Jobs::DragonKnight::DragonBlood].bact.push_back(true);
	// End act buffs

	// Begin map buffs
	// 4001003 - Dark Sight
	player.type = 0x80;
	player.byte = Type1;
	player.value = SkillSpeed;
	skillsinfo[Jobs::Rogue::DarkSight].player.push_back(player);
	player.type = 0x04;
	player.byte = Type2;
	player.value = SkillX;
	player.hasmapval = true;
	skillsinfo[Jobs::Rogue::DarkSight].player.push_back(player);
	// 3101004 & 3201004 - Soul Arrow
	player.type = 0x1;
	player.byte = Type3;
	player.value = SkillX;
	player.hasmapval = true;
	skillsinfo[Jobs::Hunter::SoulArrow].player.push_back(player);
	skillsinfo[Jobs::Crossbowman::SoulArrow].player.push_back(player);
	// 5101007 - Oak Barrel 
	player.type = 0x02;
	player.byte = Type5;
	player.value = SkillMorph;
	player.hasmapval = true;
	skillsinfo[Jobs::Infighter::OakBarrel].player.push_back(player);
	// 5111005, 5121003 - Transformation & Super Transformation
	player.type = 0x02;
	player.byte = Type1;
	player.value = SkillWdef;
	player.hasmapval = false;
	skillsinfo[Jobs::Marauder::Transformation].player.push_back(player);
	skillsinfo[Jobs::Buccaneer::SuperTransformation].player.push_back(player);
	player.type = 0x08;
	player.byte = Type1;
	player.value = SkillMdef;
	skillsinfo[Jobs::Marauder::Transformation].player.push_back(player);
	skillsinfo[Jobs::Buccaneer::SuperTransformation].player.push_back(player);
	player.type = 0x80;
	player.byte = Type1;
	player.value = SkillSpeed;
	skillsinfo[Jobs::Marauder::Transformation].player.push_back(player);
	skillsinfo[Jobs::Buccaneer::SuperTransformation].player.push_back(player);
	player.type = 0x1;
	player.byte = Type2;
	player.value = SkillJump;
	skillsinfo[Jobs::Marauder::Transformation].player.push_back(player);
	skillsinfo[Jobs::Buccaneer::SuperTransformation].player.push_back(player);
	player.type = 0x02;
	player.byte = Type5;
	player.value = SkillMorph;
	player.hasmapval = true;
	skillsinfo[Jobs::Marauder::Transformation].player.push_back(player);
	skillsinfo[Jobs::Buccaneer::SuperTransformation].player.push_back(player);
	// 4111002 - Shadow Partner
	player.type = 0x4;
	player.byte = Type4;
	player.value = SkillX;
	player.hasmapval = true;
	skillsinfo[Jobs::Hermit::ShadowPartner].player.push_back(player);
	// 1111002 - Combo Attack
	player.type = 0x20;
	player.byte = Type3;
	player.value = SkillX;
	//player.hasmapval = true;
	//player.useval = true;
	skillsinfo[Jobs::Crusader::ComboAttack].player.push_back(player);
	// End map buffs

	// Begin mount buffs
	player = BuffInfo(); // Placed to clear any previous value pushes
	// 1004 - Monster Rider
	player.type = 0x40;
	player.byte = Type8;
	player.value = SkillX;
	player.ismount = true;
	skillsinfo[Jobs::Beginner::MonsterRider].player.push_back(player);
	// 5221006 - Battleship
	player.type = 0x40;
	player.byte = Type8;
	player.value = SkillX;
	player.ismount = true;
	skillsinfo[Jobs::Corsair::Battleship].player.push_back(player);
	// End mount buffs
}

int16_t Buffs::getValue(int8_t value, int32_t skillid, uint8_t level) {
	int16_t rvalue = 0;
	switch (value) {
		case SkillX: rvalue = Skills::skills[skillid][level].x; break;
		case SkillY: rvalue = Skills::skills[skillid][level].y; break;
		case SkillSpeed: rvalue = Skills::skills[skillid][level].speed; break;
		case SkillJump: rvalue = Skills::skills[skillid][level].jump; break;
		case SkillWatk: rvalue = Skills::skills[skillid][level].watk; break;
		case SkillWdef: rvalue = Skills::skills[skillid][level].wdef; break;
		case SkillMatk: rvalue = Skills::skills[skillid][level].matk; break;
		case SkillMdef: rvalue = Skills::skills[skillid][level].mdef; break;
		case SkillAcc: rvalue = Skills::skills[skillid][level].acc; break;
		case SkillAvo: rvalue = Skills::skills[skillid][level].avo; break;
		case SkillProp: rvalue = Skills::skills[skillid][level].prop; break;
		case SkillMorph: rvalue = Skills::skills[skillid][level].morph; break;
		case SkillLv: rvalue = level; break;
	}
	return rvalue;
}

SkillActiveInfo Buffs::parseBuffInfo(Player *player, int32_t skillid, uint8_t level, int32_t &mountid) {
	SkillActiveInfo playerskill;
	memset(playerskill.types, 0, 8 * sizeof(uint8_t)); // Reset player/map types to 0
	for (size_t i = 0; i < skillsinfo[skillid].player.size(); i++) {
		BuffInfo cur = skillsinfo[skillid].player[i];
		playerskill.types[cur.byte] += cur.type;
		int8_t val = cur.value;
		if (skillid == Jobs::Rogue::DarkSight && level == 20 && val == SkillSpeed) { // Cancel speed change for maxed dark sight
			playerskill.types[Type1] = 0;
			continue;
		}
		int16_t value = 0;
		switch (skillid) {
			case Jobs::Beginner::MonsterRider: // Monster Rider
				mountid = player->getInventory()->getEquippedID(18);
				break;
			case Jobs::Corsair::Battleship:
				mountid = 1932000; // Battleship item ID
				break;
			case Jobs::Bowmaster::SharpEyes: // Sharp Eyes
			case Jobs::Marksman::SharpEyes: // Sharp Eyes
			case 4111002: // Shadow Partner
				value = Skills::skills[skillid][level].x * 256 + Skills::skills[skillid][level].y;
				break;
			case Jobs::Crusader::ComboAttack: // Combo
				player->getActiveBuffs()->setCombo(0, false);
				value = 1;
				break;
			case Jobs::NightLord::ShadowClaw: // Shadow Claw
				value = (player->getInventory()->doShadowClaw() % 10000) + 1;
				break;
			case Jobs::Marauder::Transformation: // Transformation
			case Jobs::Buccaneer::SuperTransformation: // Super Transformation
				value = getValue(val, skillid, level) + (player->getGender() * 100); // Females are +100
				break;
			case Jobs::Marauder::EnergyCharge: // Energy Charge
				value = player->getActiveBuffs()->getEnergyChargeLevel();
				break;
			default:
				value = getValue(val, skillid, level);
				break;
		}
		playerskill.vals.push_back(value);
	}
	return playerskill;
}

SkillActiveInfo Buffs::parseBuffMapInfo(Player *player, int32_t skillid, uint8_t level, vector<SkillMapActiveInfo> &mapenterskill) {
	SkillActiveInfo mapskill;
	memset(mapskill.types, 0, 8 * sizeof(uint8_t));
	for (size_t i = 0; i < skillsinfo[skillid].player.size(); i++) {
		BuffInfo cur = skillsinfo[skillid].player[i];
		if (!cur.hasmapval && !cur.ismount)
			continue;
		mapskill.types[cur.byte] += cur.type;
		int8_t val = cur.value;
		if (skillid == Jobs::Rogue::DarkSight && level == 20 && val == SkillSpeed) { // Cancel speed update for maxed dark sight
			mapskill.types[Type1] = 0;
			continue;
		}
		int16_t value = 0;
		switch (skillid) {
			case Jobs::Hermit::ShadowPartner: // Shadow Partner
				value = Skills::skills[skillid][level].x * 256 + Skills::skills[skillid][level].y;
				break;
			case Jobs::Crusader::ComboAttack: // Combo Attack
				value = player->getActiveBuffs()->getCombo() + 1;
				break;
			default:
				value = getValue(val, skillid, level);
				break;
		}
		mapskill.vals.push_back(value);
		if (!cur.ismount) {
			SkillMapActiveInfo map;
			map.byte = cur.byte;
			map.type = cur.type;
			if (cur.useval) {
				map.isvalue = true;
				map.value = (int8_t)value;
			}
			else {
				map.isvalue = false;
				map.value = 0;
			}
			map.skill = skillid;
			mapenterskill.push_back(map);
		}
	}
	return mapskill;
}

bool Buffs::addBuff(Player *player, int32_t skillid, uint8_t level, int16_t addedinfo) {
	if (skillsinfo.find(skillid) == skillsinfo.end())
		return false; // Not a buff, so return false

	int32_t mountid = 0;
	PlayerActiveBuffs *playerbuffs = player->getActiveBuffs();
	vector<SkillMapActiveInfo> mapenterskill;
	SkillActiveInfo playerskill = parseBuffInfo(player, skillid, level, mountid);
	SkillActiveInfo mapskill = parseBuffMapInfo(player, skillid, level, mapenterskill);
	int32_t time = Skills::skills[skillid][level].time;
	switch (skillid) {
		case Jobs::Beginner::MonsterRider: // Monster Rider
			if (mountid == 0) {
				// Hacking
				return true;
			}
			break;
		case Jobs::SuperGM::Hide: // GM Hide
			time = 2100000;
			break;
		case Jobs::Spearman::HyperBody: // Hyper Body
		case Jobs::SuperGM::HyperBody: // GM Hyper Body
			// TODO: Party
			player->setHyperBody(Skills::skills[skillid][level].x, Skills::skills[skillid][level].y);
			break;
		case Jobs::Hero::Enrage: // Enrage
			if (playerbuffs->getCombo() != 10)
				return true;
			playerbuffs->setCombo(0, true);
			break;
	}
	if (mountid > 0)
		BuffsPacket::useMount(player, skillid, time, playerskill, addedinfo, mountid);
	else {
		if (skillid == Jobs::Pirate::Dash) // I honestly wish I didn't have to do this
			BuffsPacket::usePirateBuff(player, skillid, time, playerskill);
		else if (skillid == Jobs::Marauder::EnergyCharge)
			BuffsPacket::usePirateBuff(player, 0, (player->getActiveBuffs()->getEnergyChargeLevel() == 10000 ? time : 0), playerskill);
		else if (skillid == Jobs::Buccaneer::SpeedInfusion) // SIGH
			BuffsPacket::useSpeedInfusion(player, time, playerskill, addedinfo);
		else
			BuffsPacket::useSkill(player, skillid, time, playerskill, addedinfo);
	}
	if (skillid != Jobs::Marauder::EnergyCharge || playerbuffs->getEnergyChargeLevel() == 10000) {
		playerbuffs->setBuffInfo(skillid, playerskill);
		playerbuffs->setSkillMapEnterInfo(skillid, mapenterskill);
		playerbuffs->setActiveSkillLevel(skillid, level);
		playerbuffs->removeBuff(skillid);
		if (Buffs::skillsinfo[skillid].bact.size() > 0) {
			int16_t value = getValue(skillsinfo[skillid].act.value, skillid, level);
			playerbuffs->addAct(skillid, skillsinfo[skillid].act.type, value, skillsinfo[skillid].act.time);
		}
		playerbuffs->addBuff(skillid, time);
	}
	return true;
}

void Buffs::addBuff(Player *player, int32_t itemid, int32_t time, SkillActiveInfo &iteminfo, bool morph) {
	itemid = itemid * -1; // Make the Item ID negative for the packet and to discern from skill buffs
	PlayerActiveBuffs *playerbuffs = player->getActiveBuffs();
	BuffsPacket::useSkill(player, itemid, time, iteminfo, 0, morph, true);
	playerbuffs->setBuffInfo(itemid, iteminfo);
	playerbuffs->removeBuff(itemid);
	playerbuffs->addBuff(itemid, time);
}

void Buffs::endBuff(Player *player, int32_t skill) {
	switch (skill) {
		case Jobs::Spearman::HyperBody: // Hyper Body
		case Jobs::SuperGM::HyperBody: // GM Hyper Body
			player->setHyperBody(0, 0);
			player->setHP(player->getHP());
			player->setMP(player->getMP());
			break;
		case Jobs::Marauder::EnergyCharge: // Energy Charge
			player->getActiveBuffs()->resetEnergyChargeLevel();
			break;
	}
	BuffsPacket::endSkill(player, player->getActiveBuffs()->getBuffInfo(skill));
	player->getActiveBuffs()->deleteSkillMapEnterInfo(skill);
	player->getActiveBuffs()->setActiveSkillLevel(skill, 0);
}
