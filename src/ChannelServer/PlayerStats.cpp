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
#include "PlayerStats.hpp"
#include "Algorithm.hpp"
#include "ChannelServer.hpp"
#include "EquipDataProvider.hpp"
#include "GameConstants.hpp"
#include "GameLogicUtilities.hpp"
#include "Instance.hpp"
#include "InterHeader.hpp"
#include "Inventory.hpp"
#include "InventoryPacket.hpp"
#include "LevelsPacket.hpp"
#include "Maps.hpp"
#include "MiscUtilities.hpp"
#include "PacketReader.hpp"
#include "PacketWrapper.hpp"
#include "Party.hpp"
#include "Player.hpp"
#include "PlayerDataProvider.hpp"
#include "PlayerPacket.hpp"
#include "PlayersPacket.hpp"
#include "Randomizer.hpp"
#include "SkillDataProvider.hpp"
#include "SummonHandler.hpp"
#include <iostream>
#include <limits>
#include <string>

PlayerStats::PlayerStats(Player *player, player_level_t level, job_id_t job, fame_t fame, stat_t str, stat_t dex, stat_t intt, stat_t luk, stat_t ap, health_ap_t hpMpAp, stat_t sp, health_t hp, health_t maxHp, health_t mp, health_t maxMp, experience_t exp) :
	m_player{player},
	m_level{level},
	m_job{job},
	m_fame{fame},
	m_str{str},
	m_dex{dex},
	m_int{intt},
	m_luk{luk},
	m_ap{ap},
	m_hpMpAp{hpMpAp},
	m_sp{sp},
	m_hp{hp},
	m_maxHp{maxHp},
	m_mp{mp},
	m_maxMp{maxMp},
	m_exp{exp}
{
	if (isDead()) {
		m_hp = Stats::DefaultHp;
	}
}

auto PlayerStats::isDead() const -> bool {
	return m_hp == Stats::MinHp;
}

// Equip stat bonus handling
auto PlayerStats::updateBonuses(bool updateEquips, bool isLoading) -> void {
	if (m_mapleWarrior > 0) {
		setMapleWarrior(m_mapleWarrior);
	}
	if (updateEquips) {
		m_equipBonuses = BonusSet();
		for (const auto &kvp : m_equipStats) {
			const EquipBonus &info = kvp.second;
			if (ChannelServer::getInstance().getEquipDataProvider().canEquip(info.id, m_player->getGender(), getJob(), getStr(true), getDex(true), getInt(true), getLuk(true), getFame())) {
				m_equipBonuses.hp += info.hp;
				m_equipBonuses.mp += info.mp;
				m_equipBonuses.str += info.str;
				m_equipBonuses.dex += info.dex;
				m_equipBonuses.intt += info.intt;
				m_equipBonuses.luk += info.luk;
			}
		}
	}

	if (m_hyperBodyX > 0) {
		setHyperBodyHp(m_hyperBodyX);
	}
	if (m_hyperBodyY > 0) {
		setHyperBodyMp(m_hyperBodyY);
	}

	if (!isLoading) {
		// Adjust current HP/MP down if necessary
		if (getHp() > getMaxHp()) {
			setHp(getHp());
		}
		if (getMp() > getMaxMp()) {
			setMp(getMp());
		}
	}
}

auto PlayerStats::setEquip(inventory_slot_t slot, Item *equip, bool isLoading) -> void {
	slot = std::abs(slot);
	if (equip != nullptr) {
		m_equipStats[slot].id = equip->getId();
		m_equipStats[slot].hp = equip->getHp();
		m_equipStats[slot].mp = equip->getMp();
		m_equipStats[slot].str = equip->getStr();
		m_equipStats[slot].dex = equip->getDex();
		m_equipStats[slot].intt = equip->getInt();
		m_equipStats[slot].luk = equip->getLuk();
	}
	else {
		m_equipStats.erase(slot);
	}

	updateBonuses(true, isLoading);
}

