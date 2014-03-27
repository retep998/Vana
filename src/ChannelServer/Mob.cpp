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
#include "Mob.hpp"
#include "Algorithm.hpp"
#include "ChannelServer.hpp"
#include "DropHandler.hpp"
#include "GameConstants.hpp"
#include "Instance.hpp"
#include "Maps.hpp"
#include "MiscUtilities.hpp"
#include "Mist.hpp"
#include "MobsPacket.hpp"
#include "Party.hpp"
#include "Player.hpp"
#include "PlayerDataProvider.hpp"
#include "PlayerPacket.hpp"
#include "Randomizer.hpp"
#include "Skills.hpp"
#include "SkillsPacket.hpp"
#include "StatusInfo.hpp"
#include "TimeUtilities.hpp"
#include "Timer.hpp"
#include <functional>
#include <initializer_list>

Mob::Mob(map_object_t mapMobId, map_id_t mapId, mob_id_t mobId, view_ptr_t<Mob> owner, const Pos &pos, int32_t spawnId, bool facesLeft, foothold_id_t foothold, MobControlStatus controlStatus) :
	MovableLife(foothold, pos, facesLeft ? 1 : 2),
	m_mapMobId(mapMobId),
	m_mapId(mapId),
	m_spawnId(spawnId),
	m_mobId(mobId),
	m_owner(owner),
	m_info(ChannelServer::getInstance().getMobDataProvider().getMobInfo(mobId)),
	m_controlStatus(controlStatus)
{
	m_hp = getMaxHp();
	m_mp = getMaxMp();
	if (!canFly()) {
		m_originFoothold = foothold;
	}

	m_totalHealth = m_hp;

	m_status = StatusEffects::Mob::Empty;
	StatusInfo empty = StatusInfo(StatusEffects::Mob::Empty, 0, 0, 0);
	m_statuses[empty.status] = empty;

	if (m_info->hpRecovery > 0 || m_info->mpRecovery > 0) {
		int32_t hpRecovery = m_info->hpRecovery;
		int32_t mpRecovery = m_info->mpRecovery;
		Timer::Timer::create([this, hpRecovery, mpRecovery](const time_point_t &now) { this->naturalHeal(hpRecovery, mpRecovery); },
			Timer::Id(Timer::Types::MobHealTimer, 0, 0),
			getTimers(), seconds_t(1), seconds_t(10));
	}
	if (m_info->removeAfter > 0) {
		Timer::Timer::create([this](const time_point_t &now) { this->kill(); },
			Timer::Id(Timer::Types::MobRemoveTimer, m_mapMobId, 0),
			getTimers(), seconds_t(m_info->removeAfter));
	}
}

auto Mob::naturalHeal(int32_t hpHeal, int32_t mpHeal) -> void {
	if (hpHeal > 0 && getHp() < getMaxHp()) {
		int32_t hp = getHp() + hpHeal;
		int32_t spongeHp = hpHeal;
		if (hp < 0 || hp > getMaxHp()) {
			spongeHp = getMaxHp() - getHp();
			hp = getMaxHp();
		}
		m_hp = hp;
		m_totalHealth += spongeHp;
		if (auto sponge = m_sponge.lock()) {
			sponge->m_hp += spongeHp;
		}
	}
	if (mpHeal > 0 && m_mp < getMaxMp()) {
		int32_t mp = getMp() + mpHeal;
		if (mp < 0 || mp > getMaxMp()) {
			mp = getMaxMp();
		}
		m_mp = mp;
	}
}

