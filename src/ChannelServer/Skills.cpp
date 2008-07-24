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
#include "Player.h"
#include "SkillsPacket.h"
#include "Inventory.h"
#include "Maps.h"
#include "MapPacket.h"
#include "Drops.h"
#include "Randomizer.h"
#include "Timer.h"
#include "ReadPacket.h"

hash_map <int, SkillsLevelInfo> Skills::skills;
hash_map <int, SkillsInfo> Skills::skillsinfo;

#define BEGINNER_SKILL(x) (x<1003)

class SkillTimer : public Timer::TimerHandler {
public:
	static SkillTimer * Instance() {
		if (singleton == 0)
			singleton = new SkillTimer;
		return singleton;
	}
	void setSkillTimer(Player *player, int skill, int time) {
		STimer timer;
		timer.id = Timer::Instance()->setTimer(time, this);
		timer.player = player;
		timer.skill = skill;
		timer.time = time;
		timers.push_back(timer);
		act[timer.id] = false;
	}
	void setSkillTimer(Player *player, int skill, char *name, short value, int time) {
		SActTimer timer;
		timer.id = Timer::Instance()->setTimer(time, this);
		timer.player = player;
		timer.skill = skill;
		strcpy_s(timer.act, 50, name);
		timer.time = time;
		timer.value = value;
		acttimers.push_back(timer);
		act[timer.id] = true;
	}
	void stop (Player *player, int skill) {
		for (size_t i = 0; i < timers.size(); i++) {
			if (player == timers[i].player && timers[i].skill == skill) {
				Timer::Instance()->cancelTimer(timers[i].id);
				break;
			}
		}
	}
	void stop (Player *player, int skill, char *name) {
		for (size_t i = 0; i < acttimers.size(); i++) {
			if (player == acttimers[i].player && strcmp(acttimers[i].act, name) == 0 && skill == acttimers[i].skill) {
				Timer::Instance()->cancelTimer(acttimers[i].id);
				break;
			}
		}
	}
	void stop (Player *player) {
		for (size_t i = timers.size(); i > 0; i--) {
			if (player == timers[i-1].player) {
				Timer::Instance()->cancelTimer(timers[i-1].id);
			}
		} 
		for (size_t i = acttimers.size(); i > 0; i--) { 
			if (player == acttimers[i-1].player) {
				Timer::Instance()->cancelTimer(acttimers[i-1].id);
			}
		} 
	}
	void stopSkills (Player *player) {
		for (size_t i = timers.size(); i > 0; i--) {
			if (player == timers[i-1].player) {
				Skills::endSkill(player, timers[i-1].skill, true);
				Timer::Instance()->cancelTimer(timers[i-1].id);
			}
		} 
		for (size_t i = acttimers.size(); i > 0; i--) { 
			if (player == acttimers[i-1].player) {
				Timer::Instance()->cancelTimer(acttimers[i-1].id);
			}
		}
	}
	int skillTime(Player *player, int skillid) { // Get skill time
		int timeleft = 0;
		for (size_t i = 0; i < timers.size(); i++) {
			if (player == timers[i].player && timers[i].skill == skillid) {
				timeleft = Timer::Instance()->timeLeft(timers[i].id);
			}
		}
		return timeleft;
	}
private:
	static SkillTimer *singleton;
	SkillTimer() {};
	SkillTimer(const SkillTimer&);
	SkillTimer& operator=(const SkillTimer&);
	struct STimer {
		int id;
		Player *player;
		int skill;
		int time;
	};
	struct SActTimer {
		int id;
		Player *player;
		int skill;
		int time;
		char act[50];
		short value;
	};
	vector <STimer> timers;
	vector <SActTimer> acttimers;
	hash_map <int, bool> act;
	void handle (Timer *timer, int id) {
		int skill;
		Player *player;
		if (act[id]) {
			char name[50];
			short value;
			for (size_t i = 0; i < acttimers.size(); i++) {
				if (acttimers[i].id == id) {
					player = acttimers[i].player;
					skill = acttimers[i].skill;
					strcpy_s(name, 50, acttimers[i].act);
					value = acttimers[i].value;
					break;
				}
			}
			if (strcmp(name, "heal") == 0) Skills::heal(player, value, skill);
			// else if (...
		}
		else {
			for (size_t i = 0; i < timers.size(); i++) {
				if (timers[i].id == id) {
					player = timers[i].player;
					skill = timers[i].skill;
					break;
				}
			}
			Skills::endSkill(player, skill);
		}
	}
	void remove (int id) {
		if (act[id]) {
			for (size_t i = 0; i < acttimers.size(); i++) {
				if (acttimers[i].id == id) {	
					acttimers.erase(acttimers.begin()+i);	
					return;
				}
			}
		}
		else {
			for (size_t i = 0; i < timers.size(); i++) {
				if (timers[i].id == id) {	
					timers.erase(timers.begin()+i);	
					return;
				}
			}
		}
		act.erase(id);
	}
};
SkillTimer * SkillTimer::singleton = 0;

