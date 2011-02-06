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
#include "Skills.h"
#include "Buffs.h"
#include "GameConstants.h"
#include "GameLogicUtilities.h"
#include "Inventory.h"
#include "PacketReader.h"
#include "Player.h"
#include "PlayerDataProvider.h"
#include "Randomizer.h"
#include "SkillDataProvider.h"
#include "Summons.h"
#include "Timer/Time.h"
#include "Timer/Timer.h"
#include <functional>

using std::tr1::bind;

void Skills::addSkill(Player *player, PacketReader &packet) {
	packet.skipBytes(4);
	int32_t skillid = packet.get<int32_t>();
	if (!GameLogicUtilities::isBeginnerSkill(skillid)) {
		if (player->getStats()->getSp() == 0) {
			// Hacking
			return;
		}
		if (!player->isGm() && !GameLogicUtilities::skillMatchesJob(skillid, player->getStats()->getJob())) {
			// Hacking
			return;
		}
	}
	if (player->getSkills()->addSkillLevel(skillid, 1) && !GameLogicUtilities::isBeginnerSkill(skillid)) {
		player->getStats()->setSp(player->getStats()->getSp() - 1);
	}
}

void Skills::cancelSkill(Player *player, PacketReader &packet) {
	stopSkill(player, packet.get<int32_t>());
}

void Skills::stopSkill(Player *player, int32_t skillid, bool fromTimer) {
	switch (skillid) {
		case Jobs::Bowmaster::Hurricane:
		case Jobs::WindArcher::Hurricane:
		case Jobs::Marksman::PiercingArrow:
		case Jobs::FPArchMage::BigBang:
		case Jobs::ILArchMage::BigBang:
		case Jobs::Bishop::BigBang:
		case Jobs::Corsair::RapidFire: // Special skills
			player->setSpecialSkill(SpecialSkillInfo());
			break;
		default:
			if (player->getActiveBuffs()->getActiveSkillLevel(skillid) == 0) {
				// Hacking
				return;
			}
			player->getActiveBuffs()->removeBuff(skillid, fromTimer);
			if (GameLogicUtilities::isMobSkill(skillid))
				Buffs::endDebuff(player, (uint8_t)(skillid));
			else
				Buffs::endBuff(player, skillid);
			break;
	}
}

void Skills::useSkill(Player *player, PacketReader &packet) {

}

void Skills::applySkillCosts(Player *player, int32_t skillid, uint8_t level, bool elementalamp) {
	SkillLevelInfo *skill = SkillDataProvider::Instance()->getSkill(skillid, level);
	int16_t cooltime = skill->cooltime;
	int16_t mpuse = skill->mp;
	int16_t hpuse = skill->hp;
	int16_t moneycon = skill->moneyConsume;
	int32_t item = skill->item;
	if (mpuse > 0) {
		if (SkillLevelInfo *conc = player->getActiveBuffs()->getActiveSkillInfo(Jobs::Bowmaster::Concentrate)) { // Reduced MP usage for Concentration
			int16_t mprate = conc->x;
			int16_t mploss = (mpuse * mprate) / 100;
			player->getStats()->modifyMp(-mploss, true);
		}
		else if (elementalamp && player->getSkills()->hasElementalAmp()) {
			player->getStats()->modifyMp(-1 * (mpuse * player->getSkills()->getSkillInfo(player->getSkills()->getElementalAmp())->x / 100), true);
		}
		else {
			player->getStats()->modifyMp(-mpuse, true);
		}
	}
	else
		player->getStats()->setMp(player->getStats()->getMp(), true);
	if (hpuse > 0)
		player->getStats()->modifyHp(-hpuse);
	if (item > 0)
		Inventory::takeItem(player, item, skill->itemCount);
	if (cooltime > 0 && skillid != Jobs::Corsair::Battleship)
		startCooldown(player, skillid, cooltime);
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
	if (skillid != Jobs::All::RegularAttack) {
		uint8_t level = player->getSkills()->getSkillLevel(skillid);
		if (!SkillDataProvider::Instance()->isSkill(skillid) || level == 0)
			return;
		applySkillCosts(player, skillid, level, true);
	}
}

void Skills::useAttackSkillRanged(Player *player, int32_t skillid, int16_t pos) {
	uint8_t level = 0;
	if (skillid != Jobs::All::RegularAttack) {
		level = player->getSkills()->getSkillLevel(skillid);
		if (!SkillDataProvider::Instance()->isSkill(skillid) || level == 0)
			return;
		applySkillCosts(player, skillid, level);
	}
	uint16_t hits = 1;
	if (skillid != Jobs::All::RegularAttack) {
		uint16_t bullets = SkillDataProvider::Instance()->getSkill(skillid, level)->bulletConsume;
		if (bullets > 0) {
			hits = bullets;
		}
	}
	if (player->getActiveBuffs()->hasShadowPartner()) {
		hits *= 2;
	}
	if (pos > 0 && !(player->getActiveBuffs()->hasShadowStars() || player->getActiveBuffs()->hasSoulArrow())) {
		// If they don't have Shadow Stars or Soul Arrow, take the items
		Inventory::takeItemSlot(player, Inventories::UseInventory, pos, hits);
	}
}

void Skills::heal(Player *player, int16_t value, int32_t skillid) {
	if (player->getStats()->getHp() < player->getStats()->getMaxHp() && player->getStats()->getHp() > 0) {
		player->getStats()->modifyHp(value);
	}
}

void Skills::hurt(Player *player, int16_t value, int32_t skillid) {
	if (player->getStats()->getHp() - value > 1) {
		player->getStats()->modifyHp(-value);
	}
	else {
		Buffs::endBuff(player, skillid);
	}
}

void Skills::startCooldown(Player *player, int32_t skillid, int16_t cooltime, bool initialload) {
	if (isCooling(player, skillid)) {
		// Hacking
		return;
	}
	if (!initialload) {
		player->getSkills()->addCooldown(skillid, cooltime);
	}
	new Timer::Timer(bind(&Skills::stopCooldown, player, skillid),
		Timer::Id(Timer::Types::CoolTimer, skillid, 0),
		player->getTimers(), Timer::Time::fromNow(cooltime * 1000));
}

void Skills::stopCooldown(Player *player, int32_t skillid) {
	player->getSkills()->removeCooldown(skillid);
	if (skillid == Jobs::Corsair::Battleship) {
		player->getActiveBuffs()->resetBattleshipHp();
	}

	Timer::Id id(Timer::Types::CoolTimer, skillid, 0);
	if (player->getTimers()->checkTimer(id) > 0) {
		player->getTimers()->removeTimer(id);
	}
}

bool Skills::isCooling(Player *player, int32_t skillid) {
	Timer::Id id(Timer::Types::CoolTimer, skillid, 0);
	return player->getTimers()->checkTimer(id) > 0;
}

int16_t Skills::getCooldownTimeLeft(Player *player, int32_t skillid) {
	int16_t cooltime = 0;
	if (isCooling(player, skillid)) {
		Timer::Id id(Timer::Types::CoolTimer, skillid, 0);
		cooltime = static_cast<int16_t>(player->getTimers()->checkTimer(id));
	}
	return cooltime;
}
