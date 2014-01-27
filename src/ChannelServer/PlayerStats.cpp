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
#include "PlayerStats.hpp"
#include "ChannelServer.hpp"
#include "EquipDataProvider.hpp"
#include "GameConstants.hpp"
#include "GameLogicUtilities.hpp"
#include "Instance.hpp"
#include "Inventory.hpp"
#include "InventoryPacket.hpp"
#include "LevelsPacket.hpp"
#include "Maps.hpp"
#include "MiscUtilities.hpp"
#include "PacketCreator.hpp"
#include "PacketReader.hpp"
#include "Party.hpp"
#include "Player.hpp"
#include "PlayerPacket.hpp"
#include "PlayersPacket.hpp"
#include "Randomizer.hpp"
#include "SkillDataProvider.hpp"
#include "SummonHandler.hpp"
#include "SyncPacket.hpp"
#include <iostream>
#include <limits>
#include <string>

PlayerStats::PlayerStats(Player *player, uint8_t level, int16_t job, int16_t fame, int16_t str, int16_t dex, int16_t intt, int16_t luk, int16_t ap, uint16_t hpMpAp, int16_t sp, int16_t hp, int16_t maxHp, int16_t mp, int16_t maxMp, int32_t exp) :
	m_player(player),
	m_level(level),
	m_job(job),
	m_fame(fame),
	m_str(str),
	m_dex(dex),
	m_int(intt),
	m_luk(luk),
	m_ap(ap),
	m_hpMpAp(hpMpAp),
	m_sp(sp),
	m_hp(hp),
	m_maxHp(maxHp),
	m_mp(mp),
	m_maxMp(maxMp),
	m_exp(exp)
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
			if (EquipDataProvider::getInstance().canEquip(info.id, getJob(), getStr(true), getDex(true), getInt(true), getLuk(true), getFame())) {
				m_equipBonuses.hp += info.hp;
				m_equipBonuses.mp += info.mp;
				m_equipBonuses.str += info.str;
				m_equipBonuses.dex += info.dex;
				m_equipBonuses.intt += info.intt;
				m_equipBonuses.luk += info.luk;
			}
		}
	}

	if (m_hyperBodyX > 0 && m_hyperBodyY > 0) {
		setHyperBody(m_hyperBodyX, m_hyperBodyY);
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

auto PlayerStats::setEquip(int16_t slot, Item *equip, bool isLoading) -> void {
	slot = abs(slot);
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
auto PlayerStats::connectData(PacketCreator &packet) -> void {
	packet.add<int8_t>(getLevel());
	packet.add<int16_t>(getJob());
	packet.add<int16_t>(getStr());
	packet.add<int16_t>(getDex());
	packet.add<int16_t>(getInt());
	packet.add<int16_t>(getLuk());
	packet.add<int16_t>(getHp());
	packet.add<int16_t>(getMaxHp(true));
	packet.add<int16_t>(getMp());
	packet.add<int16_t>(getMaxMp(true));
	packet.add<int16_t>(getAp());
	packet.add<int16_t>(getSp());
	packet.add<int32_t>(getExp());
	packet.add<int16_t>(getFame());
}

auto PlayerStats::getMaxHp(bool withoutBonus) -> int16_t {
	if (!withoutBonus) {
		return static_cast<int16_t>(std::min<int32_t>(m_maxHp + m_equipBonuses.hp + m_buffBonuses.hp, Stats::MaxMaxHp));
	}
	return m_maxHp;
}

auto PlayerStats::getMaxMp(bool withoutBonus) -> int16_t {
	if (!withoutBonus) {
		return static_cast<int16_t>(std::min<int32_t>(m_maxMp + m_equipBonuses.mp + m_buffBonuses.mp, Stats::MaxMaxMp));
	}
	return m_maxMp;
}

auto PlayerStats::statUtility(int32_t test) -> int16_t {
	return static_cast<int16_t>(std::min<int32_t>(std::numeric_limits<int16_t>::max(), test));
}

auto PlayerStats::getStr(bool withBonus) -> int16_t {
	if (withBonus) {
		return statUtility(m_str + m_buffBonuses.str + m_equipBonuses.str);
	}
	return m_str;
}

auto PlayerStats::getDex(bool withBonus) -> int16_t {
	if (withBonus) {
		return statUtility(m_dex + m_buffBonuses.dex + m_equipBonuses.dex);
	}
	return m_dex;
}

auto PlayerStats::getInt(bool withBonus) -> int16_t {
	if (withBonus) {
		return statUtility(m_int + m_buffBonuses.intt + m_equipBonuses.intt);
	}
	return m_int;
}

auto PlayerStats::getLuk(bool withBonus) -> int16_t {
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

auto PlayerStats::setLevel(uint8_t level) -> void {
	m_level = level;
	PlayerPacket::updateStat(m_player, Stats::Level, level);
	LevelsPacket::levelUp(m_player);
	SyncPacket::PlayerPacket::updateLevel(m_player->getId(), level);
}

auto PlayerStats::setHp(int16_t hp, bool sendPacket) -> void {
	m_hp = MiscUtilities::constrainToRange<int16_t>(hp, Stats::MinHp, getMaxHp());
	if (sendPacket) {
		PlayerPacket::updateStat(m_player, Stats::Hp, m_hp);
	}
	modifiedHp();
}

auto PlayerStats::modifyHp(int32_t hpMod, bool sendPacket) -> void {
	int32_t tempHp = m_hp + hpMod;
	tempHp = MiscUtilities::constrainToRange<int32_t>(tempHp, Stats::MinHp, getMaxHp());
	m_hp = static_cast<int16_t>(tempHp);

	if (sendPacket) {
		PlayerPacket::updateStat(m_player, Stats::Hp, m_hp);
	}
	modifiedHp();
}

auto PlayerStats::damageHp(int32_t damageHp) -> void {
	m_hp = std::max<int32_t>(Stats::MinHp, static_cast<int32_t>(m_hp) - damageHp);
	PlayerPacket::updateStat(m_player, Stats::Hp, m_hp);
	modifiedHp();
}

auto PlayerStats::modifiedHp() -> void {
	if (Party *p = m_player->getParty()) {
		p->showHpBar(m_player);
	}
	m_player->getActiveBuffs()->checkBerserk();
	if (m_hp == Stats::MinHp) {
		if (Instance *i = m_player->getInstance()) {
			i->sendMessage(PlayerDeath, m_player->getId());
		}
		loseExp();
		SummonHandler::removeSummon(m_player, false, false, SummonMessages::Disappearing);
	}
}

auto PlayerStats::setMp(int16_t mp, bool sendPacket) -> void {
	if (!m_player->getActiveBuffs()->hasInfinity()) {
		m_mp = MiscUtilities::constrainToRange<int16_t>(mp, Stats::MinMp, getMaxMp());
	}
	PlayerPacket::updateStat(m_player, Stats::Mp, m_mp, sendPacket);
}

auto PlayerStats::modifyMp(int32_t mpMod, bool sendPacket) -> void {
	if (!m_player->getActiveBuffs()->hasInfinity()) {
		int32_t tempMp = m_mp + mpMod;
		tempMp = MiscUtilities::constrainToRange<int32_t>(tempMp, Stats::MinMp, getMaxMp());
		m_mp = static_cast<int16_t>(tempMp);
	}
	PlayerPacket::updateStat(m_player, Stats::Mp, m_mp, sendPacket);
}

auto PlayerStats::damageMp(int32_t damageMp) -> void {
	if (!m_player->getActiveBuffs()->hasInfinity()) {
		m_mp = std::max<int32_t>(Stats::MinMp, static_cast<int32_t>(m_mp) - damageMp);
	}
	PlayerPacket::updateStat(m_player, Stats::Mp, m_mp, false);
}

auto PlayerStats::setSp(int16_t sp) -> void {
	m_sp = sp;
	PlayerPacket::updateStat(m_player, Stats::Sp, sp);
}

auto PlayerStats::setAp(int16_t ap) -> void {
	m_ap = ap;
	PlayerPacket::updateStat(m_player, Stats::Ap, ap);
}

auto PlayerStats::setJob(int16_t job) -> void {
	m_job = job;
	PlayerPacket::updateStat(m_player, Stats::Job, job);
	LevelsPacket::jobChange(m_player);
	SyncPacket::PlayerPacket::updateJob(m_player->getId(), job);
}

auto PlayerStats::setStr(int16_t str) -> void {
	m_str = str;
	PlayerPacket::updateStat(m_player, Stats::Str, str);
}

auto PlayerStats::setDex(int16_t dex) -> void {
	m_dex = dex;
	PlayerPacket::updateStat(m_player, Stats::Dex, dex);
}

auto PlayerStats::setInt(int16_t intt) -> void {
	m_int = intt;
	PlayerPacket::updateStat(m_player, Stats::Int, intt);
}

auto PlayerStats::setLuk(int16_t luk) -> void {
	m_luk = luk;
	PlayerPacket::updateStat(m_player, Stats::Luk, luk);
}

auto PlayerStats::setMapleWarrior(int16_t xMod) -> void {
	m_buffBonuses.str = (m_str * xMod) / 100;
	m_buffBonuses.dex = (m_dex * xMod) / 100;
	m_buffBonuses.intt = (m_int * xMod) / 100;
	m_buffBonuses.luk = (m_luk * xMod) / 100;
	if (m_mapleWarrior != xMod) {
		m_mapleWarrior = xMod;
		updateBonuses();
	}
}

auto PlayerStats::setMaxHp(int16_t maxHp) -> void {
	m_maxHp = MiscUtilities::constrainToRange(maxHp, Stats::MinMaxHp, Stats::MaxMaxHp);
	PlayerPacket::updateStat(m_player, Stats::MaxHp, m_maxHp);
	modifiedHp();
}

auto PlayerStats::setMaxMp(int16_t maxMp) -> void {
	m_maxMp = MiscUtilities::constrainToRange(maxMp, Stats::MinMaxMp, Stats::MaxMaxMp);
	PlayerPacket::updateStat(m_player, Stats::MaxMp, m_maxMp);
}

auto PlayerStats::setHyperBody(int16_t xMod, int16_t yMod) -> void {
	m_hyperBodyX = xMod;
	m_hyperBodyY = yMod;
	m_buffBonuses.hp = std::min<uint16_t>((m_maxHp + m_equipBonuses.hp) * xMod / 100, Stats::MaxMaxHp);
	m_buffBonuses.mp = std::min<uint16_t>((m_maxMp + m_equipBonuses.mp) * yMod / 100, Stats::MaxMaxMp);
	PlayerPacket::updateStat(m_player, Stats::MaxHp, m_maxHp);
	PlayerPacket::updateStat(m_player, Stats::MaxMp, m_maxMp);
	if (Party *p = m_player->getParty()) {
		p->showHpBar(m_player);
	}
	m_player->getActiveBuffs()->checkBerserk();
}

auto PlayerStats::modifyMaxHp(int16_t mod) -> void {
	m_maxHp = std::min<int16_t>(m_maxHp + mod, Stats::MaxMaxHp);
	PlayerPacket::updateStat(m_player, Stats::MaxHp, m_maxHp);
}

auto PlayerStats::modifyMaxMp(int16_t mod) -> void {
	m_maxMp = std::min<int16_t>(m_maxMp + mod, Stats::MaxMaxMp);
	PlayerPacket::updateStat(m_player, Stats::MaxMp, m_maxMp);
}

auto PlayerStats::setExp(int32_t exp) -> void {
	m_exp = std::max(exp, 0);
	PlayerPacket::updateStat(m_player, Stats::Exp, m_exp);
}

auto PlayerStats::setFame(int16_t fame) -> void {
	m_fame = MiscUtilities::constrainToRange(fame, Stats::MinFame, Stats::MaxFame);
	PlayerPacket::updateStat(m_player, Stats::Fame, fame);
}

auto PlayerStats::loseExp() -> void {
	if (!GameLogicUtilities::isBeginnerJob(getJob()) && getLevel() < GameLogicUtilities::getMaxLevel(getJob()) && m_player->getMapId() != Maps::SorcerersRoom) {
		uint16_t charms = m_player->getInventory()->getItemAmount(Items::SafetyCharm);
		if (charms > 0) {
			Inventory::takeItem(m_player, Items::SafetyCharm, 1);
			charms = std::min<uint16_t>(--charms, 0xFF);
			InventoryPacket::useCharm(m_player, static_cast<uint8_t>(charms));
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
		int32_t exp = getExp();
		exp -= static_cast<int32_t>(static_cast<int64_t>(getExp(getLevel())) * expLoss / 100);
		setExp(exp);
	}
}

// Level related functions
auto PlayerStats::giveExp(uint64_t exp, bool inChat, bool white) -> void {
	int16_t fullJob = getJob();
	uint8_t level = getLevel();
	uint8_t jobMax = GameLogicUtilities::getMaxLevel(fullJob);
	if (level >= jobMax) {
		// Do not give EXP to characters of max level or over
		return;
	}

	uint64_t curExp = getExp() + exp;
	if (exp > 0) {
		uint64_t expCounter = exp;
		uint64_t batchSize = std::numeric_limits<int32_t>::max();
		while (expCounter > 0) {

			int32_t allocate = static_cast<int32_t>(std::min(expCounter, batchSize));
			LevelsPacket::showExp(m_player, allocate, white, inChat);
			expCounter -= allocate;
		}
	}

	if (curExp >= getExp(level)) {
		bool cygnus = GameLogicUtilities::isCygnus(fullJob);
		uint8_t levelsGained = 0;
		uint8_t levelsMax = ChannelServer::getInstance().getMaxMultiLevel();
		int16_t apGain = 0;
		int16_t spGain = 0;
		int16_t hpGain = 0;
		int16_t mpGain = 0;
		int16_t jobLine = GameLogicUtilities::getJobLine(fullJob);
		int16_t intt = getInt(true) / 10;
		int16_t x = 0; // X value for Improving *P Increase skills, cached, only needs to be set once

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
			// If the desired number of level ups have passed and they're still above, set it to where it should be
			curExp = getExp(level) - 1;
		}

		if (levelsGained) {
			// Check if the m_player has leveled up at all, it is possible that the user hasn't leveled up if multi-level limit is 0
			modifyMaxHp(hpGain);
			modifyMaxMp(mpGain);
			setLevel(level);
			setAp(getAp() + apGain);
			setSp(getSp() + spGain);
			// Let Hyper Body remain on if on during a level up, as it should
			if (m_player->getActiveBuffs()->hasHyperBody()) {
				int32_t skillId = m_player->getActiveBuffs()->getHyperBody();
				uint8_t hbLevel = m_player->getActiveBuffs()->getActiveSkillLevel(skillId);
				SkillLevelInfo *hb = SkillDataProvider::getInstance().getSkill(skillId, hbLevel);
				setHyperBody(hb->x, hb->y);
			}

			setHp(getMaxHp());
			setMp(getMaxMp());
			m_player->setLevelDate();
			if (getLevel() == jobMax && !m_player->isGm()) {
				out_stream_t message;
				message << "[Congrats] " << m_player->getName() << " has reached Level "
						<< static_cast<int16_t>(jobMax) << "! Congratulate "
						<< m_player->getName() << " on such an amazing achievement!";

				PlayerPacket::showMessageWorld(message.str(), PlayerPacket::NoticeTypes::Blue);
			}
		}
	}

	// By this point, the EXP should be a valid EXP in the range of 0 to int32_t max
	setExp(static_cast<int32_t>(curExp));
}

auto PlayerStats::addStat(PacketReader &packet) -> void {
	uint32_t ticks = packet.get<uint32_t>();
	int32_t type = packet.get<int32_t>();
	if (getAp() == 0) {
		// Hacking
		return;
	}
	LevelsPacket::statOk(m_player);
	addStat(type);
}

auto PlayerStats::addStatMulti(PacketReader &packet) -> void {
	uint32_t ticks = packet.get<uint32_t>();
	uint32_t amount = packet.get<uint32_t>();

	LevelsPacket::statOk(m_player);

	for (uint32_t i = 0; i < amount; i++) {
		int32_t type = packet.get<int32_t>();
		int32_t value = packet.get<int32_t>();

		if (value < 0 || getAp() < value) {
			// Hacking
			return;
		}

		addStat(type, static_cast<int16_t>(value)); // Prefer a single cast to countless casts/modification down the line
	}
}

auto PlayerStats::addStat(int32_t type, int16_t mod, bool isReset) -> void {
	int16_t maxStat = ChannelServer::getInstance().getMaxStats();
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
			int16_t job = GameLogicUtilities::getJobTrack(getJob());
			int16_t hpGain = 0;
			int16_t mpGain = 0;
			int16_t y = 0;
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
			if (m_player->getActiveBuffs()->hasHyperBody()) {
				int32_t skillId = m_player->getActiveBuffs()->getHyperBody();
				uint8_t hbLevel = m_player->getActiveBuffs()->getActiveSkillLevel(skillId);
				SkillLevelInfo *hb = SkillDataProvider::getInstance().getSkill(skillId, hbLevel);
				setHyperBody(hb->x, hb->y);
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

auto PlayerStats::randHp() -> int16_t {
	return Randomizer::rand<int16_t>(Stats::BaseHp::Variation); // Max HP range per class (e.g. Beginner is 8-12)
}

auto PlayerStats::randMp() -> int16_t {
	return Randomizer::rand<int16_t>(Stats::BaseMp::Variation); // Max MP range per class (e.g. Beginner is 6-8)
}

auto PlayerStats::getX(int32_t skillId) -> int16_t {
	return m_player->getSkills()->getSkillInfo(skillId)->x;
}

auto PlayerStats::getY(int32_t skillId) -> int16_t {
	return m_player->getSkills()->getSkillInfo(skillId)->y;
}

auto PlayerStats::apResetHp(bool isReset, bool isSubtract, int16_t val, int16_t sVal) -> int16_t {
	return (isReset ? (isSubtract ? -(sVal + val + Stats::BaseHp::Variation) : val) : levelHp(val, sVal));
}

auto PlayerStats::apResetMp(bool isReset, bool isSubtract, int16_t val, int16_t sVal) -> int16_t {
	return (isReset ? (isSubtract ? -(sVal + val + Stats::BaseMp::Variation) : val) : levelMp(val, sVal));
}

auto PlayerStats::levelHp(int16_t val, int16_t bonus) -> int16_t {
	return randHp() + val + bonus;
}

auto PlayerStats::levelMp(int16_t val, int16_t bonus) -> int16_t {
	return randMp() + val + bonus;
}

auto PlayerStats::getExp(uint8_t level) -> uint32_t {
	return Stats::PlayerExp[level - 1];
}