auto Mob::applyDamage(player_id_t playerId, damage_t damage, bool poison) -> void {
	damage = std::max(damage, 0);
	if (damage > m_hp) {
		damage = m_hp - poison; // Keep HP from hitting 0 for poison and from going below 0
	}

	m_damages[playerId] += damage;
	m_hp -= damage;

	if (!poison) {
		// HP bar packet does nothing for showing damage when poison is damaging for whatever reason
		Player *player = ChannelServer::getInstance().getPlayerDataProvider().getPlayer(playerId);
		Map *map = getMap();

		uint8_t percent = static_cast<uint8_t>(m_hp * 100 / m_info->hp);

		if (m_info->hasHpBar()) {
			// Boss HP bars - Horntail's damage sponge isn't a boss in the data
			map->send(MobsPacket::showBossHp(shared_from_this()));
		}
		else if (m_info->boss) {
			// Minibosses
			map->send(MobsPacket::showHp(m_mapMobId, percent));
		}
		else if (m_info->friendly) {
			map->send(MobsPacket::damageFriendlyMob(shared_from_this(), damage));
		}
		else if (player != nullptr) {
			player->send(MobsPacket::showHp(m_mapMobId, percent));
		}

		// Need to preserve the pointer through mob deletion in die()
		auto sponge = m_sponge.lock();
		if (m_hp == Stats::MinHp) {
			die(player);
		}
		if (sponge != nullptr) {
			sponge->applyDamage(playerId, damage, false);
			// Apply damage after you can be sure that all the units are linked and ready
		}
	}
	else if (m_hp == 1) {
		removeStatus(StatusEffects::Mob::Poison);
	}
}

auto Mob::applyWebDamage() -> void {
	damage_t webDamage = getMaxHp() / (50 - m_webLevel);
	if (webDamage > m_hp) {
		// Keep HP from hitting 0
		webDamage = m_hp - 1;
	}
	if (webDamage != 0) {
		m_damages[m_webPlayerId] += webDamage;
		m_hp -= webDamage;
		getMap()->send(MobsPacket::hurtMob(m_mapMobId, webDamage));
	}
}

auto Mob::addStatus(player_id_t playerId, vector_t<StatusInfo> &statusInfo) -> void {
	int32_t addedStatus = 0;
	vector_t<int32_t> reflection;
	Map *map = getMap();

	for (auto &info : statusInfo) {
		int32_t cStatus = info.status;
		bool alreadyHasStatus = m_statuses.find(cStatus) != std::end(m_statuses);
		switch (cStatus) {
			case StatusEffects::Mob::Poison: // Status effects that do not renew
			case StatusEffects::Mob::Doom:
				if (alreadyHasStatus) {
					continue;
				}
				break;
			case StatusEffects::Mob::ShadowWeb:
				m_webPlayerId = playerId;
				m_webLevel = static_cast<skill_level_t>(info.val);
				map->addWebbedMob(getMapMobId());
				break;
			case StatusEffects::Mob::MagicAttackUp:
				switch (info.skillId) {
					case Skills::NightLord::Taunt:
					case Skills::Shadower::Taunt: {
						m_tauntEffect = (100 - info.val) + 100;
						// Value passed as 100 - x, so 100 - value will = x
						break;
					}
				}
				break;
			case StatusEffects::Mob::VenomousWeapon:
				m_venomCount++;
				if (alreadyHasStatus) {
					info.val += m_statuses[cStatus].val; // Increase the damage
				}
				break;
			case StatusEffects::Mob::WeaponDamageReflect:
			case StatusEffects::Mob::MagicDamageReflect:
				reflection.push_back(info.reflection);
				break;
		}

		m_statuses[cStatus] = info;
		addedStatus += cStatus;

		switch (cStatus) {
			case StatusEffects::Mob::Poison:
			case StatusEffects::Mob::VenomousWeapon:
			case StatusEffects::Mob::NinjaAmbush:
				damage_t poisonDamage = info.val;
				Timer::Timer::create([this, playerId, poisonDamage](const time_point_t &now) { this->applyDamage(playerId, poisonDamage, true); },
					Timer::Id(Timer::Types::MobStatusTimer, cStatus, 1),
					getTimers(), seconds_t(1), seconds_t(1));
				break;
		}

		// We add some milliseconds to our times in order to allow poisons to not end one hit early
		Timer::Timer::create([this, cStatus](const time_point_t &now) { this->removeStatus(cStatus, true); },
			Timer::Id(Timer::Types::MobStatusTimer, cStatus, 0),
			getTimers(), milliseconds_t(info.time.count() * 1000 + 100));
	}

	// Calculate new status mask
	m_status = 0;
	for (const auto &kvp : m_statuses) {
		m_status |= kvp.first;
	}
	map->send(MobsPacket::applyStatus(m_mapMobId, addedStatus, statusInfo, 300, reflection));
}

