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
#pragma once

#include "Types.h"
#include <ctime>
#include <string>

using std::string;

namespace TimeUtilities {
	int64_t getServerTime();
	int64_t timeToTick(time_t time);
	int32_t tickToTick32(int64_t tick);
	int32_t getDate(time_t ctime = time(0));
	int32_t getMonth(time_t ctime = time(0));
	int32_t getYear(bool twoDigit, time_t ctime = time(0));
	int32_t getHour(bool nonMilitary, time_t ctime = time(0));
	int32_t getMinute(time_t ctime = time(0));
	int32_t getSecond(time_t ctime = time(0));
	int32_t getDay(time_t ctime = time(0));
	int32_t getWeek(time_t ctime = time(0));
	int32_t getNearestMinuteMark(int32_t interval, time_t ctime = time(0));
	string getDayString(bool shortened, time_t ctime = time(0));
	string getMonthString(bool shortened, time_t ctime = time(0));
	int64_t addDaysToTicks(int64_t ticks, int16_t days);
	time_t addDaysToTime(int16_t days);
	bool isDst(time_t ctime = time(0));
	string getTimeZone();
	int32_t getTimeZoneOffset();
	uint32_t getTickCount(); // The relative value can be used like GetTickCount on windows
}
