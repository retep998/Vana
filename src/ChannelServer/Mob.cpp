/*
Copyright (C) 2008-2012 Vana Development Team

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
#include "Mob.h"
#include "ChannelServer.h"
#include "DropHandler.h"
#include "Instance.h"
#include "Maps.h"
#include "MiscUtilities.h"
#include "MobConstants.h"
#include "MobsPacket.h"
#include "PacketCreator.h"
#include "Party.h"
#include "Player.h"
#include "PlayerDataProvider.h"
#include "Randomizer.h"
#include "Skills.h"
#include "SkillsPacket.h"
#include "TimeUtilities.h"
#include "Timer.h"
#include <functional>

StatusInfo::StatusInfo(int32_t status, int32_t val, int32_t skillId, clock_t time) :
	status(status),
	val(val),
	skillId(skillId),
	mobSkill(0),
	level(0),
	reflection(0),
	time(time)
{
	switch (val) {
		case StatusEffects::Mob::Freeze:
			if (skillId == Jobs::FpArchMage::Paralyze) {
				break;
			}
		case StatusEffects::Mob::Stun:
			this->time = time + 1 + Randomizer::Instance()->randInt(time * 2); // The 1 accounts for the skill cast time
			if (skillId == Jobs::IlArchMage::Blizzard) {
				time += 2; // Account for skill cast time, ideally we'd like to remove both these additions with MCDB suport for cast times
			}
			break;
	}
}

StatusInfo::StatusInfo(int32_t status, int32_t val, int16_t mobSkill, int16_t level, clock_t time) :
	status(status),
	val(val),
	skillId(-1),
	mobSkill(mobSkill),
	level(level),
	time(time),
	reflection(-1)
{
}

StatusInfo::StatusInfo(int32_t status, int32_t val, int16_t mobSkill, int16_t level, int32_t reflect, clock_t time) :
	status(status),
	val(val),
	skillId(-1),
	mobSkill(mobSkill),
	level(level),
	time(time),
	reflection(reflect)
{
}

Mob::Mob(int32_t id, int32_t mapId, int32_t mobId, const Pos &pos, int16_t fh, int8_t controlStatus) :
	MovableLife(fh, pos, 2),
	m_originFh(fh),
	m_id(id),
	m_mapId(mapId),
	m_spawnId(-1),
	m_mobId(mobId),
	m_timers(new Timer::Container),
	m_info(MobDataProvider::Instance()->getMobInfo(mobId)),
	m_facingDirection(1),
	m_controlStatus(controlStatus)
{
	initMob();
}

Mob::Mob(int32_t id, int32_t mapId, int32_t mobId, const Pos &pos, int32_t spawnId, int8_t direction, int16_t fh) :
	MovableLife(fh, pos, 2),
	m_originFh(fh),
	m_id(id),
	m_mapId(mapId),
	m_spawnId(spawnId),
	m_mobId(mobId),
	m_timers(new Timer::Container),
	m_info(MobDataProvider::Instance()->getMobInfo(mobId)),
	m_facingDirection(direction),
	m_controlStatus(Mobs::ControlStatus::ControlNormal)
{
	initMob();
}

void Mob::initMob() {
	m_hp = getMaxHp();
	m_mp = getMaxMp();
	if (canFly()) {
		m_originFh = 0;
	}

	m_totalHealth = m_hp;

	m_owner = nullptr; // Pointers
	m_sponge = nullptr;
	m_controller = nullptr;

	m_webPlayerId = 0; // Skill stuff
	m_webLevel = 0;
	m_counter = 0;
	m_venomCount = 0;
	m_mpEaterCount = 0;
	m_tauntEffect = 100;

	Map *map = Maps::getMap(m_mapId);

	if (Instance *instance = map->getInstance()) {
		instance->sendMessage(MobSpawn, m_mobId, m_id, m_mapId);
	}

	m_status = StatusEffects::Mob::Empty;
	m_status |= StatusEffects::Mob::NoClue7;

	StatusInfo empty = StatusInfo(StatusEffects::Mob::Empty, 0, 0, 0);
	m_statuses[empty.status] = empty;

	empty = StatusInfo(StatusEffects::Mob::NoClue7, 0, 0, 0);
	m_statuses[empty.status] = empty;

	if (m_info->hpRecovery > 0) {
		new Timer::Timer(std::bind(&Mob::naturalHealHp, this, m_info->hpRecovery),
			Timer::Id(Timer::Types::MobHealTimer, 0, 0),
			getTimers(), 0, 10 * 1000);
	}
	if (m_info->mpRecovery > 0) {
		new Timer::Timer(std::bind(&Mob::naturalHealMp, this, m_info->mpRecovery),
			Timer::Id(Timer::Types::MobHealTimer, 1, 1),
			getTimers(), 0, 10 * 1000);
	}
	if (m_info->removeAfter > 0) {
		new Timer::Timer(std::bind(&Mob::applyDamage, this, 0, m_info->hp, false),
			Timer::Id(Timer::Types::MobRemoveTimer, m_mobId, m_id),
			map->getTimers(), TimeUtilities::fromNow(m_info->removeAfter * 1000));
	}
}

void Mob::naturalHealHp(int32_t amount) {
	if (getHp() < getMaxHp()) {
		int32_t hp = getHp() + amount;
		int32_t spongeHp = amount;
		if (hp < 0 || hp > getMaxHp()) {
			spongeHp = getMaxHp() - getHp(); // This is the amount for the sponge
			hp = getMaxHp();
		}
		setHp(hp);
		m_totalHealth += spongeHp;
		if (Mob *sponge = getSponge()) {
			sponge->setHp(sponge->getHp() + spongeHp);
		}
	}
}

void Mob::naturalHealMp(int32_t amount) {
	if (getMp() < getMaxMp()) {
		int32_t mp = getMp() + amount;
		if (mp < 0 || mp > getMaxMp()) {
			mp = getMaxMp();
		}
		setMp(mp);
	}
}

void Mob::applyDamage(int32_t playerId, int32_t damage, bool poison) {
	damage = std::max(damage, 0);
	if (damage > m_hp) {
		damage = m_hp - poison; // Keep HP from hitting 0 for poison and from going below 0
	}

	m_damages[playerId] += damage;
	m_hp -= damage;

	if (!poison) {
		// HP bar packet does nothing for showing damage when poison is damaging for whatever reason
		Player *player = PlayerDataProvider::Instance()->getPlayer(playerId);

		uint8_t percent = static_cast<uint8_t>(m_hp * 100 / m_info->hp);

		if (m_info->hasHpBar()) {
			// Boss HP bars - Horntail's damage sponge isn't a boss in the data
			MobsPacket::showBossHp(this);
		}
		else if (m_info->boss) {
			// Minibosses
			MobsPacket::showHp(m_mapId, m_id, percent);
		}
		else if (m_info->friendly) {
			MobsPacket::damageFriendlyMob(this, damage);
		}
		else if (player != nullptr) {
			MobsPacket::showHp(player, m_id, percent);
		}

		Mob *sponge = getSponge(); // Need to preserve the pointer through mob deletion in die()
		if (m_hp == Stats::MinHp) {
			// Time to die
			if (isSponge()) {
				// Workaround for GCC
				// In particular, it was not selecting overloads properly in the timer statement
				// It had to choose between (Player *, Player * + bool, bool, or no args)
				// It couldn't manage that simple task in a context where it's obvious which one
				void (Mob::* properOverload)(bool) = &Mob::die;
				for (unordered_map<int32_t, Mob *>::iterator spawnIter = m_spawns.begin(); spawnIter != m_spawns.end(); ++spawnIter) {
					new Timer::Timer(std::bind(properOverload, spawnIter->second, true),
						Timer::Id(Timer::Types::SpongeCleanupTimer, m_id, spawnIter->first),
						nullptr, TimeUtilities::fromNow(400));
				}
			}
			else {
				switch (getMobId()) {
					case Mobs::ZakumArm1:
					case Mobs::ZakumArm2:
					case Mobs::ZakumArm3:
					case Mobs::ZakumArm4:
					case Mobs::ZakumArm5:
					case Mobs::ZakumArm6:
					case Mobs::ZakumArm7:
					case Mobs::ZakumArm8:
						if (getOwner() != nullptr && getOwner()->getSpawnCount() == 1) {
							// Last linked arm died
							int8_t cStatus = Mobs::ControlStatus::ControlNormal;
							getOwner()->setControlStatus(cStatus);
						}
						break;
				}
			}
			die(player);
		}
		if (sponge != nullptr) {
			sponge->applyDamage(playerId, damage, false);
			// Apply damage after you can be sure that all the units are linked and ready
		}
	}
	// TODO: Fix this, for some reason, it causes issues within the timer container
//	else if (hp == 1) {
//		removeStatus(StatusEffects::Mob::Poison);
//	}
}

void Mob::applyWebDamage() {
	int32_t webDamage = getMaxHp() / (50 - m_webLevel);
	if (webDamage > m_hp) {
		// Keep HP from hitting 0
		webDamage = m_hp - 1;
	}
	if (webDamage != 0) {
		m_damages[m_webPlayerId] += webDamage;
		m_hp -= webDamage;
		MobsPacket::hurtMob(this, webDamage);
	}
}

void Mob::addStatus(int32_t playerId, vector<StatusInfo> &statusInfo) {
	int32_t addedStatus = 0;
	vector<int32_t> reflection;
	for (size_t i = 0; i < statusInfo.size(); i++) {
		int32_t cStatus = statusInfo[i].status;
		bool alreadyHas = (m_statuses.find(cStatus) != m_statuses.end());
		switch (cStatus) {
			case StatusEffects::Mob::Poison: // Status effects that do not renew
			case StatusEffects::Mob::Doom:
				if (alreadyHas) {
					continue;
				}
				break;
			case StatusEffects::Mob::ShadowWeb:
				m_webPlayerId = playerId;
				m_webLevel = static_cast<uint8_t>(statusInfo[i].val);
				Maps::getMap(m_mapId)->addWebbedMob(this);
				break;
			case StatusEffects::Mob::MagicAttackUp:
				switch (statusInfo[i].skillId) {
					case Jobs::NightLord::Taunt:
					case Jobs::Shadower::Taunt: {
						m_tauntEffect = (100 - statusInfo[i].val) + 100;
						// Value passed as 100 - x, so 100 - value will = x
						break;
					}
				}
				break;
			case StatusEffects::Mob::VenomousWeapon:
				setVenomCount(getVenomCount() + 1);
				if (alreadyHas) {
					statusInfo[i].val += m_statuses[cStatus].val; // Increase the damage
				}
				break;
			case StatusEffects::Mob::WeaponDamageReflect:
			case StatusEffects::Mob::MagicDamageReflect:
				reflection.push_back(statusInfo[i].reflection);
				break;
		}

		m_statuses[cStatus] = statusInfo[i];
		addedStatus += cStatus;

		switch (cStatus) {
			case StatusEffects::Mob::Poison:
			case StatusEffects::Mob::VenomousWeapon:
			case StatusEffects::Mob::NinjaAmbush:
				// Damage timer for poison(s)
				new Timer::Timer(std::bind(&Mob::applyDamage, this, playerId, statusInfo[i].val, true),
					Timer::Id(Timer::Types::MobStatusTimer, cStatus, 1),
					getTimers(), 0, 1000);
				break;
		}

		new Timer::Timer(std::bind(&Mob::removeStatus, this, cStatus, true),
			Timer::Id(Timer::Types::MobStatusTimer, cStatus, 0),
			getTimers(), TimeUtilities::fromNow(statusInfo[i].time * 1000));
	}
	// Calculate new status mask
	m_status = 0;
	for (map<int32_t, StatusInfo>::iterator iter = m_statuses.begin(); iter != m_statuses.end(); ++iter) {
		m_status |= iter->first;
	}
	MobsPacket::applyStatus(this, addedStatus, statusInfo, 300, reflection);
}

void Mob::statusPacket(PacketCreator &packet) {
	//m_buffs.AppendBytes(packet);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);

	packet.add<int32_t>(0);
	packet.add<int32_t>(0);

	packet.add<int32_t>(0);
	packet.add<int32_t>(0);

	packet.add<int32_t>(0x07C0); // C0 07 00 00
	packet.add<int32_t>(m_status); // Fuck.
	for (map<int32_t, StatusInfo>::iterator iter = m_statuses.begin(); iter != m_statuses.end(); ++iter) {
		// Val/skillId pairs must be ordered in the packet by status value ascending, this is done for us by std::map
		if (iter->first != StatusEffects::Mob::Empty) {
			packet.add<int16_t>(static_cast<int16_t>(iter->second.val));
			if (iter->second.skillId >= 0) {
				packet.add<int32_t>(iter->second.skillId);
			}
			else {
				packet.add<int16_t>(iter->second.mobSkill);
				packet.add<int16_t>(iter->second.level);
			}
			packet.add<int16_t>(1);
		}
		else {
			packet.add<int32_t>(0);
		}
	}
}

void Mob::removeStatus(int32_t status, bool fromTimer) {
	if (hasStatus(status) && getHp() > 0) {
		StatusInfo *stat = &m_statuses[status];
		switch (status) {
			case StatusEffects::Mob::ShadowWeb:
				m_webLevel = 0;
				m_webPlayerId = 0;
				Maps::getMap(m_mapId)->removeWebbedMob(getId());
				break;
			case StatusEffects::Mob::MagicAttackUp:
				switch (stat->skillId) {
					case Jobs::NightLord::Taunt:
					case Jobs::Shadower::Taunt:
						m_tauntEffect = 100;
						break;
				}
				break;
			case StatusEffects::Mob::VenomousWeapon:
				setVenomCount(0);
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
		m_statuses.erase(status);
		MobsPacket::removeStatus(this, status);
	}
}

bool Mob::hasStatus(int32_t status) const {
	return ((m_status & status) != 0);
}

bool Mob::hasImmunity() const {
	int32_t mask = StatusEffects::Mob::WeaponImmunity | StatusEffects::Mob::MagicImmunity;
	return ((m_status & mask) != 0 || hasReflect());
}

bool Mob::hasReflect() const {
	int32_t mask = StatusEffects::Mob::WeaponDamageReflect | StatusEffects::Mob::MagicDamageReflect;
	return ((m_status & mask) != 0);
}

bool Mob::hasWeaponReflect() const {
	return hasStatus(StatusEffects::Mob::WeaponDamageReflect);
}

bool Mob::hasMagicReflect() const {
	return hasStatus(StatusEffects::Mob::MagicDamageReflect);
}

int32_t Mob::getStatusValue(int32_t status) {
	return (hasStatus(status) ? m_statuses[status].val : 0);
}

int32_t Mob::getMagicReflection() {
	return getStatusValue(StatusEffects::Mob::MagicDamageReflect);
}

int32_t Mob::getWeaponReflection() {
	return getStatusValue(StatusEffects::Mob::WeaponDamageReflect);
}

void Mob::setControl(Player *control, bool spawn, Player *display) {
	m_controller = control;
	if (control != nullptr) {
		MobsPacket::requestControl(control, this, spawn);
	}
	else if (getControlStatus() == Mobs::ControlStatus::ControlNone) {
		MobsPacket::requestControl(control, this, spawn, display);
	}
}

void Mob::endControl() {
	if (m_controller != nullptr && m_controller->getMap() == getMapId()) {
		MobsPacket::endControlMob(m_controller, this);
	}
}

void Mob::die(Player *player, bool fromExplosion) {
	Map *map = Maps::getMap(m_mapId);

	endControl();

	Timer::Id tid(Timer::Types::MobRemoveTimer, m_mobId, m_id);
	if (map->getTimers()->checkTimer(tid) > 0) {
		map->getTimers()->removeTimer(tid);
	}

	int32_t highestDamager = giveExp(player);
	spawnDeathMobs(map);
	updateSpawnLinks();

	if (hasStatus(StatusEffects::Mob::ShadowWeb)) {
		map->removeWebbedMob(getId());
	}

	// Ending of death stuff
	MobsPacket::dieMob(this, fromExplosion ? 4 : 1);
	DropHandler::doDrops(highestDamager, m_mapId, getLevel(), m_mobId, getPos(), hasExplosiveDrop(), hasFfaDrop(), getTauntEffect());

	if (player != nullptr) {
		Party *party = player->getParty();
		if (party != nullptr) {
			vector<Player *> members = party->getPartyMembers(m_mapId);
			for (size_t memSize = 0; memSize < members.size(); memSize++) {
				members[memSize]->getQuests()->updateQuestMob(m_mobId);
			}
		}
		else {
			player->getQuests()->updateQuestMob(m_mobId);
		}
	}

	if (getDeathBuff() != 0) {
		map->buffPlayers(getDeathBuff());
	}

	if (Instance *instance = map->getInstance()) {
		instance->sendMessage(MobDeath, m_mobId, m_id, m_mapId);
	}
	map->removeMob(m_id, m_spawnId);

	delete this;
}

void Mob::explode() {
	die(nullptr, true);
}

void Mob::die(bool showPacket) {
	if (showPacket) {
		endControl();
		MobsPacket::dieMob(this);
		if (Instance *instance = Maps::getMap(m_mapId)->getInstance()) {
			instance->sendMessage(MobDeath, m_mobId, m_id);
		}
	}
	Maps::getMap(m_mapId)->removeMob(m_id, m_spawnId);
	delete this;
}

int32_t Mob::giveExp(Player *killer) {
	int32_t highestDamager = 0;
	uint64_t highestDamage = 0;

	if (m_damages.size()) {
		// Don't really want to bother with construction of the iterators and stuff if we won't use them
		unordered_map<int32_t, PartyExp> parties;
		Player *damager = nullptr;
		uint8_t damagerLevel = 0;
		Party *damagerParty = nullptr;
		for (unordered_map<int32_t, uint64_t>::iterator iter = m_damages.begin(); iter != m_damages.end(); ++iter) {
			if (iter->second > highestDamage) {
				// Find the highest damager to give drop ownership
				highestDamager = iter->first;
				highestDamage = iter->second;
			}
			damager = PlayerDataProvider::Instance()->getPlayer(iter->first);
			if (damager == nullptr || damager->getMap() != m_mapId || damager->getStats()->isDead()) {
				// Only give EXP if the damager is in the same channel, on the same map and is alive
				continue;
			}
			damagerLevel = damager->getStats()->getLevel();
			damagerParty = damager->getParty();

			uint32_t exp = static_cast<uint32_t>(getExp() * ((8 * iter->second / m_totalHealth) + (damager == killer ? 2 : 0)) / 10);
			if (damagerParty != nullptr) {
				int32_t pId = damagerParty->getId();
				if (parties.find(pId) != parties.end()) {
					parties[pId].totalExp += exp;
				}
				else {
					parties[pId].totalExp = exp;
					parties[pId].party = damagerParty;
				}
				if (damagerLevel < parties[pId].minHitLevel) {
					parties[pId].minHitLevel = damagerLevel;
				}
				if (iter->second > parties[pId].highestDamage) {
					parties[pId].highestDamager = damager;
					parties[pId].highestDamage = iter->second;
				}
			}
			else {
				// Account for EXP increasing junk
				int16_t hsRate = damager->getActiveBuffs()->getHolySymbolRate();
				exp = exp * getTauntEffect() / 100;
				exp *= ChannelServer::Instance()->getExpRate();
				exp += ((exp * hsRate) / 100);
				damager->getStats()->giveExp(exp, false, (damager == killer));
			}
		}
		if (parties.size()) {
			vector<Player *> partyMembers;
			for (unordered_map<int32_t, PartyExp>::iterator partyIter = parties.begin(); partyIter != parties.end(); ++partyIter) {
				damagerParty = partyIter->second.party;
				partyMembers = damagerParty->getPartyMembers(getMapId());
				uint16_t totalLevel = 0;
				uint16_t leechCount = 0;
				for (size_t i = 0; i < partyMembers.size(); i++) {
					damager = partyMembers[i];
					if (damagerLevel < (partyIter->second.minHitLevel - 5) && damagerLevel < (getLevel() - 5)) {
						continue;
					}
					totalLevel += damagerLevel;
					leechCount++;
				}
				for (size_t i = 0; i < partyMembers.size(); i++) {
					damager = partyMembers[i];
					if (damagerLevel < (partyIter->second.minHitLevel - 5) && damagerLevel < (getLevel() - 5)) {
						continue;
					}
					uint32_t exp = static_cast<uint32_t>(m_info->exp * ((8 * damagerLevel / totalLevel) + (damager == partyIter->second.highestDamager ? 2 : 0)) / 10);
					int16_t hsRate = damager->getActiveBuffs()->getHolySymbolRate();
					exp = exp * getTauntEffect() / 100;
					exp *= ChannelServer::Instance()->getExpRate();
					exp += ((exp * hsRate) / 100);
					damager->getStats()->giveExp(exp, false, (damager == killer));
				}
			}
		}
	}
	return highestDamager;
}

void Mob::spawnDeathMobs(Map *map) {
	if (Mob::spawnsSponge(getMobId())) {
		// Special logic to keep units linked
		int32_t spongeId = 0;
		vector<int32_t> parts;
		for (size_t i = 0; i < m_info->summon.size(); i++) {
			int32_t spawnId = m_info->summon[i];
			if (isSponge(spawnId)) {
				spongeId = map->spawnMob(spawnId, m_pos, getFh(), this);
			}
			else {
				int32_t identifier = map->spawnMob(spawnId, m_pos, getFh(), this);
				parts.push_back(identifier);
			}
		}
		Mob *sponge = Maps::getMap(m_mapId)->getMob(spongeId);
		for (size_t m = 0; m < parts.size(); m++) {
			Mob *f = map->getMob(parts[m]);
			f->setSponge(sponge);
			sponge->addSpawn(parts[m], f);
		}
	}
	else if (Mob *sponge = getSponge()) {
		// More special logic to keep units linked
		sponge->removeSpawn(getId());
		for (size_t i = 0; i < m_info->summon.size(); i++) {
			int32_t ident = map->spawnMob(m_info->summon[i], m_pos, getFh(), this);
			Mob *mob = map->getMob(ident);
			sponge->addSpawn(ident, mob);
		}
	}
	else {
		for (size_t i = 0; i < m_info->summon.size(); i++) {
			map->spawnMob(m_info->summon[i], m_pos, getFh(), this);
		}
	}
}

void Mob::updateSpawnLinks() {
	if (m_spawns.size() > 0) {
		for (unordered_map<int32_t, Mob *>::iterator spawnIter = m_spawns.begin(); spawnIter != m_spawns.end(); ++spawnIter) {
			spawnIter->second->setOwner(nullptr);
		}
	}
	if (getOwner() != nullptr) {
		m_owner->removeSpawn(getId());
	}
}

void Mob::skillHeal(int32_t healHp, int32_t healRange) {
	if (isSponge()) {
		return;
	}
	int32_t min = (healHp - (healRange / 2));
	int32_t max = (healHp + (healRange / 2));
	int32_t amount = Randomizer::Instance()->randInt(max, min);
	int32_t original = amount;

	if (m_hp + amount > getMaxHp()) {
		amount = getMaxHp() - m_hp;
		m_hp = getMaxHp();
	}
	else {
		m_hp += amount;
	}
	m_totalHealth += amount;

	if (Mob *sponge = getSponge()) {
		healHp = sponge->getHp() + amount;
		healHp = MiscUtilities::constrainToRange<int32_t>(healHp, Stats::MinHp, sponge->getMaxHp());
		getSponge()->setHp(healHp);
	}

	MobsPacket::healMob(this, original);
}

void Mob::dispelBuffs() {
	removeStatus(StatusEffects::Mob::Watk);
	removeStatus(StatusEffects::Mob::Wdef);
	removeStatus(StatusEffects::Mob::Matk);
	removeStatus(StatusEffects::Mob::Mdef);
	removeStatus(StatusEffects::Mob::Acc);
	removeStatus(StatusEffects::Mob::Avoid);
	removeStatus(StatusEffects::Mob::Speed);
}

void Mob::doCrashSkill(int32_t skillId) {
	switch (skillId) {
		case Jobs::Crusader::ArmorCrash: removeStatus(StatusEffects::Mob::Wdef); break;
		case Jobs::WhiteKnight::MagicCrash: removeStatus(StatusEffects::Mob::Matk); break;
		case Jobs::DragonKnight::PowerCrash: removeStatus(StatusEffects::Mob::Watk); break;
	}
}

void Mob::mpEat(Player *player, MpEaterInfo *mp) {
	if ((m_mpEaterCount < 3) && (getMp() > 0) && (Randomizer::Instance()->randInt(99) < mp->prop)) {
		mp->used = true;
		int32_t eatenMp = getMaxMp() * mp->x / 100;

		eatenMp = std::min<int32_t>(eatenMp, getMp());
		setMp(getMp() - eatenMp);

		eatenMp = std::min<int32_t>(eatenMp, Stats::MaxMaxMp);
		player->getStats()->modifyMp(eatenMp);

		SkillsPacket::showSkillEffect(player, mp->skillId);
		m_mpEaterCount++;
	}
}

void Mob::setControlStatus(int8_t newStat) {
	MobsPacket::endControlMob(nullptr, this);
	MobsPacket::spawnMob(nullptr, this, 0, nullptr);
	m_controlStatus = newStat;
	Maps::getMap(getMapId())->updateMobControl(this);
}

bool Mob::canCastSkills() const {
	return !(hasStatus(StatusEffects::Mob::Freeze) || hasStatus(StatusEffects::Mob::Stun) || hasStatus(StatusEffects::Mob::ShadowWeb) || hasStatus(StatusEffects::Mob::Seal));
}

bool Mob::isSponge(int32_t mobId) {
	switch (mobId) {
		case Mobs::HorntailSponge: return true; break;
	}
	return false;
}

bool Mob::spawnsSponge(int32_t mobId) {
	switch (mobId) {
		case Mobs::SummonHorntail: return true; break;
	}
	return false;
}