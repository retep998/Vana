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
#include "Levels.h"
#include "ChannelServer.h"
#include "GameConstants.h"
#include "GameLogicUtilities.h"
#include "LevelsPacket.h"
#include "PacketReader.h"
#include "Player.h"
#include "PlayersPacket.h"
#include "Randomizer.h"
#include "SkillDataProvider.h"
#include "VersionConstants.h"
#include <boost/lexical_cast.hpp>
#include <string>

using std::string;

void Levels::giveExp(Player *player, uint32_t exp, bool inChat, bool white) {
	if (player->getStats()->getLevel() >= Stats::PlayerLevels) // Do not give EXP to characters of max level or over
		return;
	uint32_t cexp = player->getStats()->getExp() + exp;
	if (exp != 0)
		LevelsPacket::showEXP(player, exp, white, inChat);
	uint8_t level = player->getStats()->getLevel();
	if (cexp >= getExp(level)) {
		uint8_t levelsgained = 0;
		uint8_t levelsmax = ChannelServer::Instance()->getMaxMultiLevel();
		int16_t apgain = 0;
		int16_t spgain = 0;
		int16_t hpgain = 0;
		int16_t mpgain = 0;
		int16_t fulljob = player->getStats()->getJob();
		int16_t job = GameLogicUtilities::getJobTrack(fulljob);
		int16_t intt = player->getStats()->getTotalStat(Stats::Int) / 10;
		int16_t x = 0; // X value for Improving *P Increase skills, cached, only needs to be set once
		while (cexp >= getExp(level) && levelsgained < levelsmax) {
			cexp -= getExp(player->getStats()->getLevel());
			level++;
			levelsgained++;
			apgain += Stats::ApPerLevel;
			switch (job) {
				case Jobs::JobTracks::Beginner:
					hpgain += levelHp(Stats::BaseHp::Beginner);
					mpgain += levelMp(Stats::BaseMp::Beginner, intt);
					break;
				case Jobs::JobTracks::Warrior:
					if (levelsgained == 1 && player->getSkills()->hasHpIncrease())
						x = getX(player, player->getSkills()->getHpIncrease());
					hpgain += levelHp(Stats::BaseHp::Warrior, x);
					mpgain += levelMp(Stats::BaseMp::Warrior, intt);
					break;
				case Jobs::JobTracks::Magician:
					if (levelsgained == 1 && player->getSkills()->hasMpIncrease())
						x = getX(player, player->getSkills()->getMpIncrease());
					hpgain += levelHp(Stats::BaseHp::Magician);
					mpgain += levelMp(Stats::BaseMp::Magician, 2 * x + intt);
					break;
				case Jobs::JobTracks::Bowman:
					hpgain += levelHp(Stats::BaseHp::Bowman);
					mpgain += levelMp(Stats::BaseMp::Bowman, intt);
					break;
				case Jobs::JobTracks::Thief:
					hpgain += levelHp(Stats::BaseHp::Thief);
					mpgain += levelMp(Stats::BaseMp::Thief, intt);
					break;
				case Jobs::JobTracks::Pirate:
					if (levelsgained == 1 && player->getSkills()->hasHpIncrease())
						x = getX(player, player->getSkills()->getHpIncrease());
					hpgain += levelHp(Stats::BaseHp::Pirate, x);
					mpgain += levelMp(Stats::BaseMp::Pirate, intt);
					break;
				default: // GM
					hpgain += Stats::BaseHp::Gm;
					mpgain += Stats::BaseMp::Gm;
			}
			if (!GameLogicUtilities::isBeginnerJob(fulljob))
				spgain += Stats::SpPerLevel;
			if (level >= Stats::PlayerLevels) { // Do not let people level past the level cap
				cexp = 0;
				break;
			}
		}

		if (cexp >= getExp(level)) { // If the desired number of level ups have passed and they're still above, set it to where it should be
			cexp = getExp(level) - 1;
		}

		if (levelsgained) { // Check if the player has leveled up at all, it is possible that the user hasn't leveled up if multi-level limit is 0
			player->getStats()->modifyRMHp(hpgain);
			player->getStats()->modifyRMMp(mpgain);
			player->getStats()->setLevel(level);
			player->getStats()->setBaseStat(Stats::Ap, player->getStats()->getBaseStat(Stats::Ap) + apgain);
			player->getStats()->setBaseStat(Stats::Sp, player->getStats()->getBaseStat(Stats::Sp) + spgain);
			// Let hyperbody remain on if on during a level up, as it should
			if (player->getActiveBuffs()->hasHyperBody()) {
				int32_t skillid = player->getActiveBuffs()->getHyperBody();
				uint8_t hblevel = player->getActiveBuffs()->getActiveSkillLevel(skillid);
				SkillLevelInfo *hb = SkillDataProvider::Instance()->getSkill(skillid, hblevel);
				player->getStats()->setHyperBody(hb->x, hb->y);
			}
			else {
				player->getStats()->setMHp(player->getStats()->getRMHp());
				player->getStats()->setMMp(player->getStats()->getRMMp());
			}
			player->getStats()->setHp(player->getStats()->getMHp());
			player->getStats()->setMp(player->getStats()->getMMp());
			player->setLevelDate();
			if (player->getStats()->getLevel() == Stats::PlayerLevels && !player->isGm()) {
				string message;
				message = "[Congrats] ";
				message += player->getName();
				message += " has reached Level ";
				message += boost::lexical_cast<string>(Stats::PlayerLevels);
				message += "! Congratulate ";
				message += player->getName();
				message += " on such an amazing achievement!";
				PlayersPacket::showMessageWorld(message, 6);
			}
		}
	}
	player->getStats()->setExp(cexp);
}

