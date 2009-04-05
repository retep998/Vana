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
	player.byte = Type_2;
	player.value = Skill_X;
	skillsinfo[Jobs::Fighter::Sword_Booster].player.push_back(player); // Sword Booster
	skillsinfo[Jobs::Fighter::Axe_Booster].player.push_back(player); // Axe Booster
	skillsinfo[Jobs::Page::Sword_Booster].player.push_back(player); // Sword Booster
	skillsinfo[Jobs::Page::Bw_Booster].player.push_back(player); // Blunt Booster
	skillsinfo[Jobs::Spearman::Spear_Booster].player.push_back(player); // Spear Booster
	skillsinfo[Jobs::Spearman::Polearm_Booster].player.push_back(player); // Polearm Booster
	skillsinfo[Jobs::FPMage::Spell_Booster].player.push_back(player); // Spell Booster
	skillsinfo[Jobs::ILMage::Spell_Booster].player.push_back(player); // Spell Booster
	skillsinfo[Jobs::Hunter::Bow_Booster].player.push_back(player); // Bow Booster
	skillsinfo[Jobs::Crossbowman::Crossbow_Booster].player.push_back(player); // Crossbow Booster
	skillsinfo[Jobs::Assassin::Claw_Booster].player.push_back(player); // Claw Booster
	skillsinfo[Jobs::Bandit::Dagger_Booster].player.push_back(player); // Dagger Booster
	skillsinfo[Jobs::Infighter::Knuckler_Booster].player.push_back(player); // Knuckle Booster
	skillsinfo[Jobs::Gunslinger::Gun_Booster].player.push_back(player); // Gun Booster
	// Speed Infusion
	player.type = 0x80;
	player.byte = Type_8;
	player.value = Skill_X;
	skillsinfo[Jobs::Buccaneer::Speed_Infusion].player.push_back(player);
	// Maple Warriors
	player.type = 0x8;
	player.byte = Type_5;
	player.value = Skill_X;
	skillsinfo[Jobs::Hero::Maple_Warrior].player.push_back(player); // Hero
	skillsinfo[Jobs::Paladin::Maple_Warrior].player.push_back(player); // Paladin
	skillsinfo[Jobs::DarkKnight::Maple_Warrior].player.push_back(player); // Dark Knight
	skillsinfo[Jobs::FPArchMage::Maple_Warrior].player.push_back(player); // FP Arch Mage
	skillsinfo[Jobs::ILArchMage::Maple_Warrior].player.push_back(player); // IL Arch Mage
	skillsinfo[Jobs::Bishop::Maple_Warrior].player.push_back(player); // Bishop
	skillsinfo[Jobs::Bowmaster::Maple_Warrior].player.push_back(player); // Bowmaster
	skillsinfo[Jobs::Marksman::Maple_Warrior].player.push_back(player); // Marksman
	skillsinfo[Jobs::NightLord::Maple_Warrior].player.push_back(player); // Night Lord
	skillsinfo[Jobs::Shadower::Maple_Warrior].player.push_back(player); // Shadower
	skillsinfo[Jobs::Buccaneer::Maple_Warrior].player.push_back(player); // Buccaneer
	skillsinfo[Jobs::Corsair::Maple_Warrior].player.push_back(player); // Corsair
	// 1002 - Nimble Feet
	player.type = 0x80;
	player.byte = Type_1;
	player.value = Skill_Speed;
	skillsinfo[Jobs::Beginner::Nimble_Feet].player.push_back(player);
	// 1001003 - Iron Body
	player.type = 0x02;
	player.byte = Type_1;
	player.value = Skill_Wdef;
	skillsinfo[Jobs::Swordsman::Iron_Body].player.push_back(player);
	// 2001002 - Magic Guard
	player.type = 0x02;
	player.byte = Type_2;
	player.value = Skill_X;
	skillsinfo[Jobs::Magician::Magic_Guard].player.push_back(player);
	// 2001003 - Magic Armor
	player.type = 0x02;
	player.byte = Type_1;
	player.value = Skill_Wdef;
	skillsinfo[Jobs::Magician::Magic_Armor].player.push_back(player);
	// 3001003 - Focus
	player.type = 0x10;
	player.byte = Type_1;
	player.value = Skill_Acc;
	skillsinfo[Jobs::Archer::Focus].player.push_back(player);
	player.type = 0x20;
	player.byte = Type_1;
	player.value = Skill_Avo;
	skillsinfo[Jobs::Archer::Focus].player.push_back(player);
	// 1101006 - Rage
	player.type = 0x1;
	player.byte = Type_1;
	player.value = Skill_Watk;
	skillsinfo[Jobs::Fighter::Rage].player.push_back(player);
	player.type = 0x2;
	player.byte = Type_1;
	player.value = Skill_Wdef;
	skillsinfo[Jobs::Fighter::Rage].player.push_back(player);
	// 1101007 & 1201007 - Power Guard
	player.type = 0x10;
	player.byte = Type_2;
	player.value = Skill_X;
	skillsinfo[Jobs::Fighter::Power_Guard].player.push_back(player);
	skillsinfo[Jobs::Page::Power_Guard].player.push_back(player);
	// 1301006 - Iron Will
	player.type = 0x2;
	player.byte = Type_1;
	player.value = Skill_Wdef;
	skillsinfo[Jobs::Spearman::Iron_Will].player.push_back(player);
	player.type = 0x8;
	player.byte = Type_1;
	player.value = Skill_Mdef;
	skillsinfo[Jobs::Spearman::Iron_Will].player.push_back(player);
	// 1301007 & 9101008 - Hyper Body, GM Hyper Body
	player.type = 0x20;
	player.byte = Type_2;
	player.value = Skill_X;
	skillsinfo[Jobs::Spearman::Hyper_Body].player.push_back(player);
	skillsinfo[Jobs::SuperGM::Hyper_Body].player.push_back(player);
	player.type = 0x40;
	player.byte = Type_2;
	player.value = Skill_Y;
	skillsinfo[Jobs::Spearman::Hyper_Body].player.push_back(player);
	skillsinfo[Jobs::SuperGM::Hyper_Body].player.push_back(player);
	// 2101001 & 2201001 - Meditation
	player.type = 0x04;
	player.byte = Type_1;
	player.value = Skill_Matk;
	skillsinfo[Jobs::FPWizard::Meditation].player.push_back(player);
	skillsinfo[Jobs::ILWizard::Meditation].player.push_back(player);
	// 2301003 - Invincible
	player.type = 0x80;
	player.byte = Type_2;
	player.value = Skill_X;
	skillsinfo[Jobs::Cleric::Invincible].player.push_back(player);
	// 2301004 - Bless
	player.type = 0x2;
	player.byte = Type_1;
	player.value = Skill_Wdef;
	skillsinfo[Jobs::Cleric::Bless].player.push_back(player);
	player.type = 0x8;
	player.byte = Type_1;
	player.value = Skill_Mdef;
	skillsinfo[Jobs::Cleric::Bless].player.push_back(player);
	player.type = 0x10;
	player.byte = Type_1;
	player.value = Skill_Acc;
	skillsinfo[Jobs::Cleric::Bless].player.push_back(player);
	player.type = 0x20;
	player.byte = Type_1;
	player.value = Skill_Avo;
	skillsinfo[Jobs::Cleric::Bless].player.push_back(player);
	// 9101003 - GM Bless
	player.type = 0x1;
	player.byte = Type_1;
	player.value = Skill_Watk;
	skillsinfo[Jobs::SuperGM::Bless].player.push_back(player);
	player.type = 0x2;
	player.byte = Type_1;
	player.value = Skill_Wdef;
	skillsinfo[Jobs::SuperGM::Bless].player.push_back(player);
	player.type = 0x4;
	player.byte = Type_1;
	player.value = Skill_Matk;
	skillsinfo[Jobs::SuperGM::Bless].player.push_back(player);
	player.type = 0x8;
	player.byte = Type_1;
	player.value = Skill_Mdef;
	skillsinfo[Jobs::SuperGM::Bless].player.push_back(player);
	player.type = 0x10;
	player.byte = Type_1;
	player.value = Skill_Acc;
	skillsinfo[Jobs::SuperGM::Bless].player.push_back(player);
	player.type = 0x20;
	player.byte = Type_1;
	player.value = Skill_Avo;
	skillsinfo[Jobs::SuperGM::Bless].player.push_back(player);
	// 9101004 - GM Hide
	player.type = 0x01;
	player.byte = Type_8;
	player.value = 0;
	skillsinfo[Jobs::SuperGM::Hide].player.push_back(player);
	// 4101004, 4201003, 9001000, & 9101001 - Haste
	player.type = 0x80;
	player.byte = Type_1;
	player.value = Skill_Speed;
	skillsinfo[Jobs::Assassin::Haste].player.push_back(player);
	skillsinfo[Jobs::Bandit::Haste].player.push_back(player);
	skillsinfo[Jobs::GM::Haste].player.push_back(player);
	skillsinfo[Jobs::SuperGM::Haste].player.push_back(player);
	player.type = 0x1;
	player.byte = Type_2;
	player.value = Skill_Jump;
	skillsinfo[Jobs::Assassin::Haste].player.push_back(player);
	skillsinfo[Jobs::Bandit::Haste].player.push_back(player);
	skillsinfo[Jobs::GM::Haste].player.push_back(player);
	skillsinfo[Jobs::SuperGM::Haste].player.push_back(player);
	// 4211005 - Meso Guard
	player.type = 0x10;
	player.byte = Type_4;
	player.value = Skill_X;
	skillsinfo[Jobs::ChiefBandit::Meso_Guard].player.push_back(player);
	// 3121002 & 3221002 - Sharp Eyes
	player.type = 0x20;
	player.byte = Type_5;
	player.value = Skill_X;
	skillsinfo[Jobs::Bowmaster::Sharp_Eyes].player.push_back(player);
	skillsinfo[Jobs::Marksman::Sharp_Eyes].player.push_back(player);
	// 1211003, 1211004, 1211005, 1211006, 1211007, 1211008, 1221003, & 1221004 - WK/Paladin Charges
	player.type = 0x4;
	player.byte = Type_1;
	player.value = Skill_Matk;
	skillsinfo[Jobs::WhiteKnight::Sword_Fire_Charge].player.push_back(player);
	skillsinfo[Jobs::WhiteKnight::Bw_Fire_Charge].player.push_back(player);
	skillsinfo[Jobs::WhiteKnight::Sword_Ice_Charge].player.push_back(player);
	skillsinfo[Jobs::WhiteKnight::Bw_Ice_Charge].player.push_back(player);
	skillsinfo[Jobs::WhiteKnight::Sword_Lit_Charge].player.push_back(player);
	skillsinfo[Jobs::WhiteKnight::Bw_Lit_Charge].player.push_back(player);
	skillsinfo[Jobs::Paladin::Sword_Holy_Charge].player.push_back(player);
	skillsinfo[Jobs::Paladin::Bw_Holy_Charge].player.push_back(player);
	player.type = 0x40;
	player.byte = Type_3;
	player.value = Skill_Matk;
	skillsinfo[Jobs::WhiteKnight::Sword_Fire_Charge].player.push_back(player);
	skillsinfo[Jobs::WhiteKnight::Bw_Fire_Charge].player.push_back(player);
	skillsinfo[Jobs::WhiteKnight::Sword_Ice_Charge].player.push_back(player);
	skillsinfo[Jobs::WhiteKnight::Bw_Ice_Charge].player.push_back(player);
	skillsinfo[Jobs::WhiteKnight::Sword_Lit_Charge].player.push_back(player);
	skillsinfo[Jobs::WhiteKnight::Bw_Lit_Charge].player.push_back(player);
	skillsinfo[Jobs::Paladin::Sword_Holy_Charge].player.push_back(player);
	skillsinfo[Jobs::Paladin::Bw_Holy_Charge].player.push_back(player);
	// 2311003 & 9101002 - Holy Symbol
	player.type = 0x1;
	player.byte = Type_4;
	player.value = Skill_X;
	skillsinfo[Jobs::Priest::Holy_Symbol].player.push_back(player); // Priest
	skillsinfo[Jobs::SuperGM::Holy_Symbol].player.push_back(player); // Super GM
	// 1121010 - Enrage
	player.type = 0x1;
	player.byte = Type_1;
	player.value = Skill_Watk;
	skillsinfo[Jobs::Hero::Enrage].player.push_back(player);
	// 3121008 - Concentrate
	player.type = 0x1;
	player.byte = Type_1;
	player.value = Skill_Watk;
	skillsinfo[Jobs::Bowmaster::Concentrate].player.push_back(player);
	// 4211003 - Pickpocket
	player.type = 0x8;
	player.byte = Type_4;
	player.value = Skill_X;
	skillsinfo[Jobs::ChiefBandit::Pickpocket].player.push_back(player);
	// 4111001 - Meso Up
	player.type = 0x8;
	player.byte = Type_4;
	player.value = Skill_X;
	skillsinfo[Jobs::Hermit::Meso_Up].player.push_back(player);
	// 4121006 - Shadow Claw
	player.type = 0x1;
	player.byte = Type_6;
	player.value = Skill_X;
	skillsinfo[Jobs::NightLord::Shadow_Claw].player.push_back(player);
	// 2121004, 2221004, & 2321004 - Infinity
	player.type = 0x2;
	player.byte = Type_6;
	player.value = Skill_X;
	skillsinfo[Jobs::FPArchMage::Infinity].player.push_back(player);
	skillsinfo[Jobs::ILArchMage::Infinity].player.push_back(player);
	skillsinfo[Jobs::Bishop::Infinity].player.push_back(player);
	// 1005 - Echo of Hero
	player.type = 0x80;
	player.byte = Type_6;
	player.value = Skill_X;
	skillsinfo[Jobs::Beginner::Echo_Of_Hero].player.push_back(player);
	// 1121002, 1221002, & 1321002 - Power Stance
	player.type = 0x10;
	player.byte = Type_5;
	player.value = Skill_Prop;
	skillsinfo[Jobs::Hero::Power_Stance].player.push_back(player);
	skillsinfo[Jobs::Paladin::Power_Stance].player.push_back(player);
	skillsinfo[Jobs::DarkKnight::Power_Stance].player.push_back(player);
	// 2121002, 2221002, & 2321002 - Mana Reflection
	player.type = 0x40;
	player.byte = Type_5;
	player.value = Skill_Lv;
	skillsinfo[Jobs::FPArchMage::Mana_Reflection].player.push_back(player);
	skillsinfo[Jobs::ILArchMage::Mana_Reflection].player.push_back(player);
	skillsinfo[Jobs::Bishop::Mana_Reflection].player.push_back(player);
	// 5001005 - Dash
	player.type = 0x10;
	player.byte = Type_8;
	player.value = Skill_X;
	skillsinfo[Jobs::Pirate::Dash].player.push_back(player);
	player.type = 0x20;
	player.byte = Type_8;
	player.value = Skill_Y;
	skillsinfo[Jobs::Pirate::Dash].player.push_back(player);
	// 5110001 - Energy Charge
	player.type = 0x8;
	player.byte = Type_8;
	player.value = Skill_X;
	skillsinfo[Jobs::Marauder::Energy_Charge].player.push_back(player);
	// End regular buffs

	// Begin act buffs
	// 1001 - Recovery
	player.type = 0x4;
	player.byte = Type_5;
	player.value = Skill_X;
	act.type = Act_Heal;
	act.time = 4900;
	act.value = Skill_X;
	skillsinfo[Jobs::Beginner::Recovery].player.push_back(player);
	skillsinfo[Jobs::Beginner::Recovery].act = act;
	skillsinfo[Jobs::Beginner::Recovery].bact.push_back(true);
	// 1311008 - Dragon Blood
	player.type = 0x1;
	player.byte = Type_1;
	player.value = Skill_Watk;
	skillsinfo[Jobs::DragonKnight::Dragon_Blood].player.push_back(player);
	player.type = 0x80;
	player.byte = Type_3;
	player.value = Skill_Lv;
	skillsinfo[Jobs::DragonKnight::Dragon_Blood].player.push_back(player);
	act.type = Act_Hurt;
	act.time = 4000;
	act.value = Skill_X;
	skillsinfo[Jobs::DragonKnight::Dragon_Blood].act = act;
	skillsinfo[Jobs::DragonKnight::Dragon_Blood].bact.push_back(true);
	// End act buffs

	// Begin map buffs
	// 4001003 - Dark Sight
	player.type = 0x80;
	player.byte = Type_1;
	player.value = Skill_Speed;
	skillsinfo[Jobs::Rogue::Dark_Sight].player.push_back(player);
	player.type = 0x04;
	player.byte = Type_2;
	player.value = Skill_X;
	player.hasmapval = true;
	skillsinfo[Jobs::Rogue::Dark_Sight].player.push_back(player);
	// 3101004 & 3201004 - Soul Arrow
	player.type = 0x1;
	player.byte = Type_3;
	player.value = Skill_X;
	player.hasmapval = true;
	skillsinfo[Jobs::Hunter::Soul_Arrow].player.push_back(player);
	skillsinfo[Jobs::Crossbowman::Soul_Arrow].player.push_back(player);
	// 5101007 - Oak Barrel 
	player.type = 0x02;
	player.byte = Type_5;
	player.value = Skill_Morph;
	player.hasmapval = true;
	skillsinfo[Jobs::Infighter::Oak_Barrel].player.push_back(player);
	// 5111005, 5121003 - Transformation & Super Transformation
	player.type = 0x02;
	player.byte = Type_1;
	player.value = Skill_Wdef;
	player.hasmapval = false;
	skillsinfo[Jobs::Marauder::Transformation].player.push_back(player);
	skillsinfo[Jobs::Buccaneer::Super_Transformation].player.push_back(player);
	player.type = 0x08;
	player.byte = Type_1;
	player.value = Skill_Mdef;
	skillsinfo[Jobs::Marauder::Transformation].player.push_back(player);
	skillsinfo[Jobs::Buccaneer::Super_Transformation].player.push_back(player);
	player.type = 0x80;
	player.byte = Type_1;
	player.value = Skill_Speed;
	skillsinfo[Jobs::Marauder::Transformation].player.push_back(player);
	skillsinfo[Jobs::Buccaneer::Super_Transformation].player.push_back(player);
	player.type = 0x1;
	player.byte = Type_2;
	player.value = Skill_Jump;
	skillsinfo[Jobs::Marauder::Transformation].player.push_back(player);
	skillsinfo[Jobs::Buccaneer::Super_Transformation].player.push_back(player);
	player.type = 0x02;
	player.byte = Type_5;
	player.value = Skill_Morph;
	player.hasmapval = true;
	skillsinfo[Jobs::Marauder::Transformation].player.push_back(player);
	skillsinfo[Jobs::Buccaneer::Super_Transformation].player.push_back(player);
	// 4111002 - Shadow Partner
	player.type = 0x4;
	player.byte = Type_4;
	player.value = Skill_X;
	player.hasmapval = true;
	skillsinfo[Jobs::Hermit::Shadow_Partner].player.push_back(player);
	// 1111002 - Combo Attack
	player.type = 0x20;
	player.byte = Type_3;
	player.value = Skill_X;
	//player.hasmapval = true;
	//player.useval = true;
	skillsinfo[Jobs::Crusader::Combo_Attack].player.push_back(player);
	// End map buffs

	// Begin mount buffs
	player = BuffInfo(); // Placed to clear any previous value pushes
	// 1004 - Monster Rider
	player.type = 0x40;
	player.byte = Type_8;
	player.value = Skill_X;
	player.ismount = true;
	skillsinfo[Jobs::Beginner::Monster_Rider].player.push_back(player);
	// 5221006 - Battleship
	player.type = 0x40;
	player.byte = Type_8;
	player.value = Skill_X;
	player.ismount = true;
	skillsinfo[Jobs::Corsair::Battleship].player.push_back(player);
	// End mount buffs
}

