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
#include "PlayerStats.h"
#include "ChannelServer.h"
#include "EquipDataProvider.h"
#include "GameConstants.h"
#include "GameLogicUtilities.h"
#include "Instance.h"
#include "Inventory.h"
#include "InventoryPacket.h"
#include "LevelsPacket.h"
#include "Maps.h"
#include "MiscUtilities.h"
#include "PacketCreator.h"
#include "PacketReader.h"
#include "Party.h"
#include "Player.h"
#include "PlayerPacket.h"
#include "PlayersPacket.h"
#include "Randomizer.h"
#include "SkillDataProvider.h"
#include "SummonHandler.h"
#include "SyncPacket.h"
#include <iostream>
#include <limits>
#include <string>

using std::string;

PlayerStats::PlayerStats(Player *player, uint8_t level, int16_t job, int8_t jobType, int32_t fame, int16_t str, int16_t dex, int16_t intt, int16_t luk, int16_t ap, uint16_t hpMpAp, int32_t hp, int32_t maxHp, int32_t mp, int32_t maxMp, int32_t exp) :
	m_player(player),
	m_level(level),
	m_job(job),
	m_jobType(jobType),
	m_fame(fame),
	m_str(str),
	m_dex(dex),
	m_int(intt),
	m_luk(luk),
	m_ap(ap),
	m_hpMpAp(hpMpAp),
	m_hp(hp),
	m_maxHp(maxHp),
	m_mp(mp),
	m_maxMp(maxMp),
	m_exp(exp),
	m_hyperBodyX(0),
	m_hyperBodyY(0),
	m_mapleWarrior(0)
{
	if (isDead()) {
		m_hp = Stats::DefaultHp;
	}
}

bool PlayerStats::isDead() const {
	return (m_hp == Stats::MinHp);
}

