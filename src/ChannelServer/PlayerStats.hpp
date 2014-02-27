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
#pragma once

#include "Types.hpp"
#include <map>

class Item;
class PacketBuilder;
class PacketReader;
class Player;

struct BonusSet {
	int32_t hp = 0;
	int32_t mp = 0;
	int32_t str = 0;
	int32_t dex = 0;
	int32_t intt = 0;
	int32_t luk = 0;
};

struct EquipBonus : public BonusSet {
	int32_t id = 0;
};

class PlayerStats {
	NONCOPYABLE(PlayerStats);
	NO_DEFAULT_CONSTRUCTOR(PlayerStats);
public:
	PlayerStats(Player *player,
		player_level_t level,
		job_id_t job,
		fame_t fame,
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
		experience_t exp);

	// Data modification
	auto checkHpMp() -> void;
	auto setLevel(player_level_t level) -> void;
	auto modifyHp(int32_t hpMod, bool sendPacket = true) -> void;
	auto modifyMp(int32_t mpMod, bool sendPacket = false) -> void;
	auto damageHp(int32_t damageHp) -> void;
	auto damageMp(int32_t damageMp) -> void;
	auto setHp(int16_t hp, bool sendPacket = true) -> void;
	auto setMp(int16_t mp, bool sendPacket = false) -> void;
	auto setMaxHp(int16_t maxHp) -> void;
	auto setMaxMp(int16_t maxMp) -> void;
	auto modifyMaxHp(int16_t mod) -> void;
	auto modifyMaxMp(int16_t mod) -> void;
	auto setHyperBody(int16_t xMod, int16_t yMod) -> void;
	auto setHpMpAp(uint16_t ap) -> void { m_hpMpAp = ap; }
	auto setExp(experience_t exp) -> void;
	auto setAp(int16_t ap) -> void;
	auto setSp(int16_t sp) -> void;

	auto setFame(fame_t fame) -> void;
	auto setJob(job_id_t job) -> void;
	auto setStr(int16_t str) -> void;
	auto setDex(int16_t dex) -> void;
	auto setInt(int16_t intt) -> void;
	auto setLuk(int16_t luk) -> void;
	auto setMapleWarrior(int16_t xMod) -> void;
	auto loseExp() -> void;

	auto setEquip(inventory_slot_t slot, Item *equip, bool isLoading = false) -> void;

	// Level related functions
	auto giveExp(uint64_t exp, bool inChat = false, bool white = true) -> void;
	auto addStat(PacketReader &reader) -> void;
	auto addStatMulti(PacketReader &reader) -> void;
	auto addStat(int32_t type, int16_t mod = 1, bool isReset = false) -> void;
	auto randHp() -> int16_t;
	auto randMp() -> int16_t;
	auto levelHp(int16_t val, int16_t bonus = 0) -> int16_t;
	auto levelMp(int16_t val, int16_t bonus = 0) -> int16_t;
	auto apResetHp(bool isReset, bool isSubtract, int16_t val, int16_t sVal = 0) -> int16_t;
	auto apResetMp(bool isReset, bool isSubtract, int16_t val, int16_t sVal = 0) -> int16_t;
	auto getX(skill_id_t skillId) -> int16_t;
	auto getY(skill_id_t skillId) -> int16_t;
	auto getExp(player_level_t level) -> experience_t;

	// Data acquisition
	auto connectData(PacketBuilder &packet) -> void;
	auto getLevel() const -> player_level_t { return m_level; }
	auto getJob() const -> job_id_t { return m_job; }
	auto getExp() const -> experience_t { return m_exp; }
	auto getAp() const -> int16_t { return m_ap; }
	auto getHpMpAp() const -> uint16_t { return m_hpMpAp; }
	auto getSp() const -> int16_t { return m_sp; }
	auto getFame() const -> fame_t { return m_fame; }

	auto getStr(bool withBonus = false) -> int16_t;
	auto getDex(bool withBonus = false) -> int16_t;
	auto getInt(bool withBonus = false) -> int16_t;
	auto getLuk(bool withBonus = false) -> int16_t;
	auto getHp() const -> int16_t { return m_hp; }
	auto getMaxHp(bool withoutBonus = false) -> int16_t;
	auto getMp() const -> int16_t { return m_mp; }
	auto getMaxMp(bool withoutBonus = false) -> int16_t;
	auto isDead() const -> bool;
private:
	auto updateBonuses(bool updateEquips = false, bool isLoading = false) -> void;
	auto modifiedHp() -> void;
	auto statUtility(int32_t test) -> int16_t;

	player_level_t m_level = 0;
	job_id_t m_job = 0;
	int16_t m_ap = 0;
	int16_t m_sp = 0;
	fame_t m_fame = 0;
	int16_t m_hp = 0;
	int16_t m_maxHp = 0;
	int16_t m_mp = 0;
	int16_t m_maxMp = 0;
	int16_t m_str = 0;
	int16_t m_dex = 0;
	int16_t m_int = 0;
	int16_t m_luk = 0;
	int16_t m_hyperBodyX = 0;
	int16_t m_hyperBodyY = 0;
	int16_t m_mapleWarrior = 0;
	uint16_t m_hpMpAp = 0;
	experience_t m_exp = 0;

	BonusSet m_equipBonuses;
	BonusSet m_buffBonuses;
	Player *m_player = nullptr;
	ord_map_t<int16_t, EquipBonus> m_equipStats;
};