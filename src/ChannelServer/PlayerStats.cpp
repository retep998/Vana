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
#include "PlayerStats.h"
#include "ChannelServer.h"
#include "GameConstants.h"
#include "GameLogicUtilities.h"
#include "Instance.h"
#include "Inventory.h"
#include "InventoryPacket.h"
#include "LevelsPacket.h"
#include "Maps.h"
#include "PacketCreator.h"
#include "PacketReader.h"
#include "Party.h"
#include "Player.h"
#include "PlayerPacket.h"
#include "PlayersPacket.h"
#include "Randomizer.h"
#include "SkillDataProvider.h"
#include "Summons.h"
#include "VersionConstants.h"
#include "WorldServerConnectPacket.h"
#include <boost/lexical_cast.hpp>
#include <string>

using std::string;

PlayerStats::PlayerStats(Player *player,
	uint8_t level,
	int16_t job,
	int16_t fame,
	int16_t str,
	int16_t dex,
	int16_t intt,
	int16_t luk,
	int16_t ap,
	uint16_t hpmp_ap,
	int16_t sp,
	int16_t hp,
	int16_t mhp,
	int16_t mp,
	int16_t mmp,
	int32_t exp) : player(player), level(level), job(job), fame(fame), str(str), dex(dex), intt(intt), luk(luk), ap(ap), hpmp_ap(hpmp_ap), sp(sp), hp(hp), mhp(mhp), mp(mp), mmp(mmp), exp(exp), hp_bonus(0), hb_hp(0), mp_bonus(0), hb_mp(0), str_bonus(0), dex_bonus(0), int_bonus(0), luk_bonus(0) {
		if (this->hp == 0)
			this->hp = 50;
		else if (this->hp > mhp)
			this->hp = mhp;

		if (this->mp > mmp)
			this->mp = mmp;
}

// Data Acquisition
void PlayerStats::connectData(PacketCreator &packet) {
	packet.add<int8_t>(getLevel());
	packet.add<int16_t>(getJob());
	packet.add<int16_t>(getStr());
	packet.add<int16_t>(getDex());
	packet.add<int16_t>(getInt());
	packet.add<int16_t>(getLuk());
	packet.add<int16_t>(getHp());
	packet.add<int16_t>(getMHp(true));
	packet.add<int16_t>(getMp());
	packet.add<int16_t>(getMMp(true));
	packet.add<int16_t>(getAp());
	packet.add<int16_t>(getSp());
	packet.add<int32_t>(getExp());
	packet.add<int16_t>(getFame());
}

int16_t PlayerStats::getMHp(bool withoutbonus) {
	if (!withoutbonus) {
		if ((hb_hp + hp_bonus + mhp) > 30000)
			return 30000;
		else
			return (hb_hp + hp_bonus + mhp);
	}
	return mhp;
}

int16_t PlayerStats::getMMp(bool withoutbonus) {
	if (!withoutbonus) {
		if ((hb_mp + mp_bonus + mmp) > 30000)
			return 30000;
		else
			return (hb_mp + mp_bonus + mmp);
	}
	return mmp;
}

// Data Modification
void PlayerStats::setLevel(uint8_t level) {
	this->level = level;
	PlayerPacket::updateStatShort(player, Stats::Level, level);
	LevelsPacket::levelUp(player);
	WorldServerConnectPacket::updateLevel(ChannelServer::Instance()->getWorldConnection(), player->getId(), level);
}

void PlayerStats::setHp(int16_t shp, bool is) {
	if (shp < 0)
		hp = 0;
	else if (shp > getMHp())
		hp = getMHp();
	else
		hp = shp;
	if (is)
		PlayerPacket::updateStatShort(player, Stats::Hp, hp);
	modifiedHp();
}

void PlayerStats::modifyHp(int16_t nhp, bool is) {
	if ((hp + nhp) < 0)
		hp = 0;
	else if ((hp + nhp) > getMHp())
		hp = getMHp();
	else
		hp = (hp + nhp);
	if (is)
		PlayerPacket::updateStatShort(player, Stats::Hp, hp);
	modifiedHp();
}

void PlayerStats::damageHp(uint16_t dhp) {
	hp = (dhp > hp ? 0 : hp - dhp);
	PlayerPacket::updateStatShort(player, Stats::Hp, hp);
	modifiedHp();
}

void PlayerStats::modifiedHp() {
	if (player->getParty())
		player->getParty()->showHpBar(player);
	player->getActiveBuffs()->checkBerserk();
	if (hp == 0) {
		if (player->getInstance() != 0) {
			player->getInstance()->sendMessage(PlayerDeath, player->getId());
		}
		loseExp();
		Summons::removeSummon(player, false, true, false, 2);
	}
}

