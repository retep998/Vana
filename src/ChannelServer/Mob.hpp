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

#include "MobConstants.hpp"
#include "MobDataProvider.hpp"
#include "MovableLife.hpp"
#include "Pos.hpp"
#include "TimerContainerHolder.hpp"
#include "Types.hpp"
#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

class Map;
class PacketBuilder;
class Party;
class Player;
struct MobSkillInfo;
struct MobSkillLevelInfo;
struct MpEaterInfo;
struct StatusInfo;

class Mob : public MovableLife, public enable_shared<Mob>, public TimerContainerHolder {
	NONCOPYABLE(Mob);
	NO_DEFAULT_CONSTRUCTOR(Mob);
public:
	Mob(map_object_t mapMobId, map_id_t mapId, mob_id_t mobId, view_ptr_t<Mob> owner, const Pos &pos, int32_t spawnId, bool facesLeft, foothold_id_t foothold, MobControlStatus controlStatus);

	auto applyDamage(player_id_t playerId, damage_t damage, bool poison = false) -> void;
	auto applyWebDamage() -> void;
	auto addStatus(player_id_t playerId, vector_t<StatusInfo> &statusInfo) -> void;
	auto skillHeal(int32_t healHp, int32_t healRange) -> void;
	auto dispelBuffs() -> void;
	auto doCrashSkill(skill_id_t skillId) -> void;
	auto explode() -> void;
	auto kill() -> void;
	auto consumeMp(int32_t mp) -> void;
	auto mpEat(Player *player, MpEaterInfo *mp) -> void;
	auto setSkillFeasibility(bool skillFeasible) -> void { m_skillFeasible = skillFeasible; }
	auto useAnticipatedSkill() -> Result;
	auto resetAnticipatedSkill() -> void;
	auto getStatusBits() const -> int32_t;
	auto getStatusInfo() const -> const ord_map_t<int32_t, StatusInfo> &;

	auto chooseRandomSkill(mob_skill_id_t &skillId, mob_skill_level_t &skillLevel) -> void;
	auto getSkillFeasibility() const -> bool { return m_skillFeasible; }
	auto getAnticipatedSkill() const -> mob_skill_id_t { return m_anticipatedSkill; }
	auto getAnticipatedSkillLevel() const -> mob_skill_level_t { return m_anticipatedSkillLevel; }
	auto getHpBarColor() const -> int8_t { return m_info->hpColor; }
	auto getHpBarBgColor() const -> int8_t { return m_info->hpBackgroundColor; }
	auto getVenomCount() const -> int8_t { return m_venomCount; }
	auto getOriginFoothold() const -> foothold_id_t { return m_originFoothold; }
	auto getLevel() const -> uint16_t { return m_info->level; }
	auto getMapMobId() const -> map_object_t { return m_mapMobId; }
	auto getMapId() const -> map_id_t { return m_mapId; }
	auto getMobId() const -> mob_id_t { return m_mobId; }
	auto getHp() const -> int32_t { return m_hp; }
	auto getMp() const -> int32_t { return m_mp; }
	auto getMaxHp() const -> int32_t { return m_info->hp; }
	auto getMaxMp() const -> int32_t { return m_info->mp; }
	auto getMobIdOrLink() const -> mob_id_t { return m_info->link != 0 ? m_info->link : m_mobId; }
	auto getSelfDestructHp() const -> int32_t { return m_info->selfDestruction; }
	auto getTauntEffect() const -> int32_t { return m_tauntEffect; }
	auto isBoss() const -> bool { return m_info->boss; }
	auto canFreeze() const -> bool { return m_info->canFreeze; }
	auto canPoison() const -> bool { return m_info->canPoison; }
	auto canFly() const -> bool { return m_info->flying; }
	auto isFriendly() const -> bool { return m_info->friendly; }
	auto isUndead() const -> bool { return m_info->undead; }
	auto hasExplosiveDrop() const -> bool { return m_info->explosiveReward; }
	auto hasFfaDrop() const -> bool { return m_info->publicReward; }
	auto isSponge() const -> bool { return isSponge(getMobId()); }
	auto getPos() const -> Pos override { return Pos(m_pos.x, m_pos.y - 1); }
	auto getControlStatus() const -> MobControlStatus { return m_controlStatus; }

	auto getController() const -> Player * { return m_controller; }
	auto getMap() const -> Map *;
private:
	static auto isSponge(mob_id_t mobId) -> bool;
	static auto spawnsSponge(mob_id_t mobId) -> bool;

	friend class Map;

	auto setController(Player *control, MobSpawnType spawn = MobSpawnType::Existing, Player *display = nullptr) -> void;
	auto die(Player *player, bool fromExplosion = false) -> void;
	auto distributeExpAndGetDropRecipient(Player *killer) -> player_id_t;
	auto naturalHeal(int32_t hpHeal, int32_t mpHeal) -> void;
	auto removeStatus(int32_t status, bool fromTimer = false) -> void;
	auto endControl() -> void;
	auto addSpawn(map_object_t mapMobId, view_ptr_t<Mob> mob) -> void { m_spawns[mapMobId] = mob; }
	auto setOwner(view_ptr_t<Mob> owner) -> void { m_owner = owner; }
	auto getStatusValue(int32_t status) -> int32_t;
	auto getWeaponReflection() -> int32_t;
	auto getMagicReflection() -> int32_t;
	auto hasImmunity() const -> bool;
	auto hasStatus(int32_t status) const -> bool;
	auto canCastSkills() const -> bool;
	auto getSpawnId() const -> int32_t { return m_spawnId; }
	auto getSponge() const -> view_ptr_t<Mob> { return m_sponge; }

	bool m_skillFeasible = false;
	int8_t m_venomCount = 0;
	int8_t m_mpEaterCount = 0;
	skill_level_t m_webLevel = 0;
	mob_skill_id_t m_anticipatedSkill = 0;
	mob_skill_level_t m_anticipatedSkillLevel = 0;
	foothold_id_t m_originFoothold = 0;
	int32_t m_tauntEffect = 100;
	map_object_t m_mapMobId = 0;
	map_id_t m_mapId = 0;
	int32_t m_spawnId = 0;
	mob_id_t m_mobId = 0;
	int32_t m_hp = 0;
	int32_t m_mp = 0;
	int32_t m_status = 0;
	player_id_t m_webPlayerId = 0;
	uint64_t m_totalHealth = 0;
	Player *m_controller = nullptr;
	MobControlStatus m_controlStatus = MobControlStatus::Normal;
	time_point_t m_lastSkillUse;
	view_ptr_t<Mob> m_owner;
	view_ptr_t<Mob> m_sponge;
	const ref_ptr_t<MobInfo> m_info;
	ord_map_t<int32_t, StatusInfo> m_statuses;
	hash_map_t<player_id_t, uint64_t> m_damages;
	hash_map_t<uint8_t, time_point_t> m_skillUse;
	hash_map_t<map_object_t, view_ptr_t<Mob>> m_spawns;
};