// Data acquisition
auto PlayerStats::connectData(PacketBuilder &packet) -> void {
	packet.add<player_level_t>(getLevel());
	packet.add<job_id_t>(getJob());
	packet.add<stat_t>(getStr());
	packet.add<stat_t>(getDex());
	packet.add<stat_t>(getInt());
	packet.add<stat_t>(getLuk());
	packet.add<health_t>(getHp());
	packet.add<health_t>(getMaxHp(true));
	packet.add<health_t>(getMp());
	packet.add<health_t>(getMaxMp(true));
	packet.add<stat_t>(getAp());
	packet.add<stat_t>(getSp());
	packet.add<experience_t>(getExp());
	packet.add<fame_t>(getFame());
}

auto PlayerStats::getMaxHp(bool withoutBonus) -> health_t {
	if (!withoutBonus) {
		return static_cast<health_t>(std::min<int32_t>(m_maxHp + m_equipBonuses.hp + m_buffBonuses.hp, Stats::MaxMaxHp));
	}
	return m_maxHp;
}

auto PlayerStats::getMaxMp(bool withoutBonus) -> health_t {
	if (!withoutBonus) {
		return static_cast<health_t>(std::min<int32_t>(m_maxMp + m_equipBonuses.mp + m_buffBonuses.mp, Stats::MaxMaxMp));
	}
	return m_maxMp;
}

auto PlayerStats::statUtility(int32_t test) -> int16_t {
	return static_cast<int16_t>(std::min<int32_t>(std::numeric_limits<int16_t>::max(), test));
}

auto PlayerStats::getStr(bool withBonus) -> stat_t {
	if (withBonus) {
		return statUtility(m_str + m_buffBonuses.str + m_equipBonuses.str);
	}
	return m_str;
}

auto PlayerStats::getDex(bool withBonus) -> stat_t {
	if (withBonus) {
		return statUtility(m_dex + m_buffBonuses.dex + m_equipBonuses.dex);
	}
	return m_dex;
}

auto PlayerStats::getInt(bool withBonus) -> stat_t {
	if (withBonus) {
		return statUtility(m_int + m_buffBonuses.intt + m_equipBonuses.intt);
	}
	return m_int;
}

auto PlayerStats::getLuk(bool withBonus) -> stat_t {
	if (withBonus) {
		return statUtility(m_luk + m_buffBonuses.luk + m_equipBonuses.luk);
	}
	return m_luk;
}

// Data modification
auto PlayerStats::checkHpMp() -> void {
	if (m_hp > getMaxHp()) {
		m_hp = getMaxHp();
	}
	if (m_mp > getMaxMp()) {
		m_mp = getMaxMp();
	}
}

auto PlayerStats::setLevel(player_level_t level) -> void {
	m_level = level;
	m_player->send(PlayerPacket::updateStat(Stats::Level, level));
	m_player->sendMap(LevelsPacket::levelUp(m_player->getId()));
	ChannelServer::getInstance().getPlayerDataProvider().updatePlayerLevel(m_player);
}

auto PlayerStats::setHp(health_t hp, bool sendPacket) -> void {
	m_hp = ext::constrain_range<health_t>(hp, Stats::MinHp, getMaxHp());
	if (sendPacket) {
		m_player->send(PlayerPacket::updateStat(Stats::Hp, m_hp));
	}
	modifiedHp();
}

auto PlayerStats::modifyHp(int32_t hpMod, bool sendPacket) -> void {
	int32_t tempHp = m_hp + hpMod;
	tempHp = ext::constrain_range<int32_t>(tempHp, Stats::MinHp, getMaxHp());
	m_hp = static_cast<health_t>(tempHp);

	if (sendPacket) {
		m_player->send(PlayerPacket::updateStat(Stats::Hp, m_hp));
	}
	modifiedHp();
}

auto PlayerStats::damageHp(int32_t damageHp) -> void {
	m_hp = std::max<int32_t>(Stats::MinHp, static_cast<int32_t>(m_hp) - damageHp);
	m_player->send(PlayerPacket::updateStat(Stats::Hp, m_hp));
	modifiedHp();
}

