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
#include "Timer.h"
#include <Winbase.h>
#include <ctime>

Timer *Timer::singleton = 0;

void _timerThread(Timer *timerObject) {
	timerObject->timerThread();
}

Timer::Timer() {
	id = 0;
	terminate = false;

	HANDLE thread = CreateThread(NULL, 20000,
		(LPTHREAD_START_ROUTINE)_timerThread,
		(LPVOID)this,
		NULL,
		NULL);

	timerEvent = CreateEvent( 
            NULL,     // no security attributes
            FALSE,    // auto-reset event
            FALSE,    // initial state is not signaled
            NULL);    // object not named
}

Timer::~Timer() {
	terminate = true;
	SetEvent(timerEvent);
}

int Timer::setTimer(int msec, TimerHandler *handler, bool persistent) {
	timers[id] = new OneTimer(msec, id, handler, persistent);
	SetEvent(timerEvent);
	return id++;
}

void Timer::remove(int id) {
	getTimer(id)->handler->remove(id);
	timers.erase(id);
}

void Timer::cancelTimer(int id) {
	remove(id);
	SetEvent(timerEvent);
}

void Timer::resetTimer(int id) {
	getTimer(id)->reset();
	SetEvent(timerEvent);
}

int Timer::timeLeft(int id) { // check timer time
	return getTimer(id)->t - clock();
}

Timer::OneTimer * Timer::findMin() {
	if (timers.size() == 0)
		return NULL;
	OneTimer *min;
	for (hash_map <int, OneTimer *>::iterator iter = timers.begin(); iter != timers.end(); iter++) {
		if (iter == timers.begin() || iter->second->t < min->t)
			min = iter->second;
	}
	return min;
}

Timer::OneTimer * Timer::getTimer(int id) {
	if (timers[id])
		return timers[id];
	return NULL;
}

void Timer::timerThread() {
	while (!terminate) {
		// Find minimum wakeup time
		OneTimer *minTimer = findMin();
		long msec = (minTimer == NULL) ? msec = 1000000000 : minTimer->t - clock();
		if (msec <= 0) {
			minTimer->handler->handle(this, minTimer->id);
			if (minTimer->persistent) {
				minTimer->reset();
			}
			else {
				remove(minTimer->id);
				delete minTimer;
			}
			continue;
		}
		DWORD r = WaitForSingleObject(timerEvent, msec);
		if (r == WAIT_OBJECT_0) continue;
		if (r == WAIT_FAILED) {
			// TODO: write message
			return;
		}
		if (minTimer != NULL) {
			minTimer->handler->handle(this, minTimer->id);
			if (minTimer->persistent) {
				minTimer->reset();
			}
			else {
				remove(minTimer->id);
				delete minTimer;
			}
		}
	}
}

Timer::OneTimer::OneTimer(long msec, int id, TimerHandler* handler, bool persistent) {
	this->msec = msec;
	this->id = id;
	this->handler = handler;
	this->persistent = persistent;
	reset();
}

void Timer::OneTimer::reset() {
	this->t = msec + clock();
}