auto Mob::removeStatus(int32_t status, bool fromTimer) -> void {
	auto kvp = m_statuses.find(status);
	if (kvp != std::end(m_statuses) && getHp() > 0) {
		const StatusInfo &stat = kvp->second;
		Map *map = getMap();
		switch (status) {
			case StatusEffects::Mob::ShadowWeb:
				m_webLevel = 0;
				m_webPlayerId = 0;
				map->removeWebbedMob(getMapMobId());
				break;
			case StatusEffects::Mob::MagicAttackUp:
				switch (stat.skillId) {
					case Skills::NightLord::Taunt:
					case Skills::Shadower::Taunt:
						m_tauntEffect = 100;
						break;
				}
				break;
			case StatusEffects::Mob::VenomousWeapon:
				m_venomCount = 0;
				// Intentional fallthrough
			case StatusEffects::Mob::Poison:
				// Stop poison damage timer
				getTimers()->removeTimer(Timer::Id(Timer::Types::MobStatusTimer, status, 1));
				break;
		}
		if (!fromTimer) {
			getTimers()->removeTimer(Timer::Id(Timer::Types::MobStatusTimer, status, 0));
		}
		m_status -= status;
		m_statuses.erase(kvp);
		map->send(MobsPacket::removeStatus(m_mapMobId, status));
	}
}

auto Mob::hasImmunity() const -> bool {
	int32_t mask = StatusEffects::Mob::WeaponImmunity | StatusEffects::Mob::MagicImmunity | StatusEffects::Mob::WeaponDamageReflect | StatusEffects::Mob::MagicDamageReflect;
	return (m_status & mask) != 0;
}

auto Mob::hasStatus(int32_t status) const -> bool {
	return (m_status & status) != 0;
}

auto Mob::getStatusValue(int32_t status) -> int32_t {
	auto kvp = m_statuses.find(status);
	return kvp != std::end(m_statuses) ? kvp->second.val : 0;
}

auto Mob::getStatusBits() const -> int32_t {
	return m_status;
}

auto Mob::getStatusInfo() const -> const ord_map_t<int32_t, StatusInfo> & {
	return m_statuses;
}

auto Mob::getMagicReflection() -> int32_t {
	return getStatusValue(StatusEffects::Mob::MagicDamageReflect);
}

auto Mob::getWeaponReflection() -> int32_t {
	return getStatusValue(StatusEffects::Mob::WeaponDamageReflect);
}

auto Mob::setController(Player *control, bool spawn, Player *display) -> void {
	endControl();

	m_controller = control;
	if (control != nullptr) {
		control->send(MobsPacket::requestControl(shared_from_this(), spawn));
	}
	else if (getControlStatus() == MobControlStatus::None) {
		if (display != nullptr) {
			display->send(MobsPacket::requestControl(shared_from_this(), spawn));
		}
		else {
			getMap()->send(MobsPacket::requestControl(shared_from_this(), spawn));
		}
	}

	m_anticipatedSkill = 0;
	m_anticipatedSkillLevel = 0;
	m_skillFeasible = false;
}

auto Mob::endControl() -> void {
	// TODO FIXME resource
	// isDisconnecting should not be necessary here, but it requires a great deal of structural fixing to properly fix
	if (m_controller != nullptr && m_controller->getMapId() == getMapId() && !m_controller->isDisconnecting()) {
		m_controller->send(MobsPacket::endControlMob(m_mapMobId));
	}
}

auto Mob::die(Player *player, bool fromExplosion) -> void {
	Map *map = getMap();

	endControl();

	player_id_t highestDamager = distributeExpAndGetDropRecipient(player);

	// Ending of death stuff
	DropHandler::doDrops(highestDamager, m_mapId, getLevel(), m_mobId, getPos(), hasExplosiveDrop(), hasFfaDrop(), getTauntEffect());

	if (player != nullptr) {
		Party *party = player->getParty();
		if (party != nullptr) {
			auto members = party->getPartyMembers(m_mapId);
			for (const auto &member : members) {
				member->getQuests()->updateQuestMob(m_mobId);
			}
		}
		else {
			player->getQuests()->updateQuestMob(m_mobId);
		}
	}

	map->mobDeath(shared_from_this(), fromExplosion);
}