auto PlayerStats::modifiedHp() -> void {
	if (Party *p = m_player->getParty()) {
		p->showHpBar(m_player);
	}
	m_player->getActiveBuffs()->checkBerserk();
	if (m_hp == Stats::MinHp) {
		if (Instance *instance = m_player->getInstance()) {
			instance->playerDeath(m_player->getId());
		}
		loseExp();
		m_player->getSummons()->forEach([this](Summon *summon) {
			SummonHandler::removeSummon(m_player, summon->getId(), false, SummonMessages::Disappearing);
		});
	}
}

auto PlayerStats::setMp(health_t mp, bool sendPacket) -> void {
	if (!m_player->getActiveBuffs()->hasInfinity()) {
		m_mp = ext::constrain_range<health_t>(mp, Stats::MinMp, getMaxMp());
	}
	m_player->send(PlayerPacket::updateStat(Stats::Mp, m_mp, sendPacket));
}

auto PlayerStats::modifyMp(int32_t mpMod, bool sendPacket) -> void {
	if (!m_player->getActiveBuffs()->hasInfinity()) {
		int32_t tempMp = m_mp + mpMod;
		tempMp = ext::constrain_range<int32_t>(tempMp, Stats::MinMp, getMaxMp());
		m_mp = static_cast<health_t>(tempMp);
	}
	m_player->send(PlayerPacket::updateStat(Stats::Mp, m_mp, sendPacket));
}

auto PlayerStats::damageMp(int32_t damageMp) -> void {
	if (!m_player->getActiveBuffs()->hasInfinity()) {
		m_mp = std::max<int32_t>(Stats::MinMp, static_cast<int32_t>(m_mp) - damageMp);
	}
	m_player->send(PlayerPacket::updateStat(Stats::Mp, m_mp, false));
}

auto PlayerStats::setSp(stat_t sp) -> void {
	m_sp = sp;
	m_player->send(PlayerPacket::updateStat(Stats::Sp, sp));
}

auto PlayerStats::setAp(stat_t ap) -> void {
	m_ap = ap;
	m_player->send(PlayerPacket::updateStat(Stats::Ap, ap));
}

auto PlayerStats::setJob(job_id_t job) -> void {
	m_job = job;
	m_player->send(PlayerPacket::updateStat(Stats::Job, job));
	m_player->sendMap(LevelsPacket::jobChange(m_player->getId()));
	ChannelServer::getInstance().getPlayerDataProvider().updatePlayerJob(m_player);
}

auto PlayerStats::setStr(stat_t str) -> void {
	m_str = str;
	m_player->send(PlayerPacket::updateStat(Stats::Str, str));
}

auto PlayerStats::setDex(stat_t dex) -> void {
	m_dex = dex;
	m_player->send(PlayerPacket::updateStat(Stats::Dex, dex));
}

auto PlayerStats::setInt(stat_t intt) -> void {
	m_int = intt;
	m_player->send(PlayerPacket::updateStat(Stats::Int, intt));
}

auto PlayerStats::setLuk(stat_t luk) -> void {
	m_luk = luk;
	m_player->send(PlayerPacket::updateStat(Stats::Luk, luk));
}

auto PlayerStats::setMapleWarrior(int16_t mod) -> void {
	m_buffBonuses.str = (m_str * mod) / 100;
	m_buffBonuses.dex = (m_dex * mod) / 100;
	m_buffBonuses.intt = (m_int * mod) / 100;
	m_buffBonuses.luk = (m_luk * mod) / 100;
	if (m_mapleWarrior != mod) {
		m_mapleWarrior = mod;
		updateBonuses();
	}
}

auto PlayerStats::setMaxHp(health_t maxHp) -> void {
	m_maxHp = ext::constrain_range(maxHp, Stats::MinMaxHp, Stats::MaxMaxHp);
	m_player->send(PlayerPacket::updateStat(Stats::MaxHp, m_maxHp));
	modifiedHp();
}

auto PlayerStats::setMaxMp(health_t maxMp) -> void {
	m_maxMp = ext::constrain_range(maxMp, Stats::MinMaxMp, Stats::MaxMaxMp);
	m_player->send(PlayerPacket::updateStat(Stats::MaxMp, m_maxMp));
}