class CoolTimer : public Timer::TimerHandler {
public:
	static CoolTimer * Instance() {
		if (singleton == 0)
			singleton = new CoolTimer;
		return singleton;
	}
	void setCoolTimer(Player *player, int skill, int time) {
		CTimer timer;
		timer.id = Timer::Instance()->setTimer(time, this);
		timer.player = player;
		timer.skill = (skill + 10000000);
		timer.time = time;
		timers.push_back(timer);
	}
	void stop (Player *player, int skill) {
		for (size_t i = 0; i < timers.size(); i++) {
			if (player == timers[i].player && timers[i].skill == (skill + 10000000)) {
				Timer::Instance()->cancelTimer(timers[i].id);
				break;
			}
		}
	}
	void stop (Player *player) {
		for (size_t i = timers.size(); i > 0; i--) {
			if (player == timers[i-1].player) {
				Timer::Instance()->cancelTimer(timers[i-1].id);
			}
		}
	}
	int coolTime(Player *player, int skillid) {
		int timeleft = 0;
		for (size_t i = 0; i < timers.size(); i++) {
			if (player == timers[i].player && timers[i].skill == (skillid + 10000000)) {
				timeleft = Timer::Instance()->timeLeft(timers[i].id);
			}
		}
		return timeleft;
	}
private:
	static CoolTimer *singleton;
	CoolTimer() {};
	CoolTimer(const CoolTimer&);
	CoolTimer& operator=(const CoolTimer&);
	struct CTimer {
		int id;
		Player *player;
		int skill;
		int time;
	};
	vector <CTimer> timers;
	void handle (Timer *timer, int id) {
		int skill;
		Player *player;
		for (size_t i = 0; i < timers.size(); i++) {
			if (timers[i].id == id) {
				player = timers[i].player;
				skill = timers[i].skill;
				break;
			}
		}
		Skills::stopCooldown(player, (skill - 10000000));
	}
	void remove (int id) {
		for (size_t i = 0; i < timers.size(); i++) {
			if (timers[i].id == id) {	
				timers.erase(timers.begin()+i);	
				return;
			}
		}
	}
};
CoolTimer * CoolTimer::singleton = 0;

void Skills::stopTimersPlayer(Player *player) {
	SkillTimer::Instance()->stop(player);
}

void Skills::stopAllBuffs(Player *player) {
	SkillTimer::Instance()->stopSkills(player);
}

