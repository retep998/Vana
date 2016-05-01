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
#include "Common/FileTime.hpp"
#include "Common/UnixTime.hpp"

namespace Vana {

FileTime::FileTime() :
	FileTime{UnixTime{}}
{
}

FileTime::FileTime(int64_t t) :
	m_value{t}
{
}

FileTime::FileTime(const UnixTime &t) :
	FileTime{convert(static_cast<time_t>(t))}
{
}

FileTime::FileTime(const FileTime &t) :
	m_value{t.m_value}
{
}

auto FileTime::getValue() const -> int64_t {
	return m_value;
}

auto FileTime::convert(time_t time) -> int64_t {
	if (time == -1) {
		return -1;
	}

	std::tm *timeInfo;
	timeInfo = localtime(&time);
	if (timeInfo == nullptr) {
		// Couldn't parse the time, so return the given time
		return time;
	}

	int64_t ticks = 0;
	// Calculate leap days
	int32_t leapDays = 0;
	int32_t years = timeInfo->tm_year + 299;
	leapDays += (years / 100) * 24; // 24 more days for each 100 years
	leapDays += (years / 400); // and one more day for each 400 years
	leapDays += (years % 100) / 4; // and of course, 1 day for each 4 years in the current century

	ticks += timeInfo->tm_sec * 1;
	ticks += timeInfo->tm_min * 60;
	ticks += timeInfo->tm_hour * 3600;
	ticks += (static_cast<int64_t>(timeInfo->tm_yday + leapDays)) * 86400;
	ticks += static_cast<int64_t>(years) * 86400 * 365; // Excluding leap years

	ticks *= 10000000; // Convert to 100-nanoseconds
	return ticks;
}

}