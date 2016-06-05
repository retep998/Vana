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
#include "common/FileTime.hpp"
#include "common/UnixTime.hpp"

namespace vana {

file_time::file_time() :
	file_time{unix_time{}}
{
}

file_time::file_time(int64_t t) :
	m_value{t}
{
}

file_time::file_time(const unix_time &t) :
	file_time{convert(static_cast<time_t>(t))}
{
}

file_time::file_time(const file_time &t) :
	m_value{t.m_value}
{
}

auto file_time::get_value() const -> int64_t {
	return m_value;
}

auto file_time::convert(time_t time) -> int64_t {
	if (time == -1) {
		return -1;
	}

	std::tm *time_info;
	time_info = localtime(&time);
	if (time_info == nullptr) {
		// Couldn't parse the time, so return the given time
		return time;
	}

	int64_t ticks = 0;
	// Calculate leap days
	int32_t leap_days = 0;
	int32_t years = time_info->tm_year + 299;
	leap_days += (years / 100) * 24; // 24 more days for each 100 years
	leap_days += (years / 400); // And one more day for each 400 years
	leap_days += (years % 100) / 4; // And, of course, 1 day for every 4 years in the current century

	ticks += time_info->tm_sec * 1;
	ticks += time_info->tm_min * 60;
	ticks += time_info->tm_hour * 3600;
	ticks += (static_cast<int64_t>(time_info->tm_yday + leap_days)) * 86400;
	ticks += static_cast<int64_t>(years) * 86400 * 365; // Excluding leap years

	ticks *= 10000000; // Convert to 100-nanoseconds
	return ticks;
}

}