void Levels::addStat(Player *player, PacketReader &packet) {
	packet.skipBytes(4);
	int32_t type = packet.get<int32_t>();
	if (player->getStats()->getBaseStat(Stats::Ap) == 0) {
		// hacking
		return;
	}
	LevelsPacket::statOK(player);
	addStat(player, type);
}

void Levels::addStat(Player *player, int32_t type, int16_t mod, bool isreset) {
	int16_t maxstat = ChannelServer::Instance()->getMaxStats();
	bool issubtract = mod < 0;
	switch (type) {
		case Stats::Str:
			if (player->getStats()->getBaseStat(Stats::Str) >= maxstat)
				return;
			player->getStats()->setBaseStat(Stats::Str, player->getStats()->getBaseStat(Stats::Str) + mod);
			break;
		case Stats::Dex:
			if (player->getStats()->getBaseStat(Stats::Dex) >= maxstat)
				return;
			player->getStats()->setBaseStat(Stats::Dex, player->getStats()->getBaseStat(Stats::Dex) + mod);
			break;
		case Stats::Int:
			if (player->getStats()->getBaseStat(Stats::Int) >= maxstat)
				return;
			player->getStats()->setBaseStat(Stats::Int, player->getStats()->getBaseStat(Stats::Int) + mod);
			break;
		case Stats::Luk:
			if (player->getStats()->getBaseStat(Stats::Luk) >= maxstat)
				return;
			player->getStats()->setBaseStat(Stats::Luk, player->getStats()->getBaseStat(Stats::Luk) + mod);
			break;
		case Stats::MaxHp:
		case Stats::MaxMp: {
			if (type == Stats::MaxHp && player->getStats()->getRMHp() >= Stats::MaxMaxHp)
				return;
			if (type == Stats::MaxMp && player->getStats()->getRMMp() >= Stats::MaxMaxMp)
				return;
			if (issubtract && player->getStats()->getBaseStat(Stats::HpMpAp) == 0) {
				// Hacking
				return;
			}
			int16_t job = GameLogicUtilities::getJobTrack(player->getStats()->getJob());
			int16_t hpgain = 0;
			int16_t mpgain = 0;
			int16_t y = 0;
			switch (job) {
				case Jobs::JobTracks::Beginner: // Beginner
					hpgain = apResetHp(isreset, issubtract, Stats::BaseHp::BeginnerAp);
					mpgain = apResetMp(isreset, issubtract, Stats::BaseMp::BeginnerAp);
					break;
				case Jobs::JobTracks::Warrior:
					if (player->getSkills()->hasHpIncrease())
						y = getY(player, player->getSkills()->getHpIncrease());
					hpgain = apResetHp(isreset, issubtract, Stats::BaseHp::WarriorAp, y);
					mpgain = apResetMp(isreset, issubtract, Stats::BaseMp::WarriorAp);
					break;
				case Jobs::JobTracks::Magician:
					if (player->getSkills()->hasMpIncrease())
						y = getY(player, player->getSkills()->getMpIncrease());
					hpgain = apResetHp(isreset, issubtract, Stats::BaseHp::MagicianAp);
					mpgain = apResetMp(isreset, issubtract, Stats::BaseMp::MagicianAp, 2 * y);
					break;
				case Jobs::JobTracks::Bowman:
					hpgain = apResetHp(isreset, issubtract, Stats::BaseHp::BowmanAp);
					mpgain = apResetMp(isreset, issubtract, Stats::BaseMp::BowmanAp);
					break;
				case Jobs::JobTracks::Thief:
					hpgain = apResetHp(isreset, issubtract, Stats::BaseHp::ThiefAp);
					mpgain = apResetMp(isreset, issubtract, Stats::BaseMp::ThiefAp);
					break;
				case Jobs::JobTracks::Pirate:
					if (player->getSkills()->hasHpIncrease())
						y = getY(player, player->getSkills()->getHpIncrease());
					hpgain = apResetHp(isreset, issubtract, Stats::BaseHp::PirateAp, y);
					mpgain = apResetMp(isreset, issubtract, Stats::BaseMp::PirateAp);
					break;
				default: // GM
					hpgain = apResetHp(isreset, issubtract, Stats::BaseHp::GmAp);
					mpgain = apResetMp(isreset, issubtract, Stats::BaseMp::GmAp);
					break;
			}
			player->getStats()->setBaseStat(Stats::HpMpAp, player->getStats()->getBaseStat(Stats::HpMpAp) + mod, false);
			switch (type) {
				case Stats::MaxHp: player->getStats()->modifyRMHp(hpgain); break;
				case Stats::MaxMp: player->getStats()->modifyRMMp(mpgain); break;
			}
			if (player->getActiveBuffs()->hasHyperBody()) {
				int32_t skillid = player->getActiveBuffs()->getHyperBody();
				uint8_t hblevel = player->getActiveBuffs()->getActiveSkillLevel(skillid);
				SkillLevelInfo *hb = SkillDataProvider::Instance()->getSkill(skillid, hblevel);
				player->getStats()->setHyperBody(hb->x, hb->y);
			}
			else {
				player->getStats()->setMHp(player->getStats()->getRMHp());
				player->getStats()->setMMp(player->getStats()->getRMMp());
			}
			player->getStats()->setHp(player->getStats()->getHp());
			player->getStats()->setMp(player->getStats()->getMp());
			break;
		}
		default:
			// Hacking, one assumes
			break;
	}
	if (!isreset)
		player->getStats()->setBaseStat(Stats::Ap, player->getStats()->getBaseStat(Stats::Ap) - 1);
}