auto PlayerStats::setHyperBodyHp(int16_t mod) -> void {
	m_hyperBodyX = mod;
	m_buffBonuses.hp = std::min<uint16_t>((m_maxHp + m_equipBonuses.hp) * mod / 100, Stats::MaxMaxHp);
	m_player->send(PlayerPacket::updateStat(Stats::MaxHp, m_maxHp));
	if (mod == 0) {
		setHp(getHp());
	}
	if (Party *p = m_player->getParty()) {
		p->showHpBar(m_player);
	}
	m_player->getActiveBuffs()->checkBerserk();
}

auto PlayerStats::setHyperBodyMp(int16_t mod) -> void {
	m_hyperBodyY = mod;
	m_buffBonuses.mp = std::min<uint16_t>((m_maxMp + m_equipBonuses.mp) * mod / 100, Stats::MaxMaxMp);
	m_player->send(PlayerPacket::updateStat(Stats::MaxMp, m_maxMp));
	if (mod == 0) {
		setMp(getMp());
	}
}

auto PlayerStats::modifyMaxHp(health_t mod) -> void {
	m_maxHp = std::min<health_t>(m_maxHp + mod, Stats::MaxMaxHp);
	m_player->send(PlayerPacket::updateStat(Stats::MaxHp, m_maxHp));
}

auto PlayerStats::modifyMaxMp(health_t mod) -> void {
	m_maxMp = std::min<health_t>(m_maxMp + mod, Stats::MaxMaxMp);
	m_player->send(PlayerPacket::updateStat(Stats::MaxMp, m_maxMp));
}

auto PlayerStats::setExp(experience_t exp) -> void {
	m_exp = std::max(exp, 0);
	m_player->send(PlayerPacket::updateStat(Stats::Exp, m_exp));
}

auto PlayerStats::setFame(fame_t fame) -> void {
	m_fame = ext::constrain_range(fame, Stats::MinFame, Stats::MaxFame);
	m_player->send(PlayerPacket::updateStat(Stats::Fame, fame));
}

auto PlayerStats::loseExp() -> void {
	if (!GameLogicUtilities::isBeginnerJob(getJob()) && getLevel() < GameLogicUtilities::getMaxLevel(getJob()) && m_player->getMapId() != Maps::SorcerersRoom) {
		slot_qty_t charms = m_player->getInventory()->getItemAmount(Items::SafetyCharm);
		if (charms > 0) {
			Inventory::takeItem(m_player, Items::SafetyCharm, 1);
			// TODO FIXME REVIEW
			charms = --charms;
			m_player->send(InventoryPacket::useCharm(static_cast<uint8_t>(charms)));
			return;
		}
		Map *loc = m_player->getMap();
		int8_t expLoss = 10;
		if (loc->loseOnePercent()) {
			expLoss = 1;
		}
		else {
			switch (GameLogicUtilities::getJobLine(getJob())) {
				case Jobs::JobLines::Magician:
					expLoss = 7;
					break;
				case Jobs::JobLines::Thief:
					expLoss = 5;
					break;
			}
		}
		experience_t exp = getExp();
		exp -= static_cast<experience_t>(static_cast<int64_t>(getExp(getLevel())) * expLoss / 100);
		setExp(exp);
	}
}

