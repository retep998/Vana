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
	int16_t fulljob = player->getJob();
	if (player->getLevel() >= getMaxLevel(fulljob)) // Do not give EXP to characters of max level or over
		return;
	uint32_t cexp = player->getExp() + exp;
	if (exp != 0)
		LevelsPacket::showEXP(player, exp, white, inChat);
	uint8_t level = player->getLevel();
	if (cexp >= getExp(level)) {
		bool cygnus = GameLogicUtilities::isCygnus(fulljob);
		uint8_t levelsgained = 0;
		uint8_t levelsmax = ChannelServer::Instance()->getMaxMultiLevel();
		int16_t apgain = 0;
		int16_t spgain = 0;
		int16_t hpgain = 0;
		int16_t mpgain = 0;
		int16_t job = GameLogicUtilities::getJobTrack(fulljob, true);
		int16_t intt = player->getInt() / 10;
		int16_t x = 0; // X value for Improving *P Increase skills, cached, only needs to be set once
		while (cexp >= getExp(level) && levelsgained < levelsmax) {
			cexp -= getExp(player->getLevel());
			level++;
			levelsgained++;
			if (cygnus && level <= Stats::CygnusApCutoff) {
				apgain += Stats::ApPerCygnusLevel;
			}
			else {
				apgain += Stats::ApPerLevel;
			}
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
			if (level >= getMaxLevel(fulljob)) { // Do not let people level past the level cap
				cexp = 0;
				break;
			}
		}

		if (cexp >= getExp(level)) { // If the desired number of level ups have passed and they're still above, set it to where it should be
			cexp = getExp(level) - 1;
		}

		if (levelsgained) { // Check if the player has leveled up at all, it is possible that the user hasn't leveled up if multi-level limit is 0
			player->modifyRMHp(hpgain);
			player->modifyRMMp(mpgain);
			player->setLevel(level);
			player->setAp(player->getAp() + apgain);
			player->setSp(player->getSp() + spgain);
			// Let hyperbody remain on if on during a level up, as it should
			if (player->getActiveBuffs()->hasHyperBody()) {
				int32_t skillid = player->getActiveBuffs()->getHyperBody();
				uint8_t hblevel = player->getActiveBuffs()->getActiveSkillLevel(skillid);
				SkillLevelInfo *hb = SkillDataProvider::Instance()->getSkill(skillid, hblevel);
				player->setHyperBody(hb->x, hb->y);
			}
			else {
				player->setMHp(player->getRMHp());
				player->setMMp(player->getRMMp());
			}
			player->setHp(player->getMHp());
			player->setMp(player->getMMp());
			player->setLevelDate();
			if (player->getLevel() == getMaxLevel(fulljob) && !player->isGm()) {
				string message;
				message = "[Congrats] ";
				message += player->getName();
				message += " has reached Level ";
				message += boost::lexical_cast<string>((int16_t) getMaxLevel(fulljob));
				message += "! Congratulate ";
				message += player->getName();
				message += " on such an amazing achievement!";
				PlayersPacket::showMessageWorld(message, 6);
			}
		}
	}
	player->setExp(cexp);
}

void Levels::addStat(Player *player, PacketReader &packet) {
	packet.skipBytes(4);
	int32_t type = packet.get<int32_t>();
	if (player->getAp() == 0) {
		// hacking
		return;
	}
	LevelsPacket::statOK(player);
	addStat(player, type);
}

void Levels::addStatMulti(Player *player, PacketReader &packet) {
	packet.skipBytes(4);
	uint32_t amount = packet.get<uint32_t>();

	LevelsPacket::statOK(player);

	for (uint32_t i = 0; i < amount; i++) {
		int32_t type = packet.get<int32_t>();
		int32_t value = packet.get<int32_t>();

		if (value < 0 || player->getAp() < value) {
			//hacking
			return;
		}

		addStat(player, type, static_cast<int16_t>(value)); // Prefer a single cast to countless casts/modification down the line
	}
}

void Levels::addStat(Player *player, int32_t type, int16_t mod, bool isreset) {
	int16_t maxstat = ChannelServer::Instance()->getMaxStats();
	bool issubtract = mod < 0;
	switch (type) {
		case Stats::Str:
			if (player->getStr() >= maxstat)
				return;
			player->setStr(player->getStr() + mod);
			break;
		case Stats::Dex:
			if (player->getDex() >= maxstat)
				return;
			player->setDex(player->getDex() + mod);
			break;
		case Stats::Int:
			if (player->getInt() >= maxstat)
				return;
			player->setInt(player->getInt() + mod);
			break;
		case Stats::Luk:
			if (player->getLuk() >= maxstat)
				return;
			player->setLuk(player->getLuk() + mod);
			break;
		case Stats::MaxHp:
		case Stats::MaxMp: {
			if (type == Stats::MaxHp && player->getRMHp() >= Stats::MaxMaxHp)
				return;
			if (type == Stats::MaxMp && player->getRMMp() >= Stats::MaxMaxMp)
				return;
			if (issubtract && player->getHpMpAp() == 0) {
				// Hacking
				return;
			}
			int16_t job = GameLogicUtilities::getJobTrack(player->getJob());
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
			player->setHpMpAp(player->getHpMpAp() + mod);
			switch (type) {
				case Stats::MaxHp: player->modifyRMHp(hpgain); break;
				case Stats::MaxMp: player->modifyRMMp(mpgain); break;
			}
			if (player->getActiveBuffs()->hasHyperBody()) {
				int32_t skillid = player->getActiveBuffs()->getHyperBody();
				uint8_t hblevel = player->getActiveBuffs()->getActiveSkillLevel(skillid);
				SkillLevelInfo *hb = SkillDataProvider::Instance()->getSkill(skillid, hblevel);
				player->setHyperBody(hb->x, hb->y);
			}
			else {
				player->setMHp(player->getRMHp());
				player->setMMp(player->getRMMp());
			}
			player->setHp(player->getHp());
			player->setMp(player->getMp());
			break;
		}
		default:
			// Hacking, one assumes
			break;
	}
	if (!isreset)
		player->setAp(player->getAp() - mod);
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

uint8_t Levels::getMaxLevel(int16_t jobid) {
	return (GameLogicUtilities::isCygnus(jobid) ? Stats::CygnusLevels : Stats::PlayerLevels);
}