auto Mob::explode() -> void {
	die(nullptr, true);
}

auto Mob::kill() -> void {
	applyDamage(0, getHp());
}

auto Mob::consumeMp(int32_t mp) -> void {
	m_mp = std::max(m_mp - mp, 0);
}

auto Mob::distributeExpAndGetDropRecipient(Player *killer) -> player_id_t {
	player_id_t highestDamager = 0;
	uint64_t highestDamage = 0;

	if (m_damages.size() > 0) {
		struct PartyExp {
			PartyExp() : totalExp(0), party(nullptr), highestDamager(nullptr), highestDamage(0), minHitLevel(Stats::PlayerLevels) { }
			player_level_t minHitLevel;
			uint64_t totalExp;
			uint64_t highestDamage;
			Player *highestDamager;
			Party *party;
		};

		hash_map_t<party_id_t, PartyExp> parties;

		int32_t mobExpRate = ChannelServer::getInstance().getConfig().rates.mobExpRate;

		for (const auto &kvp : m_damages) {
			player_id_t damagerId = kvp.first;
			uint64_t damage = kvp.second;
			if (damage > highestDamage) {
				// Find the highest damager to give drop ownership
				highestDamager = damagerId;
				highestDamage = damage;
			}

			Player *damager = ChannelServer::getInstance().getPlayerDataProvider().getPlayer(damagerId);
			if (damager == nullptr || damager->getMapId() != m_mapId || damager->getStats()->isDead()) {
				// Only give EXP if the damager is in the same channel, on the same map and is alive
				continue;
			}
			player_level_t damagerLevel = damager->getStats()->getLevel();
			Party *damagerParty = damager->getParty();

			uint64_t exp = static_cast<uint64_t>(m_info->exp) * ((8 * damage / m_totalHealth) + (damager == killer ? 2 : 0)) / 10;
			if (damagerParty != nullptr) {
				party_id_t partyId = damagerParty->getId();
				auto kvp = parties.find(partyId);
				if (kvp == std::end(parties)) {
					PartyExp newParty;
					newParty.totalExp = 0;
					newParty.party = damagerParty;
					kvp = parties.emplace(partyId, newParty).first;
				}

				PartyExp &damagingParty = kvp->second;
				damagingParty.totalExp += exp;

				if (damagerLevel < damagingParty.minHitLevel) {
					damagingParty.minHitLevel = damagerLevel;
				}
				if (damage > damagingParty.highestDamage) {
					damagingParty.highestDamager = damager;
					damagingParty.highestDamage = damage;
				}
			}
			else {
				// Account for EXP increasing junk
				int16_t hsRate = damager->getActiveBuffs()->getHolySymbolRate();
				exp = exp * getTauntEffect() / 100;
				exp *= mobExpRate;
				exp += ((exp * hsRate) / 100);
				damager->getStats()->giveExp(exp, false, (damager == killer));
			}
		}

		if (parties.size() > 0) {
			for (const auto &kvp : parties) {
				const PartyExp &info = kvp.second;
				Party *damagerParty = info.party;
				vector_t<Player *> partyMembers = damagerParty->getPartyMembers(getMapId());
				uint16_t totalLevel = 0;
				uint16_t leechCount = 0;
				for (const auto &partyMember : partyMembers) {
					player_level_t damagerLevel = partyMember->getStats()->getLevel();
					if (damagerLevel < (info.minHitLevel - 5) && damagerLevel < (getLevel() - 5)) {
						continue;
					}
					totalLevel += damagerLevel;
					leechCount++;
				}
				for (const auto &partyMember : partyMembers) {
					player_level_t damagerLevel = partyMember->getStats()->getLevel();
					if (damagerLevel < (info.minHitLevel - 5) && damagerLevel < (getLevel() - 5)) {
						continue;
					}
					uint64_t exp = static_cast<uint64_t>(m_info->exp) * ((8 * damagerLevel / totalLevel) + (partyMember == info.highestDamager ? 2 : 0)) / 10;
					int16_t hsRate = partyMember->getActiveBuffs()->getHolySymbolRate();
					exp = exp * getTauntEffect() / 100;
					exp *= mobExpRate;
					exp += ((exp * hsRate) / 100);
					partyMember->getStats()->giveExp(exp, false, (partyMember == killer));
				}
			}
		}
	}
	return highestDamager;
}