// Level related functions
auto PlayerStats::giveExp(uint64_t exp, bool inChat, bool white) -> void {
	job_id_t fullJob = getJob();
	player_level_t level = getLevel();
	player_level_t jobMax = GameLogicUtilities::getMaxLevel(fullJob);
	if (level >= jobMax) {
		// Do not give EXP to characters of max level or over
		return;
	}

	uint64_t curExp = getExp() + exp;
	if (exp > 0) {
		uint64_t expCounter = exp;
		uint64_t batchSize = std::numeric_limits<experience_t>::max();
		while (expCounter > 0) {
			experience_t allocate = static_cast<experience_t>(std::min(expCounter, batchSize));
			m_player->send(LevelsPacket::showExp(allocate, white, inChat));
			expCounter -= allocate;
		}
	}

	if (curExp >= getExp(level)) {
		bool cygnus = GameLogicUtilities::isCygnus(fullJob);
		player_level_t levelsGained = 0;
		player_level_t levelsMax = ChannelServer::getInstance().getConfig().maxMultiLevel;
		stat_t apGain = 0;
		stat_t spGain = 0;
		health_t hpGain = 0;
		health_t mpGain = 0;
		int8_t jobLine = GameLogicUtilities::getJobLine(fullJob);
		stat_t intt = getInt(true) / 10;
		health_t x = 0; // X value for Improving *P Increase skills, cached, only needs to be set once

		while (curExp >= getExp(level) && levelsGained < levelsMax) {
			curExp -= getExp(getLevel());
			level++;
			levelsGained++;
			if (cygnus && level <= Stats::CygnusApCutoff) {
				apGain += Stats::ApPerCygnusLevel;
			}
			else {
				apGain += Stats::ApPerLevel;
			}
			switch (jobLine) {
				case Jobs::JobLines::Beginner:
					hpGain += levelHp(Stats::BaseHp::Beginner);
					mpGain += levelMp(Stats::BaseMp::Beginner, intt);
					break;
				case Jobs::JobLines::Warrior:
					if (levelsGained == 1 && m_player->getSkills()->hasHpIncrease()) {
						x = getX(m_player->getSkills()->getHpIncrease());
					}
					hpGain += levelHp(Stats::BaseHp::Warrior, x);
					mpGain += levelMp(Stats::BaseMp::Warrior, intt);
					break;
				case Jobs::JobLines::Magician:
					if (levelsGained == 1 && m_player->getSkills()->hasMpIncrease()) {
						x = getX(m_player->getSkills()->getMpIncrease());
					}
					hpGain += levelHp(Stats::BaseHp::Magician);
					mpGain += levelMp(Stats::BaseMp::Magician, 2 * x + intt);
					break;
				case Jobs::JobLines::Bowman:
					hpGain += levelHp(Stats::BaseHp::Bowman);
					mpGain += levelMp(Stats::BaseMp::Bowman, intt);
					break;
				case Jobs::JobLines::Thief:
					hpGain += levelHp(Stats::BaseHp::Thief);
					mpGain += levelMp(Stats::BaseMp::Thief, intt);
					break;
				case Jobs::JobLines::Pirate:
					if (levelsGained == 1 && m_player->getSkills()->hasHpIncrease()) {
						x = getX(m_player->getSkills()->getHpIncrease());
					}
					hpGain += levelHp(Stats::BaseHp::Pirate, x);
					mpGain += levelMp(Stats::BaseMp::Pirate, intt);
					break;
				default: // GM
					hpGain += Stats::BaseHp::Gm;
					mpGain += Stats::BaseMp::Gm;
			}
			if (!GameLogicUtilities::isBeginnerJob(fullJob)) {
				spGain += Stats::SpPerLevel;
			}
			if (level >= jobMax) {
				// Do not let people level past the level cap
				curExp = 0;
				break;
			}
		}

		if (curExp >= getExp(level)) {
			// If the desired number of level ups have passed and they're still above, we chop
			curExp = getExp(level) - 1;
		}

		// Check if the m_player has leveled up at all, it is possible that the user hasn't leveled up if multi-level limit is 0
		if (levelsGained) {
			modifyMaxHp(hpGain);
			modifyMaxMp(mpGain);
			setLevel(level);
			setAp(getAp() + apGain);
			setSp(getSp() + spGain);

			// Let Hyper Body remain on during a level up, as it should
			auto activeBuffs = m_player->getActiveBuffs();
			auto hyperBodyHp = activeBuffs->getHyperBodyHpSource();
			auto hyperBodyMp = activeBuffs->getHyperBodyMpSource();
			if (hyperBodyHp.is_initialized()) {
				setHyperBodyHp(activeBuffs->getBuffSkillInfo(hyperBodyHp.get())->x);
			}
			if (hyperBodyMp.is_initialized()) {
				setHyperBodyMp(activeBuffs->getBuffSkillInfo(hyperBodyHp.get())->y);
			}

			setHp(getMaxHp());
			setMp(getMaxMp());
			m_player->setLevelDate();
			if (getLevel() == jobMax && !m_player->isGm()) {
				out_stream_t message;
				message << "[Congrats] " << m_player->getName() << " has reached Level "
						<< static_cast<int16_t>(jobMax) << "! Congratulate "
						<< m_player->getName() << " on such an amazing achievement!";

				ChannelServer::getInstance().sendWorld(
					Packets::prepend(PlayerPacket::showMessage(message.str(), PlayerPacket::NoticeTypes::Blue), [](PacketBuilder &builder) {
						builder.add<header_t>(IMSG_TO_ALL_CHANNELS);
						builder.add<header_t>(IMSG_TO_ALL_PLAYERS);
					}));
			}
		}
	}

	// By this point, the EXP should be a valid EXP in the range of 0 to experience_t max
	setExp(static_cast<experience_t>(curExp));
}

