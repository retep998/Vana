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
#ifndef ITEMTIMER_H
#define ITEMTIMER_H

#include <vector>
using std::vector;

#include "Timer.h"
class Player;

class ItemTimer: public Timer::TimerHandler {
public:
	static ItemTimer * Instance() {
		if (singleton == 0)
			singleton = new ItemTimer;
		return singleton;
	}
	void setItemTimer(Player *player, int item, int time);
	void stop(Player *player, int item);
	void stop(Player *player);
private:
	static ItemTimer *singleton;
	ItemTimer() {};
	ItemTimer(const ItemTimer&);
	ItemTimer& operator=(const ItemTimer&);

	struct ITimer {
		int id;
		Player *player;
		int item;
		int time;
	};
	static vector <ITimer> timers;
	void handle(Timer *timer, int id);
	void remove (int id);
};

#endif
