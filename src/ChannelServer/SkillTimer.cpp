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
#include "SkillTimer.h"
#include "Skills.h"

/* Skill Timer */
SkillTimer * SkillTimer::singleton = 0;

void SkillTimer::setSkillTimer(Player *player, int skill, int time) {
	STimer timer;
	timer.id = Timer::Instance()->setTimer(time, this);
	timer.player = player;
	timer.skill = skill;
	timer.time = time;
	timers.push_back(timer);
}

void SkillTimer::stop(Player *player, int skill) {
	for (size_t i = 0; i < timers.size(); i++) {
		if (player == timers[i].player && timers[i].skill == skill) {
			Timer::Instance()->cancelTimer(timers[i].id);
			break;
		}
	}
}

void SkillTimer::stop(Player *player, bool endSkills) {
	for (size_t i = 0; i < timers.size(); i++) {
		if (player == timers[i].player) {
			if (endSkills)
				Skills::endBuff(player, timers[i].skill, false);
			Timer::Instance()->cancelTimer(timers[i].id);
			i--;
		}
	}
}

int SkillTimer::skillTime(Player *player, int skillid) { // Get skill time
	int timeleft = 0;
	for (size_t i = 0; i < timers.size(); i++) {
		if (player == timers[i].player && timers[i].skill == skillid) {
			timeleft = Timer::Instance()->timeLeft(timers[i].id);
		}
	}
	return timeleft;
}

void SkillTimer::handle(Timer *timer, int id) {
	int skill;
	Player *player;
	for (size_t i = 0; i < timers.size(); i++) {
		if (timers[i].id == id) {
			player = timers[i].player;
			skill = timers[i].skill;
			break;
		}
	}
	Skills::endBuff(player, skill, false);
}

void SkillTimer::remove(int id) {
	for (size_t i = 0; i < timers.size(); i++) {
		if (timers[i].id == id) {	
			timers.erase(timers.begin()+i);	
			return;
		}
	}
}
