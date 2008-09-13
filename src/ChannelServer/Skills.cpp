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
#include "Skills.h"
#include "Inventory.h"
#include "MapPacket.h"
#include "Maps.h"
#include "Player.h"
#include "Randomizer.h"
#include "ReadPacket.h"
#include "SkillsPacket.h"
#include "Timer/Timer.h"
#include <functional>

using std::tr1::bind;

unordered_map<int32_t, SkillsLevelInfo> Skills::skills;
unordered_map<int32_t, unsigned char> Skills::maxlevels;
unordered_map<int32_t, SkillsInfo> Skills::skillsinfo;

void Skills::init() {
	// NOTE: type can be only 0x1/0x2/0x4/0x8/0x10/0x20/0x40/0x80.
	SkillPlayerInfo player;
	SkillMapInfo map;
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
	// 4001003 - Dark Sight
	player.type = 0x80;
	player.byte = TYPE_1;
	player.value = SKILL_SPEED;
	skillsinfo[4001003].player.push_back(player);
	player.type = 0x04;
	player.byte = TYPE_2;
	player.value = SKILL_X;
	skillsinfo[4001003].player.push_back(player);
	map.type = 0x04;
	map.byte = TYPE_2;
	map.value = SKILL_X;
	map.val = false;
	skillsinfo[4001003].map.push_back(map);
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
	// 3101004 & 3201004 - Soul Arrow
	player.type = 0x1;
	player.byte = TYPE_3;
	player.value = SKILL_X;
	skillsinfo[3101004].player.push_back(player);
	skillsinfo[3201004].player.push_back(player);
	// Map value for soul arrow causes DCs. Looking into the proper way to update it
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
	// 4111002 - Shadow Partner
	player.type = 0x4;
	player.byte = TYPE_4;
	player.value = SKILL_X;
	skillsinfo[4111002].player.push_back(player);
	map.type = 0x4;
	map.byte = TYPE_4;
	map.value = SKILL_X;
	map.val = false;
	skillsinfo[4111002].map.push_back(map);
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
	// 1111002 - Combo Attack
	player.type = 0x20;
	player.byte = TYPE_3;
	player.value = SKILL_X;
	skillsinfo[1111002].player.push_back(player);
	/*map.type = 0x20; // Currently causes dc issues when other people enter a map with someone using combo attack, so disable map for now
	map.byte = TYPE_3;
	map.value = SKILL_X;
	map.val = false;
	skillsinfo[1111002].map.push_back(map);*/
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
	// 4211003 - Pickpocket TODO: Add server-side to make it drop mesos
	player.type = 0x8;
	player.byte = TYPE_4;
	player.value = SKILL_X;
	skillsinfo[4211003].player.push_back(player);
	// 1004 - Monster Rider
	player.type = 0x40;
	player.byte = TYPE_8;
	player.value = SKILL_X;
	skillsinfo[1004].player.push_back(player);
	map.type = 0x40;
	map.byte = TYPE_8;
	map.value = SKILL_X;
	map.val = false;
	skillsinfo[1004].map.push_back(map);
	// 4111001 - Meso Up
	player.type = 0x8;
	player.byte = TYPE_4;
	player.value = SKILL_X;
	skillsinfo[4111001].player.push_back(player);
	// 4121006 - Shadow claw
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
	player.type = 0x1;
	player.byte = TYPE_7;
	player.value = SKILL_X;
	skillsinfo[1005].player.push_back(player);
	// Stance - 1121002, 1221002, and 1321002
	player.type = 0x10;
	player.byte = TYPE_5;
	player.value = SKILL_PROP;
	skillsinfo[1121002].player.push_back(player);
	skillsinfo[1221002].player.push_back(player);
	skillsinfo[1321002].player.push_back(player);
	// Super Sayan thing[blue] - 5111005
	player.type = 0x02;
	player.byte = TYPE_5;
	player.value = SKILL_MORPH;
	skillsinfo[5111005].player.push_back(player);
	map.type = 0x02;
	map.byte = TYPE_5;
	map.value = SKILL_MORPH;
	map.val = false;
	skillsinfo[5111005].map.push_back(map);
	// Super Sayan thing[orange] - 5121003
	player.type = 0x02;
	player.byte = TYPE_5;
	player.value = SKILL_MORPH;
	skillsinfo[5121003].player.push_back(player);
	map.type = 0x02;
	map.byte = TYPE_5;
	map.value = SKILL_MORPH;
	map.val = false;
	skillsinfo[5121003].map.push_back(map);
	// Mana Reflection - 2121002, 2221002, and 2321002
	player.type = 0x40;
	player.byte = TYPE_5;
	player.value = SKILL_LV;
	skillsinfo[2121002].player.push_back(player);
	skillsinfo[2221002].player.push_back(player);
	skillsinfo[2321002].player.push_back(player);
}