auto PlayerStats::addStat(PacketReader &reader) -> void {
	tick_count_t ticks = reader.get<uint32_t>();
	int32_t type = reader.get<int32_t>();
	if (getAp() == 0) {
		// Hacking
		return;
	}
	m_player->send(LevelsPacket::statOk());
	addStat(type);
}

auto PlayerStats::addStatMulti(PacketReader &reader) -> void {
	tick_count_t ticks = reader.get<uint32_t>();
	uint32_t amount = reader.get<uint32_t>();

	m_player->send(LevelsPacket::statOk());

	for (uint32_t i = 0; i < amount; i++) {
		int32_t type = reader.get<int32_t>();
		int32_t value = reader.get<int32_t>();

		if (value < 0 || getAp() < value) {
			// Hacking
			return;
		}

		addStat(type, static_cast<int16_t>(value)); // Prefer a single cast to countless casts/modification down the line
	}
}

auto PlayerStats::addStat(int32_t type, int16_t mod, bool isReset) -> void {
	stat_t maxStat = ChannelServer::getInstance().getConfig().maxStats;
	bool isSubtract = mod < 0;
	switch (type) {
		case Stats::Str:
			if (getStr() >= maxStat) {
				return;
			}
			setStr(getStr() + mod);
			break;
		case Stats::Dex:
			if (getDex() >= maxStat) {
				return;
			}
			setDex(getDex() + mod);
			break;
		case Stats::Int:
			if (getInt() >= maxStat) {
				return;
			}
			setInt(getInt() + mod);
			break;
		case Stats::Luk:
			if (getLuk() >= maxStat) {
				return;
			}
			setLuk(getLuk() + mod);
			break;
		case Stats::MaxHp:
		case Stats::MaxMp: {
			if (type == Stats::MaxHp && getMaxHp(true) >= Stats::MaxMaxHp) {
				return;
			}
			if (type == Stats::MaxMp && getMaxMp(true) >= Stats::MaxMaxMp) {
				return;
			}
			if (isSubtract && getHpMpAp() == 0) {
				// Hacking
				return;
			}
			int8_t job = GameLogicUtilities::getJobTrack(getJob());
			health_t hpGain = 0;
			health_t mpGain = 0;
			health_t y = 0;
			switch (job) {
				case Jobs::JobTracks::Beginner:
					hpGain = apResetHp(isReset, isSubtract, Stats::BaseHp::BeginnerAp);
					mpGain = apResetMp(isReset, isSubtract, Stats::BaseMp::BeginnerAp);
					break;
				case Jobs::JobTracks::Warrior:
					if (m_player->getSkills()->hasHpIncrease()) {
						y = getY(m_player->getSkills()->getHpIncrease());
					}
					hpGain = apResetHp(isReset, isSubtract, Stats::BaseHp::WarriorAp, y);
					mpGain = apResetMp(isReset, isSubtract, Stats::BaseMp::WarriorAp);
					break;
				case Jobs::JobTracks::Magician:
					if (m_player->getSkills()->hasMpIncrease()) {
						y = getY(m_player->getSkills()->getMpIncrease());
					}
					hpGain = apResetHp(isReset, isSubtract, Stats::BaseHp::MagicianAp);
					mpGain = apResetMp(isReset, isSubtract, Stats::BaseMp::MagicianAp, 2 * y);
					break;
				case Jobs::JobTracks::Bowman:
					hpGain = apResetHp(isReset, isSubtract, Stats::BaseHp::BowmanAp);
					mpGain = apResetMp(isReset, isSubtract, Stats::BaseMp::BowmanAp);
					break;
				case Jobs::JobTracks::Thief:
					hpGain = apResetHp(isReset, isSubtract, Stats::BaseHp::ThiefAp);
					mpGain = apResetMp(isReset, isSubtract, Stats::BaseMp::ThiefAp);
					break;
				case Jobs::JobTracks::Pirate:
					if (m_player->getSkills()->hasHpIncrease()) {
						y = getY(m_player->getSkills()->getHpIncrease());
					}
					hpGain = apResetHp(isReset, isSubtract, Stats::BaseHp::PirateAp, y);
					mpGain = apResetMp(isReset, isSubtract, Stats::BaseMp::PirateAp);
					break;
				default: // GM
					hpGain = apResetHp(isReset, isSubtract, Stats::BaseHp::GmAp);
					mpGain = apResetMp(isReset, isSubtract, Stats::BaseMp::GmAp);
					break;
			}
			setHpMpAp(getHpMpAp() + mod);
			switch (type) {
				case Stats::MaxHp: modifyMaxHp(hpGain); break;
				case Stats::MaxMp: modifyMaxMp(mpGain); break;
			}

			auto activeBuffs = m_player->getActiveBuffs();
			auto hyperBodyHp = activeBuffs->getHyperBodyHpSource();
			auto hyperBodyMp = activeBuffs->getHyperBodyMpSource();
			if (hyperBodyHp.is_initialized()) {
				setHyperBodyHp(activeBuffs->getBuffSkillInfo(hyperBodyHp.get())->x);
			}
			if (hyperBodyMp.is_initialized()) {
				setHyperBodyMp(activeBuffs->getBuffSkillInfo(hyperBodyHp.get())->y);
			}

			setHp(getHp());
			setMp(getMp());
			break;
		}
		default:
			// Hacking, one assumes
			break;
	}
	if (!isReset) {
		setAp(getAp() - mod);
	}
	updateBonuses();
}