void PlayerStats::setMp(int16_t smp, bool is) {
	if (!player->getActiveBuffs()->hasInfinity()) {
		if (smp < 0)
			mp = 0;
		else if (smp > getMMp())
			mp = getMMp();
		else
			mp = smp;
	}
	PlayerPacket::updateStatShort(player, Stats::Mp, mp, is);
}

void PlayerStats::modifyMp(int16_t nmp, bool is) {
	if (!player->getActiveBuffs()->hasInfinity()) {
		if ((mp + nmp) < 0)
			mp = 0;
		else if ((mp + nmp) > getMMp())
			mp = getMMp();
		else
			mp = (mp + nmp);
	}
	PlayerPacket::updateStatShort(player, Stats::Mp, mp, is);
}

void PlayerStats::damageMp(uint16_t dmp) {
	if (!player->getActiveBuffs()->hasInfinity()) {
		mp = (dmp > mp ? 0 : mp - dmp);
	}
	PlayerPacket::updateStatShort(player, Stats::Mp, mp, false);
}

void PlayerStats::setSp(int16_t sp) {
	this->sp = sp;
	PlayerPacket::updateStatShort(player, Stats::Sp, sp);
}

void PlayerStats::setAp(int16_t ap) {
	this->ap = ap;
	PlayerPacket::updateStatShort(player, Stats::Ap, ap);
}

void PlayerStats::setJob(int16_t job) {
	this->job = job;
	PlayerPacket::updateStatShort(player, Stats::Job, job);
	LevelsPacket::jobChange(player);
	WorldServerConnectPacket::updateJob(ChannelServer::Instance()->getWorldConnection(), player->getId(), job);
}

void PlayerStats::setStr(int16_t str) {
	this->str = str;
	PlayerPacket::updateStatShort(player, Stats::Str, str);
}

void PlayerStats::setDex(int16_t dex) {
	this->dex = dex;
	PlayerPacket::updateStatShort(player, Stats::Dex, dex);
}

void PlayerStats::setInt(int16_t intt) {
	this->intt = intt;
	PlayerPacket::updateStatShort(player, Stats::Int, intt);
}

void PlayerStats::setLuk(int16_t luk) {
	this->luk = luk;
	PlayerPacket::updateStatShort(player, Stats::Luk, luk);
}

void PlayerStats::setMHp(int16_t mhp) {
	if (mhp > Stats::MaxMaxHp)
		mhp = Stats::MaxMaxHp;
	else if (mhp < Stats::MinMaxHp)
		mhp = Stats::MinMaxHp;
	this->mhp = mhp;
	PlayerPacket::updateStatShort(player, Stats::MaxHp, mhp);
	modifiedHp();
}

void PlayerStats::setMMp(int16_t mmp) {
	if (mmp > Stats::MaxMaxMp)
		mmp = Stats::MaxMaxMp;
	else if (mmp < Stats::MinMaxMp)
		mmp = Stats::MinMaxMp;
	this->mmp = mmp;
	PlayerPacket::updateStatShort(player, Stats::MaxMp, mmp);
}

void PlayerStats::setHyperBody(int16_t modx, int16_t mody) {
	hb_hp = (((mhp + hp_bonus) * modx / 100) > Stats::MaxMaxHp ? Stats::MaxMaxHp : (mhp + hp_bonus) * modx / 100);
	hb_mp = (((mmp + mp_bonus) * mody / 100) > Stats::MaxMaxMp ? Stats::MaxMaxMp : (mmp + mp_bonus) * mody / 100);
	PlayerPacket::updateStatShort(player, Stats::MaxHp, mhp);
	PlayerPacket::updateStatShort(player, Stats::MaxMp, mmp);
	if (player->getParty())
		player->getParty()->showHpBar(player);
	player->getActiveBuffs()->checkBerserk();
}

void PlayerStats::modifyMHp(int16_t mod) {
	mhp = (((mhp + mod) > Stats::MaxMaxHp) ? Stats::MaxMaxHp : (mhp + mod));
	PlayerPacket::updateStatShort(player, Stats::MaxHp, mhp);
}

void PlayerStats::modifyMMp(int16_t mod) {
	mmp = (((mmp + mod) > Stats::MaxMaxMp) ? Stats::MaxMaxMp : (mmp + mod));
	PlayerPacket::updateStatShort(player, Stats::MaxMp, mmp);
}

void PlayerStats::setExp(int32_t exp) {
	if (exp < 0)
		exp = 0;
	this->exp = exp;
	PlayerPacket::updateStatInt(player, Stats::Exp, exp);
}

