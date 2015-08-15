/*
Copyright (C) 2008-2015 Vana Development Team

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
#include "TimeUtilities.hpp"
#include <iomanip>
#include <sstream>

auto TimeUtilities::getDate(time_t ctime) -> int32_t {
	std::tm *timeInfo = localtime(&ctime);
	int32_t result = timeInfo->tm_mday;
	return result;
}

auto TimeUtilities::getMonth(time_t ctime) -> int32_t {
	std::tm *timeInfo = localtime(&ctime);
	int32_t result = timeInfo->tm_mon + 1;
	return result;
}

auto TimeUtilities::getYear(bool twoYear, time_t ctime) -> int32_t {
	std::tm *timeInfo = localtime(&ctime);
	int32_t result = timeInfo->tm_year + 1900;
	if (twoYear) {
		return result % 100;
	}
	return result;
}

auto TimeUtilities::getDay(time_t ctime) -> int32_t {
	std::tm *timeInfo = localtime(&ctime);
	int32_t result = timeInfo->tm_wday + 1;
	return result;
}

auto TimeUtilities::getDayString(bool shortened, time_t ctime) -> string_t {
	string_t result = "fail";
	switch (getDay(ctime)) {
		case 1: result = (shortened ? "Sun" : "Sunday"); break;
		case 2: result = (shortened ? "Mon" : "Monday"); break;
		case 3: result = (shortened ? "Tue" : "Tuesday"); break;
		case 4: result = (shortened ? "Wed" : "Wednesday"); break;
		case 5: result = (shortened ? "Thu" : "Thursday"); break;
		case 6: result = (shortened ? "Fri" : "Friday"); break;
		case 7: result = (shortened ? "Sat" : "Saturday"); break;
	}
	return result;
}

auto TimeUtilities::getMonthString(bool shortened, time_t ctime) -> string_t {
	string_t result = "fail";
	switch (getMonth(ctime)) {
		case 1: result = (shortened ? "Jan" : "January"); break;
		case 2: result = (shortened ? "Feb" : "February"); break;
		case 3: result = (shortened ? "Mar" : "March"); break;
		case 4: result = (shortened ? "Apr" : "April"); break;
		case 5: result = (shortened ? "May" : "May"); break;
		case 6: result = (shortened ? "Jun" : "June"); break;
		case 7: result = (shortened ? "Jul" : "July"); break;
		case 8: result = (shortened ? "Aug" : "August"); break;
		case 9: result = (shortened ? "Sep" : "September"); break;
		case 10: result = (shortened ? "Oct" : "October"); break;
		case 11: result = (shortened ? "Nov" : "November"); break;
		case 12: result = (shortened ? "Dec" : "December"); break;
	}
	return result;
}

auto TimeUtilities::getHour(bool nonMilitary, time_t ctime) -> int32_t {
	std::tm *timeInfo = localtime(&ctime);
	int32_t result = timeInfo->tm_hour;
	if (nonMilitary && result > 12) {
		result -= 12;
	}
	return result;
}

auto TimeUtilities::getMinute(time_t ctime) -> int32_t {
	std::tm *timeInfo = localtime(&ctime);
	int32_t result = timeInfo->tm_min;
	return result;
}

auto TimeUtilities::getSecond(time_t ctime) -> int32_t {
	std::tm *timeInfo = localtime(&ctime);
	int32_t result = timeInfo->tm_sec;
	return result;
}

auto TimeUtilities::getWeek(time_t ctime) -> int32_t {
	std::tm *timeInfo = localtime(&ctime);
	int32_t result = ((timeInfo->tm_yday + 1) + (timeInfo->tm_wday + (timeInfo->tm_yday % 7))) / 7; // Determine which day the year started on and start counting from the first full week
	return result;
}

auto TimeUtilities::isDst(time_t ctime) -> bool {
	std::tm *timeInfo = localtime(&ctime);
	return (timeInfo->tm_isdst > 0);
}

auto TimeUtilities::getTimeZone() -> string_t {
	int32_t offset = getTimeZoneOffset() / 60 / 60 * 100; // Offset in hours
	bool negative = false;
	if (offset < 0) {
		negative = true;
		offset *= -1;
	}

	out_stream_t t;
	if (negative) {
		t << "-";
	}
	else if (offset != 0) {
		t << "+";
	}
	t << std::setw(4) << std::setfill('0') << offset;
	return t.str();
}

auto TimeUtilities::getTimeZoneOffset() -> int32_t {
	time_t ctime = time(nullptr);

	std::tm *ts = localtime(&ctime);
	int32_t localTime = ts->tm_hour * 100 + ts->tm_min;

	ts = gmtime(&ctime);
	int32_t greenwichTime = ts->tm_hour * 100 + ts->tm_min;

	return ((localTime - greenwichTime) * 60 * 60 / 100); // Number of seconds as an offset
}

auto TimeUtilities::simpleTimestamp() -> string_t {
	char buffer[50];
	time_t ctime = time(nullptr);
	std::tm *timeInfo = std::localtime(&ctime);
	std::strftime(buffer, 50, "[%Y-%m-%d %H:%M:%S] ", timeInfo);
	string_t timestamp{buffer};
	return timestamp;
}

auto TimeUtilities::addDaysToTime(int16_t days) -> time_t {
	time_t now = time(nullptr);
	struct tm* tm = localtime(&now);
	tm->tm_mday += days;
	return mktime(tm);
}

auto TimeUtilities::addDaysToTicks(int64_t ticks, int16_t days) -> int64_t {
	// For expiration time increases
	return ticks + (days * 24 * 60 * 60);
}