auto PlayerStats::randHp() -> health_t {
	return Randomizer::rand<health_t>(Stats::BaseHp::Variation); // Max HP range per class (e.g. Beginner is 8-12)
}

auto PlayerStats::randMp() -> health_t {
	return Randomizer::rand<health_t>(Stats::BaseMp::Variation); // Max MP range per class (e.g. Beginner is 6-8)
}

auto PlayerStats::getX(skill_id_t skillId) -> int16_t {
	return m_player->getSkills()->getSkillInfo(skillId)->x;
}

auto PlayerStats::getY(skill_id_t skillId) -> int16_t {
	return m_player->getSkills()->getSkillInfo(skillId)->y;
}

auto PlayerStats::apResetHp(bool isReset, bool isSubtract, int16_t val, int16_t sVal) -> int16_t {
	return (isReset ? (isSubtract ? -(sVal + val + Stats::BaseHp::Variation) : val) : levelHp(val, sVal));
}

auto PlayerStats::apResetMp(bool isReset, bool isSubtract, int16_t val, int16_t sVal) -> int16_t {
	return (isReset ? (isSubtract ? -(sVal + val + Stats::BaseMp::Variation) : val) : levelMp(val, sVal));
}

auto PlayerStats::levelHp(health_t val, health_t bonus) -> health_t {
	return randHp() + val + bonus;
}

auto PlayerStats::levelMp(health_t val, health_t bonus) -> health_t {
	return randMp() + val + bonus;
}

auto PlayerStats::getExp(player_level_t level) -> experience_t {
	return Stats::PlayerExp[level - 1];
}