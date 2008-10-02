/*
Copyright (C) 2008 Vana Development Team

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
#include "Skills.h"
#include "Buffs.h"
#include "Summons.h"
#include "Inventory.h"
#include "MapPacket.h"
#include "Maps.h"
#include "Player.h"
#include "Randomizer.h"
#include "ReadPacket.h"
#include "SkillsPacket.h"
#include "Timer/Timer.h"
#include <functional>

using std::tr1::bind;

unordered_map<int32_t, SkillsLevelInfo> Skills::skills;
unordered_map<int32_t, uint8_t> Skills::maxlevels;

void Skills::addSkillLevelInfo(int32_t skillid, uint8_t level, SkillLevelInfo levelinfo) {
	skills[skillid][level] = levelinfo;

	if (maxlevels.find(skillid) == maxlevels.end() || maxlevels[skillid] < level) {
		maxlevels[skillid] = level;
	}
}

void Skills::addSkill(Player *player, ReadPacket *packet) {
	packet->skipBytes(4);
	int32_t skillid = packet->getInt();
	if (!BEGINNER_SKILL(skillid) && player->getSp() == 0) {
		// hacking
		return;
	}
	if (player->getSkills()->addSkillLevel(skillid, 1) && !BEGINNER_SKILL(skillid)) {
		player->setSp(player->getSp() - 1);
	}
}

void Skills::cancelSkill(Player *player, ReadPacket *packet) {
	stopSkill(player, packet->getInt());
}
void Skills::stopSkill(Player *player, int32_t skillid, bool fromTimer) {
	switch (skillid) {
		case 3121004:
		case 3221001:
		case 2121001:
		case 2221001:
		case 2321001:
		case 5221004: // Special skills like hurricane, monster magnet, rapid fire, and etc
			SkillsPacket::endSpecialSkill(player, player->getSpecialSkillInfo());
			player->setSpecialSkill(SpecialSkillInfo());
			break;
		default:
			if (skillid == 9101004) // GM Hide
				MapPacket::showPlayer(player);
			player->getActiveBuffs()->removeBuff(skillid, fromTimer);
			Buffs::endBuff(player, skillid);
			break;
	}
}

void Skills::useSkill(Player *player, ReadPacket *packet) {
	packet->skipBytes(4); //Ticks
	int32_t skillid = packet->getInt();
	int16_t addedinfo = 0;
	uint8_t level = packet->getByte();
	uint8_t type = 0;
	switch (skillid) { // Packet processing
		case 1121001: // Monster Magnet processing
		case 1221001:
		case 1321001: {
			int32_t mobs = packet->getInt();
			for (int8_t k = 0; k < mobs; k++) {
				int32_t mapmobid = packet->getInt();
				uint8_t success = packet->getByte();
				SkillsPacket::showMagnetSuccess(player, mapmobid, success);
			}
			break;
		}
		case 2301002: { // Heal
			//TODO PARTY
			uint16_t healrate = skills[skillid][level].hpP / 1;
			if (healrate > 100)
				healrate = 100;
			player->modifyHP(healrate * player->getMHP() / 100);
			break;
		}
		case 9101000: // GM Heal + Dispel - needs to be modified for map?
			player->setHP(player->getMHP());
			player->setMP(player->getMMP());
			break;
		case 9101005: { // GM Resurrection
			for (size_t i = 0; i < Maps::maps[player->getMap()]->getNumPlayers(); i++) {
				Player *resplayer;
				resplayer = Maps::maps[player->getMap()]->getPlayer(i);
				if (resplayer->getHP() <= 0) {
					resplayer->setHP(resplayer->getMHP());
				}
			}
			break;
		}
		case 9101004: // GM Hide
			MapPacket::removePlayer(player);
			break;
		default:
			type = packet->getByte();
			switch (type) {
				case 0x80:
					addedinfo = packet->getShort();
					break;
			}
			break;
	}
	if (level == 0) {
		// hacking
		return;
	}
	Skills::applySkillCosts(player, skillid, level);
	SkillsPacket::showSkill(player, skillid, level);
	if (Buffs::isBuff(skillid)) 
		Buffs::addBuff(player, skillid, level, addedinfo);
	else if (ISSUMMON(skillid))
		Summons::useSummon(player, skillid, level);
}

void Skills::applySkillCosts(Player *player, int32_t skillid, uint8_t level, bool elementalamp) {
	int16_t cooltime = Skills::skills[skillid][level].cooltime;
	int16_t mpuse = skills[skillid][level].mp;
	int16_t hpuse = skills[skillid][level].hp;
	int16_t moneycon = skills[skillid][level].moneycon;
	int32_t item = skills[skillid][level].item;
	if (mpuse > 0) {
		if (player->getActiveBuffs()->getActiveSkillLevel(3121008) > 0) { // Reduced MP usage for Concentration
			uint16_t mprate = Skills::skills[3121008][player->getActiveBuffs()->getActiveSkillLevel(3121008)].x;
			int16_t mploss = (mpuse * mprate) / 100;
			player->modifyMP(-mploss, true);
		}
		else {
			if (elementalamp) {
				int32_t sid = ((player->getJob() / 10) == 22 ? 2210001 : 2110001);
				int8_t slv = player->getSkills()->getSkillLevel(sid);
				if (slv > 0)
					player->modifyMP(-1 * (mpuse * skills[sid][slv].x / 100), true);
				else
					player->modifyMP(-mpuse, true);
			}
			else
				player->modifyMP(-mpuse, true);
		}
	}
	else
		player->setMP(player->getMP(), true);
	if (hpuse > 0)
		player->modifyHP(-hpuse);
	if (item > 0)
		Inventory::takeItem(player, item, skills[skillid][level].itemcount);
	if (cooltime > 0)
		Skills::startCooldown(player, skillid, cooltime);
	if (moneycon > 0) {
		int16_t mesos_min = moneycon - (80 + level * 5);
		int16_t mesos_max = moneycon + (80 + level * 5);
		int16_t difference = mesos_max - mesos_min; // Randomize up to this, add minimum for range
		int16_t amount = Randomizer::Instance()->randShort(difference) + mesos_min;
		int32_t mesos = player->getInventory()->getMesos();
		if (mesos - amount > -1) 
			player->getInventory()->modifyMesos(-amount);
		else {
			// Hacking
			return;
		}
	}
}

void Skills::useAttackSkill(Player *player, int32_t skillid) {
	uint8_t level = player->getSkills()->getSkillLevel(skillid);
	if (skills.find(skillid) == skills.end() || level == 0)
		return;
	Skills::applySkillCosts(player, skillid, level, true);
}

void Skills::useAttackSkillRanged(Player *player, int32_t skillid, int16_t pos, uint8_t display) {
	uint8_t level = 0;
	if (skillid != 0) {
		level = player->getSkills()->getSkillLevel(skillid);
		if (skills.find(skillid) == skills.end() || level == 0)
			return;
		Skills::applySkillCosts(player, skillid, level);
	}
	uint16_t hits = 1;
	if (skills[skillid][level].bulletcon > 0)
		hits = skills[skillid][level].bulletcon;
	if (display == 0x08)
		hits = hits * 2;
	if (pos > 0 && (!((display & 0x40) == 0x40 || display == 0x02)))
		// Display is 0x40 for Shadow Claw and 0x48 for Shadow Claw + Shadow Partner
		// Bitwise and with 0x40 will make it 0x40 for both
		Inventory::takeItemSlot(player, 2, pos, hits);
}

void Skills::heal(Player *player, int16_t value, int32_t skillid) {
	if (player->getHP() < player->getMHP() && player->getHP() > 0) {
		player->modifyHP(value);
		SkillsPacket::healHP(player, value);
	}
}

void Skills::hurt(Player *player, int16_t value, int32_t skillid) {
	if (player->getHP() - value > 1) {
		player->modifyHP(-value);
		SkillsPacket::showSkillEffect(player, skillid);
	}
	else {
		Buffs::endBuff(player, skillid);
	}
}

void Skills::startCooldown(Player *player, int32_t skillid, int16_t cooltime) {
	if (Skills::isCooling(player, skillid)) {
		// Hacking
		return;
	}
	SkillsPacket::sendCooldown(player, skillid, cooltime);

	new Timer::Timer(bind(&Skills::stopCooldown, player,
		skillid), Timer::Id(Timer::Types::CoolTimer,
		skillid, 0), player->getTimers(), cooltime * 1000, false);
}

void Skills::stopCooldown(Player *player, int32_t skillid) {
	SkillsPacket::sendCooldown(player, skillid, 0);	
}

bool Skills::isCooling(Player *player, int32_t skillid) {
	Timer::Id id(Timer::Types::CoolTimer, skillid, 0);
	return player->getTimers()->checkTimer(id) > 0;
}

