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
	player.byte = TYPE_2;
	player.value = SKILL_X;
	skillsinfo[Fighter::SWORDBOOSTER].player.push_back(player); // Sword Booster
	skillsinfo[Fighter::AXEBOOSTER].player.push_back(player); // Axe Booster
	skillsinfo[Page::SWORDBOOSTER].player.push_back(player); // Sword Booster
	skillsinfo[Page::BWBOOSTER].player.push_back(player); // Blunt Booster
	skillsinfo[Spearman::SPEARBOOSTER].player.push_back(player); // Spear Booster
	skillsinfo[Spearman::POLEARMBOOSTER].player.push_back(player); // Polearm Booster
	skillsinfo[FPMage::SPELLBOOSTER].player.push_back(player); // Spell Booster
	skillsinfo[ILMage::SPELLBOOSTER].player.push_back(player); // Spell Booster
	skillsinfo[Hunter::BOWBOOSTER].player.push_back(player); // Bow Booster
	skillsinfo[Crossbowman::CROSSBOWBOOSTER].player.push_back(player); // Crossbow Booster
	skillsinfo[Assassin::CLAWBOOSTER].player.push_back(player); // Claw Booster
	skillsinfo[Bandit::DAGGERBOOSTER].player.push_back(player); // Dagger Booster
	skillsinfo[Infighter::KNUCKLERBOOSTER].player.push_back(player); // Knuckle Booster
	skillsinfo[Gunslinger::GUNBOOSTER].player.push_back(player); // Gun Booster
	// Speed Infusion
	player.type = 0x80;
	player.byte = TYPE_8;
	player.value = SKILL_X;
	skillsinfo[Buccaneer::SPEEDINFUSION].player.push_back(player);
	// Maple Warriors
	player.type = 0x8;
	player.byte = TYPE_5;
	player.value = SKILL_X;
	skillsinfo[Hero::MAPLEWARRIOR].player.push_back(player); // Hero
	skillsinfo[Paladin::MAPLEWARRIOR].player.push_back(player); // Paladin
	skillsinfo[DarkKnight::MAPLEWARRIOR].player.push_back(player); // Dark Knight
	skillsinfo[FPArchMage::MAPLEWARRIOR].player.push_back(player); // FP Arch Mage
	skillsinfo[ILArchMage::MAPLEWARRIOR].player.push_back(player); // IL Arch Mage
	skillsinfo[Bishop::MAPLEWARRIOR].player.push_back(player); // Bishop
	skillsinfo[Bowmaster::MAPLEWARRIOR].player.push_back(player); // Bowmaster
	skillsinfo[Marksman::MAPLEWARRIOR].player.push_back(player); // Marksman
	skillsinfo[NightLord::MAPLEWARRIOR].player.push_back(player); // Night Lord
	skillsinfo[Shadower::MAPLEWARRIOR].player.push_back(player); // Shadower
	skillsinfo[Buccaneer::MAPLEWARRIOR].player.push_back(player); // Buccaneer
	skillsinfo[Corsair::MAPLEWARRIOR].player.push_back(player); // Corsair
	// 1002 - Nimble Feet
	player.type = 0x80;
	player.byte = TYPE_1;
	player.value = SKILL_SPEED;
	skillsinfo[Beginner::NIMBLEFEET].player.push_back(player);
	// 1001003 - Iron Body
	player.type = 0x02;
	player.byte = TYPE_1;
	player.value = SKILL_WDEF;
	skillsinfo[Swordsman::IRONBODY].player.push_back(player);
	// 2001002 - Magic Guard
	player.type = 0x02;
	player.byte = TYPE_2;
	player.value = SKILL_X;
	skillsinfo[Magician::MAGICGUARD].player.push_back(player);
	// 2001003 - Magic Armor
	player.type = 0x02;
	player.byte = TYPE_1;
	player.value = SKILL_WDEF;
	skillsinfo[Magician::MAGICARMOR].player.push_back(player);
	// 3001003 - Focus
	player.type = 0x10;
	player.byte = TYPE_1;
	player.value = SKILL_ACC;
	skillsinfo[Archer::FOCUS].player.push_back(player);
	player.type = 0x20;
	player.byte = TYPE_1;
	player.value = SKILL_AVO;
	skillsinfo[Archer::FOCUS].player.push_back(player);
	// 1101006 - Rage
	player.type = 0x1;
	player.byte = TYPE_1;
	player.value = SKILL_WATK;
	skillsinfo[Fighter::RAGE].player.push_back(player);
	player.type = 0x2;
	player.byte = TYPE_1;
	player.value = SKILL_WDEF;
	skillsinfo[Fighter::RAGE].player.push_back(player);
	// 1101007 & 1201007 - Power Guard
	player.type = 0x10;
	player.byte = TYPE_2;
	player.value = SKILL_X;
	skillsinfo[Fighter::POWERGUARD].player.push_back(player);
	skillsinfo[Page::POWERGUARD].player.push_back(player);
	// 1301006 - Iron Will
	player.type = 0x2;
	player.byte = TYPE_1;
	player.value = SKILL_WDEF;
	skillsinfo[Spearman::IRONWILL].player.push_back(player);
	player.type = 0x8;
	player.byte = TYPE_1;
	player.value = SKILL_MDEF;
	skillsinfo[Spearman::IRONWILL].player.push_back(player);
	// 1301007 & 9101008 - Hyper Body, GM Hyper Body
	player.type = 0x20;
	player.byte = TYPE_2;
	player.value = SKILL_X;
	skillsinfo[Spearman::HYPERBODY].player.push_back(player);
	skillsinfo[SuperGM::HYPERBODY].player.push_back(player);
	player.type = 0x40;
	player.byte = TYPE_2;
	player.value = SKILL_Y;
	skillsinfo[Spearman::HYPERBODY].player.push_back(player);
	skillsinfo[SuperGM::HYPERBODY].player.push_back(player);
	// 2101001 & 2201001 - Meditation
	player.type = 0x04;
	player.byte = TYPE_1;
	player.value = SKILL_MATK;
	skillsinfo[FPWizard::MEDITATION].player.push_back(player);
	skillsinfo[ILWizard::MEDITATION].player.push_back(player);
	// 2301003 - Invincible
	player.type = 0x80;
	player.byte = TYPE_2;
	player.value = SKILL_X;
	skillsinfo[Cleric::INVINCIBLE].player.push_back(player);
	// 2301004 - Bless
	player.type = 0x2;
	player.byte = TYPE_1;
	player.value = SKILL_WDEF;
	skillsinfo[Cleric::BLESS].player.push_back(player);
	player.type = 0x8;
	player.byte = TYPE_1;
	player.value = SKILL_MDEF;
	skillsinfo[Cleric::BLESS].player.push_back(player);
	player.type = 0x10;
	player.byte = TYPE_1;
	player.value = SKILL_ACC;
	skillsinfo[Cleric::BLESS].player.push_back(player);
	player.type = 0x20;
	player.byte = TYPE_1;
	player.value = SKILL_AVO;
	skillsinfo[Cleric::BLESS].player.push_back(player);
	// 9101003 - GM Bless
	player.type = 0x1;
	player.byte = TYPE_1;
	player.value = SKILL_WATK;
	skillsinfo[SuperGM::BLESS].player.push_back(player);
	player.type = 0x2;
	player.byte = TYPE_1;
	player.value = SKILL_WDEF;
	skillsinfo[SuperGM::BLESS].player.push_back(player);
	player.type = 0x4;
	player.byte = TYPE_1;
	player.value = SKILL_MATK;
	skillsinfo[SuperGM::BLESS].player.push_back(player);
	player.type = 0x8;
	player.byte = TYPE_1;
	player.value = SKILL_MDEF;
	skillsinfo[SuperGM::BLESS].player.push_back(player);
	player.type = 0x10;
	player.byte = TYPE_1;
	player.value = SKILL_ACC;
	skillsinfo[SuperGM::BLESS].player.push_back(player);
	player.type = 0x20;
	player.byte = TYPE_1;
	player.value = SKILL_AVO;
	skillsinfo[SuperGM::BLESS].player.push_back(player);
	// 9101004 - GM Hide
	player.type = 0x01;
	player.byte = TYPE_8;
	player.value = 0;
	skillsinfo[SuperGM::HIDE].player.push_back(player);
	// 4101004, 4201003, 9001000, & 9101001 - Haste
	player.type = 0x80;
	player.byte = TYPE_1;
	player.value = SKILL_SPEED;
	skillsinfo[Assassin::HASTE].player.push_back(player);
	skillsinfo[Bandit::HASTE].player.push_back(player);
	skillsinfo[GM::HASTE].player.push_back(player);
	skillsinfo[SuperGM::HASTE].player.push_back(player);
	player.type = 0x1;
	player.byte = TYPE_2;
	player.value = SKILL_JUMP;
	skillsinfo[Assassin::HASTE].player.push_back(player);
	skillsinfo[Bandit::HASTE].player.push_back(player);
	skillsinfo[GM::HASTE].player.push_back(player);
	skillsinfo[SuperGM::HASTE].player.push_back(player);
	// 4211005 - Meso Guard
	player.type = 0x10;
	player.byte = TYPE_4;
	player.value = SKILL_X;
	skillsinfo[ChiefBandit::MESOGUARD].player.push_back(player);
	// 3121002 & 3221002 - Sharp Eyes
	player.type = 0x20;
	player.byte = TYPE_5;
	player.value = SKILL_X;
	skillsinfo[Bowmaster::SHARPEYES].player.push_back(player);
	skillsinfo[Marksman::SHARPEYES].player.push_back(player);
	// 1211003, 1211004, 1211005, 1211006, 1211007, 1211008, 1221003, & 1221004 - WK/Paladin Charges
	player.type = 0x4;
	player.byte = TYPE_1;
	player.value = SKILL_MATK;
	skillsinfo[WhiteKnight::SWORDFIRECHARGE].player.push_back(player);
	skillsinfo[WhiteKnight::BWFIRECHARGE].player.push_back(player);
	skillsinfo[WhiteKnight::SWORDICECHARGE].player.push_back(player);
	skillsinfo[WhiteKnight::BWICECHARGE].player.push_back(player);
	skillsinfo[WhiteKnight::SWORDLITCHARGE].player.push_back(player);
	skillsinfo[WhiteKnight::BWLITCHARGE].player.push_back(player);
	skillsinfo[Paladin::SWORDHOLYCHARGE].player.push_back(player);
	skillsinfo[Paladin::BWHOLYCHARGE].player.push_back(player);
	player.type = 0x40;
	player.byte = TYPE_3;
	player.value = SKILL_MATK;
	skillsinfo[WhiteKnight::SWORDFIRECHARGE].player.push_back(player);
	skillsinfo[WhiteKnight::BWFIRECHARGE].player.push_back(player);
	skillsinfo[WhiteKnight::SWORDICECHARGE].player.push_back(player);
	skillsinfo[WhiteKnight::BWICECHARGE].player.push_back(player);
	skillsinfo[WhiteKnight::SWORDLITCHARGE].player.push_back(player);
	skillsinfo[WhiteKnight::BWLITCHARGE].player.push_back(player);
	skillsinfo[Paladin::SWORDHOLYCHARGE].player.push_back(player);
	skillsinfo[Paladin::BWHOLYCHARGE].player.push_back(player);
	// 2311003 & 9101002 - Holy Symbol
	player.type = 0x1;
	player.byte = TYPE_4;
	player.value = SKILL_X;
	skillsinfo[Priest::HOLYSYMBOL].player.push_back(player); // Priest
	skillsinfo[SuperGM::HOLYSYMBOL].player.push_back(player); // Super GM
	// 1121010 - Enrage
	player.type = 0x1;
	player.byte = TYPE_1;
	player.value = SKILL_WATK;
	skillsinfo[Hero::ENRAGE].player.push_back(player);
	// 3121008 - Concentrate
	player.type = 0x1;
	player.byte = TYPE_1;
	player.value = SKILL_WATK;
	skillsinfo[Bowmaster::CONCENTRATE].player.push_back(player);
	// 4211003 - Pickpocket
	player.type = 0x8;
	player.byte = TYPE_4;
	player.value = SKILL_X;
	skillsinfo[ChiefBandit::PICKPOCKET].player.push_back(player);
	// 4111001 - Meso Up
	player.type = 0x8;
	player.byte = TYPE_4;
	player.value = SKILL_X;
	skillsinfo[Hermit::MESOUP].player.push_back(player);
	// 4121006 - Shadow Claw
	player.type = 0x1;
	player.byte = TYPE_6;
	player.value = SKILL_X;
	skillsinfo[NightLord::SHADOWCLAW].player.push_back(player);
	// 2121004, 2221004, & 2321004 - Infinity
	player.type = 0x2;
	player.byte = TYPE_6;
	player.value = SKILL_X;
	skillsinfo[FPArchMage::INFINITY].player.push_back(player);
	skillsinfo[ILArchMage::INFINITY].player.push_back(player);
	skillsinfo[Bishop::INFINITY].player.push_back(player);
	// 1005 - Echo of Hero
	player.type = 0x80;
	player.byte = TYPE_6;
	player.value = SKILL_X;
	skillsinfo[Beginner::ECHOOFHERO].player.push_back(player);
	// 1121002, 1221002, & 1321002 - Power Stance
	player.type = 0x10;
	player.byte = TYPE_5;
	player.value = SKILL_PROP;
	skillsinfo[Hero::POWERSTANCE].player.push_back(player);
	skillsinfo[Paladin::POWERSTANCE].player.push_back(player);
	skillsinfo[DarkKnight::POWERSTANCE].player.push_back(player);
	// 2121002, 2221002, & 2321002 - Mana Reflection
	player.type = 0x40;
	player.byte = TYPE_5;
	player.value = SKILL_LV;
	skillsinfo[FPArchMage::MANAREFLECTION].player.push_back(player);
	skillsinfo[ILArchMage::MANAREFLECTION].player.push_back(player);
	skillsinfo[Bishop::MANAREFLECTION].player.push_back(player);
	// 5001005 - Dash
	player.type = 0x10;
	player.byte = TYPE_8;
	player.value = SKILL_X;
	skillsinfo[Pirate::DASH].player.push_back(player);
	player.type = 0x20;
	player.byte = TYPE_8;
	player.value = SKILL_Y;
	skillsinfo[Pirate::DASH].player.push_back(player);
	// 5110001 - Energy Charge
	player.type = 0x8;
	player.byte = TYPE_8;
	player.value = SKILL_X;
	skillsinfo[Marauder::ENERGYCHARGE].player.push_back(player);
	// End regular buffs

	// Begin act buffs
	// 1001 - Recovery
	player.type = 0x4;
	player.byte = TYPE_5;
	player.value = SKILL_X;
	act.type = ACT_HEAL;
	act.time = 4900;
	act.value = SKILL_X;
	skillsinfo[Beginner::RECOVERY].player.push_back(player);
	skillsinfo[Beginner::RECOVERY].act = act;
	skillsinfo[Beginner::RECOVERY].bact.push_back(true);
	// 1311008 - Dragon Blood
	player.type = 0x1;
	player.byte = TYPE_1;
	player.value = SKILL_WATK;
	skillsinfo[DragonKnight::DRAGONBLOOD].player.push_back(player);
	player.type = 0x80;
	player.byte = TYPE_3;
	player.value = SKILL_LV;
	skillsinfo[DragonKnight::DRAGONBLOOD].player.push_back(player);
	act.type = ACT_HURT;
	act.time = 4000;
	act.value = SKILL_X;
	skillsinfo[DragonKnight::DRAGONBLOOD].act = act;
	skillsinfo[DragonKnight::DRAGONBLOOD].bact.push_back(true);
	// End act buffs

	// Begin map buffs
	// 4001003 - Dark Sight
	player.type = 0x80;
	player.byte = TYPE_1;
	player.value = SKILL_SPEED;
	skillsinfo[Rogue::DARKSIGHT].player.push_back(player);
	player.type = 0x04;
	player.byte = TYPE_2;
	player.value = SKILL_X;
	player.hasmapval = true;
	skillsinfo[Rogue::DARKSIGHT].player.push_back(player);
	// 3101004 & 3201004 - Soul Arrow
	player.type = 0x1;
	player.byte = TYPE_3;
	player.value = SKILL_X;
	player.hasmapval = true;
	skillsinfo[Hunter::SOULARROW].player.push_back(player);
	skillsinfo[Crossbowman::SOULARROW].player.push_back(player);
	// 5101007 - Oak Barrel 
	player.type = 0x02;
	player.byte = TYPE_5;
	player.value = SKILL_MORPH;
	player.hasmapval = true;
	skillsinfo[Infighter::OAKBARREL].player.push_back(player);
	// 5111005, 5121003 - Transformation & Super Transformation
	player.type = 0x02;
	player.byte = TYPE_1;
	player.value = SKILL_WDEF;
	player.hasmapval = false;
	skillsinfo[Marauder::TRANSFORMATION].player.push_back(player);
	skillsinfo[Buccaneer::SUPERTRANSFORMATION].player.push_back(player);
	player.type = 0x08;
	player.byte = TYPE_1;
	player.value = SKILL_MDEF;
	skillsinfo[Marauder::TRANSFORMATION].player.push_back(player);
	skillsinfo[Buccaneer::SUPERTRANSFORMATION].player.push_back(player);
	player.type = 0x80;
	player.byte = TYPE_1;
	player.value = SKILL_SPEED;
	skillsinfo[Marauder::TRANSFORMATION].player.push_back(player);
	skillsinfo[Buccaneer::SUPERTRANSFORMATION].player.push_back(player);
	player.type = 0x1;
	player.byte = TYPE_2;
	player.value = SKILL_JUMP;
	skillsinfo[Marauder::TRANSFORMATION].player.push_back(player);
	skillsinfo[Buccaneer::SUPERTRANSFORMATION].player.push_back(player);
	player.type = 0x02;
	player.byte = TYPE_5;
	player.value = SKILL_MORPH;
	player.hasmapval = true;
	skillsinfo[Marauder::TRANSFORMATION].player.push_back(player);
	skillsinfo[Buccaneer::SUPERTRANSFORMATION].player.push_back(player);
	// 4111002 - Shadow Partner
	player.type = 0x4;
	player.byte = TYPE_4;
	player.value = SKILL_X;
	player.hasmapval = true;
	skillsinfo[Hermit::SHADOWPARTER].player.push_back(player);
	// 1111002 - Combo Attack
	player.type = 0x20;
	player.byte = TYPE_3;
	player.value = SKILL_X;
	//player.hasmapval = true;
	//player.useval = true;
	skillsinfo[Crusader::COMBOATTACK].player.push_back(player);
	// End map buffs

	// Begin mount buffs
	player = BuffInfo(); // Placed to clear any previous value pushes
	// 1004 - Monster Rider
	player.type = 0x40;
	player.byte = TYPE_8;
	player.value = SKILL_X;
	player.ismount = true;
	skillsinfo[Beginner::MONSTERRIDER].player.push_back(player);
	// 5221006 - Battleship
	player.type = 0x40;
	player.byte = TYPE_8;
	player.value = SKILL_X;
	player.ismount = true;
	skillsinfo[Corsair::BATTLESHIP].player.push_back(player);
	// End mount buffs
}

