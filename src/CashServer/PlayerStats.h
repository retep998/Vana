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
#pragma once

#include "Types.h"
#include <map>

using std::map;

class Player;
class PacketCreator;
class PacketReader;
class Item;

struct BonusSet {
	uint16_t Hp,
		Mp,
		Str,
		Dex,
		Int,
		Luk;

	BonusSet() : Hp(0), Mp(0), Str(0), Dex(0), Int(0), Luk(0) {}
};

struct EquipBonus : public BonusSet {
	int32_t Id;

	EquipBonus() : Id(0) {}
};
typedef map<int16_t, EquipBonus> EquipBonuses;

class PlayerStats {
public:
	PlayerStats(Player *player,
		uint8_t level,
		int16_t job,
		int16_t fame,
		int16_t str,
		int16_t dex,
		int16_t intt,
		int16_t luk,
		int16_t ap,
		uint16_t hpmp_ap,
		int16_t sp,
		int16_t hp,
		int16_t mhp,
		int16_t mp,
		int16_t mmp,
		int32_t exp);

	// Data Modification
	void checkHpMp();
	void setLevel(uint8_t level);
	void modifyHp(int16_t hp, bool is = true); // Bases its calculations on current HP/MP
	void modifyMp(int16_t mp, bool is = false);
	void damageHp(uint16_t dhp); // Calculations done based on the fact that damage can range from 0 to ~55k
	void damageMp(uint16_t dmp);
	void setHp(int16_t hp, bool is = true); // Only use setHp/Mp if you're going to do checking, they fall easily to datatype issues
	void setMp(int16_t mp, bool is = false); // For example, Power Elixir at 30k HP = 30k + 30k = 60k, but wait! That's > 32767, so it's negative
	void setMaxHp(int16_t mhp);
	void setMaxMp(int16_t mmp);
	void modifyMaxHp(int16_t mod);
	void modifyMaxMp(int16_t mod);
	void setHyperBody(int16_t modx, int16_t mody);
	void setHpMpAp(uint16_t ap) { hpmp_ap = ap; }
	void setExp(int32_t exp);
	void setAp(int16_t ap);
	void setSp(int16_t sp);

	void setFame(int16_t fame);
	void setJob(int16_t job);
	void setStr(int16_t str);
	void setDex(int16_t dex);
	void setInt(int16_t intt);
	void setLuk(int16_t luk);
	void setMapleWarrior(int16_t modx);
	void loseExp();

	void setEquip(int16_t slot, Item *equip, bool isLoading = false);

	// Level Related Functions
	void giveExp(uint32_t exp, bool inChat = false, bool white = true);
	void addStat(PacketReader &packet);
	void addStatMulti(PacketReader &packet);
	void addStat(int32_t type, int16_t mod = 1, bool isreset = false);
	int16_t randHp();
	int16_t randMp();
	int16_t levelHp(int16_t val, int16_t bonus = 0);
	int16_t levelMp(int16_t val, int16_t bonus = 0);
	int16_t apResetHp(bool isreset, bool issubtract, int16_t val, int16_t sval = 0);
	int16_t apResetMp(bool isreset, bool issubtract, int16_t val, int16_t sval = 0);
	int16_t getX(int32_t skillid);
	int16_t getY(int32_t skillid);
	uint32_t getExp(uint8_t level);
	uint8_t getMaxLevel(int16_t jobid);

	// Data Acquisition
	void connectData(PacketCreator &packet);
	uint8_t getLevel() const { return level; }
	int16_t getJob() const { return job; }
	int32_t getExp() const { return exp; }
	int16_t getAp() const { return ap; }
	uint16_t getHpMpAp() const { return hpmp_ap; }
	int16_t getSp() const { return sp; }
	int16_t getFame() const { return fame; }

	int16_t getStr(bool withbonus = false);
	int16_t getDex(bool withbonus = false);
	int16_t getInt(bool withbonus = false);
	int16_t getLuk(bool withbonus = false);
	int16_t getHp() const { return hp; }
	int16_t getMaxHp(bool withoutbonus = false);
	int16_t getMp() const { return mp; }
	int16_t getMaxMp(bool withoutbonus = false);
private:
	Player *player;
	uint8_t level;
	int16_t job;
	int32_t exp;
	int16_t ap;
	uint16_t hpmp_ap;
	int16_t sp;
	int16_t fame;

	int16_t hp;
	int16_t mhp;
	int16_t mp;
	int16_t mmp;
	int16_t str;
	int16_t dex;
	int16_t intt;
	int16_t luk;

	EquipBonuses equipStats;
	BonusSet equipBonuses;
	int16_t hbx, hby, mw; // Keep track of Hyper Body X and Y values along with Maple Warrior's X value in order to update new amounts upon stat up
	BonusSet buffBonuses;
	void updateBonuses(bool updateEquips = false, bool isLoading = false);

	void modifiedHp();
};
