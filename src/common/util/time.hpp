/*
Copyright (C) 2008-2016 Vana Development Team

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

#include "common/types.hpp"
#include <chrono>
#include <ctime>
#include <string>

namespace vana {
	namespace util {
		namespace time {
			auto get_date(time_t ctime = std::time(nullptr)) -> int32_t;
			auto get_month(time_t ctime = std::time(nullptr)) -> int32_t;
			auto get_year(bool two_digit, time_t ctime = std::time(nullptr)) -> int32_t;
			auto get_hour(bool non_military, time_t ctime = std::time(nullptr)) -> int32_t;
			auto get_minute(time_t ctime = std::time(nullptr)) -> int32_t;
			auto get_second(time_t ctime = std::time(nullptr)) -> int32_t;
			auto get_day(time_t ctime = std::time(nullptr)) -> int32_t;
			auto get_week(time_t ctime = std::time(nullptr)) -> int32_t;
			auto get_day_string(bool shortened, time_t ctime = std::time(nullptr)) -> string;
			auto get_month_string(bool shortened, time_t ctime = std::time(nullptr)) -> string;
			auto add_days_to_ticks(int64_t ticks, int16_t days) -> int64_t;
			auto add_days_to_time(int16_t days) -> time_t;
			auto is_dst(time_t ctime = std::time(nullptr)) -> bool;
			auto get_time_zone() -> string;
			auto get_time_zone_offset() -> int32_t;
			auto simple_timestamp() -> string;

			auto get_now() -> time_point;
			template <typename TDuration>
			auto get_now_with_time_added(const TDuration &time_unit) -> time_point;
			auto get_nearest_minute_mark(int32_t interval, const time_point &start_point = get_now()) -> time_point;
			auto get_next_occurring_second_of_hour(uint16_t second, const time_point &start_point = get_now()) -> time_point;
			template <typename TDuration>
			auto get_distance(const time_point &t1, const time_point &t2) -> typename TDuration::rep;
			auto get_distance_in_seconds(const time_point &t1, const time_point &t2) -> seconds;
			auto get_distance_to_next_minute_mark(int32_t interval, const time_point &start_point = get_now()) -> seconds;
			auto get_distance_to_next_occurring_second_of_hour(uint16_t second, const time_point &start_point = get_now()) -> seconds;
		}
	}

	inline
	auto util::time::get_now() -> time_point {
		return effective_clock::now();
	}

	template <typename TDuration>
	inline
	auto util::time::get_now_with_time_added(const TDuration &time_unit) -> time_point {
		return effective_clock::now() + time_unit;
	}

	inline
	auto util::time::get_nearest_minute_mark(int32_t interval, const time_point &start_point) -> time_point {
		// Returns the closest interval minute mark in seconds
		time_t tmp = effective_clock::to_time_t(start_point);
		tm *local_time = std::localtime(&tmp);

		int32_t next_trigger = ((local_time->tm_min / interval) + 1) * interval;
		if (next_trigger >= 60) {
			local_time->tm_hour += (next_trigger / 60);
			next_trigger %= 60;
		}
		local_time->tm_min = next_trigger;
		local_time->tm_sec = 0;

		return effective_clock::from_time_t(std::mktime(local_time));
	}

	inline
	auto util::time::get_next_occurring_second_of_hour(uint16_t second, const time_point &start_point) -> time_point {
		time_t tmp = effective_clock::to_time_t(start_point);
		tm *local_time = std::localtime(&tmp);
		uint16_t current_seconds = local_time->tm_min * 60 + local_time->tm_sec;

		if (current_seconds >= second) {
			// Already passed the time in this hour, get next hour
			local_time->tm_hour += 1;
		}
		local_time->tm_min = second / 60;
		local_time->tm_sec = second % 60;

		return effective_clock::from_time_t(std::mktime(local_time));
	}

	template <typename TDuration>
	inline
	auto util::time::get_distance(const time_point &t1, const time_point &t2) -> typename TDuration::rep {
		return duration_cast<TDuration>(t1 - t2).count();
	}

	inline
	auto util::time::get_distance_in_seconds(const time_point &t1, const time_point &t2) -> seconds {
		return duration_cast<seconds>(t1 - t2);
	}

	inline
	auto util::time::get_distance_to_next_minute_mark(int32_t interval, const time_point &start_point) -> seconds {
		return get_distance_in_seconds(get_nearest_minute_mark(interval), start_point);
	}

	inline
	auto util::time::get_distance_to_next_occurring_second_of_hour(uint16_t second, const time_point &start_point) -> seconds {
		return get_distance_in_seconds(get_next_occurring_second_of_hour(second), start_point);
	}
}