auto Mob::skillHeal(int32_t healHp, int32_t healRange) -> void {
	if (isSponge()) {
		return;
	}
	int32_t min = (healHp - (healRange / 2));
	int32_t max = (healHp + (healRange / 2));
	int32_t amount = Randomizer::rand<int32_t>(max, min);
	int32_t original = amount;

	if (m_hp + amount > getMaxHp()) {
		amount = getMaxHp() - m_hp;
		m_hp = getMaxHp();
	}
	else {
		m_hp += amount;
	}
	m_totalHealth += amount;

	if (auto sponge = m_sponge.lock()) {
		healHp = sponge->getHp() + amount;
		healHp = ext::constrain_range<int32_t>(healHp, Stats::MinHp, sponge->getMaxHp());
		sponge->m_hp = healHp;
	}

	getMap()->send(MobsPacket::healMob(m_mapMobId, original));
}

auto Mob::dispelBuffs() -> void {
	auto statuses = {
		StatusEffects::Mob::Watk, StatusEffects::Mob::Wdef,
		StatusEffects::Mob::Matk, StatusEffects::Mob::Mdef,
		StatusEffects::Mob::Acc, StatusEffects::Mob::Avoid,
		StatusEffects::Mob::Speed,
	};

	for (const auto &status : statuses) {
		removeStatus(status);
	}
}

auto Mob::doCrashSkill(skill_id_t skillId) -> void {
	switch (skillId) {
		case Skills::Crusader::ArmorCrash: removeStatus(StatusEffects::Mob::Wdef); break;
		case Skills::WhiteKnight::MagicCrash: removeStatus(StatusEffects::Mob::Matk); break;
		case Skills::DragonKnight::PowerCrash: removeStatus(StatusEffects::Mob::Watk); break;
	}
}

auto Mob::mpEat(Player *player, MpEaterInfo *mp) -> void {
	if (m_mpEaterCount < 3 && getMp() > 0 && Randomizer::rand<uint16_t>(99) < mp->prop) {
		mp->used = true;
		int32_t eatenMp = getMaxMp() * mp->x / 100;

		eatenMp = std::min<int32_t>(eatenMp, getMp());
		m_mp = getMp() - eatenMp;

		eatenMp = std::min<int32_t>(eatenMp, Stats::MaxMaxMp);
		player->getStats()->modifyMp(eatenMp);

		player->sendMap(SkillsPacket::showSkillEffect(player->getId(), mp->skillId));
		m_mpEaterCount++;
	}
}

auto Mob::chooseRandomSkill(mob_skill_id_t &skillId, mob_skill_level_t &skillLevel) -> void {
	if (m_info->skillCount == 0 || m_anticipatedSkill != 0 || !canCastSkills()) {
		return;
	}

	time_point_t now = TimeUtilities::getNow();
	if (TimeUtilities::getDistanceInSeconds(now, m_lastSkillUse) < seconds_t(3)) {
		return;
	}

	vector_t<const MobSkillInfo *> viableSkills;
	auto &skills = ChannelServer::getInstance().getMobDataProvider().getSkills(getMobIdOrLink());
	for (const auto &info : skills) {
		bool stop = false;
		auto mobSkill = ChannelServer::getInstance().getSkillDataProvider().getMobSkill(info.skillId, info.level);

		switch (info.skillId) {
			case MobSkills::WeaponAttackUp:
			case MobSkills::WeaponAttackUpAoe:
				stop = hasStatus(StatusEffects::Mob::Watk);
				break;
			case MobSkills::MagicAttackUp:
			case MobSkills::MagicAttackUpAoe:
				stop = hasStatus(StatusEffects::Mob::Matk);
				break;
			case MobSkills::WeaponDefenseUp:
			case MobSkills::WeaponDefenseUpAoe:
				stop = hasStatus(StatusEffects::Mob::Wdef);
				break;
			case MobSkills::MagicDefenseUp:
			case MobSkills::MagicDefenseUpAoe:
				stop = hasStatus(StatusEffects::Mob::Mdef);
				break;
			case MobSkills::WeaponImmunity:
			case MobSkills::MagicImmunity:
			case MobSkills::WeaponDamageReflect:
			case MobSkills::MagicDamageReflect:
				stop = hasImmunity();
				break;
			case MobSkills::McSpeedUp:
				stop = hasStatus(StatusEffects::Mob::Speed);
				break;
			case MobSkills::Summon:
				stop = static_cast<int16_t>(m_spawns.size()) > mobSkill->limit;
				break;
		}

		if (stop) {
			continue;
		}

		auto kvp = m_skillUse.find(info.skillId);
		if (kvp != std::end(m_skillUse)) {
			time_point_t targetTime = kvp->second + seconds_t(mobSkill->cooldown);
			stop = now < targetTime;
		}

		if (!stop) {
			double currentMobHpPercentage = static_cast<double>(getHp()) * 100. / static_cast<double>(getMaxHp());
			stop = currentMobHpPercentage > static_cast<double>(mobSkill->hp);
		}

		if (!stop) {
			viableSkills.push_back(&info);
		}
	}

	if (viableSkills.size() == 0) {
		return;
	}

	auto skill = *Randomizer::select(viableSkills);
	skillId = skill->skillId;
	skillLevel = skill->level;
	m_anticipatedSkill = skillId;
	m_anticipatedSkillLevel = skillLevel;
}