void Skills::init() {
	// NOTE: type can be only 0x1/0x2/0x4/0x8/0x10/0x20/0x40/0x80.
	SkillPlayerInfo player;
	SkillMapInfo map;
	SkillAct act;
	//Booster
	player.type = 0x08;
	player.byte = 6;
	player.value = SKILL_X;
	//
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
	player.byte = 1;
	player.value = SKILL_X;
	strcpy_s(act.name, 50, "heal");
	act.time = 4900;
	act.value = SKILL_X;
	skillsinfo[1001].player.push_back(player);
	skillsinfo[1001].act = act;
	skillsinfo[1001].bact.push_back(true);
	// 1002 - Nimble Feet
	player.type = 0x80;
	player.byte = 5;
	player.value = SKILL_SPEED;
	map.type = 0x80;
	map.byte = 5;
	map.value = SKILL_SPEED;
	map.val = true;
	skillsinfo[1002].player.push_back(player);
	skillsinfo[1002].map.push_back(map);
	// 1001003 - Iron Body
	player.type = 0x02;
	player.byte = 5;
	player.value = SKILL_WDEF;
	skillsinfo[1001003].player.push_back(player);
	// 2001002 - Magic Guard
	player.type = 0x02;
	player.byte = 6;
	player.value = SKILL_X;
	skillsinfo[2001002].player.push_back(player);
	// 2001003 - Magic Armor
	player.type = 0x02;
	player.byte = 5;
	player.value = SKILL_WDEF;
	skillsinfo[2001003].player.push_back(player);
	// 3001003 - Focus
	player.type = 0x10;
	player.byte = 5;
	player.value = SKILL_ACC;
	skillsinfo[3001003].player.push_back(player);
	player.type = 0x20;
	player.byte = 5;
	player.value = SKILL_AVO;
	skillsinfo[3001003].player.push_back(player);
	// 4001003 - Dark Sight
	player.type = 0x80;
	player.byte = 5;
	player.value = SKILL_SPEED;
	skillsinfo[4001003].player.push_back(player);
	player.type = 0x04;
	player.byte = 6;
	player.value = SKILL_X;
	skillsinfo[4001003].player.push_back(player);
	map.type = 0x80;
	map.byte = 5;
	map.value = SKILL_SPEED;
	map.val = true;
	skillsinfo[4001003].map.push_back(map);
	map.type = 0x04;
	map.byte = 6;
	map.value = SKILL_X;
	map.val = false;
	skillsinfo[4001003].map.push_back(map);
	// 1101006 - Rage
	player.type = 0x1;
	player.byte = 5;
	player.value = SKILL_WATK;
	skillsinfo[1101006].player.push_back(player);
	player.type = 0x2;
	player.byte = 5;
	player.value = SKILL_WDEF;
	skillsinfo[1101006].player.push_back(player);
	// 1101007 & 1201007 - Power Guard
	player.type = 0x10;
	player.byte = 6;
	player.value = SKILL_X;
	skillsinfo[1101007].player.push_back(player);
	skillsinfo[1201007].player.push_back(player);
	map.type = 0x10;
	map.byte = 6;
	map.value = SKILL_X;
	map.val = false;
	skillsinfo[1101007].map.push_back(map);
	skillsinfo[1201007].map.push_back(map);
	// 1301006 - Iron Will
	player.type = 0x2;
	player.byte = 5;
	player.value = SKILL_WDEF;
	skillsinfo[1301006].player.push_back(player);
	player.type = 0x8;
	player.byte = 5;
	player.value = SKILL_MDEF;
	skillsinfo[1301006].player.push_back(player);
	// 1301007 & 9101008 - Hyper Body
	player.type = 0x20;
	player.byte = 6;
	player.value = SKILL_X;
	skillsinfo[1301007].player.push_back(player);
	skillsinfo[9101008].player.push_back(player);
	player.type = 0x40;
	player.byte = 6;
	player.value = SKILL_Y;
	skillsinfo[1301007].player.push_back(player);
	skillsinfo[9101008].player.push_back(player);
	// 2101001 & 2201001 - Meditation
	player.type = 0x04;
	player.byte = 5;
	player.value = SKILL_MATK;
	skillsinfo[2101001].player.push_back(player);
	skillsinfo[2201001].player.push_back(player);
	// 2301003 - Invincible
	player.type = 0x80;
	player.byte = 6;
	player.value = SKILL_X;
	skillsinfo[2301003].player.push_back(player);
	// 2301004 - Bless
	player.type = 0x2;
	player.byte = 5;
	player.value = SKILL_WDEF;
	skillsinfo[2301004].player.push_back(player);
	player.type = 0x8;
	player.byte = 5;
	player.value = SKILL_MDEF;
	skillsinfo[2301004].player.push_back(player);
	player.type = 0x10;
	player.byte = 5;
	player.value = SKILL_ACC;
	skillsinfo[2301004].player.push_back(player);
	player.type = 0x20;
	player.byte = 5;
	player.value = SKILL_AVO;
	skillsinfo[2301004].player.push_back(player);
	// 9101003 - GM Bless
	player.type = 0x1;
	player.byte = 5;
	player.value = SKILL_WATK;
	skillsinfo[9101003].player.push_back(player);
	player.type = 0x2;
	player.byte = 5;
	player.value = SKILL_WDEF;
	skillsinfo[9101003].player.push_back(player);
	player.type = 0x4;
	player.byte = 5;
	player.value = SKILL_MATK;
	skillsinfo[9101003].player.push_back(player);
	player.type = 0x8;
	player.byte = 5;
	player.value = SKILL_MDEF;
	skillsinfo[9101003].player.push_back(player);
	player.type = 0x10;
	player.byte = 5;
	player.value = SKILL_ACC;
	skillsinfo[9101003].player.push_back(player);
	player.type = 0x20;
	player.byte = 5;
	player.value = SKILL_AVO;
	skillsinfo[9101003].player.push_back(player);
	// 9101004 - GM Hide
	player.type = 0x40;
	player.byte = 5;
	player.value = 0;
	skillsinfo[9101004].player.push_back(player);
	// 3101004 & 3201004 - Soul Arrow
	player.type = 0x1;
	player.byte = 7;
	player.value = SKILL_X;
	skillsinfo[3101004].player.push_back(player);
	skillsinfo[3201004].player.push_back(player);
	// Map value for soul arrow causes DCs. Looking into the proper way to update it
	// 4101004, 4201003, 9001000, 9101001 - Haste
	player.type = 0x80;
	player.byte = 5;
	player.value = SKILL_SPEED;
	skillsinfo[4101004].player.push_back(player);
	skillsinfo[4201003].player.push_back(player);
	skillsinfo[9001000].player.push_back(player);
	skillsinfo[9101001].player.push_back(player);
	player.type = 0x1;
	player.byte = 6;
	player.value = SKILL_JUMP;
	skillsinfo[4101004].player.push_back(player);
	skillsinfo[4201003].player.push_back(player);
	skillsinfo[9001000].player.push_back(player);
	skillsinfo[9101001].player.push_back(player);
	map.type = 0x80;
	map.byte = 5;
	map.value = SKILL_SPEED;
	map.val = true;
	skillsinfo[4101004].map.push_back(map);
	skillsinfo[4201003].map.push_back(map);
	skillsinfo[9001000].map.push_back(map);
	skillsinfo[9101001].map.push_back(map);
	// 4211005 - Meso Guard
	player.type = 0x10;
	player.byte = 8;
	player.value = SKILL_X;
	skillsinfo[4211005].player.push_back(player);
	// 1311008 - Dragon Blood
	player.type = 0x1;
	player.byte = 5;
	player.value = SKILL_WATK;
	skillsinfo[1311008].player.push_back(player);
	// 3121002 & 3221002 - Sharp Eyes
	player.type = 0x20;
	player.byte = 1;
	player.value = SKILL_X;
	skillsinfo[3121002].player.push_back(player);
	skillsinfo[3221002].player.push_back(player);
	// 4111002 - Shadow Partner
	player.type = 0x4;
	player.byte = 8;
	player.value = SKILL_X;
	skillsinfo[4111002].player.push_back(player);
	map.type = 0x4;
	map.byte = 8;
	map.value = SKILL_X;
	map.val = false;
	skillsinfo[4111002].map.push_back(map);
	// WK/Paladin Charges - 1211003, 1211004, 1211005, 1211006, 1211007, 1211008, 1221003, 1221004
	player.type = 0x4;
	player.byte = 5;
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
	player.byte = 7;
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
	player.byte = 1;
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
	// 2311003 & 9101002 - Holy Symbol
	player.type = 0x1;
	player.byte = 8;
	player.value = SKILL_X;
	skillsinfo[2311003].player.push_back(player); // Priest
	skillsinfo[9101002].player.push_back(player); // Super GM
	// 1111002 - Combo Attack
	player.type = 0x20;
	player.byte = 7;
	player.value = SKILL_X;
	skillsinfo[1111002].player.push_back(player);
	/*map.type = 0x20; // Currently causes dc issues when other people enter a map with someone using combo attack, so disable map for now
	map.byte = 7;
	map.value = SKILL_X;
	map.val = false;
	skillsinfo[1111002].map.push_back(map);*/
	// 1121010 - Enrage
	player.type = 0x1;
	player.byte = 5;
	player.value = SKILL_WATK;
	skillsinfo[1121010].player.push_back(player);
	// 3121008 - Concentration
	player.type = 0x1;
	player.byte = 5;
	player.value = SKILL_WATK;
	skillsinfo[3121008].player.push_back(player);
	// 4211003 - Pickpocket TODO: Add server-side to make it drop mesos
	player.type = 0x8;
	player.byte = 8;
	player.value = SKILL_X;
	skillsinfo[4211003].player.push_back(player);
	// 1004 - Monster Rider
	player.type = 0x40;
	player.byte = 2;
	player.value = SKILL_X;
	skillsinfo[1004].player.push_back(player);
	map.type = 0x40;
	map.byte = 2;
	map.value = SKILL_X;
	map.val = false;
	skillsinfo[1004].map.push_back(map);
	// 4111001 - Meso Up
	player.type = 0x8;
	player.byte = 8;
	player.value = SKILL_X;
	skillsinfo[4111001].player.push_back(player);
	// 4121006 - Shadow claw
	player.type = 0x1;
	player.byte = 2;
	player.value = SKILL_X;
	skillsinfo[4121006].player.push_back(player);
	// 2121004, 2221004, 2321004 - Infinity
	player.type = 0x2;
	player.byte = 2;
	player.value = SKILL_X;
	skillsinfo[2121004].player.push_back(player);
	skillsinfo[2221004].player.push_back(player);
	skillsinfo[2321004].player.push_back(player);
	// 1005 - Echo of Hero
	player.type = 0x1;
	player.byte = 3;
	player.value = SKILL_X;
	skillsinfo[1005].player.push_back(player);
	// Stance - 1121002, 1221002, and 1321002
	player.type = 0x10;
	player.byte = 1;
	player.value = SKILL_PROP;
	skillsinfo[1121002].player.push_back(player);
	skillsinfo[1221002].player.push_back(player);
	skillsinfo[1321002].player.push_back(player);
	// Super Sayan thing[blue] - 5111005
	player.type = 0x02;
	player.byte = 1;
	player.value = SKILL_MORPH;
	skillsinfo[5111005].player.push_back(player);
	map.type = 0x02;
	map.byte = 1;
	map.value = SKILL_MORPH;
	map.val = false;
	skillsinfo[5111005].map.push_back(map);
	// Super Sayan thing[orange] - 5121003
	player.type = 0x02;
	player.byte = 1;
	player.value = SKILL_MORPH;
	skillsinfo[5121003].player.push_back(player);
	map.type = 0x02;
	map.byte = 1;
	map.value = SKILL_MORPH;
	map.val = false;
	skillsinfo[5121003].map.push_back(map);
	// Mana Reflection - 2121002, 2221002, and 2321002
	player.type = 0x40;
	player.byte = 1;
	player.value = SKILL_LV;
	skillsinfo[2121002].player.push_back(player);
	skillsinfo[2221002].player.push_back(player);
	skillsinfo[2321002].player.push_back(player);
}

