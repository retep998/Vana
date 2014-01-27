/*
Copyright (C) 2008-2014 Vana Development Team

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

#include "Types.hpp"
#include <chrono>
#include <ctime>
#include <string>

namespace TimeUtilities {
	auto getServerTime() -> int64_t;
	auto timeToTick(time_t time) -> int64_t;
	auto timeToTick32(time_t time) -> int32_t;
	auto tickToTick32(int64_t tick) -> int32_t;
	auto getDate(time_t ctime = time(nullptr)) -> int32_t;
	auto getMonth(time_t ctime = time(nullptr)) -> int32_t;
	auto getYear(bool twoDigit, time_t ctime = time(nullptr)) -> int32_t;
	auto getHour(bool nonMilitary, time_t ctime = time(nullptr)) -> int32_t;
	auto getMinute(time_t ctime = time(nullptr)) -> int32_t;
	auto getSecond(time_t ctime = time(nullptr)) -> int32_t;
	auto getDay(time_t ctime = time(nullptr)) -> int32_t;
	auto getWeek(time_t ctime = time(nullptr)) -> int32_t;
	auto getDayString(bool shortened, time_t ctime = time(nullptr)) -> string_t;
	auto getMonthString(bool shortened, time_t ctime = time(nullptr)) -> string_t;
	auto addDaysToTicks(int64_t ticks, int16_t days) -> int64_t;
	auto addDaysToTime(int16_t days) -> time_t;
	auto isDst(time_t ctime = time(nullptr)) -> bool;
	auto getTimeZone() -> string_t;
	auto getTimeZoneOffset() -> int32_t;
	auto simpleTimestamp() -> string_t;

	auto getNow() -> time_point_t;
	template <typename TDuration>
	auto getNowWithTimeAdded(const TDuration &timeUnit) -> time_point_t;
	auto getNearestMinuteMark(int32_t interval, const time_point_t &startPoint = getNow()) -> time_point_t;
	auto getNextOccurringSecondOfHour(uint16_t second, const time_point_t &startPoint = getNow()) -> time_point_t;
	template <typename TDuration>
	auto getDistance(const time_point_t &t1, const time_point_t &t2) -> typename TDuration::rep;
	auto getDistanceInSeconds(const time_point_t &t1, const time_point_t &t2) -> seconds_t;
	auto getDistanceToNextMinuteMark(int32_t interval, const time_point_t &startPoint = getNow()) -> seconds_t;
	auto getDistanceToNextOccurringSecondOfHour(uint16_t second, const time_point_t &startPoint = getNow()) -> seconds_t;
}

inline
auto TimeUtilities::getNow() -> time_point_t {
	return effective_clock_t::now();
}

template <typename TDuration>
inline
auto TimeUtilities::getNowWithTimeAdded(const TDuration &timeUnit) -> time_point_t {
	return effective_clock_t::now() + timeUnit;
}

inline
auto TimeUtilities::getNearestMinuteMark(int32_t interval, const time_point_t &startPoint) -> time_point_t {
	// Returns the closest interval minute mark in seconds
	time_t tmp = effective_clock_t::to_time_t(startPoint);
	tm *localTime = std::localtime(&tmp);

	int32_t nextTrigger = ((localTime->tm_min / interval) + 1) * interval;
	if (nextTrigger >= 60) {
		localTime->tm_hour += (nextTrigger / 60);
		nextTrigger %= 60;
	}
	localTime->tm_min = nextTrigger;
	localTime->tm_sec = 0;

	return effective_clock_t::from_time_t(std::mktime(localTime));
}

inline
auto TimeUtilities::getNextOccurringSecondOfHour(uint16_t second, const time_point_t &startPoint) -> time_point_t {
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

template <typename TDuration>
inline
auto TimeUtilities::getDistance(const time_point_t &t1, const time_point_t &t2) -> typename TDuration::rep {
	return duration_cast<TDuration>(t1 - t2).count();
}

inline
auto TimeUtilities::getDistanceInSeconds(const time_point_t &t1, const time_point_t &t2) -> seconds_t {
	return duration_cast<seconds_t>(t1 - t2);
}

inline
auto TimeUtilities::getDistanceToNextMinuteMark(int32_t interval, const time_point_t &startPoint) -> seconds_t {
	return getDistanceInSeconds(getNearestMinuteMark(interval), startPoint);
}

inline
auto TimeUtilities::getDistanceToNextOccurringSecondOfHour(uint16_t second, const time_point_t &startPoint) -> seconds_t {
	return getDistanceInSeconds(getNextOccurringSecondOfHour(second), startPoint);
}