/*
Copyright (C) 2008-2013 Vana Development Team

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

#include "noncopyable.hpp"
#include "Types.h"
#include <map>

using std::map;

class Item;
class PacketCreator;
class PacketReader;
class Player;

struct BonusSet {
	int32_t hp;
	int32_t mp;
	int32_t str;
	int32_t dex;
	int32_t intt;
	int32_t luk;

	BonusSet() : hp(0), mp(0), str(0), dex(0), intt(0), luk(0) {}
};

struct EquipBonus : public BonusSet {
	int32_t id;

	EquipBonus() : id(0) {}
};
typedef map<int16_t, EquipBonus> EquipBonuses;

class PlayerStats : boost::noncopyable {
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
		uint16_t hpMpAp,
		int16_t sp,
		int16_t hp,
		int16_t maxHp,
		int16_t mp,
		int16_t maxMp,
		int32_t exp);

	// Data modification
	void checkHpMp();
	void setLevel(uint8_t level);
	void modifyHp(int32_t hpMod, bool sendPacket = true); // Bases its calculations on current HP/MP
	void modifyMp(int32_t mpMod, bool sendPacket = false);
	void damageHp(int32_t damageHp);
	void damageMp(int32_t damageMp);
	void setHp(int16_t hp, bool sendPacket = true); // Only use setHp/Mp if you're going to do checking, they fall easily to datatype issues
	void setMp(int16_t mp, bool sendPacket = false); // For example, Power Elixir at 30k HP = 30k + 30k = 60k, but wait! That's > 32767, so it's negative
	void setMaxHp(int16_t maxHp);
	void setMaxMp(int16_t maxMp);
	void modifyMaxHp(int16_t mod);
	void modifyMaxMp(int16_t mod);
	void setHyperBody(int16_t xMod, int16_t yMod);
	void setHpMpAp(uint16_t ap) { m_hpMpAp = ap; }
	void setExp(int32_t exp);
	void setAp(int16_t ap);
	void setSp(int16_t sp);

	void setFame(int16_t fame);
	void setJob(int16_t job);
	void setStr(int16_t str);
	void setDex(int16_t dex);
	void setInt(int16_t intt);
	void setLuk(int16_t luk);
	void setMapleWarrior(int16_t xMod);
	void loseExp();

	void setEquip(int16_t slot, Item *equip, bool isLoading = false);

	// Level related functions
	void giveExp(uint64_t exp, bool inChat = false, bool white = true);
	void addStat(PacketReader &packet);
	void addStatMulti(PacketReader &packet);
	void addStat(int32_t type, int16_t mod = 1, bool isReset = false);
	int16_t randHp();
	int16_t randMp();
	int16_t levelHp(int16_t val, int16_t bonus = 0);
	int16_t levelMp(int16_t val, int16_t bonus = 0);
	int16_t apResetHp(bool isReset, bool isSubtract, int16_t val, int16_t sVal = 0);
	int16_t apResetMp(bool isReset, bool isSubtract, int16_t val, int16_t sVal = 0);
	int16_t getX(int32_t skillId);
	int16_t getY(int32_t skillId);
	uint32_t getExp(uint8_t level);

	// Data acquisition
	void connectData(PacketCreator &packet);
	uint8_t getLevel() const { return m_level; }
	int16_t getJob() const { return m_job; }
	int32_t getExp() const { return m_exp; }
	int16_t getAp() const { return m_ap; }
	uint16_t getHpMpAp() const { return m_hpMpAp; }
	int16_t getSp() const { return m_sp; }
	int16_t getFame() const { return m_fame; }

	int16_t getStr(bool withBonus = false);
	int16_t getDex(bool withBonus = false);
	int16_t getInt(bool withBonus = false);
	int16_t getLuk(bool withBonus = false);
	int16_t getHp() const { return m_hp; }
	int16_t getMaxHp(bool withoutBonus = false);
	int16_t getMp() const { return m_mp; }
	int16_t getMaxMp(bool withoutBonus = false);
	bool isDead() const;
private:
	void updateBonuses(bool updateEquips = false, bool isLoading = false);
	void modifiedHp();
	int16_t statUtility(int32_t test);

	uint8_t m_level;
	int16_t m_job;
	int16_t m_ap;
	int16_t m_sp;
	int16_t m_fame;
	int16_t m_hp;
	int16_t m_maxHp;
	int16_t m_mp;
	int16_t m_maxMp;
	int16_t m_str;
	int16_t m_dex;
	int16_t m_int;
	int16_t m_luk;
	int16_t m_hyperBodyX;
	int16_t m_hyperBodyY;
	int16_t m_mapleWarrior; // Keep track of Hyper Body X and Y values along with Maple Warrior's X value in order to update new amounts upon stat up
	uint16_t m_hpMpAp;
	int32_t m_exp;

	EquipBonuses m_equipStats;
	BonusSet m_equipBonuses;
	BonusSet m_buffBonuses;
	Player *m_player;
};