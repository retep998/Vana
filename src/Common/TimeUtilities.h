/*
Copyright (C) 2008-2013 Vana Development Team

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
#include <chrono>
#include <ctime>
#include <string>

using std::string;

namespace TimeUtilities {
	int64_t getServerTime();
	int64_t timeToTick(time_t time);
	int32_t timeToTick32(time_t time);
	int32_t tickToTick32(int64_t tick);
	int32_t getDate(time_t ctime = time(nullptr));
	int32_t getMonth(time_t ctime = time(nullptr));
	int32_t getYear(bool twoDigit, time_t ctime = time(nullptr));
	int32_t getHour(bool nonMilitary, time_t ctime = time(nullptr));
	int32_t getMinute(time_t ctime = time(nullptr));
	int32_t getSecond(time_t ctime = time(nullptr));
	int32_t getDay(time_t ctime = time(nullptr));
	int32_t getWeek(time_t ctime = time(nullptr));
	string getDayString(bool shortened, time_t ctime = time(nullptr));
	string getMonthString(bool shortened, time_t ctime = time(nullptr));
	int64_t addDaysToTicks(int64_t ticks, int16_t days);
	time_t addDaysToTime(int16_t days);
	bool isDst(time_t ctime = time(nullptr));
	string getTimeZone();
	int32_t getTimeZoneOffset();

	time_point_t getNow();
	template <typename T>
	time_point_t getNowWithTimeAdded(const T &timeUnit);
	time_point_t getNearestMinuteMark(int32_t interval, const time_point_t &startPoint = getNow());
	time_point_t getNextOccurringSecondOfHour(uint16_t second, const time_point_t &startPoint = getNow());
	template <typename T>
	typename T::rep getDistance(const time_point_t &t1, const time_point_t &t2);
	seconds_t getDistanceInSeconds(const time_point_t &t1, const time_point_t &t2);
	seconds_t getDistanceToNextMinuteMark(int32_t interval, const time_point_t &startPoint = getNow());
	seconds_t getDistanceToNextOccurringSecondOfHour(uint16_t second, const time_point_t &startPoint = getNow());
}

inline
time_point_t TimeUtilities::getNow() {
	return effective_clock_t::now();
}

template <typename T>
inline
time_point_t TimeUtilities::getNowWithTimeAdded(const T &timeUnit) {
	return effective_clock_t::now() + timeUnit;
}

inline
time_point_t TimeUtilities::getNearestMinuteMark(int32_t interval, const time_point_t &startPoint) {
	// Returns the closest interval minute mark in seconds
	time_t tmp = effective_clock_t::to_time_t(startPoint);
	tm *localTime = std::localtime(&tmp);

	int32_t nextTrigger = ((localTime->tm_min / interval) + 1) * interval;
	if (nextTrigger >= 60) {
		localTime->tm_hour += (nextTrigger / 60);
		nextTrigger %= 60;
	}
	localTime->tm_min = nextTrigger;

	return effective_clock_t::from_time_t(std::mktime(localTime));
}

inline
time_point_t TimeUtilities::getNextOccurringSecondOfHour(uint16_t second, const time_point_t &startPoint) {
	time_t tmp = effective_clock_t::to_time_t(startPoint);
	tm *localTime = std::localtime(&tmp);
	uint16_t currentSeconds = localTime->tm_min * 60 + localTime->tm_sec;

	if (currentSeconds >= second) {
		// Already passed the time in this hour, get next hour
		localTime->tm_hour += 1;
	}
	localTime->tm_min = second / 60;
	localTime->tm_sec = second % 60;

	return effective_clock_t::from_time_t(std::mktime(localTime));
}

template <typename T>
inline
typename T::rep TimeUtilities::getDistance(const time_point_t &t1, const time_point_t &t2) {
	return std::chrono::duration_cast<T>(t1 - t2).count();
}

inline
seconds_t TimeUtilities::getDistanceInSeconds(const time_point_t &t1, const time_point_t &t2) {
	return std::chrono::duration_cast<seconds_t>(t1 - t2);
}

inline
seconds_t TimeUtilities::getDistanceToNextMinuteMark(int32_t interval, const time_point_t &startPoint) {
	return getDistanceInSeconds(getNearestMinuteMark(interval), startPoint);
}

inline
seconds_t TimeUtilities::getDistanceToNextOccurringSecondOfHour(uint16_t second, const time_point_t &startPoint) {
	return getDistanceInSeconds(getNextOccurringSecondOfHour(second), startPoint);
}