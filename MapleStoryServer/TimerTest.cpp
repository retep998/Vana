#include "Timer.h"
#include "stdio.h"

long time;
/*
class TimerTest: public Timer::TimerHandler {
	void handle (Timer* timer, int id) {
		printf("received id: %d %d \n", id, GetTickCount() - time);
		timer->setTimer (2000, 3, this);

	}
	void remove(int id){}
};

void timermain () {
	Timer* timer = new Timer();
	TimerTest* handler = new TimerTest();
	time = GetTickCount();
	timer->setTimer (1000, 1, handler);
	timer->setTimer (10000, 2, handler);
	timer->setTimer (10000, 5, handler);
	timer->setTimer (10000, 6, handler);
	timer->setTimer (2000, 3, handler);
	printf("AB");
	Sleep(1000000000);
}
*/