int16_t Buffs::getValue(int8_t value, int32_t skillid, uint8_t level) {
	int16_t rvalue = 0;
	switch (value) {
		case Skill_X: rvalue = Skills::skills[skillid][level].x; break;
		case Skill_Y: rvalue = Skills::skills[skillid][level].y; break;
		case Skill_Speed: rvalue = Skills::skills[skillid][level].speed; break;
		case Skill_Jump: rvalue = Skills::skills[skillid][level].jump; break;
		case Skill_Watk: rvalue = Skills::skills[skillid][level].watk; break;
		case Skill_Wdef: rvalue = Skills::skills[skillid][level].wdef; break;
		case Skill_Matk: rvalue = Skills::skills[skillid][level].matk; break;
		case Skill_Mdef: rvalue = Skills::skills[skillid][level].mdef; break;
		case Skill_Acc: rvalue = Skills::skills[skillid][level].acc; break;
		case Skill_Avo: rvalue = Skills::skills[skillid][level].avo; break;
		case Skill_Prop: rvalue = Skills::skills[skillid][level].prop; break;
		case Skill_Morph: rvalue = Skills::skills[skillid][level].morph; break;
		case Skill_Lv: rvalue = level; break;
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
		if (skillid == Jobs::Rogue::Dark_Sight && level == 20 && val == Skill_Speed) { // Cancel speed change for maxed dark sight
			playerskill.types[Type_1] = 0;
			continue;
		}
		int16_t value = 0;
		switch (skillid) {
			case Jobs::Beginner::Monster_Rider: // Monster Rider
				mountid = player->getInventory()->getEquippedID(18);
				break;
			case Jobs::Corsair::Battleship:
				mountid = 1932000; // Battleship item ID
				break;
			case Jobs::Bowmaster::Sharp_Eyes: // Sharp Eyes
			case Jobs::Marksman::Sharp_Eyes: // Sharp Eyes
			case 4111002: // Shadow Partner
				value = Skills::skills[skillid][level].x * 256 + Skills::skills[skillid][level].y;
				break;
			case Jobs::Crusader::Combo_Attack: // Combo
				player->getActiveBuffs()->setCombo(0, false);
				value = 1;
				break;
			case Jobs::NightLord::Shadow_Claw: // Shadow Claw
				value = (player->getInventory()->doShadowClaw() % 10000) + 1;
				break;
			case Jobs::Marauder::Transformation: // Transformation
			case Jobs::Buccaneer::Super_Transformation: // Super Transformation
				value = getValue(val, skillid, level) + (player->getGender() * 100); // Females are +100
				break;
			case Jobs::Marauder::Energy_Charge: // Energy Charge
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
		if (skillid == Jobs::Rogue::Dark_Sight && level == 20 && val == Skill_Speed) { // Cancel speed update for maxed dark sight
			mapskill.types[Type_1] = 0;
			continue;
		}
		int16_t value = 0;
		switch (skillid) {
			case Jobs::Hermit::Shadow_Partner: // Shadow Partner
				value = Skills::skills[skillid][level].x * 256 + Skills::skills[skillid][level].y;
				break;
			case Jobs::Crusader::Combo_Attack: // Combo Attack
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
		case Jobs::Beginner::Monster_Rider: // Monster Rider
			if (mountid == 0) {
				// Hacking
				return true;
			}
			break;
		case Jobs::SuperGM::Hide: // GM Hide
			time = 2100000;
			break;
		case Jobs::Spearman::Hyper_Body: // Hyper Body
		case Jobs::SuperGM::Hyper_Body: // GM Hyper Body
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
		else if (skillid == Jobs::Marauder::Energy_Charge)
			BuffsPacket::usePirateBuff(player, 0, (player->getActiveBuffs()->getEnergyChargeLevel() == 10000 ? time : 0), playerskill);
		else if (skillid == Jobs::Buccaneer::Speed_Infusion) // SIGH
			BuffsPacket::useSpeedInfusion(player, time, playerskill, addedinfo);
		else
			BuffsPacket::useSkill(player, skillid, time, playerskill, addedinfo);
	}
	if (skillid != Jobs::Marauder::Energy_Charge || playerbuffs->getEnergyChargeLevel() == 10000) {
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
		case Jobs::Spearman::Hyper_Body: // Hyper Body
		case Jobs::SuperGM::Hyper_Body: // GM Hyper Body
			player->setHyperBody(0, 0);
			player->setHP(player->getHP());
			player->setMP(player->getMP());
			break;
		case Jobs::Marauder::Energy_Charge: // Energy Charge
			player->getActiveBuffs()->resetEnergyChargeLevel();
			break;
	}
	BuffsPacket::endSkill(player, player->getActiveBuffs()->getBuffInfo(skill));
	player->getActiveBuffs()->deleteSkillMapEnterInfo(skill);
	player->getActiveBuffs()->setActiveSkillLevel(skill, 0);
}