void Skills::addSkillLevelInfo(int32_t skillid, uint8_t level, SkillLevelInfo levelinfo) {
	skills[skillid][level] = levelinfo;

	if (maxlevels.find(skillid) == maxlevels.end() || maxlevels[skillid] < level) {
		maxlevels[skillid] = level;
	}
}

void Skills::addSkill(Player *player, ReadPacket *packet) {
	packet->skipBytes(4);
	int32_t skillid = packet->getInt();
	if (!BEGINNER_SKILL(skillid) && player->getSp() == 0) {
		// hacking
		return;
	}
	if (player->getSkills()->addSkillLevel(skillid, 1) && !BEGINNER_SKILL(skillid)) {
		player->setSp(player->getSp() - 1);
	}
}
void Skills::cancelSkill(Player *player, ReadPacket *packet) {
	stopSkill(player, packet->getInt());
}
void Skills::stopSkill(Player *player, int32_t skillid, bool fromTimer) {
	player->getActiveBuffs()->removeBuff(skillid, fromTimer);
	switch (skillid) {
		case 3121004:
		case 3221001:
		case 2121001:
		case 2221001:
		case 2321001:
		case 5221004: { // Special skills like hurricane, monster magnet, rapid fire, and etc
			SkillsPacket::endSpecialSkill(player, player->getSpecialSkillInfo());
			SpecialSkillInfo info;
			player->setSpecialSkill(info);
			break;
		}
		default:
			endBuff(player, skillid);
			break;
	}
}
void Skills::useSkill(Player *player, ReadPacket *packet) {
	packet->skipBytes(4); //Ticks
	int32_t skillid = packet->getInt();
	int16_t addedinfo = 0;
	uint8_t level = packet->getByte();
	uint8_t type = 0;
	switch (skillid) {
		case 1121001: // Monster Magnet processing
		case 1221001:
		case 1321001: {
			int32_t mobs = packet->getInt();
			for (int8_t k = 0; k < mobs; k++) {
				int32_t mapmobid = packet->getInt();
				uint8_t success = packet->getByte();
				SkillsPacket::showMagnetSuccess(player, mapmobid, success);
			}
			break;
		}
		default:
			type = packet->getByte();
			switch (type) {
				case 0x80:
					addedinfo = packet->getShort();
					break;
			}
			break;
	}
	if (level == 0) {
		// hacking
		return;
	}
	Skills::applySkillCosts(player, skillid, level);
	SkillsPacket::showSkill(player, skillid, level);
	if (Skills::isBuff(skillid)) {
		int32_t mountid = 0;
		PlayerActiveBuffs *playerbuffs = player->getActiveBuffs();
		vector<SkillMapActiveInfo> mapenterskill;
		SkillActiveInfo playerskill = Skills::parsePlayerSkill(player, skillid, level, mountid);
		SkillActiveInfo mapskill = Skills::parseMapSkill(player, skillid, level, mapenterskill);
		switch (skillid) {
			case 1004: // Monster Rider
				if (mountid == 0) {
					// Hacking
					return;
				}
				break;
			case 9101004: // GM Hide
				MapPacket::removePlayer(player);
				break;
			case 1301007: // Hyper Body
			case 9101008: // GM Hyper Body
				// TODO: Party
				player->setMHP(player->getRMHP() * (100 + skills[skillid][level].x) / 100);
				player->setMMP(player->getRMMP() * (100 + skills[skillid][level].y) / 100);
				break;
			case 1121010: // Enrage
				if (playerbuffs->getCombo() != 10)
					return;
				playerbuffs->setCombo(0, true);
				break;
		}
		SkillsPacket::useSkill(player, skillid, (skillid == 9101004 ? 2100000 : skills[skillid][level].time) * 1000, playerskill, mapskill, addedinfo, mountid);
		playerbuffs->setSkillPlayerInfo(skillid, playerskill);
		playerbuffs->setSkillMapInfo(skillid, mapskill);
		playerbuffs->setSkillMapEnterInfo(skillid, mapenterskill);
		playerbuffs->setActiveSkillLevel(skillid, level);
		playerbuffs->removeBuff(skillid);
		player->setSkill(playerbuffs->getSkillMapEnterInfo());
		if (skillsinfo[skillid].bact.size() > 0) {
			int16_t value = getValue(skillsinfo[skillid].act.value, skillid, level);
			playerbuffs->addAct(skillid, skillsinfo[skillid].act.type, value, skillsinfo[skillid].act.time);
		}
		playerbuffs->addBuff(skillid, level);
	}
	else { // Nonbuffs
		switch (skillid) {
			case 2301002: { // Heal
				//TODO PARTY
				uint16_t healrate = skills[skillid][level].hpP / 1;
				if (healrate > 100)
					healrate = 100;
				player->setHP(player->getHP() + healrate * player->getMHP() / 100);
				break;
			}
			case 9101000: // GM Heal + Dispel - needs to be modified for map?
				player->setHP(player->getMHP());
				player->setMP(player->getMMP());
				break;
			case 9101005: { // GM Resurrection
				for (size_t i = 0; i < Maps::maps[player->getMap()]->getNumPlayers(); i++) {
					Player *resplayer;
					resplayer = Maps::maps[player->getMap()]->getPlayer(i);
					if (resplayer->getHP() <= 0) {
						resplayer->setHP(resplayer->getMHP());
					}
				}
				break;
			}
		}
	}
}

