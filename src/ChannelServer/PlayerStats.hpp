/*
Copyright (C) 2008-2015 Vana Development Team

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

namespace Vana {
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
			stat_t str,
			stat_t dex,
			stat_t intt,
			stat_t luk,
			stat_t ap,
			health_ap_t hpMpAp,
			stat_t sp,
			health_t hp,
			health_t maxHp,
			health_t mp,
			health_t maxMp,
			experience_t exp);

		// Data modification
		auto checkHpMp() -> void;
		auto setLevel(player_level_t level) -> void;
		auto modifyHp(int32_t hpMod, bool sendPacket = true) -> void;
		auto modifyMp(int32_t mpMod, bool sendPacket = false) -> void;
		auto damageHp(int32_t damageHp) -> void;
		auto damageMp(int32_t damageMp) -> void;
		auto setHp(health_t hp, bool sendPacket = true) -> void;
		auto setMp(health_t mp, bool sendPacket = false) -> void;
		auto setMaxHp(health_t maxHp) -> void;
		auto setMaxMp(health_t maxMp) -> void;
		auto modifyMaxHp(health_t mod) -> void;
		auto modifyMaxMp(health_t mod) -> void;
		auto setHyperBodyHp(int16_t mod) -> void;
		auto setHyperBodyMp(int16_t mod) -> void;
		auto setHpMpAp(health_ap_t ap) -> void { m_hpMpAp = ap; }
		auto setExp(experience_t exp) -> void;
		auto setAp(stat_t ap) -> void;
		auto setSp(stat_t sp) -> void;

		auto setFame(fame_t fame) -> void;
		auto setJob(job_id_t job) -> void;
		auto setStr(stat_t str) -> void;
		auto setDex(stat_t dex) -> void;
		auto setInt(stat_t intt) -> void;
		auto setLuk(stat_t luk) -> void;
		auto setMapleWarrior(int16_t mod) -> void;
		auto loseExp() -> void;

		auto setEquip(inventory_slot_t slot, Item *equip, bool isLoading = false) -> void;

		// Level related functions
		auto giveExp(uint64_t exp, bool inChat = false, bool white = true) -> void;
		auto addStat(PacketReader &reader) -> void;
		auto addStatMulti(PacketReader &reader) -> void;
		auto addStat(int32_t type, int16_t mod = 1, bool isReset = false) -> void;
		auto apResetHp(bool isReset, bool isSubtract, int16_t val, int16_t sVal = 0) -> int16_t;
		auto apResetMp(bool isReset, bool isSubtract, int16_t val, int16_t sVal = 0) -> int16_t;
		auto getX(skill_id_t skillId) -> int16_t;
		auto getY(skill_id_t skillId) -> int16_t;
		auto getExp(player_level_t level) -> experience_t;

		// Data acquisition
		auto connectPacket(PacketBuilder &builder) -> void;
		auto getLevel() const -> player_level_t { return m_level; }
		auto getJob() const -> job_id_t { return m_job; }
		auto getExp() const -> experience_t { return m_exp; }
		auto getAp() const -> stat_t { return m_ap; }
		auto getHpMpAp() const -> health_ap_t { return m_hpMpAp; }
		auto getSp() const -> stat_t { return m_sp; }
		auto getFame() const -> fame_t { return m_fame; }

		auto getStr(bool withBonus = false) -> stat_t;
		auto getDex(bool withBonus = false) -> stat_t;
		auto getInt(bool withBonus = false) -> stat_t;
		auto getLuk(bool withBonus = false) -> stat_t;
		auto getHp() const -> health_t { return m_hp; }
		auto getMaxHp(bool withoutBonus = false) -> health_t;
		auto getMp() const -> health_t { return m_mp; }
		auto getMaxMp(bool withoutBonus = false) -> health_t;
		auto isDead() const -> bool;
	private:
		auto updateBonuses(bool updateEquips = false, bool isLoading = false) -> void;
		auto modifiedHp() -> void;
		auto randHp() -> health_t;
		auto randMp() -> health_t;
		auto levelHp(health_t val, health_t bonus = 0) -> health_t;
		auto levelMp(health_t val, health_t bonus = 0) -> health_t;
		auto statUtility(int32_t test) -> int16_t;

		player_level_t m_level = 0;
		job_id_t m_job = 0;
		stat_t m_ap = 0;
		stat_t m_sp = 0;
		fame_t m_fame = 0;
		health_t m_hp = 0;
		health_t m_maxHp = 0;
		health_t m_mp = 0;
		health_t m_maxMp = 0;
		stat_t m_str = 0;
		stat_t m_dex = 0;
		stat_t m_int = 0;
		stat_t m_luk = 0;
		int16_t m_hyperBodyX = 0;
		int16_t m_hyperBodyY = 0;
		int16_t m_mapleWarrior = 0;
		health_ap_t m_hpMpAp = 0;
		experience_t m_exp = 0;

		BonusSet m_equipBonuses;
		BonusSet m_buffBonuses;
		Player *m_player = nullptr;
		ord_map_t<int16_t, EquipBonus> m_equipStats;
	};
}