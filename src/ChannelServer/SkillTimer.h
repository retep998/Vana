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
#ifndef SKILLTIMER_H
#define SKILLTIMER_H

#include <vector>
using std::vector;

#include "Timer.h"
class Player;

class SkillTimer : public Timer::TimerHandler {
public:
	static SkillTimer * Instance() {
		if (singleton == 0)
			singleton = new SkillTimer;
		return singleton;
	}
	void setSkillTimer(Player *player, int skill, int time);
	void setSkillTimer(Player *player, int skill, char *name, short value, int time);
	void stop (Player *player, int skill);
	void stop (Player *player, int skill, char *name);
	void stop (Player *player, bool endSkills);
	int skillTime(Player *player, int skillid);
private:
	static SkillTimer *singleton;
	SkillTimer() {};
	SkillTimer(const SkillTimer&);
	SkillTimer& operator=(const SkillTimer&);
	struct STimer {
		int id;
		Player *player;
		int skill;
		int time;
	};
	struct SActTimer {
		int id;
		Player *player;
		int skill;
		int time;
		char act[50];
		short value;
	};
	vector <STimer> timers;
	vector <SActTimer> acttimers;
	hash_map <int, bool> act;
	void handle (Timer *timer, int id);
	void remove (int id);
};

class CoolTimer : public Timer::TimerHandler {
public:
	static CoolTimer * Instance() {
		if (singleton == 0)
			singleton = new CoolTimer;
		return singleton;
	}
	void setCoolTimer(Player *player, int skill, int time);
	void stop (Player *player, int skill);
	void stop (Player *player);
	int coolTime(Player *player, int skillid);
private:
	static CoolTimer *singleton;
	CoolTimer() {};
	CoolTimer(const CoolTimer&);
	CoolTimer& operator=(const CoolTimer&);
	struct CTimer {
		int id;
		Player *player;
		int skill;
		int time;
	};
	vector <CTimer> timers;
	void handle (Timer *timer, int id);
	void remove (int id);
};

#endif
