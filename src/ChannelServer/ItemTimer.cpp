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
#include "ItemTimer.h"
#include "Inventory.h"

vector <ItemTimer::ITimer> ItemTimer::timers;
ItemTimer * ItemTimer::singleton = 0;

void ItemTimer::setItemTimer(Player *player, int item, int time) {
	ITimer timer;
	timer.id = Timer::Instance()->setTimer(time, this);
	timer.player = player;
	timer.item = item;
	timer.time = time;
	timers.push_back(timer);
}

void ItemTimer::stop(Player *player, int item) {
	for (size_t i = 0; i < timers.size(); i++) {
		if (player == timers[i].player && timers[i].item == item) {
			Timer::Instance()->cancelTimer(timers[i].id);
			break;
		}
	}
}

void ItemTimer::stop(Player *player) {
	for (size_t i = 0; i < timers.size(); i++) {
		if (player == timers[i].player) {
			Timer::Instance()->cancelTimer(timers[i].id);
			i--;
		}
	}
}

void ItemTimer::handle(Timer *timer, int id) {
	int item;
	Player *player;
	for (size_t i = 0; i < timers.size(); i++) {
		if (timers[i].id == id) {
			player = timers[i].player;
			item = timers[i].item;
			break;
		}
	}
	Inventory::endItem(player, item);
}

void ItemTimer::remove(int id) {
	for (size_t i = 0; i < timers.size(); i++) {
		if (timers[i].id == id) {	
			timers.erase(timers.begin()+i);
			return;
		}
	}
}
