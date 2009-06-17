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
#ifndef PLAYERSTATS_H
#define PLAYERSTATS_H

#include "Types.h"

class PacketCreator;
class Player;

class PlayerStats {
public:
	PlayerStats(Player *p);

	void save();
	void load();
	void connectData(PacketCreator &packet);

	int16_t getTotalStat(int32_t stat) const; // Cookie cutter functions, work for no less than 6 stats
	int16_t getBaseStat(int32_t stat) const;
	int16_t getGearStat(int32_t stat) const;
	int16_t getBuffStat(int32_t stat) const;
	int8_t getGender() const { return m_gender; }
	int8_t getSkin() const { return m_skin; }
	uint8_t getLevel() const { return m_level; }
	int32_t getExp() const { return m_exp; }
	int32_t getEyes() const { return m_eyes; }
	int32_t getHair() const { return m_hair; }
	int16_t getJob() const { return m_job; }
	int16_t getHp() const { return m_hp; }
	int16_t getMp() const { return m_mp; }
	int16_t getMHp() const { return m_mhp; }
	int16_t getMMp() const { return m_mmp; }
	int16_t getRMHp() const { return m_rmhp; }
	int16_t getRMMp() const { return m_rmmp; }

	void setGender(int8_t gender); // WHY would you do this?
	void setSkin(int8_t skin);
	void setLevel(uint8_t level);
	void setBaseStat(int32_t stat, int16_t amount, bool showPacket = true); // Cookie cutter function that works for most
	void setGearStat(int32_t stat, int16_t amount, bool firstLoad = false);
	void setExp(int32_t exp);
	void setEyes(int32_t eyes);
	void setHair(int32_t hair);
	void setJob(int16_t job);
	void setFame(int16_t fame);

	void modifyHp(int16_t hp, bool is = true); // Bases its calculations on current HP/MP
	void modifyMp(int16_t mp, bool is = false);
	void damageHp(uint16_t dhp); // Calculations done based on the fact that damage can range from 0 to ~55k
	void damageMp(uint16_t dmp);
	void setHp(int16_t hp, bool is = true); // Only use setHp/Mp if you're going to do checking, they fall easily to datatype issues
	void setMp(int16_t mp, bool is = false); // For example, Power Elixir at 30k HP = 30k + 30k = 60k, but wait! That's > 32767, so it's negative
	void setMHp(int16_t mhp);
	void setMMp(int16_t mmp);
	void modifyRMHp(int16_t mod);
	void modifyRMMp(int16_t mod);
	void setRMHp(int16_t rmhp);
	void setRMMp(int16_t rmmp);
	void setHyperBody(int16_t modx, int16_t mody);
	void modifiedHp();
private:
	Player *m_player;

	int8_t m_gender;
	int8_t m_skin;
	uint8_t m_level;
	int16_t m_job;
	int16_t m_str;
	int16_t m_dex;
	int16_t m_int;
	int16_t m_luk;
	int16_t m_hp;
	int16_t m_mp;
	int16_t m_hpmp_ap;
	int16_t m_mhp;
	int16_t m_mmp;
	int16_t m_rmhp;
	int16_t m_rmmp;
	int16_t m_ap;
	int16_t m_sp;
	int16_t m_fame;
	int16_t m_gear_str;
	int16_t m_gear_dex;
	int16_t m_gear_int;
	int16_t m_gear_luk;
	int16_t m_gear_hp;
	int16_t m_gear_mp;
	int32_t m_exp;
	int32_t m_eyes;
	int32_t m_hair;
};

#endif