int16_t Levels::randHp() {
	return Randomizer::Instance()->randShort(Stats::BaseHp::Variation); // Max HP range per class (e.g. Beginner is 8-12)
}

int16_t Levels::randMp() {
	return Randomizer::Instance()->randShort(Stats::BaseMp::Variation); // Max MP range per class (e.g. Beginner is 6-8)
}

int16_t Levels::getX(Player *player, int32_t skillid) {
	return SkillDataProvider::Instance()->getSkill(skillid, player->getSkills()->getSkillLevel(skillid))->x;
}

int16_t Levels::getY(Player *player, int32_t skillid) {
	return SkillDataProvider::Instance()->getSkill(skillid, player->getSkills()->getSkillLevel(skillid))->y;
}

int16_t Levels::apResetHp(bool isreset, bool issubtract, int16_t val, int16_t sval) {
	return (isreset ? (issubtract ? -(sval + val + Stats::BaseHp::Variation) : val) : levelHp(val, sval));
}

int16_t Levels::apResetMp(bool isreset, bool issubtract, int16_t val, int16_t sval) {
	return (isreset ? (issubtract ? -(sval + val + Stats::BaseMp::Variation) : val) : levelMp(val, sval));
}

int16_t Levels::levelHp(int16_t val, int16_t bonus) {
	return randHp() + val + bonus;
}

int16_t Levels::levelMp(int16_t val, int16_t bonus) {
	return randMp() + val + bonus;
}

uint32_t Levels::getExp(uint8_t level) {
	return Levels::exps[level - 1];
}