void PlayerStats::setFame(int16_t fame) {
	if (fame < Stats::MinFame)
		fame = Stats::MinFame;
	else if (fame > Stats::MaxFame)
		fame = Stats::MaxFame;
	this->fame = fame;
	PlayerPacket::updateStatInt(player, Stats::Fame, fame);
}

void PlayerStats::loseExp() {
	if (!GameLogicUtilities::isBeginnerJob(getJob()) && getLevel() < getMaxLevel(getJob()) && player->getMap() != Maps::SorcerersRoom) {
		uint16_t charms = player->getInventory()->getItemAmount(Items::SafetyCharm);
		if (charms > 0) {
			Inventory::takeItem(player, Items::SafetyCharm, 1);
			charms--;
			if (charms > 0xFF)
				charms = 0xFF;
			InventoryPacket::useCharm(player, static_cast<uint8_t>(charms));
			return;
		}
		Map *loc = Maps::getMap(player->getMap());
		int8_t exploss = 10;
		if ((loc->getInfo()->fieldLimit & FieldLimitBits::RegularExpLoss) != 0 || loc->getInfo()->town)
			exploss = 1;
		else {
			switch (GameLogicUtilities::getJobTrack(getJob(), true)) {
				case Jobs::JobTracks::Magician:
					exploss = 7;
					break;
				case Jobs::JobTracks::Thief:
					exploss = 5;
					break;
			}
		}
		int32_t exp = getExp();
		exp -= static_cast<int32_t>(static_cast<int64_t>(getExp(getLevel())) * exploss / 100);
		setExp(exp);
	}
}

