/*
Copyright (C) 2008-2011 Vana Development Team

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
#include "TimeUtilities.h"

int64_t TimeUtilities::getServerTime() {
	return timeToTick(time(0));
}

int64_t TimeUtilities::timeToTick(time_t time) {
	if (time == -1)
		return -1;
	struct tm *timeinfo;
	timeinfo = localtime(&time);
	uint64_t ticks = 0;

	// Calculate leap days
	int32_t leapdays = 0;
	int32_t years = timeinfo->tm_year + 299;
	leapdays += (years/100)*24; // 24 more days for each 100 years
	leapdays += (years/400); // and one more day for each 400 years
	leapdays += ((years%100)/4); // and of course, 1 day for each 4 years in the current century

	ticks += (timeinfo->tm_sec * 1);
	ticks += (timeinfo->tm_min * 60);
	ticks += (timeinfo->tm_hour * 3600);
	ticks += (((int64_t) timeinfo->tm_yday + leapdays) * 86400);
	ticks += (int64_t) years * 86400 * 365; // Exluding leap years

	ticks *= 10000000; // Convert to 100-nanoseconds
	return ticks;
}

int32_t TimeUtilities::tickToTick32(int64_t tick) {
	int32_t tick32;
	if (tick == -1)
		tick32 = -1;
	else
		tick32 = (int32_t) (tick / 4294967296LL + 1); // Plus one to compensate for the loss of conversion
	return tick32;
}

int32_t TimeUtilities::getDate(time_t ctime) {
	tm *timeinfo = localtime(&ctime);
	int32_t result = timeinfo->tm_mday;
	return result;
}

int32_t TimeUtilities::getMonth(time_t ctime) {
	tm *timeinfo = localtime(&ctime);
	int32_t result = timeinfo->tm_mon + 1;
	return result;
}

int32_t TimeUtilities::getYear(time_t ctime) {
	tm *timeinfo = localtime(&ctime);
	int32_t result = timeinfo->tm_year + 1900;
	return result;
}

int32_t TimeUtilities::getDay(time_t ctime) {
	tm *timeinfo = localtime(&ctime);
	int32_t result = timeinfo->tm_wday + 1;
	return result;
}

string TimeUtilities::getDayString(time_t ctime) {
	string result = "fail";
	switch (getDay(ctime)) {
		case 1: result = "Sunday"; break;
		case 2: result = "Monday"; break;
		case 3: result = "Tuesday"; break;
		case 4: result = "Wednesday"; break;
		case 5: result = "Thursday"; break;
		case 6: result = "Friday"; break;
		case 7: result = "Saturday"; break;
	}
	return result;
}

int32_t TimeUtilities::getHour(time_t ctime) {
	tm *timeinfo = localtime(&ctime);
	int32_t result = timeinfo->tm_hour;
	return result;
}

int32_t TimeUtilities::getMinute(time_t ctime) {
	tm *timeinfo = localtime(&ctime);
	int32_t result = timeinfo->tm_min;
	return result;
}

int32_t TimeUtilities::getSecond(time_t ctime) {
	tm *timeinfo = localtime(&ctime);
	int32_t result = timeinfo->tm_sec;
	return result;
}

int32_t TimeUtilities::getWeek(time_t ctime) {
	tm *timeinfo = localtime(&ctime);
	int32_t result = ((timeinfo->tm_yday + 1) + (timeinfo->tm_wday + (timeinfo->tm_yday % 7))) / 7; // Determine which day the year started on and start counting from the first full week
	return result;
}

int32_t TimeUtilities::getNearestMinuteMark(int32_t interval, time_t ctime) {
	// Returns the closest interval minute mark in seconds
	tm *timeinfo = localtime(&ctime);
	int32_t result = (((timeinfo->tm_min / interval) + 1) * interval * 60);
	return result;
}

bool TimeUtilities::getDST(time_t ctime) {
	tm *timeinfo = localtime(&ctime);
	return (timeinfo->tm_isdst > 0);
}

int32_t TimeUtilities::getTimeZoneOffset() {
	time_t ctime = time(0);
	tm *ts = localtime(&ctime);
	int32_t hour = ts->tm_hour; // C/C++ have extremely unsightly time handling, beware
	ts = gmtime(&ctime);
	return ((hour - ts->tm_hour) * 60 * 60);
}

#ifdef WIN32
# include <windows.h>
uint32_t TimeUtilities::getTickCount() {
	return GetTickCount();
}
#else
# include <sys/times.h>
# include <unistd.h>
uint32_t TimeUtilities::getTickCount() {
	tms tm;

	// Note:
	// CLOCKS_PER_SEC is always 1,000,000
	// CLK_TCK is the real vlaue (deprecated)
	// sysconf(_SC_CLK_TCK) is the current way of getting CLK_TCK
	uint32_t clkTck = sysconf(_SC_CLK_TCK);

	if (clkTck < 1000)
		return times(&tm) * (1000 / clkTck);
	else
		return times(&tm) * (clkTck / 1000);
}
#endif