void Skills::addSkill(int id, SkillsLevelInfo skill) {
	skills[id] = skill;
}

void Skills::addSkill(Player *player, ReadPacket *packet) {
	packet->skipBytes(4);
	int skillid = packet->getInt();
	if (!BEGINNER_SKILL(skillid) && player->getSp() == 0) {
		// hacking
		return;
	}
	if (!BEGINNER_SKILL(skillid))
		player->setSp(player->getSp()-1);
	player->skills->addSkillLevel(skillid, 1);
}
void Skills::cancelSkill(Player *player, ReadPacket *packet) {
	stopSkill(player, packet->getInt());
}
void Skills::stopSkill(Player *player, int skillid) {
	if (skillid == 3121004 || skillid == 3221001 || skillid == 2121001 || skillid == 2221001 || skillid == 2321001) { // Hurricane/Pierce/Big Bang x3
		SkillsPacket::endSpecialSkill(player, player->getSpecialSkillInfo());
		SpecialSkillInfo info;
		player->setSpecialSkill(info);
		return;
	}
	SkillTimer::Instance()->stop(player, skillid);
	endSkill(player, skillid);
}
void Skills::useSkill(Player *player, ReadPacket *packet) {
	packet->skipBytes(4); //Ticks
	int skillid = packet->getInt();
	short addedinfo = 0;
	unsigned char level = packet->getByte();
	unsigned char type = 0;
	int cooltime = Skills::skills[skillid][level].cooltime;
	switch (skillid) {
		case 1121001: // Monster Magnet processing
		case 1221001:
		case 1321001: {
			int mobs = packet->getInt();
			for (char k = 0; k < mobs; k++) {
				int mapmobid = packet->getInt();
				unsigned char success = packet->getByte();
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
	if (cooltime > 0)
		Skills::startCooldown(player, skillid, cooltime);
	if (skills[skillid][level].mp > 0) {
		if (player->skills->getActiveSkillLevel(3121008) > 0) { // Reduced MP useage for Concentration
			int mprate = Skills::skills[3121008][player->skills->getActiveSkillLevel(3121008)].x;
			int mploss = (skills[skillid][level].mp * mprate) / 100;
			player->setMP(player->getMP() - mploss, 1);
		}
		else
			player->setMP(player->getMP() - skills[skillid][level].mp, 1);
	}
	else
		player->setMP(player->getMP(), 1);
	if (skills[skillid][level].hp > 0) {
		player->setHP(player->getHP()-skills[skillid][level].hp);
	}
	if (skills[skillid][level].item > 0) {	
		Inventory::takeItem(player, skills[skillid][level].item, skills[skillid][level].itemcount);
	}
	if (skills[skillid][level].hpP > 0) {	
		//TODO PARTY
		int healrate = skills[skillid][level].hpP/1;
		if (healrate > 100)
			healrate = 100;
		player->setHP(player->getHP() + healrate*player->getMHP()/100);
	}
	SkillsPacket::showSkill(player, skillid, level); 
	///
	if (skillid == 1301007 || skillid == 9101008) { // Hyper Body
		player->setMHP(player->getRMHP()*(100 + skills[skillid][level].x)/100);
		player->setMMP(player->getRMMP()*(100 + skills[skillid][level].y)/100);
	}
	///
	else if (skillid == 9101000) { // GM Heal+Dispell
		player->setHP(player->getMHP());
		player->setMP(player->getMMP());
	}
	else if (skillid == 1121010) { // Enrage
		if (player->getCombo() == 10)
			Skills::clearCombo(player);
		else
			return;
	}
	else if (skillid == 9101005) { // GM Resurrection
		for (size_t i = 0; i < Maps::maps[player->getMap()]->getNumPlayers(); i++) {
			Player *resplayer;
			resplayer = Maps::maps[player->getMap()]->getPlayer(i);
			if (resplayer->getHP() <= 0) {
				resplayer->setHP(resplayer->getMHP());
			}
		}
	}
	if (skillsinfo.find(skillid) == skillsinfo.end())
		return;
	SkillActiveInfo playerskill;
	SkillActiveInfo mapskill;
	vector <SkillMapActiveInfo> mapenterskill;
	// Reset player/map types to 0
	memset(playerskill.types, 0, 8*sizeof(unsigned char));
	memset(mapskill.types, 0, 8*sizeof(unsigned char));
	for (size_t i = 0; i < skillsinfo[skillid].player.size(); i++) {
		playerskill.types[skillsinfo[skillid].player[i].byte-1] += skillsinfo[skillid].player[i].type;
		char val = skillsinfo[skillid].player[i].value;
		if (skillid == 4001003 && level == 20 && val == SKILL_SPEED) { // Cancel speed change for maxed darksight
			playerskill.types[0] = 0;
			continue;
		}
		short value = 0;
		switch(val) {
			case SKILL_X: value = skills[skillid][level].x; break;
			case SKILL_Y: value = skills[skillid][level].y; break;
			case SKILL_SPEED: value = skills[skillid][level].speed; break;
			case SKILL_JUMP: value = skills[skillid][level].jump; break;
			case SKILL_WATK: value = skills[skillid][level].watk; break;
			case SKILL_WDEF: value = skills[skillid][level].wdef; break;
			case SKILL_MATK: value = skills[skillid][level].matk; break;
			case SKILL_MDEF: value = skills[skillid][level].mdef; break;
			case SKILL_ACC: value = skills[skillid][level].acc; break;
			case SKILL_AVO: value = skills[skillid][level].avo; break;
			case SKILL_PROP: value = skills[skillid][level].prop; break;
			case SKILL_MORPH: value = skills[skillid][level].morph; break;
			case SKILL_LV: value = level; break;
		}
		if (skillid == 3121002 || skillid == 3221002) { // For Sharp Eyes
			value = skills[skillid][level].x*256+skills[skillid][level].y;
		}
		else if (skillid == 4111002) { // For Shadow Partner
			value = skills[skillid][level].x*256+skills[skillid][level].y;
		}
		else if (skillid == 1111002) { // For Combo Attack
			player->setCombo(0);
			value = player->getCombo()+1;
		}
		else if (skillid == 1004) { // For Monster Rider
			Equip *equip = player->inv->getEquip(-18);
			if (equip == 0)
				// hacking
				return;
			int mountid = equip->id;
			value = Drops::equips[mountid].tamingmob;
		}
		else if (skillid == 4121006) { // For Shadow Claw
			for (short s = 1; s <= player->inv->getMaxSlots(2); s++) {
				Item *item = player->inv->getItem(2, s);
				if (item == 0)
					continue;
				if (ISRECHARGEABLE(item->id) && item->amount >= 200) {
					Inventory::takeItemSlot(player, 2, s, 200);
					value = (item->id % 10000) + 1;
					break;
				}
			}
		}
		playerskill.vals.push_back(value);
	}
	for (size_t i = 0; i < skillsinfo[skillid].map.size(); i++) {
		mapskill.types[skillsinfo[skillid].map[i].byte-1]+= skillsinfo[skillid].map[i].type;
		char val = skillsinfo[skillid].map[i].value;
		if (skillid == 4001003 && level == 20 && val == SKILL_SPEED) { // Cancel speed update for maxed darksight
			mapskill.types[0] = 0;
			continue;
		}
		short value = 0;
		switch(val) {
			case SKILL_X: value = skills[skillid][level].x; break;
			case SKILL_Y: value = skills[skillid][level].y; break;
			case SKILL_SPEED: value = skills[skillid][level].speed; break;   
			case SKILL_JUMP: value = skills[skillid][level].jump; break; 
			case SKILL_WATK: value = skills[skillid][level].watk; break;
			case SKILL_WDEF: value = skills[skillid][level].wdef; break;
			case SKILL_MATK: value = skills[skillid][level].matk; break; 
			case SKILL_MDEF: value = skills[skillid][level].mdef; break;
			case SKILL_ACC: value = skills[skillid][level].acc; break;
			case SKILL_AVO: value = skills[skillid][level].avo; break;
			case SKILL_PROP: value = skills[skillid][level].prop; break;
			case SKILL_MORPH: value = skills[skillid][level].morph; break;
			case SKILL_LV: value = level; break;
		}
		if (skillid == 4111002) { // For Shadow Partner
			value = skills[skillid][level].x*256+skills[skillid][level].y;
		}
		else if (skillid == 1111002) { // For Combo Attack
			value = player->getCombo()+1;
		}
		mapskill.vals.push_back(value);
		SkillMapActiveInfo map;
		map.byte = skillsinfo[skillid].map[i].byte;
		map.type = skillsinfo[skillid].map[i].type;
		if (skillsinfo[skillid].map[i].val) {
			map.value = (char)value;
			map.isvalue = true;
		}
		else{
			map.isvalue = false;
			map.value = 0;
		}
		map.skill = skillid;
		mapenterskill.push_back(map);
	}
	SkillsPacket::useSkill(player, skillid, skills[skillid][level].time*1000, playerskill, mapskill, addedinfo);
	player->skills->setSkillPlayerInfo(skillid, playerskill);
	player->skills->setSkillMapInfo(skillid, mapskill);
	player->skills->setSkillMapEnterInfo(skillid, mapenterskill);
	SkillTimer::Instance()->stop(player, skillid);
	if (skillsinfo[skillid].bact.size() > 0) {
		SkillTimer::Instance()->stop(player, skillid, skillsinfo[skillid].act.name);
	}
	if (skillsinfo[skillid].bact.size() > 0) {
		int value = 0;
		switch(skillsinfo[skillid].act.value) {
			case SKILL_X: value = skills[skillid][level].x; break;
			case SKILL_Y: value = skills[skillid][level].y; break;
			case SKILL_SPEED: value = skills[skillid][level].speed; break;
			case SKILL_WATK: value = skills[skillid][level].watk; break;
			case SKILL_WDEF: value = skills[skillid][level].wdef; break;
			case SKILL_MATK: value = skills[skillid][level].matk; break;
			case SKILL_MDEF: value = skills[skillid][level].mdef; break;
			case SKILL_ACC: value = skills[skillid][level].acc; break;
			case SKILL_AVO: value = skills[skillid][level].avo; break;
			case SKILL_PROP: value = skills[skillid][level].prop; break;
			case SKILL_MORPH: value = skills[skillid][level].morph; break;
			case SKILL_LV: value = level; break;
		}
		SkillTimer::Instance()->setSkillTimer(player, skillid, skillsinfo[skillid].act.name, value, skillsinfo[skillid].act.time);
	}
	player->setSkill(player->skills->getSkillMapEnterInfo());
	SkillTimer::Instance()->setSkillTimer(player, skillid, skills[skillid][level].time*1000);
	player->skills->setActiveSkillLevel(skillid, level);
	if (skillid == 9101004) // GM Hide
		MapPacket::removePlayer(player);
}
void Skills::useAttackSkill(Player *player, int skillid) {
	if (skills.find(skillid) == skills.end())
		return;
	if (skills[skillid][player->skills->getSkillLevel(skillid)].mp > 0) {
		if (player->skills->getActiveSkillLevel(3121008) > 0) { // Reduced MP useage for Concentration
			int mprate = Skills::skills[3121008][player->skills->getActiveSkillLevel(3121008)].x;
			int mploss = (skills[skillid][player->skills->getSkillLevel(skillid)].mp * mprate) / 100;
			player->setMP(player->getMP() - mploss, 1);
		}
		else {
			int sid = ((player->getJob() / 10) == 22 ? 2210001 : 2110001);
			int slv = player->skills->getSkillLevel(sid);
			if (slv > 0)
				player->setMP(player->getMP() - (skills[skillid][player->skills->getSkillLevel(skillid)].mp * skills[sid][slv].x / 100), 1);
			else
				player->setMP(player->getMP() - skills[skillid][player->skills->getSkillLevel(skillid)].mp, 1);
		}
	}
	else
		player->setMP(player->getMP(), 1);
	if (skills[skillid][player->skills->getSkillLevel(skillid)].hp > 0) {
		player->setHP(player->getHP()-skills[skillid][player->skills->getSkillLevel(skillid)].hp);
	}
	if (skills[skillid][player->skills->getSkillLevel(skillid)].item > 0) {	
		Inventory::takeItem(player, skills[skillid][player->skills->getSkillLevel(skillid)].item, skills[skillid][player->skills->getSkillLevel(skillid)].itemcount);
	}
	int cooltime = Skills::skills[skillid][player->skills->getSkillLevel(skillid)].cooltime;
	if (cooltime > 0)
		Skills::startCooldown(player, skillid, cooltime);
}

void Skills::endSkill(Player *player, int skill) {
	/// 
	if (skill == 1301007 || skill == 9101008) { // Hyper Body
		player->setMHP(player->getRMHP());
		player->setMMP(player->getRMMP());
		player->setHP(player->getHP());
		player->setMP(player->getMP());
	}
	///
	if (skillsinfo[skill].bact.size()>0) {
		SkillTimer::Instance()->stop(player, skill, skillsinfo[skill].act.name);
	}
	if (skill == 9101004) // GM Hide
		MapPacket::showPlayer(player);
	SkillsPacket::endSkill(player, player->skills->getSkillPlayerInfo(skill), player->skills->getSkillMapInfo(skill));
	player->skills->deleteSkillMapEnterInfo(skill);
	player->skills->setActiveSkillLevel(skill, 0);
}

void Skills::heal(Player *player, short value, int skillid) {
	if (player->getHP() < player->getMHP() && player->getHP() > 0) {
		player->setHP(player->getHP() + value);
		SkillsPacket::healHP(player, value);
	}
	SkillTimer::Instance()->setSkillTimer(player, skillid, "heal", value, 5000);
}
// Combo attack stuff
void Skills::addCombo(Player *player) { // add combo orbs 
	if (player->skills->getActiveSkillLevel(1111002) > 0) {
		int maxcombo = 0;
		int advcombo = player->skills->getSkillLevel(1120003);
		if (advcombo > 0) maxcombo = Skills::skills[1120003][advcombo].x;
		else maxcombo = Skills::skills[1111002][player->skills->getSkillLevel(1111002)].x;
		if (player->getCombo() == maxcombo) {
			return;
		}
		else {
			if (advcombo > 0 && Randomizer::Instance()->randInt(99) < skills[1120003][advcombo].prop)
				player->setCombo(player->getCombo()+2); // 4th job skill gives chance to add second orb
			else
				player->setCombo(player->getCombo()+1);
		}
		if (player->getCombo() > maxcombo)
			player->setCombo(maxcombo);
		SkillsPacket::showCombo(player, SkillTimer::Instance()->skillTime(player, 1111002));
	}
}

void Skills::clearCombo(Player *player) { // finishing moves panic coma
	player->setCombo(0);
	SkillsPacket::showCombo(player, SkillTimer::Instance()->skillTime(player, 1111002));
}

void Skills::startCooldown(Player *player, int skillid, int cooltime) {
	if (Skills::isCooling(player, skillid)) {
		// Hacking
		return;
	}
	SkillsPacket::sendCooldown(player, skillid, cooltime);
	CoolTimer::Instance()->setCoolTimer(player, skillid, cooltime * 1000);
}

void Skills::stopCooldown(Player *player, int skillid) {
	SkillsPacket::sendCooldown(player, skillid, 0);	
}

bool Skills::isCooling(Player *player, int skillid) {
	if (CoolTimer::Instance()->coolTime(player, skillid) > 0)
		return true;
	return false;
}

void Skills::stopCooldownTimersPlayer(Player *player) {
	CoolTimer::Instance()->stop(player);
}