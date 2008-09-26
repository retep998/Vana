/*
Copyright (C) 2008 Vana Development Team

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
#include "Inventory.h"
#include "Player.h"
#include "Skills.h"
#include "SkillsPacket.h"

unordered_map<int32_t, SkillsInfo> Buffs::skillsinfo;

void Buffs::init() {
	BuffInfo player;
	SkillAct act;
	// Boosters
	player.type = 0x08;
	player.byte = TYPE_2;
	player.value = SKILL_X;
	skillsinfo[1101004].player.push_back(player); // Sword Booster
	skillsinfo[1101005].player.push_back(player); // Axe Booster
	skillsinfo[1201004].player.push_back(player); // Sword Booster
	skillsinfo[1201005].player.push_back(player); // Blunt Booster
	skillsinfo[1301004].player.push_back(player); // Spear Booster
	skillsinfo[1301005].player.push_back(player); // Polearm Booster
	skillsinfo[2111005].player.push_back(player); // Spell Booster
	skillsinfo[2211005].player.push_back(player); // Spell Booster
	skillsinfo[3101002].player.push_back(player); // Bow Booster
	skillsinfo[3201002].player.push_back(player); // Crossbow Booster
	skillsinfo[4101003].player.push_back(player); // Claw Booster
	skillsinfo[4201002].player.push_back(player); // Dagger Booster
	skillsinfo[5101006].player.push_back(player); // Knuckle Booster
	skillsinfo[5201003].player.push_back(player); // Gun Booster
	// 1001 - Recovery
	player.type = 0x4;
	player.byte = TYPE_5;
	player.value = SKILL_X;
	act.type = ACT_HEAL;
	act.time = 4900;
	act.value = SKILL_X;
	skillsinfo[1001].player.push_back(player);
	skillsinfo[1001].act = act;
	skillsinfo[1001].bact.push_back(true);
	// 1002 - Nimble Feet
	player.type = 0x80;
	player.byte = TYPE_1;
	player.value = SKILL_SPEED;
	skillsinfo[1002].player.push_back(player);
	// 1001003 - Iron Body
	player.type = 0x02;
	player.byte = TYPE_1;
	player.value = SKILL_WDEF;
	skillsinfo[1001003].player.push_back(player);
	// 2001002 - Magic Guard
	player.type = 0x02;
	player.byte = TYPE_2;
	player.value = SKILL_X;
	skillsinfo[2001002].player.push_back(player);
	// 2001003 - Magic Armor
	player.type = 0x02;
	player.byte = TYPE_1;
	player.value = SKILL_WDEF;
	skillsinfo[2001003].player.push_back(player);
	// 3001003 - Focus
	player.type = 0x10;
	player.byte = TYPE_1;
	player.value = SKILL_ACC;
	skillsinfo[3001003].player.push_back(player);
	player.type = 0x20;
	player.byte = TYPE_1;
	player.value = SKILL_AVO;
	skillsinfo[3001003].player.push_back(player);
	// 1101006 - Rage
	player.type = 0x1;
	player.byte = TYPE_1;
	player.value = SKILL_WATK;
	skillsinfo[1101006].player.push_back(player);
	player.type = 0x2;
	player.byte = TYPE_1;
	player.value = SKILL_WDEF;
	skillsinfo[1101006].player.push_back(player);
	// 1101007 & 1201007 - Power Guard
	player.type = 0x10;
	player.byte = TYPE_2;
	player.value = SKILL_X;
	skillsinfo[1101007].player.push_back(player);
	skillsinfo[1201007].player.push_back(player);
	// 1301006 - Iron Will
	player.type = 0x2;
	player.byte = TYPE_1;
	player.value = SKILL_WDEF;
	skillsinfo[1301006].player.push_back(player);
	player.type = 0x8;
	player.byte = TYPE_1;
	player.value = SKILL_MDEF;
	skillsinfo[1301006].player.push_back(player);
	// 1301007 & 9101008 - Hyper Body, GM Hyper Body
	player.type = 0x20;
	player.byte = TYPE_2;
	player.value = SKILL_X;
	skillsinfo[1301007].player.push_back(player);
	skillsinfo[9101008].player.push_back(player);
	player.type = 0x40;
	player.byte = TYPE_2;
	player.value = SKILL_Y;
	skillsinfo[1301007].player.push_back(player);
	skillsinfo[9101008].player.push_back(player);
	// 2101001 & 2201001 - Meditation
	player.type = 0x04;
	player.byte = TYPE_1;
	player.value = SKILL_MATK;
	skillsinfo[2101001].player.push_back(player);
	skillsinfo[2201001].player.push_back(player);
	// 2301003 - Invincible
	player.type = 0x80;
	player.byte = TYPE_2;
	player.value = SKILL_X;
	skillsinfo[2301003].player.push_back(player);
	// 2301004 - Bless
	player.type = 0x2;
	player.byte = TYPE_1;
	player.value = SKILL_WDEF;
	skillsinfo[2301004].player.push_back(player);
	player.type = 0x8;
	player.byte = TYPE_1;
	player.value = SKILL_MDEF;
	skillsinfo[2301004].player.push_back(player);
	player.type = 0x10;
	player.byte = TYPE_1;
	player.value = SKILL_ACC;
	skillsinfo[2301004].player.push_back(player);
	player.type = 0x20;
	player.byte = TYPE_1;
	player.value = SKILL_AVO;
	skillsinfo[2301004].player.push_back(player);
	// 9101003 - GM Bless
	player.type = 0x1;
	player.byte = TYPE_1;
	player.value = SKILL_WATK;
	skillsinfo[9101003].player.push_back(player);
	player.type = 0x2;
	player.byte = TYPE_1;
	player.value = SKILL_WDEF;
	skillsinfo[9101003].player.push_back(player);
	player.type = 0x4;
	player.byte = TYPE_1;
	player.value = SKILL_MATK;
	skillsinfo[9101003].player.push_back(player);
	player.type = 0x8;
	player.byte = TYPE_1;
	player.value = SKILL_MDEF;
	skillsinfo[9101003].player.push_back(player);
	player.type = 0x10;
	player.byte = TYPE_1;
	player.value = SKILL_ACC;
	skillsinfo[9101003].player.push_back(player);
	player.type = 0x20;
	player.byte = TYPE_1;
	player.value = SKILL_AVO;
	skillsinfo[9101003].player.push_back(player);
	// 9101004 - GM Hide
	player.type = 0x01;
	player.byte = TYPE_8;
	player.value = 0;
	skillsinfo[9101004].player.push_back(player);
	// 4101004, 4201003, 9001000, 9101001 - Haste
	player.type = 0x80;
	player.byte = TYPE_1;
	player.value = SKILL_SPEED;
	skillsinfo[4101004].player.push_back(player);
	skillsinfo[4201003].player.push_back(player);
	skillsinfo[9001000].player.push_back(player);
	skillsinfo[9101001].player.push_back(player);
	player.type = 0x1;
	player.byte = TYPE_2;
	player.value = SKILL_JUMP;
	skillsinfo[4101004].player.push_back(player);
	skillsinfo[4201003].player.push_back(player);
	skillsinfo[9001000].player.push_back(player);
	skillsinfo[9101001].player.push_back(player);
	// 4211005 - Meso Guard
	player.type = 0x10;
	player.byte = TYPE_4;
	player.value = SKILL_X;
	skillsinfo[4211005].player.push_back(player);
	// 1311008 - Dragon Blood
	player.type = 0x1;
	player.byte = TYPE_1;
	player.value = SKILL_WATK;
	skillsinfo[1311008].player.push_back(player);
	player.type = 0x80;
	player.byte = TYPE_3;
	player.value = SKILL_LV;
	skillsinfo[1311008].player.push_back(player);
	act.type = ACT_HURT;
	act.time = 4000;
	act.value = SKILL_X;
	skillsinfo[1311008].act = act;
	skillsinfo[1311008].bact.push_back(true);
	// 3121002 & 3221002 - Sharp Eyes
	player.type = 0x20;
	player.byte = TYPE_5;
	player.value = SKILL_X;
	skillsinfo[3121002].player.push_back(player);
	skillsinfo[3221002].player.push_back(player);
	// WK/Paladin Charges - 1211003, 1211004, 1211005, 1211006, 1211007, 1211008, 1221003, 1221004
	player.type = 0x4;
	player.byte = TYPE_1;
	player.value = SKILL_MATK;
	skillsinfo[1211003].player.push_back(player);
	skillsinfo[1211004].player.push_back(player);
	skillsinfo[1211005].player.push_back(player);
	skillsinfo[1211006].player.push_back(player);
	skillsinfo[1211007].player.push_back(player);
	skillsinfo[1211008].player.push_back(player);
	skillsinfo[1221003].player.push_back(player);
	skillsinfo[1221004].player.push_back(player);
	player.type = 0x40;
	player.byte = TYPE_3;
	player.value = SKILL_MATK;
	skillsinfo[1211003].player.push_back(player);
	skillsinfo[1211004].player.push_back(player);
	skillsinfo[1211005].player.push_back(player);
	skillsinfo[1211006].player.push_back(player);
	skillsinfo[1211007].player.push_back(player);
	skillsinfo[1211008].player.push_back(player);
	skillsinfo[1221003].player.push_back(player);
	skillsinfo[1221004].player.push_back(player);
	// Maple Warrior - All Classes
	player.type = 0x8;
	player.byte = TYPE_5;
	player.value = SKILL_X;
	skillsinfo[1121000].player.push_back(player);
	skillsinfo[1221000].player.push_back(player);
	skillsinfo[1321000].player.push_back(player);
	skillsinfo[2121000].player.push_back(player);
	skillsinfo[2221000].player.push_back(player);
	skillsinfo[2321000].player.push_back(player);
	skillsinfo[3121000].player.push_back(player);
	skillsinfo[3221000].player.push_back(player);
	skillsinfo[4121000].player.push_back(player);
	skillsinfo[4221000].player.push_back(player);
	skillsinfo[5121000].player.push_back(player);
	skillsinfo[5221000].player.push_back(player);
	// 2311003 & 9101002 - Holy Symbol
	player.type = 0x1;
	player.byte = TYPE_4;
	player.value = SKILL_X;
	skillsinfo[2311003].player.push_back(player); // Priest
	skillsinfo[9101002].player.push_back(player); // Super GM
	// 1121010 - Enrage
	player.type = 0x1;
	player.byte = TYPE_1;
	player.value = SKILL_WATK;
	skillsinfo[1121010].player.push_back(player);
	// 3121008 - Concentration
	player.type = 0x1;
	player.byte = TYPE_1;
	player.value = SKILL_WATK;
	skillsinfo[3121008].player.push_back(player);
	// 4211003 - Pickpocket - TODO: Add server-side to make it drop mesos
	player.type = 0x8;
	player.byte = TYPE_4;
	player.value = SKILL_X;
	skillsinfo[4211003].player.push_back(player);
	// 4111001 - Meso Up
	player.type = 0x8;
	player.byte = TYPE_4;
	player.value = SKILL_X;
	skillsinfo[4111001].player.push_back(player);
	// 4121006 - Shadow Claw
	player.type = 0x1;
	player.byte = TYPE_6;
	player.value = SKILL_X;
	skillsinfo[4121006].player.push_back(player);
	// 2121004, 2221004, 2321004 - Infinity
	player.type = 0x2;
	player.byte = TYPE_6;
	player.value = SKILL_X;
	skillsinfo[2121004].player.push_back(player);
	skillsinfo[2221004].player.push_back(player);
	skillsinfo[2321004].player.push_back(player);
	// 1005 - Echo of Hero
	player.type = 0x80;
	player.byte = TYPE_6;
	player.value = SKILL_X;
	skillsinfo[1005].player.push_back(player);
	// Stance - 1121002, 1221002, and 1321002
	player.type = 0x10;
	player.byte = TYPE_5;
	player.value = SKILL_PROP;
	skillsinfo[1121002].player.push_back(player);
	skillsinfo[1221002].player.push_back(player);
	skillsinfo[1321002].player.push_back(player);
	// Mana Reflection - 2121002, 2221002, and 2321002
	player.type = 0x40;
	player.byte = TYPE_5;
	player.value = SKILL_LV;
	skillsinfo[2121002].player.push_back(player);
	skillsinfo[2221002].player.push_back(player);
	skillsinfo[2321002].player.push_back(player);
	// 1111002 - Combo Attack
	player.type = 0x20;
	player.byte = TYPE_3;
	player.value = SKILL_X;
	//player.hasmapval = true;
	//player.mapval = false;
	skillsinfo[1111002].player.push_back(player);
	// 4001003 - Dark Sight
	player.type = 0x80;
	player.byte = TYPE_1;
	player.value = SKILL_SPEED;
	skillsinfo[4001003].player.push_back(player);
	player.type = 0x04;
	player.byte = TYPE_2;
	player.value = SKILL_X;
	player.hasmapval = true;
	player.mapval = false;
	skillsinfo[4001003].player.push_back(player);
	// 3101004 & 3201004 - Soul Arrow
	player.type = 0x1;
	player.byte = TYPE_3;
	player.value = SKILL_X;
	player.hasmapval = true;
	player.mapval = false;
	skillsinfo[3101004].player.push_back(player);
	skillsinfo[3201004].player.push_back(player);
	// Super Saiyan thing[blue] - 5111005
	player.type = 0x02;
	player.byte = TYPE_5;
	player.value = SKILL_MORPH;
	player.hasmapval = true;
	player.mapval = false;
	skillsinfo[5111005].player.push_back(player);
	// Super Saiyan thing[orange] - 5121003
	player.type = 0x02;
	player.byte = TYPE_5;
	player.value = SKILL_MORPH;
	player.hasmapval = true;
	player.mapval = false;
	skillsinfo[5121003].player.push_back(player);
	// 4111002 - Shadow Partner
	player.type = 0x4;
	player.byte = TYPE_4;
	player.value = SKILL_X;
	player.hasmapval = true;
	player.mapval = false;
	skillsinfo[4111002].player.push_back(player);
	// 1004 - Monster Rider
	player.type = 0x40;
	player.byte = TYPE_8;
	player.value = SKILL_X;
	player.hasmapval = true;
	player.showmaponly = true;
	player.mapval = false;
	skillsinfo[1004].player.push_back(player);
	// Battleship - 5221006
	player.type = 0x40;
	player.byte = TYPE_8;
	player.value = SKILL_X;
	player.hasmapval = true;
	player.showmaponly = true;
	player.mapval = false;
	skillsinfo[5221006].player.push_back(player);

}

bool Buffs::isBuff(int32_t skillid) {
	if (skillsinfo.find(skillid) == skillsinfo.end())
		return false;
	return true;
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
		if (skillid == 4001003 && level == 20 && val == SKILL_SPEED) { // Cancel speed change for maxed dark sight
			playerskill.types[TYPE_1] = 0;
			continue;
		}
		int16_t value = 0;
		switch (skillid) {
			case 1004: // Monster Rider
				mountid = player->getInventory()->getEquippedID(18);
				break;
			case 5221006:
				mountid = 1932000; // Battleship item ID
				break;
			case 3121002: // Sharp Eyes
			case 3221002: // Sharp Eyes
			case 4111002: // Shadow Partner
				value = Skills::skills[skillid][level].x * 256 + Skills::skills[skillid][level].y;
				break;
			case 1111002: // Combo
				player->getActiveBuffs()->setCombo(0, false);
				value = 1;
				break;
			case 4121006: { // Shadow Claw
				for (int16_t s = 1; s <= player->getInventory()->getMaxSlots(2); s++) {
					Item *item = player->getInventory()->getItem(2, s);
					if (item == 0)
						continue;
					if (ISSTAR(item->id) && item->amount >= 200) {
						Inventory::takeItemSlot(player, 2, s, 200);
						value = (item->id % 10000) + 1;
						break;
					}
				}
				break;
			}
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
		if (!cur.hasmapval)
			continue;
		mapskill.types[cur.byte] += cur.type;
		int8_t val = cur.value;
		if (skillid == 4001003 && level == 20 && val == SKILL_SPEED) { // Cancel speed update for maxed dark sight
			mapskill.types[TYPE_1] = 0;
			continue;
		}
		int16_t value = 0;
		switch (skillid) {
			case 4111002: // Shadow Partner
				value = Skills::skills[skillid][level].x * 256 + Skills::skills[skillid][level].y;
				break;
			case 1111002: // Combo Attack
				value = player->getActiveBuffs()->getCombo() + 1;
				break;
			default:
				value = getValue(val, skillid, level);
				break;
		}
		mapskill.vals.push_back(value);
		if (!cur.showmaponly) {
			SkillMapActiveInfo map;
			map.byte = cur.byte;
			map.type = cur.type;
			if (cur.mapval) {
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

void Buffs::addBuff(Player *player, int32_t skillid, uint8_t level, int16_t addedinfo) {
	int32_t mountid = 0;
	PlayerActiveBuffs *playerbuffs = player->getActiveBuffs();
	vector<SkillMapActiveInfo> mapenterskill;
	SkillActiveInfo playerskill = parseBuffInfo(player, skillid, level, mountid);
	SkillActiveInfo mapskill = parseBuffMapInfo(player, skillid, level, mapenterskill);
	int32_t time = Skills::skills[skillid][level].time;
	switch (skillid) {
		case 1004: // Monster Rider
			if (mountid == 0) {
				// Hacking
				return;
			}
			break;
		case 9101004: // GM Hide
			time = 2100000;
			break;
		case 1301007: // Hyper Body
		case 9101008: // GM Hyper Body
			// TODO: Party
			player->setHyperBody(Skills::skills[skillid][level].x, Skills::skills[skillid][level].y);
			break;
		case 1121010: // Enrage
			if (playerbuffs->getCombo() != 10)
				return;
			playerbuffs->setCombo(0, true);
			break;
	}
	SkillsPacket::useSkill(player, skillid, time * 1000, playerskill, mapskill, addedinfo, mountid);
	playerbuffs->setBuffInfo(skillid, playerskill);
	playerbuffs->setBuffMapInfo(skillid, mapskill);
	playerbuffs->setSkillMapEnterInfo(skillid, mapenterskill);
	playerbuffs->setActiveSkillLevel(skillid, level);
	playerbuffs->removeBuff(skillid);
	if (Buffs::skillsinfo[skillid].bact.size() > 0) {
		int16_t value = getValue(skillsinfo[skillid].act.value, skillid, level);
		playerbuffs->addAct(skillid, skillsinfo[skillid].act.type, value, skillsinfo[skillid].act.time);
	}
	playerbuffs->addBuff(skillid, time);
}

void Buffs::stopAllBuffs(Player *player) {
	player->getActiveBuffs()->removeBuff();
}

void Buffs::endBuff(Player *player, int32_t skill) {
	switch (skill) {
		case 1301007: // Hyper Body
		case 9101008: // GM Hyper Body
			player->setHyperBody(0, 0);
			player->setHP(player->getHP());
			player->setMP(player->getMP());
			break;
	}
	SkillsPacket::endSkill(player, player->getActiveBuffs()->getBuffInfo(skill), player->getActiveBuffs()->getBuffMapInfo(skill));
	player->getActiveBuffs()->deleteSkillMapEnterInfo(skill);
	player->getActiveBuffs()->setActiveSkillLevel(skill, 0);
}