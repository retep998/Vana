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
#ifndef TIMER_H
#define TIMER_H

#include <queue>
#include <hash_map>
#include "windows.h"

using namespace std;
using namespace stdext;

class Timer {
public:
	static Timer * Instance() {
		if (singleton == 0)
			singleton = new Timer;
		return singleton;
	}
	class TimerHandler {
	public:
		virtual void handle(Timer *timer, int id) = 0;
		virtual void remove(int id) = 0;
	};

	~Timer();

	int setTimer(int msec, TimerHandler *handler);
	void cancelTimer(int id);
	void resetTimer(int id);
	void timerThread();
	int timeLeft(int id); // check timer time
private:
	Timer();
	Timer(const Timer&);
	Timer& operator=(const Timer&);
	static Timer *singleton;

	bool terminate;
	class OneTimer {
	public:
		long t;
		long msec;
		int id;
		TimerHandler* handler;
		OneTimer(long msec, int id, TimerHandler* handler);
		void reset();
	};
	hash_map <int, OneTimer *> timers;
	HANDLE timerEvent;
	OneTimer * findMin();
	OneTimer * getTimer(int id);
	void remove(int id);
	int id;
};

#endif
