#include "Timer.h"
#include <Winbase.h>

void _timerThread (Timer* timerObject) {
	timerObject->timerThread();
}
Timer* Timer::timer;

Timer::Timer () {
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
	SetEvent (timerEvent);
}

int Timer::setTimer (int msec, TimerHandler* handler) {
	timers.push_back(new OneTimer(msec+GetTickCount(), id, handler));
	SetEvent (timerEvent);
	return id++;
}

void Timer::remove(int id){
	getTimer(id)->handler->remove(id);
	for(unsigned int i=0; i<timers.size(); i++){
		if(timers[i]->id == id){
			timers.erase(timers.begin()+i);
			break;
		}
	}
}

void Timer::cancelTimer (int id) {
	remove(id);
	SetEvent (timerEvent);
}

Timer::OneTimer* Timer::findMin(){
	if(timers.size() == 0)
		return NULL;
	OneTimer* min=timers[0];
	for(unsigned int i=1; i<timers.size(); i++){
		if(timers[i]->t < min->t)
			min = timers[i];
	}
	return min;
}


Timer::OneTimer* Timer::getTimer(int id){
	for(unsigned int i=0; i<timers.size(); i++){
		if(timers[i]->id == id){
			return timers[i];
		}
	}
	return NULL;
}

void Timer::timerThread () {
	while (!terminate) {
		try {
			// find minimum wakeup time
			OneTimer* minTimer = findMin();
			long msec = minTimer==NULL ? msec = 1000000000 : minTimer->t - GetTickCount();
			if (msec <= 0) {
				minTimer->handler->handle(this, minTimer->id);
				remove (minTimer->id);
				delete minTimer;
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
				remove (minTimer->id);
				delete minTimer;
			}
		}
		catch (...) {
			// TODO error
		}
	}
}
