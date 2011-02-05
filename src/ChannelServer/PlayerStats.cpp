/*
Copyright (C) 2008-2011 Vana Development Team

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
#include "PacketCreator.h"
#include "PacketReader.h"
#include "Party.h"
#include "Player.h"
#include "PlayerNpcDataProvider.h"
#include "PlayerPacket.h"
#include "PlayersPacket.h"
#include "Randomizer.h"
#include "SkillDataProvider.h"
#include "Summons.h"
#include "SyncPacket.h"
#include "VersionConstants.h"
#include <boost/lexical_cast.hpp>
#include <string>

using std::string;

PlayerStats::PlayerStats(Player *player, uint8_t level, int16_t job, int16_t fame, int16_t str, int16_t dex, int16_t intt, int16_t luk, int16_t ap, uint16_t hpmp_ap, int16_t sp, int16_t hp, int16_t mhp, int16_t mp, int16_t mmp, int32_t exp) : 
player(player), 
level(level), 
job(job), 
fame(fame), 
str(str), 
dex(dex), 
intt(intt), 
luk(luk), 
watt(0),
wdef(0),
matt(intt),
mdef(intt),
avo((luk / 2) + (dex / 4)),
jump(100),
speed(100),
ap(ap), 
hpmp_ap(hpmp_ap), 
sp(sp), 
hp(hp), 
mhp(mhp), 
mp(mp), 
mmp(mmp), 
exp(exp), 
hbx(0), 
hby(0), 
mw(0) 
{
	if (this->hp == 0)
		this->hp = 50;
}

// Equip stat bonus handling
void PlayerStats::updateBonuses(bool updateEquips, bool isLoading) {
	if (mw > 0)
		setMapleWarrior(mw);

	if (updateEquips) {
		equipBonuses = BonusSet();
		for (EquipBonuses::iterator iter = equipStats.begin(); iter != equipStats.end(); iter++) {
			if (EquipDataProvider::Instance()->canEquip(iter->second.Id, getJob(), getStr(true), getDex(true), getInt(true), getLuk(true), getFame())) {
				equipBonuses.Hp += iter->second.Hp;
				equipBonuses.Mp += iter->second.Mp;
				equipBonuses.Str += iter->second.Str;
				equipBonuses.Dex += iter->second.Dex;
				equipBonuses.Int += iter->second.Int;
				equipBonuses.Luk += iter->second.Luk;
				equipBonuses.Watt += iter->second.Watt;
				equipBonuses.Wdef += iter->second.Wdef;
				equipBonuses.Matt += iter->second.Matt;
				equipBonuses.Mdef += iter->second.Mdef;
				equipBonuses.Acc += iter->second.Acc;
				equipBonuses.Avo += iter->second.Avo;
				equipBonuses.Speed += iter->second.Speed;
				equipBonuses.Jump += iter->second.Jump;
				equipBonuses.Hands += iter->second.Hands;
			}
		}
	}

	if (hbx > 0 && hby > 0)
		setHyperBody(hbx, hby);

	if (!isLoading) { // Adjust current HP/MP down if necessary
		if (getHp() > getMaxHp()) {
			setHp(getHp());
		}
		if (getMp() > getMaxMp()) {
			setMp(getMp());
		}
	}

	calculateOtherStats();
}

void PlayerStats::setEquip(int16_t slot, Item *equip, bool isLoading) {
	slot = abs(slot);
	if (equip != nullptr) {
		equipStats[slot].Id = equip->getId();
		equipStats[slot].Hp = equip->getHp();
		equipStats[slot].Mp = equip->getMp();
		equipStats[slot].Str = equip->getStr();
		equipStats[slot].Dex = equip->getDex();
		equipStats[slot].Int = equip->getInt();
		equipStats[slot].Luk = equip->getLuk();
		equipStats[slot].Acc = equip->getAccuracy();
		equipStats[slot].Avo = equip->getAvoid();
		equipStats[slot].Speed = equip->getSpeed();
		equipStats[slot].Hands = equip->getHands();
		equipStats[slot].Jump = equip->getJump();
		equipStats[slot].Watt = equip->getWatk();
		equipStats[slot].Wdef = equip->getWdef();
		equipStats[slot].Matt = equip->getMatk();
		equipStats[slot].Mdef = equip->getMdef();
	}
	else {
		equipStats.erase(slot);
	}
	updateBonuses(true, isLoading);
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
	packet.add<int16_t>(getMaxHp(true));
	packet.add<int16_t>(getMp());
	packet.add<int16_t>(getMaxMp(true));
	packet.add<int16_t>(getAp());
	packet.add<int16_t>(getSp());
	packet.add<int32_t>(getExp());
	packet.add<int16_t>(getFame());
}

int16_t PlayerStats::getMaxHp(bool withoutbonus) {
	if (!withoutbonus) {
		if ((mhp + equipBonuses.Hp + buffBonuses.Hp) > Stats::MaxMaxHp)
			return Stats::MaxMaxHp;
		return (mhp + equipBonuses.Hp + buffBonuses.Hp);
	}
	return mhp;
}

int16_t PlayerStats::getMaxMp(bool withoutbonus) {
	if (!withoutbonus) {
		if ((mmp + equipBonuses.Mp + buffBonuses.Mp) > Stats::MaxMaxMp)
			return Stats::MaxMaxMp;
		return (mmp + equipBonuses.Mp + buffBonuses.Mp);
	}
	return mmp;
}

int16_t PlayerStats::getStr(bool withbonus) {
	if (withbonus)
		return ((str + buffBonuses.Str + equipBonuses.Str) > SHRT_MAX ? SHRT_MAX : (str + buffBonuses.Str + equipBonuses.Str));

	return str;
}

int16_t PlayerStats::getDex(bool withbonus) {
	if (withbonus)
		return ((dex + buffBonuses.Dex + equipBonuses.Dex) > SHRT_MAX ? SHRT_MAX : (dex + buffBonuses.Dex + equipBonuses.Dex));

	return dex;
}

int16_t PlayerStats::getInt(bool withbonus) {
	if (withbonus)
		return ((intt + buffBonuses.Int + equipBonuses.Int) > SHRT_MAX ? SHRT_MAX : (intt + buffBonuses.Int + equipBonuses.Int));

	return intt;
}

int16_t PlayerStats::getLuk(bool withbonus) {
	if (withbonus)
		return ((luk + buffBonuses.Luk + equipBonuses.Luk) > SHRT_MAX ? SHRT_MAX : (luk + buffBonuses.Luk + equipBonuses.Luk));

	return luk;
}

int16_t PlayerStats::getWeaponAttack(bool withbonus) {
	if (withbonus)
		return ((watt + buffBonuses.Watt + equipBonuses.Watt) > SHRT_MAX ? SHRT_MAX : (watt + buffBonuses.Watt + equipBonuses.Watt));

	return watt;
}

int16_t PlayerStats::getWeaponDefence(bool withbonus) {
	if (withbonus)
		return ((wdef + buffBonuses.Wdef + equipBonuses.Wdef) > SHRT_MAX ? SHRT_MAX : (wdef + buffBonuses.Wdef + equipBonuses.Wdef));

	return wdef;
}

int16_t PlayerStats::getMagicAttack(bool withbonus) {
	if (withbonus)
		return ((matt + buffBonuses.Matt + equipBonuses.Matt) > SHRT_MAX ? SHRT_MAX : (matt + buffBonuses.Matt + equipBonuses.Matt));

	return matt;
}

int16_t PlayerStats::getMagicDefence(bool withbonus) {
	if (withbonus)
		return ((mdef + buffBonuses.Mdef + equipBonuses.Mdef) > SHRT_MAX ? SHRT_MAX : (mdef + buffBonuses.Mdef + equipBonuses.Mdef));

	return mdef;
}

int16_t PlayerStats::getAccuracy(bool withbonus) {
	if (withbonus)
		return ((acc + buffBonuses.Acc + equipBonuses.Acc) > SHRT_MAX ? SHRT_MAX : (acc + buffBonuses.Acc + equipBonuses.Acc));

	return acc;
}

int16_t PlayerStats::getAvoidability(bool withbonus) {
	if (withbonus)
		return ((avo + buffBonuses.Avo + equipBonuses.Avo) > SHRT_MAX ? SHRT_MAX : (avo + buffBonuses.Avo + equipBonuses.Avo));

	return avo;
}

int16_t PlayerStats::getHands(bool withbonus) {
	if (withbonus)
		return ((hands + buffBonuses.Hands + equipBonuses.Hands) > SHRT_MAX ? SHRT_MAX : (hands + buffBonuses.Hands + equipBonuses.Hands));

	return hands;
}

int16_t PlayerStats::getSpeed(bool withbonus) {
	if (withbonus)
		return ((speed + buffBonuses.Speed + equipBonuses.Speed) > SHRT_MAX ? SHRT_MAX : (speed + buffBonuses.Speed + equipBonuses.Speed));

	return speed;
}

int16_t PlayerStats::getJump(bool withbonus) {
	if (withbonus)
		return ((jump + buffBonuses.Jump + equipBonuses.Jump) > SHRT_MAX ? SHRT_MAX : (jump + buffBonuses.Jump + equipBonuses.Jump));

	return jump;
}

// Data modification
void PlayerStats::checkHpMp() {
	if (this->hp > getMaxHp()) {
		this->hp = getMaxHp();
	}
	if (this->mp > getMaxMp()) {
		this->mp = getMaxMp();
	}
}

void PlayerStats::setLevel(uint8_t level) {
	this->level = level;
	PlayerPacket::updateStatShort(player, Stats::Level, level);
	LevelsPacket::levelUp(player);
	player->saveAll();
	SyncPacket::updateLevel(ChannelServer::Instance()->getWorldConnection(), player->getId(), level);
}

void PlayerStats::setHp(int16_t shp, bool is) {
	if (shp < 0) {
		hp = 0;
	}
	else if (shp > getMaxHp()) {
		hp = getMaxHp();
	}
	else {
		hp = shp;
	}
	if (is) {
		PlayerPacket::updateStatShort(player, Stats::Hp, hp);
	}
	modifiedHp();
}

void PlayerStats::modifyHp(int16_t nhp, bool is) {
	if ((hp + nhp) < 0) {
		hp = 0;
	}
	else if ((hp + nhp) > getMaxHp()) {
		hp = getMaxHp();
	}
	else {
		hp = (hp + nhp);
	}
	if (is) {
		PlayerPacket::updateStatShort(player, Stats::Hp, hp);
	}
	modifiedHp();
}

void PlayerStats::damageHp(uint16_t dhp) {
	hp = (dhp > hp ? 0 : hp - dhp);
	PlayerPacket::updateStatShort(player, Stats::Hp, hp);
	modifiedHp();
}

void PlayerStats::modifiedHp() {
	if (Party *p = player->getParty()) {
		p->showHpBar(player);
	}
	player->getActiveBuffs()->checkBerserk();
	if (hp == 0) {
		if (Instance *i = player->getInstance()) {
			i->sendMessage(PlayerDeath, player->getId());
		}
		loseExp();
		Summons::removeSummon(player, false, false, SummonMessages::Disappearing);
	}
}

void PlayerStats::setMp(int16_t smp, bool is) {
	if (!player->getActiveBuffs()->hasInfinity()) {
		if (smp < 0) {
			mp = 0;
		}
		else if (smp > getMaxMp()) {
			mp = getMaxMp();
		}
		else {
			mp = smp;
		}
	}
	PlayerPacket::updateStatShort(player, Stats::Mp, mp, is);
}

void PlayerStats::modifyMp(int16_t nmp, bool is) {
	if (!player->getActiveBuffs()->hasInfinity()) {
		if ((mp + nmp) < 0) {
			mp = 0;
		}
		else if ((mp + nmp) > getMaxMp()) {
			mp = getMaxMp();
		}
		else {
			mp = (mp + nmp);
		}
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
	SyncPacket::updateJob(ChannelServer::Instance()->getWorldConnection(), player->getId(), job);
}

void PlayerStats::setStr(int16_t str) {
	this->str = str;
	PlayerPacket::updateStatShort(player, Stats::Str, str);
}

void PlayerStats::setDex(int16_t dex) {
	this->dex = dex;
	calculateOtherStats();
	PlayerPacket::updateStatShort(player, Stats::Dex, dex);
}

void PlayerStats::setInt(int16_t intt) {
	this->intt = intt;
	calculateOtherStats();
	PlayerPacket::updateStatShort(player, Stats::Int, intt);
}

void PlayerStats::setLuk(int16_t luk) {
	this->luk = luk;
	calculateOtherStats();
	PlayerPacket::updateStatShort(player, Stats::Luk, luk);
}

void PlayerStats::setMapleWarrior(int16_t modx) {
	buffBonuses.Str = (str * modx) / 100;
	buffBonuses.Dex = (dex * modx) / 100;
	buffBonuses.Int = (intt * modx) / 100;
	buffBonuses.Luk = (luk * modx) / 100;
	if (mw != modx) {
		mw = modx;
		updateBonuses();
	}
}

void PlayerStats::setMaxHp(int16_t mhp) {
	if (mhp > Stats::MaxMaxHp)
		mhp = Stats::MaxMaxHp;
	else if (mhp < Stats::MinMaxHp)
		mhp = Stats::MinMaxHp;
	this->mhp = mhp;
	PlayerPacket::updateStatShort(player, Stats::MaxHp, mhp);
	modifiedHp();
}

void PlayerStats::setMaxMp(int16_t mmp) {
	if (mmp > Stats::MaxMaxMp)
		mmp = Stats::MaxMaxMp;
	else if (mmp < Stats::MinMaxMp)
		mmp = Stats::MinMaxMp;
	this->mmp = mmp;
	PlayerPacket::updateStatShort(player, Stats::MaxMp, mmp);
}

void PlayerStats::setHyperBody(int16_t modx, int16_t mody) {
	hbx = modx;
	hby = mody;
	buffBonuses.Hp = (((mhp + equipBonuses.Hp) * modx / 100) > Stats::MaxMaxHp ? Stats::MaxMaxHp : (mhp + equipBonuses.Hp) * modx / 100);
	buffBonuses.Mp = (((mmp + equipBonuses.Mp) * mody / 100) > Stats::MaxMaxMp ? Stats::MaxMaxMp : (mmp + equipBonuses.Mp) * mody / 100);
	PlayerPacket::updateStatShort(player, Stats::MaxHp, mhp);
	PlayerPacket::updateStatShort(player, Stats::MaxMp, mmp);
	if (player->getParty())
		player->getParty()->showHpBar(player);
	player->getActiveBuffs()->checkBerserk();
}

void PlayerStats::modifyMaxHp(int16_t mod) {
	mhp = (((mhp + mod) > Stats::MaxMaxHp) ? Stats::MaxMaxHp : (mhp + mod));
	PlayerPacket::updateStatShort(player, Stats::MaxHp, mhp);
}

void PlayerStats::modifyMaxMp(int16_t mod) {
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
		if (loc->loseOnePercent()) {
			exploss = 1;
		}
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

void PlayerStats::calculateOtherStats() {
	avo = (getLuk(true) / 2) + (getDex(true) / 4);
	matt = getInt(true);
	mdef = getInt(true);

	int16_t jTrack = GameLogicUtilities::getJobTrack(job, true);
	int16_t accMod1 = 0;
	int16_t accMod2 = 0;
	if (jTrack == Jobs::JobTracks::Bowman || jTrack == Jobs::JobTracks::Thief) {
		accMod1 = static_cast<int16_t>(getLuk(true) * 0.3);
		accMod2 = static_cast<int16_t>(getDex(true) * 0.6);
	}
	else {
		accMod1 = static_cast<int16_t>(getLuk(true) * 0.5);
		accMod2 = static_cast<int16_t>(getDex(true) * 0.8);
	}

	acc = accMod1 + accMod2;
	hands = getDex(true) + getLuk(true) + getInt(true);
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
			modifyMaxHp(hpgain);
			modifyMaxMp(mpgain);
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

			setHp(getMaxHp());
			setMp(getMaxMp());
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
				PlayerPacket::showMessageWorld(message, PlayerPacket::NoticeTypes::Blue);

				// Make a player NPC
				PlayerNpcDataProvider::Instance()->makePlayerNpc(player);
			}
		}
	}
	setExp(cexp);
}

void PlayerStats::addStat(PacketReader &packet) {
	if (!player->updateTickCount(packet.get<int32_t>())) {
		// Tickcount was the same or less than 100 of the difference.
		return;
	}
	int32_t type = packet.get<int32_t>();
	if (getAp() == 0) {
		// Hacking
		return;
	}
	LevelsPacket::statOK(player);
	addStat(type);
}

void PlayerStats::addStatMulti(PacketReader &packet) {
	if (!player->updateTickCount(packet.get<int32_t>())) {
		// Tickcount was the same or less than 100 of the difference.
		return;
	}
	uint32_t amount = packet.get<uint32_t>();

	LevelsPacket::statOK(player);

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
			if (type == Stats::MaxHp && getMaxHp(true) >= Stats::MaxMaxHp)
				return;
			if (type == Stats::MaxMp && getMaxMp(true) >= Stats::MaxMaxMp)
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
				case Stats::MaxHp: modifyMaxHp(hpgain); break;
				case Stats::MaxMp: modifyMaxMp(mpgain); break;
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
	updateBonuses();
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