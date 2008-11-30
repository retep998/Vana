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
#include "PlayerSummons.h"
#include "Summons.h"
#include "Timer/Time.h"
#include "Timer/Timer.h"
#include <functional>

using std::tr1::bind;

void PlayerSummons::addSummon(Summon *summon, int32_t time) {
	bool puppet = ISPUPPET(summon->getSummonID());
	if (!puppet)
		this->summon = summon;
	else
		this->puppet = summon;
	Timer::Id id(Timer::Types::BuffTimer, summon->getSummonID(), 0);
	clock_t summonExpire = time * 1000;
	new Timer::Timer(
		bind(&Summons::removeSummon, player, puppet, true, false, true, true),
		id, player->getTimers(), Timer::Time::fromNow(summonExpire));
}

void PlayerSummons::removeSummon(bool puppet, bool fromTimer) {
	if (!puppet && this->summon != 0) {
		if (!fromTimer) {
			Timer::Id id(Timer::Types::BuffTimer, summon->getSummonID(), 0);
			player->getTimers()->removeTimer(id);
		}
		delete this->summon;
		this->summon = 0;
	}
	else if (this->puppet != 0) {
		if (!fromTimer) {
			Timer::Id id(Timer::Types::BuffTimer, this->puppet->getSummonID(), 0);
			player->getTimers()->removeTimer(id);
		}
		delete this->puppet;
		this->puppet = 0;
	}
}

Summon * PlayerSummons::getSummon(int32_t summonid) {
	if (summon != 0 && summon->getID() == summonid)
		return summon;
	else if (puppet != 0 && puppet->getID() == summonid)
		return puppet;
	return 0;
}