int16_t Skills::getValue(int8_t value, int32_t skillid, uint8_t level) {
	int16_t rvalue = 0;
	switch (value) {
		case SKILL_X: rvalue = skills[skillid][level].x; break;
		case SKILL_Y: rvalue = skills[skillid][level].y; break;
		case SKILL_SPEED: rvalue = skills[skillid][level].speed; break;
		case SKILL_JUMP: rvalue = skills[skillid][level].jump; break;
		case SKILL_WATK: rvalue = skills[skillid][level].watk; break;
		case SKILL_WDEF: rvalue = skills[skillid][level].wdef; break;
		case SKILL_MATK: rvalue = skills[skillid][level].matk; break;
		case SKILL_MDEF: rvalue = skills[skillid][level].mdef; break;
		case SKILL_ACC: rvalue = skills[skillid][level].acc; break;
		case SKILL_AVO: rvalue = skills[skillid][level].avo; break;
		case SKILL_PROP: rvalue = skills[skillid][level].prop; break;
		case SKILL_MORPH: rvalue = skills[skillid][level].morph; break;
		case SKILL_LV: rvalue = level; break;
	}
	return rvalue;
}

SkillActiveInfo Skills::parsePlayerSkill(Player *player, int32_t skillid, uint8_t level, int32_t &mountid) {
	SkillActiveInfo playerskill;
	memset(playerskill.types, 0, 8 * sizeof(uint8_t)); // Reset player/map types to 0
	for (size_t i = 0; i < skillsinfo[skillid].player.size(); i++) {
		playerskill.types[skillsinfo[skillid].player[i].byte] += skillsinfo[skillid].player[i].type;
		int8_t val = skillsinfo[skillid].player[i].value;
		if (skillid == 4001003 && level == 20 && val == SKILL_SPEED) { // Cancel speed change for maxed dark sight
			playerskill.types[TYPE_1] = 0;
			continue;
		}
		int16_t value = 0;
		switch (skillid) {
			case 1004: // Monster Rider
				mountid = player->getInventory()->getEquippedID(18);
				break;
			case 3121002: // Sharp Eyes
			case 3221002: // Sharp Eyes
			case 4111002: // Shadow Partner
				value = skills[skillid][level].x * 256 + skills[skillid][level].y;
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

SkillActiveInfo Skills::parseMapSkill(Player *player, int32_t skillid, uint8_t level, vector<SkillMapActiveInfo> &mapenterskill) {
	SkillActiveInfo mapskill;
	memset(mapskill.types, 0, 8 * sizeof(uint8_t));
	for (size_t i = 0; i < skillsinfo[skillid].map.size(); i++) {
		mapskill.types[skillsinfo[skillid].map[i].byte] += skillsinfo[skillid].map[i].type;
		int8_t val = skillsinfo[skillid].map[i].value;
		if (skillid == 4001003 && level == 20 && val == SKILL_SPEED) { // Cancel speed update for maxed dark sight
			mapskill.types[TYPE_1] = 0;
			continue;
		}
		int16_t value = 0;
		switch (skillid) {
			case 4111002: // Shadow Partner
				value = skills[skillid][level].x * 256 + skills[skillid][level].y;
				break;
			case 1111002: // Combo Attack
				value = player->getActiveBuffs()->getCombo() + 1;
				break;
			default:
				value = getValue(val, skillid, level);
				break;
		}
		mapskill.vals.push_back(value);
		SkillMapActiveInfo map;
		map.byte = skillsinfo[skillid].map[i].byte;
		map.type = skillsinfo[skillid].map[i].type;
		if (skillsinfo[skillid].map[i].val) {
			map.isvalue = true;
			map.value = (char)value;
		}
		else {
			map.isvalue = false;
			map.value = 0;
		}
		map.skill = skillid;
		mapenterskill.push_back(map);
	}
	return mapskill;
}

void Skills::applySkillCosts(Player *player, int32_t skillid, uint8_t level, bool elementalamp) {
	int16_t cooltime = Skills::skills[skillid][level].cooltime;
	int16_t mpuse = skills[skillid][level].mp;
	int16_t hpuse = skills[skillid][level].hp;
	int32_t item = skills[skillid][level].item;
	if (mpuse > 0) {
		if (player->getActiveBuffs()->getActiveSkillLevel(3121008) > 0) { // Reduced MP usage for Concentration
			uint16_t mprate = Skills::skills[3121008][player->getActiveBuffs()->getActiveSkillLevel(3121008)].x;
			int16_t mploss = (mpuse * mprate) / 100;
			player->setMP(player->getMP() - mploss, true);
		}
		else {
			if (elementalamp) {
				int32_t sid = ((player->getJob() / 10) == 22 ? 2210001 : 2110001);
				int8_t slv = player->getSkills()->getSkillLevel(sid);
				if (slv > 0)
					player->setMP(player->getMP() - (mpuse * skills[sid][slv].x / 100), true);
				else
					player->setMP(player->getMP() - mpuse, true);
			}
			else
				player->setMP(player->getMP() - mpuse, true);
		}
	}
	else
		player->setMP(player->getMP(), true);
	if (hpuse > 0)
		player->setHP(player->getHP() - hpuse);
	if (item > 0)
		Inventory::takeItem(player, item, skills[skillid][level].itemcount);
	if (cooltime > 0)
		Skills::startCooldown(player, skillid, cooltime);
}

void Skills::useAttackSkill(Player *player, int32_t skillid) {
	uint8_t level = player->getSkills()->getSkillLevel(skillid);
	if (skills.find(skillid) == skills.end())
		return;
	Skills::applySkillCosts(player, skillid, level, true);
}

void Skills::useAttackSkillRanged(Player *player, int32_t skillid, int16_t pos, uint8_t display) {
	uint8_t level = player->getSkills()->getSkillLevel(skillid);
	if (skills.find(skillid) == skills.end())
		return;
	Skills::applySkillCosts(player, skillid, level);
	if (skills[skillid][level].moneycon > 0) {
		int16_t midpoint = skills[skillid][level].moneycon;
		int16_t mesos_min = midpoint - (80 + level * 5);
		int16_t mesos_max = midpoint + (80 + level * 5);
		int16_t difference = mesos_max - mesos_min; // Randomize up to this, add minimum for range
		int16_t amount = Randomizer::Instance()->randShort(difference) + mesos_min;
		int32_t mesos = player->getInventory()->getMesos();
		if (mesos - amount > -1) 
			player->getInventory()->setMesos(mesos - amount);
		else {
			// Hacking
			return;
		}
	}
	uint16_t hits = 1;
	if (skills[skillid][level].bulletcon > 0)
		hits = skills[skillid][level].bulletcon;
	if (display == 0x08)
		hits = hits * 2;
	if (pos > 0 && (!((display & 0x40) == 0x40 || display == 0x02)))
		// Display is 0x40 for Shadow Claw and 0x48 for Shadow Claw + Shadow Partner
		// Bitwise and with 0x40 will make it 0x40 for both
		Inventory::takeItemSlot(player, 2, pos, hits);
}

void Skills::useAttackRanged(Player *player, int16_t pos, uint8_t display) {
	uint16_t hits = 1;
	if (display == 0x08)
		hits = hits * 2;
	if (pos > 0 && (!((display & 0x40) == 0x40 || display == 0x02))) // See previous comment
		Inventory::takeItemSlot(player, 2, pos, hits);
}

void Skills::endBuff(Player *player, int32_t skill) {
	switch (skill) {
		case 1301007: // Hyper Body
		case 9101008: // GM Hyper Body
			player->setMHP(player->getRMHP());
			player->setMMP(player->getRMMP());
			player->setHP(player->getHP());
			player->setMP(player->getMP());
			break;
		case 9101004: // GM Hide
			MapPacket::showPlayer(player);
			break;
	}
	SkillsPacket::endSkill(player, player->getActiveBuffs()->getSkillPlayerInfo(skill), player->getActiveBuffs()->getSkillMapInfo(skill));
	player->getActiveBuffs()->deleteSkillMapEnterInfo(skill);
	player->getActiveBuffs()->setActiveSkillLevel(skill, 0);
}

void Skills::stopAllBuffs(Player *player) {
	player->getActiveBuffs()->removeBuff();
}

void Skills::heal(Player *player, int16_t value, int32_t skillid) {
	if (player->getHP() < player->getMHP() && player->getHP() > 0) {
		player->setHP(player->getHP() + value);
		SkillsPacket::healHP(player, value); 
	}
}

void Skills::hurt(Player *player, int16_t value, int32_t skillid) {
	if (player->getHP() - value > 1) {
		player->setHP(player->getHP() - value);
		SkillsPacket::showSkillEffect(player, skillid);
	}
	else {
		Skills::endBuff(player, skillid);
	}
}

void Skills::startCooldown(Player *player, int32_t skillid, int16_t cooltime) {
	if (Skills::isCooling(player, skillid)) {
		// Hacking
		return;
	}
	SkillsPacket::sendCooldown(player, skillid, cooltime);

	new Timer::Timer(bind(&Skills::stopCooldown, player,
		skillid), Timer::Id(Timer::Types::CoolTimer,
		skillid, 0), player->getTimers(), cooltime * 1000, false);
}

void Skills::stopCooldown(Player *player, int32_t skillid) {
	SkillsPacket::sendCooldown(player, skillid, 0);	
}

bool Skills::isCooling(Player *player, int32_t skillid) {
	Timer::Id id(Timer::Types::CoolTimer, skillid, 0);
	if (player->getTimers()->checkTimer(id))
		return true;
	return false;
}

bool Skills::isBuff(int32_t skillid) {
	if (skillsinfo.find(skillid) == skillsinfo.end())
		return false;
	return true;
}