// Equip stat bonus handling
void PlayerStats::updateBonuses(bool updateEquips, bool isLoading) {
	if (m_mapleWarrior > 0) {
		setMapleWarrior(m_mapleWarrior);
	}
	if (updateEquips) {
		m_equipBonuses = BonusSet();
		for (EquipBonuses::iterator iter = m_equipStats.begin(); iter != m_equipStats.end(); ++iter) {
			if (EquipDataProvider::Instance()->canEquip(iter->second.id, getJob(), getStr(true), getDex(true), getInt(true), getLuk(true), getFame())) {
				m_equipBonuses.hp += iter->second.hp;
				m_equipBonuses.mp += iter->second.mp;
				m_equipBonuses.str += iter->second.str;
				m_equipBonuses.dex += iter->second.dex;
				m_equipBonuses.intt += iter->second.intt;
				m_equipBonuses.luk += iter->second.luk;
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

void PlayerStats::setEquip(int16_t slot, Item *equip, bool isLoading) {
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
void PlayerStats::connectData(PacketCreator &packet) {
	packet.add<int8_t>(getLevel());
	packet.add<int16_t>(getJob());
	packet.add<int16_t>(getStr());
	packet.add<int16_t>(getDex());
	packet.add<int16_t>(getInt());
	packet.add<int16_t>(getLuk());
	packet.add<int32_t>(getHp());
	packet.add<int32_t>(getMaxHp(true));
	packet.add<int32_t>(getMp());
	packet.add<int32_t>(getMaxMp(true));
	packet.add<int16_t>(getAp());

	addSpData(packet);

	packet.add<int32_t>(getExp());
	packet.add<int32_t>(getFame());
}

void PlayerStats::addSpData(PacketCreator &packet) {
	if (GameLogicUtilities::isExtendedSpJob(getJob())) {
		packet.add<int8_t>(m_spTable.size());
		for (map<int8_t, int8_t>::iterator iter = m_spTable.begin(); iter != m_spTable.end(); iter++) {
			packet.add<int8_t>(iter->first);
			packet.add<int8_t>(iter->second);
		}
	}
	else {
		packet.add<int16_t>(getSp());
	}

}

int32_t PlayerStats::getMaxHp(bool withoutBonus) {
	if (!withoutBonus) {
		return static_cast<int32_t>(std::min<int32_t>(m_maxHp + m_equipBonuses.hp + m_buffBonuses.hp, Stats::MaxMaxHp));
	}
	return m_maxHp;
}

int32_t PlayerStats::getMaxMp(bool withoutBonus) {
	if (!withoutBonus) {
		return static_cast<int32_t>(std::min<int32_t>(m_maxMp + m_equipBonuses.mp + m_buffBonuses.mp, Stats::MaxMaxMp));
	}
	return m_maxMp;
}

int16_t PlayerStats::statUtility(int32_t test) {
	return static_cast<int16_t>(std::min<int32_t>(std::numeric_limits<int16_t>::max(), test));
}

int8_t PlayerStats::getSp(int8_t slot) { 
	if (m_spTable.size() < slot) {
		m_spTable[slot] = 0;
	}
	return m_spTable[slot];
}

int16_t PlayerStats::getStr(bool withBonus) {
	if (withBonus) {
		return statUtility(m_str + m_buffBonuses.str + m_equipBonuses.str);
	}
	return m_str;
}

int16_t PlayerStats::getDex(bool withBonus) {
	if (withBonus) {
		return statUtility(m_dex + m_buffBonuses.dex + m_equipBonuses.dex);
	}
	return m_dex;
}

int16_t PlayerStats::getInt(bool withBonus) {
	if (withBonus) {
		return statUtility(m_int + m_buffBonuses.intt + m_equipBonuses.intt);
	}
	return m_int;
}

int16_t PlayerStats::getLuk(bool withBonus) {
	if (withBonus) {
		return statUtility(m_luk + m_buffBonuses.luk + m_equipBonuses.luk);
	}
	return m_luk;
}

// Data modification
void PlayerStats::checkHpMp() {
	if (m_hp > getMaxHp()) {
		m_hp = getMaxHp();
	}
	if (m_mp > getMaxMp()) {
		m_mp = getMaxMp();
	}
}

void PlayerStats::setLevel(uint8_t level) {
	m_level = level;
	PlayerPacket::updateStat(m_player, Stats::Level, level);
	LevelsPacket::levelUp(m_player);
	SyncPacket::PlayerPacket::updateLevel(m_player->getId(), level);
}

void PlayerStats::setHp(int32_t hp, bool sendPacket) {
	m_hp = MiscUtilities::constrainToRange<int32_t>(hp, Stats::MinHp, getMaxHp());
	if (sendPacket) {
		PlayerPacket::updateStat(m_player, Stats::Hp, m_hp);
	}
	modifiedHp();
}

void PlayerStats::modifyHp(int32_t hpMod, bool sendPacket) {
	int32_t tempHp = m_hp + hpMod;
	tempHp = MiscUtilities::constrainToRange<int32_t>(tempHp, Stats::MinHp, getMaxHp());
	m_hp = static_cast<int32_t>(tempHp);

	if (sendPacket) {
		PlayerPacket::updateStat(m_player, Stats::Hp, m_hp);
	}
	modifiedHp();
}

void PlayerStats::damageHp(int32_t damageHp) {
	m_hp = std::max<int32_t>(Stats::MinHp, static_cast<int32_t>(m_hp) - damageHp);
	PlayerPacket::updateStat(m_player, Stats::Hp, m_hp);
	modifiedHp();
}

void PlayerStats::modifiedHp() {
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

void PlayerStats::setMp(int32_t mp, bool sendPacket) {
	if (!m_player->getActiveBuffs()->hasInfinity()) {
		m_mp = MiscUtilities::constrainToRange<int32_t>(mp, Stats::MinMp, getMaxMp());
	}
	PlayerPacket::updateStat(m_player, Stats::Mp, m_mp, sendPacket);
}

void PlayerStats::modifyMp(int32_t mpMod, bool sendPacket) {
	if (!m_player->getActiveBuffs()->hasInfinity()) {
		int32_t tempMp = m_mp + mpMod;
		tempMp = MiscUtilities::constrainToRange<int32_t>(tempMp, Stats::MinMp, getMaxMp());
		m_mp = static_cast<int32_t>(tempMp);
	}
	PlayerPacket::updateStat(m_player, Stats::Mp, m_mp, sendPacket);
}

void PlayerStats::damageMp(int32_t damageMp) {
	if (!m_player->getActiveBuffs()->hasInfinity()) {
		m_mp = std::max<int32_t>(Stats::MinMp, static_cast<int32_t>(m_mp) - damageMp);
	}
	PlayerPacket::updateStat(m_player, Stats::Mp, m_mp, false);
}

void PlayerStats::setSp(int8_t sp, int8_t slot, bool init) {
	m_spTable[slot] = sp;
	if (!init)
		PlayerPacket::updateStat(m_player, Stats::Sp, sp);
}

void PlayerStats::setAp(int16_t ap) {
	m_ap = ap;
	PlayerPacket::updateStat(m_player, Stats::Ap, ap);
}

void PlayerStats::setJob(int16_t job) {
	m_job = job;
	PlayerPacket::updateStat(m_player, Stats::Job, job);
	LevelsPacket::jobChange(m_player);
	SyncPacket::PlayerPacket::updateJob(m_player->getId(), job);
}

void PlayerStats::setStr(int16_t str) {
	m_str = str;
	PlayerPacket::updateStat(m_player, Stats::Str, str);
}

void PlayerStats::setDex(int16_t dex) {
	m_dex = dex;
	PlayerPacket::updateStat(m_player, Stats::Dex, dex);
}

void PlayerStats::setInt(int16_t intt) {
	m_int = intt;
	PlayerPacket::updateStat(m_player, Stats::Int, intt);
}

void PlayerStats::setLuk(int16_t luk) {
	m_luk = luk;
	PlayerPacket::updateStat(m_player, Stats::Luk, luk);
}

void PlayerStats::setMapleWarrior(int16_t xMod) {
	m_buffBonuses.str = (m_str * xMod) / 100;
	m_buffBonuses.dex = (m_dex * xMod) / 100;
	m_buffBonuses.intt = (m_int * xMod) / 100;
	m_buffBonuses.luk = (m_luk * xMod) / 100;
	if (m_mapleWarrior != xMod) {
		m_mapleWarrior = xMod;
		updateBonuses();
	}
}

void PlayerStats::setMaxHp(int32_t maxHp) {
	m_maxHp = MiscUtilities::constrainToRange(maxHp, Stats::MinMaxHp, Stats::MaxMaxHp);
	PlayerPacket::updateStat(m_player, Stats::MaxHp, m_maxHp);
	modifiedHp();
}

void PlayerStats::setMaxMp(int32_t maxMp) {
	m_maxMp = MiscUtilities::constrainToRange(maxMp, Stats::MinMaxMp, Stats::MaxMaxMp);
	PlayerPacket::updateStat(m_player, Stats::MaxMp, m_maxMp);
}

void PlayerStats::setHyperBody(int16_t xMod, int16_t yMod) {
	m_hyperBodyX = xMod;
	m_hyperBodyY = yMod;
	m_buffBonuses.hp = std::min<uint32_t>((m_maxHp + m_equipBonuses.hp) * xMod / 100, Stats::MaxMaxHp);
	m_buffBonuses.mp = std::min<uint32_t>((m_maxMp + m_equipBonuses.mp) * yMod / 100, Stats::MaxMaxMp);
	PlayerPacket::updateStat(m_player, Stats::MaxHp, m_maxHp);
	PlayerPacket::updateStat(m_player, Stats::MaxMp, m_maxMp);
	if (Party *p = m_player->getParty()) {
		p->showHpBar(m_player);
	}
	m_player->getActiveBuffs()->checkBerserk();
}

void PlayerStats::modifyMaxHp(int32_t mod) {
	m_maxHp = std::min<int32_t>(m_maxHp + mod, Stats::MaxMaxHp);
	PlayerPacket::updateStat(m_player, Stats::MaxHp, m_maxHp);
}

void PlayerStats::modifyMaxMp(int32_t mod) {
	m_maxMp = std::min<int32_t>(m_maxMp + mod, Stats::MaxMaxMp);
	PlayerPacket::updateStat(m_player, Stats::MaxMp, m_maxMp);
}

void PlayerStats::setExp(int32_t exp) {
	m_exp = std::max(exp, 0);
	PlayerPacket::updateStat(m_player, Stats::Exp, exp);
}

void PlayerStats::setFame(int32_t fame) {
	m_fame = MiscUtilities::constrainToRange(fame, Stats::MinFame, Stats::MaxFame);
	PlayerPacket::updateStat(m_player, Stats::Fame, fame);
}

void PlayerStats::loseExp() {
	if (!GameLogicUtilities::isBeginnerJob(getJob()) && getLevel() < GameLogicUtilities::getMaxLevel(getJob()) && m_player->getMap() != Maps::SorcerersRoom) {
		uint16_t charms = m_player->getInventory()->getItemAmount(Items::SafetyCharm);
		if (charms > 0) {
			Inventory::takeItem(m_player, Items::SafetyCharm, 1);
			charms = std::min<uint16_t>(--charms, 0xFF);
			InventoryPacket::useCharm(m_player, static_cast<uint8_t>(charms));
			return;
		}
		Map *loc = Maps::getMap(m_player->getMap());
		int8_t expLoss = 10;
		if (loc->loseOnePercent()) {
			expLoss = 1;
		}
		else {
			switch (GameLogicUtilities::getJobTrack(getJob(), true)) {
				case Jobs::JobTracks::Magician:
					expLoss = 7;
					break;
				case Jobs::JobTracks::Thief:
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
void PlayerStats::giveExp(uint32_t exp, bool inChat, bool white) {
	int16_t fullJob = getJob();
	uint8_t level = getLevel();
	uint8_t jobMax = GameLogicUtilities::getMaxLevel(fullJob);
	if (level >= jobMax) {
		// Do not give EXP to characters of max level or over
		return;
	}
	uint32_t curExp = getExp() + exp;
	if (exp != 0) {
		LevelsPacket::showExp(m_player, exp, white, inChat);
	}
	if (curExp >= getExp(level)) {
		bool cygnus = GameLogicUtilities::isCygnus(fullJob);
		uint8_t levelsGained = 0;
		uint8_t levelsMax = ChannelServer::Instance()->getMaxMultiLevel();
		int16_t apGain = 0;
		int16_t spGain = 0;
		int16_t hpGain = 0;
		int16_t mpGain = 0;
		int16_t job = GameLogicUtilities::getJobTrack(fullJob, true);
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
			switch (job) {
				case Jobs::JobTracks::Beginner:
					hpGain += levelHp(Stats::BaseHp::Beginner);
					mpGain += levelMp(Stats::BaseMp::Beginner, intt);
					break;
				case Jobs::JobTracks::Warrior:
					if (levelsGained == 1 && m_player->getSkills()->hasHpIncrease()) {
						x = getX(m_player->getSkills()->getHpIncrease());
					}
					hpGain += levelHp(Stats::BaseHp::Warrior, x);
					mpGain += levelMp(Stats::BaseMp::Warrior, intt);
					break;
				case Jobs::JobTracks::Magician:
					if (levelsGained == 1 && m_player->getSkills()->hasMpIncrease()) {
						x = getX(m_player->getSkills()->getMpIncrease());
					}
					hpGain += levelHp(Stats::BaseHp::Magician);
					mpGain += levelMp(Stats::BaseMp::Magician, 2 * x + intt);
					break;
				case Jobs::JobTracks::Bowman:
					hpGain += levelHp(Stats::BaseHp::Bowman);
					mpGain += levelMp(Stats::BaseMp::Bowman, intt);
					break;
				case Jobs::JobTracks::Thief:
					hpGain += levelHp(Stats::BaseHp::Thief);
					mpGain += levelMp(Stats::BaseMp::Thief, intt);
					break;
				case Jobs::JobTracks::Pirate:
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
				SkillLevelInfo *hb = SkillDataProvider::Instance()->getSkill(skillId, hbLevel);
				setHyperBody(hb->x, hb->y);
			}

			setHp(getMaxHp());
			setMp(getMaxMp());
			m_player->setLevelDate();
			if (getLevel() == jobMax && !m_player->isGm()) {
				std::ostringstream message;
				message << "[Congrats] " << m_player->getName() << " has reached Level "
						<< static_cast<int16_t>(jobMax) << "! Congratulate "
						<< m_player->getName() << " on such an amazing achievement!";

				PlayerPacket::showMessageWorld(message.str(), PlayerPacket::NoticeTypes::Blue);
			}
		}
	}
	setExp(curExp);
}

void PlayerStats::addStat(PacketReader &packet) {
	packet.skipBytes(4);
	int64_t type = packet.get<int64_t>();
	if (getAp() == 0) {
		// Hacking
		return;
	}
	LevelsPacket::statOk(m_player);
	addStat(type);
}

void PlayerStats::addStatMulti(PacketReader &packet) {
	packet.skipBytes(4);
	uint32_t amount = packet.get<uint32_t>();

	LevelsPacket::statOk(m_player);

	for (uint32_t i = 0; i < amount; i++) {
		int64_t type = packet.get<int64_t>();
		int32_t value = packet.get<int32_t>();

		if (value < 0 || getAp() < value) {
			// Hacking
			return;
		}

		addStat(type, static_cast<int16_t>(value)); // Prefer a single cast to countless casts/modification down the line
	}
}

void PlayerStats::addStat(int64_t type, int16_t mod, bool isReset) {
	int16_t maxStat = ChannelServer::Instance()->getMaxStats();
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
				SkillLevelInfo *hb = SkillDataProvider::Instance()->getSkill(skillId, hbLevel);
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

int32_t PlayerStats::randHp() {
	return Randomizer::Instance()->randShort(Stats::BaseHp::Variation); // Max HP range per class (e.g. Beginner is 8-12)
}

int32_t PlayerStats::randMp() {
	return Randomizer::Instance()->randShort(Stats::BaseMp::Variation); // Max MP range per class (e.g. Beginner is 6-8)
}

int16_t PlayerStats::getX(int32_t skillId) {
	return m_player->getSkills()->getSkillInfo(skillId)->x;
}

int16_t PlayerStats::getY(int32_t skillId) {
	return m_player->getSkills()->getSkillInfo(skillId)->y;
}

int16_t PlayerStats::apResetHp(bool isReset, bool isSubtract, int16_t val, int16_t sVal) {
	return (isReset ? (isSubtract ? -(sVal + val + Stats::BaseHp::Variation) : val) : levelHp(val, sVal));
}

int16_t PlayerStats::apResetMp(bool isReset, bool isSubtract, int16_t val, int16_t sVal) {
	return (isReset ? (isSubtract ? -(sVal + val + Stats::BaseMp::Variation) : val) : levelMp(val, sVal));
}

int32_t PlayerStats::levelHp(int16_t val, int16_t bonus) {
	return randHp() + val + bonus;
}

int32_t PlayerStats::levelMp(int16_t val, int16_t bonus) {
	return randMp() + val + bonus;
}

uint32_t PlayerStats::getExp(uint8_t level) {
	return Stats::PlayerExp[level - 1];
}