// Level Related Functions
void PlayerStats::giveExp(uint32_t exp, bool inChat, bool white) {
	int16_t fulljob = getJob();
	if (getLevel() >= getMaxLevel(fulljob)) // Do not give EXP to characters of max level or over
		return;
	uint32_t cexp = getExp() + exp;
	if (exp != 0)
		LevelsPacket::showEXP(player, exp, white, inChat);
	uint8_t level = getLevel();
	if (cexp >= getExp(level)) {
		bool cygnus = GameLogicUtilities::isCygnus(fulljob);
		uint8_t levelsgained = 0;
		uint8_t levelsmax = ChannelServer::Instance()->getMaxMultiLevel();
		int16_t apgain = 0;
		int16_t spgain = 0;
		int16_t hpgain = 0;
		int16_t mpgain = 0;
		int16_t job = GameLogicUtilities::getJobTrack(fulljob, true);
		int16_t intt = getInt(true) / 10;
		int16_t x = 0; // X value for Improving *P Increase skills, cached, only needs to be set once
		while (cexp >= getExp(level) && levelsgained < levelsmax) {
			cexp -= getExp(getLevel());
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
						x = getX(player->getSkills()->getHpIncrease());
					hpgain += levelHp(Stats::BaseHp::Warrior, x);
					mpgain += levelMp(Stats::BaseMp::Warrior, intt);
					break;
				case Jobs::JobTracks::Magician:
					if (levelsgained == 1 && player->getSkills()->hasMpIncrease())
						x = getX(player->getSkills()->getMpIncrease());
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
						x = getX(player->getSkills()->getHpIncrease());
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
			modifyMHp(hpgain);
			modifyMMp(mpgain);
			setLevel(level);
			setAp(getAp() + apgain);
			setSp(getSp() + spgain);
			// Let hyperbody remain on if on during a level up, as it should
			if (player->getActiveBuffs()->hasHyperBody()) {
				int32_t skillid = player->getActiveBuffs()->getHyperBody();
				uint8_t hblevel = player->getActiveBuffs()->getActiveSkillLevel(skillid);
				SkillLevelInfo *hb = SkillDataProvider::Instance()->getSkill(skillid, hblevel);
				setHyperBody(hb->x, hb->y);
			}

			setHp(getMHp());
			setMp(getMMp());
			player->setLevelDate();
			if (getLevel() == getMaxLevel(fulljob) && !player->isGm()) {
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
	setExp(cexp);
}

void PlayerStats::addStat(PacketReader &packet) {
	packet.skipBytes(4);
	int32_t type = packet.get<int32_t>();
	if (getAp() == 0) {
		// hacking
		return;
	}
	LevelsPacket::statOK(player);
	addStat(type);
}

void PlayerStats::addStatMulti(PacketReader &packet) {
	packet.skipBytes(4);
	uint32_t amount = packet.get<uint32_t>();

	LevelsPacket::statOK(player);

	for (uint32_t i = 0; i < amount; i++) {
		int32_t type = packet.get<int32_t>();
		int32_t value = packet.get<int32_t>();

		if (value < 0 || getAp() < value) {
			//hacking
			return;
		}

		addStat(type, static_cast<int16_t>(value)); // Prefer a single cast to countless casts/modification down the line
	}
}

void PlayerStats::addStat(int32_t type, int16_t mod, bool isreset) {
	int16_t maxstat = ChannelServer::Instance()->getMaxStats();
	bool issubtract = mod < 0;
	switch (type) {
		case Stats::Str:
			if (getStr() >= maxstat)
				return;
			setStr(getStr() + mod);
			break;
		case Stats::Dex:
			if (getDex() >= maxstat)
				return;
			setDex(getDex() + mod);
			break;
		case Stats::Int:
			if (getInt() >= maxstat)
				return;
			setInt(getInt() + mod);
			break;
		case Stats::Luk:
			if (getLuk() >= maxstat)
				return;
			setLuk(getLuk() + mod);
			break;
		case Stats::MaxHp:
		case Stats::MaxMp: {
			if (type == Stats::MaxHp && getMHp(true) >= Stats::MaxMaxHp)
				return;
			if (type == Stats::MaxMp && getMMp(true) >= Stats::MaxMaxMp)
				return;
			if (issubtract && getHpMpAp() == 0) {
				// Hacking
				return;
			}
			int16_t job = GameLogicUtilities::getJobTrack(getJob());
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
						y = getY(player->getSkills()->getHpIncrease());
					hpgain = apResetHp(isreset, issubtract, Stats::BaseHp::WarriorAp, y);
					mpgain = apResetMp(isreset, issubtract, Stats::BaseMp::WarriorAp);
					break;
				case Jobs::JobTracks::Magician:
					if (player->getSkills()->hasMpIncrease())
						y = getY(player->getSkills()->getMpIncrease());
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
						y = getY(player->getSkills()->getHpIncrease());
					hpgain = apResetHp(isreset, issubtract, Stats::BaseHp::PirateAp, y);
					mpgain = apResetMp(isreset, issubtract, Stats::BaseMp::PirateAp);
					break;
				default: // GM
					hpgain = apResetHp(isreset, issubtract, Stats::BaseHp::GmAp);
					mpgain = apResetMp(isreset, issubtract, Stats::BaseMp::GmAp);
					break;
			}
			setHpMpAp(getHpMpAp() + mod);
			switch (type) {
				case Stats::MaxHp: modifyMHp(hpgain); break;
				case Stats::MaxMp: modifyMMp(mpgain); break;
			}
			if (player->getActiveBuffs()->hasHyperBody()) {
				int32_t skillid = player->getActiveBuffs()->getHyperBody();
				uint8_t hblevel = player->getActiveBuffs()->getActiveSkillLevel(skillid);
				SkillLevelInfo *hb = SkillDataProvider::Instance()->getSkill(skillid, hblevel);
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
	if (!isreset)
		setAp(getAp() - mod);
}

int16_t PlayerStats::randHp() {
	return Randomizer::Instance()->randShort(Stats::BaseHp::Variation); // Max HP range per class (e.g. Beginner is 8-12)
}

int16_t PlayerStats::randMp() {
	return Randomizer::Instance()->randShort(Stats::BaseMp::Variation); // Max MP range per class (e.g. Beginner is 6-8)
}

int16_t PlayerStats::getX(int32_t skillid) {
	return player->getSkills()->getSkillInfo(skillid)->x;
}

int16_t PlayerStats::getY(int32_t skillid) {
	return player->getSkills()->getSkillInfo(skillid)->y;
}

int16_t PlayerStats::apResetHp(bool isreset, bool issubtract, int16_t val, int16_t sval) {
	return (isreset ? (issubtract ? -(sval + val + Stats::BaseHp::Variation) : val) : levelHp(val, sval));
}

int16_t PlayerStats::apResetMp(bool isreset, bool issubtract, int16_t val, int16_t sval) {
	return (isreset ? (issubtract ? -(sval + val + Stats::BaseMp::Variation) : val) : levelMp(val, sval));
}

int16_t PlayerStats::levelHp(int16_t val, int16_t bonus) {
	return randHp() + val + bonus;
}

int16_t PlayerStats::levelMp(int16_t val, int16_t bonus) {
	return randMp() + val + bonus;
}

uint32_t PlayerStats::getExp(uint8_t level) {
	return Levels::exps[level - 1];
}

uint8_t PlayerStats::getMaxLevel(int16_t jobid) {
	return (GameLogicUtilities::isCygnus(jobid) ? Stats::CygnusLevels : Stats::PlayerLevels);
}