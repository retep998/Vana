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
#include "TimeUtilities.h"

int64_t TimeUtilities::getServerTime() {
	return timeToTick(time(0));
}

int64_t TimeUtilities::timeToTick(time_t time) {
	if (time == -1)
		return -1;
	struct tm timeinfo;
	localtime_s(&timeinfo, &time);
	uint64_t ticks = 0;

	// Calculate leap days
	int32_t leapdays = 0;
	int32_t years = timeinfo.tm_year + 299;
	leapdays += (years/100)*24; // 24 more days for each 100 years
	leapdays += (years/400); // and one more day for each 400 years
	leapdays += ((years%100)/4); // and of course, 1 day for each 4 years in the current century

	ticks += (timeinfo.tm_sec * 1);
	ticks += (timeinfo.tm_min * 60);
	ticks += (timeinfo.tm_hour * 3600);
	ticks += (((int64_t) timeinfo.tm_yday + leapdays) * 86400);
	ticks += (int64_t) years * 86400 * 365; // Exluding leap years

	ticks *= 10000000; // Convert to 100-nanoseconds
	return ticks;
}

int32_t TimeUtilities::tickToTick32(int64_t tick) {
	int32_t tick32;
	if (tick == -1)
		tick32 = -1;
	else
		tick32 = (int32_t) (tick/4294967296 + 1); // Plus one to compensate for the loss of conversion
	return tick32;
}
