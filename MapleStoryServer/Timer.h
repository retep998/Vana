#ifndef TIMER_H
#define TIMER_H

#include <queue>
#include <hash_map>
#include "windows.h"

using namespace std;
using namespace stdext;

class Timer {
public:
	static Timer* timer;
	class TimerHandler {
	public:
		virtual void handle (Timer* timer, int id) = 0;
		virtual void remove (int id) = 0;
	};

	Timer ();
	~Timer();

	int setTimer 
		(int msec, TimerHandler* handler);

	void cancelTimer (int id);

	void timerThread();

private:
	bool terminate;
	class OneTimer {
	public:
		long t;
		int id;
		TimerHandler* handler;
		OneTimer (long t, int id, TimerHandler* handler):t(t),id(id),handler(handler){}
	};
	vector<OneTimer*> timers;
	HANDLE timerEvent;
	OneTimer* findMin();
	OneTimer* getTimer(int id);
	void remove(int id);
	int id;
};

#endif