int16_t Buffs::getValue(int8_t value, int32_t skillid, uint8_t level) {
	int16_t rvalue = 0;
	switch (value) {
		case SKILL_X: rvalue = Skills::skills[skillid][level].x; break;
		case SKILL_Y: rvalue = Skills::skills[skillid][level].y; break;
		case SKILL_SPEED: rvalue = Skills::skills[skillid][level].speed; break;
		case SKILL_JUMP: rvalue = Skills::skills[skillid][level].jump; break;
		case SKILL_WATK: rvalue = Skills::skills[skillid][level].watk; break;
		case SKILL_WDEF: rvalue = Skills::skills[skillid][level].wdef; break;
		case SKILL_MATK: rvalue = Skills::skills[skillid][level].matk; break;
		case SKILL_MDEF: rvalue = Skills::skills[skillid][level].mdef; break;
		case SKILL_ACC: rvalue = Skills::skills[skillid][level].acc; break;
		case SKILL_AVO: rvalue = Skills::skills[skillid][level].avo; break;
		case SKILL_PROP: rvalue = Skills::skills[skillid][level].prop; break;
		case SKILL_MORPH: rvalue = Skills::skills[skillid][level].morph; break;
		case SKILL_LV: rvalue = level; break;
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
		if (skillid == Rogue::DARKSIGHT && level == 20 && val == SKILL_SPEED) { // Cancel speed change for maxed dark sight
			playerskill.types[TYPE_1] = 0;
			continue;
		}
		int16_t value = 0;
		switch (skillid) {
			case Beginner::MONSTERRIDER: // Monster Rider
				mountid = player->getInventory()->getEquippedID(18);
				break;
			case Corsair::BATTLESHIP:
				mountid = 1932000; // Battleship item ID
				break;
			case Bowmaster::SHARPEYES: // Sharp Eyes
			case Marksman::SHARPEYES: // Sharp Eyes
			case 4111002: // Shadow Partner
				value = Skills::skills[skillid][level].x * 256 + Skills::skills[skillid][level].y;
				break;
			case Crusader::COMBOATTACK: // Combo
				player->getActiveBuffs()->setCombo(0, false);
				value = 1;
				break;
			case NightLord::SHADOWCLAW: // Shadow Claw
				value = (player->getInventory()->doShadowClaw() % 10000) + 1;
				break;
			case Marauder::TRANSFORMATION: // Transformation
			case Buccaneer::SUPERTRANSFORMATION: // Super Transformation
				value = getValue(val, skillid, level) + (player->getGender() * 100); // Females are +100
				break;
			case Marauder::ENERGYCHARGE: // Energy Charge
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
		if (skillid == Rogue::DARKSIGHT && level == 20 && val == SKILL_SPEED) { // Cancel speed update for maxed dark sight
			mapskill.types[TYPE_1] = 0;
			continue;
		}
		int16_t value = 0;
		switch (skillid) {
			case Hermit::SHADOWPARTER: // Shadow Partner
				value = Skills::skills[skillid][level].x * 256 + Skills::skills[skillid][level].y;
				break;
			case Crusader::COMBOATTACK: // Combo Attack
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
		case Beginner::MONSTERRIDER: // Monster Rider
			if (mountid == 0) {
				// Hacking
				return true;
			}
			break;
		case SuperGM::HIDE: // GM Hide
			time = 2100000;
			break;
		case Spearman::HYPERBODY: // Hyper Body
		case SuperGM::HYPERBODY: // GM Hyper Body
			// TODO: Party
			player->setHyperBody(Skills::skills[skillid][level].x, Skills::skills[skillid][level].y);
			break;
		case Hero::ENRAGE: // Enrage
			if (playerbuffs->getCombo() != 10)
				return true;
			playerbuffs->setCombo(0, true);
			break;
	}
	if (mountid > 0)
		BuffsPacket::useMount(player, skillid, time, playerskill, addedinfo, mountid);
	else {
		if (skillid == Pirate::DASH) // I honestly wish I didn't have to do this
			BuffsPacket::usePirateBuff(player, skillid, time, playerskill);
		else if (skillid == Marauder::ENERGYCHARGE)
			BuffsPacket::usePirateBuff(player, 0, (player->getActiveBuffs()->getEnergyChargeLevel() == 10000 ? time : 0), playerskill);
		else if (skillid == Buccaneer::SPEEDINFUSION) // SIGH
			BuffsPacket::useSpeedInfusion(player, time, playerskill, addedinfo);
		else
			BuffsPacket::useSkill(player, skillid, time, playerskill, addedinfo);
	}
	if (skillid != Marauder::ENERGYCHARGE || playerbuffs->getEnergyChargeLevel() == 10000) {
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
		case Spearman::HYPERBODY: // Hyper Body
		case SuperGM::HYPERBODY: // GM Hyper Body
			player->setHyperBody(0, 0);
			player->setHP(player->getHP());
			player->setMP(player->getMP());
			break;
		case Marauder::ENERGYCHARGE: // Energy Charge
			player->getActiveBuffs()->resetEnergyChargeLevel();
			break;
	}
	BuffsPacket::endSkill(player, player->getActiveBuffs()->getBuffInfo(skill));
	player->getActiveBuffs()->deleteSkillMapEnterInfo(skill);
	player->getActiveBuffs()->setActiveSkillLevel(skill, 0);
}