auto Mob::resetAnticipatedSkill() -> void {
	m_anticipatedSkill = 0;
	m_anticipatedSkillLevel = 0;
}

auto Mob::useAnticipatedSkill() -> Result {
	mob_skill_id_t skillId = m_anticipatedSkill;
	mob_skill_level_t level = m_anticipatedSkillLevel;

	resetAnticipatedSkill();

	if (!canCastSkills()) {
		return Result::Failure;
	}

	time_point_t now = TimeUtilities::getNow();
	m_skillUse[skillId] = now;
	m_lastSkillUse = now;

	auto skillLevelInfo = ChannelServer::getInstance().getSkillDataProvider().getMobSkill(skillId, level);
	consumeMp(skillLevelInfo->mp);

	Rect skillArea = skillLevelInfo->dimensions.move(getPos());
	Map *map = getMap();
	vector_t<StatusInfo> statuses;
	bool aoe = false;

	switch (skillId) {
		case MobSkills::WeaponAttackUpAoe:
			aoe = true;
		case MobSkills::WeaponAttackUp:
			statuses.emplace_back(StatusEffects::Mob::Watk, skillLevelInfo->x, skillId, level, skillLevelInfo->time);
			break;
		case MobSkills::MagicAttackUpAoe:
			aoe = true;
		case MobSkills::MagicAttackUp:
			statuses.emplace_back(StatusEffects::Mob::Matk, skillLevelInfo->x, skillId, level, skillLevelInfo->time);
			break;
		case MobSkills::WeaponDefenseUpAoe:
			aoe = true;
		case MobSkills::WeaponDefenseUp:
			statuses.emplace_back(StatusEffects::Mob::Wdef, skillLevelInfo->x, skillId, level, skillLevelInfo->time);
			break;
		case MobSkills::MagicDefenseUpAoe:
			aoe = true;
		case MobSkills::MagicDefenseUp:
			statuses.emplace_back(StatusEffects::Mob::Mdef, skillLevelInfo->x, skillId, level, skillLevelInfo->time);
			break;
		case MobSkills::HealAoe:
			map->healMobs(skillLevelInfo->x, skillLevelInfo->y, skillArea);
			break;
		case MobSkills::Seal:
		case MobSkills::Darkness:
		case MobSkills::Weakness:
		case MobSkills::Stun:
		case MobSkills::Curse:
		case MobSkills::Poison:
		case MobSkills::Slow:
		case MobSkills::Seduce:
		case MobSkills::CrazySkull:
		case MobSkills::Zombify: {
			auto func = [&skillId, &level](Player *player) {
				player->getActiveBuffs()->addDebuff(skillId, level);
			};
			map->runFunctionPlayers(skillArea, skillLevelInfo->prop, skillLevelInfo->count, func);
			break;
		}
		case MobSkills::Dispel: {
			map->runFunctionPlayers(skillArea, skillLevelInfo->prop, [](Player *player) {
				player->getActiveBuffs()->dispelBuffs();
			});
			break;
		}
		case MobSkills::SendToTown: {
			map_id_t field = map->getReturnMap();
			PortalInfo *portal = nullptr;
			string_t message;
			if (auto banishInfo = ChannelServer::getInstance().getSkillDataProvider().getBanishData(getMobId())) {
				field = banishInfo->field;
				message = banishInfo->message;
				if (banishInfo->portal != "" && banishInfo->portal != "sp") {
					portal = Maps::getMap(field)->getPortal(banishInfo->portal);
				}
			}
			auto func = [&message, &field, &portal](Player *player) {
				if (message != "") {
					player->send(PlayerPacket::showMessage(message, PlayerPacket::NoticeTypes::Blue));
				}
				player->setMap(field, portal);
			};
			map->runFunctionPlayers(skillArea, skillLevelInfo->prop, skillLevelInfo->count, func);
			break;
		}
		case MobSkills::PoisonMist:
			new Mist(getMapId(), this, skillLevelInfo->time, skillArea, skillId, level);
			break;
		case MobSkills::WeaponImmunity:
			statuses.emplace_back(StatusEffects::Mob::WeaponImmunity, skillLevelInfo->x, skillId, level, skillLevelInfo->time);
			break;
		case MobSkills::MagicImmunity:
			statuses.emplace_back(StatusEffects::Mob::MagicImmunity, skillLevelInfo->x, skillId, level, skillLevelInfo->time);
			break;
		case MobSkills::WeaponDamageReflect:
			statuses.emplace_back(StatusEffects::Mob::WeaponImmunity, skillLevelInfo->x, skillId, level, skillLevelInfo->time);
			statuses.emplace_back(StatusEffects::Mob::WeaponDamageReflect, skillLevelInfo->x, skillId, level, skillLevelInfo->y, skillLevelInfo->time);
			break;
		case MobSkills::MagicDamageReflect:
			statuses.emplace_back(StatusEffects::Mob::MagicImmunity, skillLevelInfo->x, skillId, level, skillLevelInfo->time);
			statuses.emplace_back(StatusEffects::Mob::MagicDamageReflect, skillLevelInfo->x, skillId, level, skillLevelInfo->y, skillLevelInfo->time);
			break;
		case MobSkills::AnyDamageReflect:
			statuses.emplace_back(StatusEffects::Mob::WeaponImmunity, skillLevelInfo->x, skillId, level, skillLevelInfo->time);
			statuses.emplace_back(StatusEffects::Mob::MagicImmunity, skillLevelInfo->x, skillId, level, skillLevelInfo->time);
			statuses.emplace_back(StatusEffects::Mob::WeaponDamageReflect, skillLevelInfo->x, skillId, level, skillLevelInfo->y, skillLevelInfo->time);
			statuses.emplace_back(StatusEffects::Mob::MagicDamageReflect, skillLevelInfo->x, skillId, level, skillLevelInfo->y, skillLevelInfo->time);
			break;
		case MobSkills::McSpeedUp:
			statuses.emplace_back(StatusEffects::Mob::Speed, skillLevelInfo->x, skillId, level, skillLevelInfo->time);
			break;
		case MobSkills::Summon:
			map->mobSummonSkillUsed(shared_from_this(), skillLevelInfo);
			break;
	}

	if (statuses.size() > 0) {
		if (aoe) {
			map->statusMobs(statuses, skillArea);
		}
		else {
			addStatus(0, statuses);
		}
	}

	return Result::Successful;
}

auto Mob::canCastSkills() const -> bool {
	return !(hasStatus(StatusEffects::Mob::Freeze) || hasStatus(StatusEffects::Mob::Stun) || hasStatus(StatusEffects::Mob::ShadowWeb) || hasStatus(StatusEffects::Mob::Seal));
}

auto Mob::isSponge(mob_id_t mobId) -> bool {
	switch (mobId) {
		case Mobs::HorntailSponge: return true;
	}
	return false;
}

auto Mob::spawnsSponge(mob_id_t mobId) -> bool {
	switch (mobId) {
		case Mobs::SummonHorntail: return true;
	}
	return false;
}

auto Mob::getMap() const -> Map * {
	return Maps::getMap(m_mapId);
}