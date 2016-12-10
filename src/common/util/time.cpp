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
#include "time.hpp"
#include <iomanip>
#include <sstream>

namespace vana {
namespace util {
namespace time {

auto get_date(time_t ctime) -> int32_t {
	std::tm *time_info = localtime(&ctime);
	int32_t result = time_info->tm_mday;
	return result;
}

auto get_month(time_t ctime) -> int32_t {
	std::tm *time_info = localtime(&ctime);
	int32_t result = time_info->tm_mon + 1;
	return result;
}

auto get_year(bool two_digit, time_t ctime) -> int32_t {
	std::tm *time_info = localtime(&ctime);
	int32_t result = time_info->tm_year + 1900;
	if (two_digit) {
		return result % 100;
	}
	return result;
}

auto get_day(time_t ctime) -> int32_t {
	std::tm *time_info = localtime(&ctime);
	int32_t result = time_info->tm_wday + 1;
	return result;
}

auto get_day_string(bool shortened, time_t ctime) -> string {
	string result = "fail";
	switch (get_day(ctime)) {
		case 1: result = (shortened ? "Sun" : "Sunday"); break;
		case 2: result = (shortened ? "Mon" : "Monday"); break;
		case 3: result = (shortened ? "Tue" : "Tuesday"); break;
		case 4: result = (shortened ? "Wed" : "Wednesday"); break;
		case 5: result = (shortened ? "Thu" : "Thursday"); break;
		case 6: result = (shortened ? "Fri" : "Friday"); break;
		case 7: result = (shortened ? "Sat" : "Saturday"); break;
		default: THROW_CODE_EXCEPTION(not_implemented_exception, "day");
	}
	return result;
}

auto get_month_string(bool shortened, time_t ctime) -> string {
	string result = "fail";
	switch (get_month(ctime)) {
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
		default: THROW_CODE_EXCEPTION(not_implemented_exception, "month");
	}
	return result;
}

auto get_hour(bool non_military, time_t ctime) -> int32_t {
	std::tm *time_info = localtime(&ctime);
	int32_t result = time_info->tm_hour;
	if (non_military && result > 12) {
		result -= 12;
	}
	return result;
}

auto get_minute(time_t ctime) -> int32_t {
	std::tm *time_info = localtime(&ctime);
	int32_t result = time_info->tm_min;
	return result;
}

auto get_second(time_t ctime) -> int32_t {
	std::tm *time_info = localtime(&ctime);
	int32_t result = time_info->tm_sec;
	return result;
}

auto get_week(time_t ctime) -> int32_t {
	std::tm *time_info = localtime(&ctime);
	int32_t result = ((time_info->tm_yday + 1) + (time_info->tm_wday + (time_info->tm_yday % 7))) / 7; // Determine which day the year started on and start counting from the first full week
	return result;
}

auto is_dst(time_t ctime) -> bool {
	std::tm *time_info = localtime(&ctime);
	return (time_info->tm_isdst > 0);
}

auto get_time_zone() -> string {
	int32_t offset = get_time_zone_offset() / 60 / 60 * 100; // Offset in hours
	bool negative = false;
	if (offset < 0) {
		negative = true;
		offset *= -1;
	}

	out_stream t;
	if (negative) {
		t << "-";
	}
	else if (offset != 0) {
		t << "+";
	}
	t << std::setw(4) << std::setfill('0') << offset;
	return t.str();
}

auto get_time_zone_offset() -> int32_t {
	time_t ctime = std::time(nullptr);

	std::tm *ts = localtime(&ctime);
	int32_t local_time = ts->tm_hour * 100 + ts->tm_min;

	ts = gmtime(&ctime);
	int32_t greenwich_time = ts->tm_hour * 100 + ts->tm_min;

	return ((local_time - greenwich_time) * 60 * 60 / 100); // Number of seconds as an offset
}

auto simple_timestamp() -> string {
	char buffer[50];
	time_t ctime = std::time(nullptr);
	std::tm *time_info = std::localtime(&ctime);
	std::strftime(buffer, 50, "[%Y-%m-%d %H:%M:%S] ", time_info);
	string timestamp{buffer};
	return timestamp;
}

auto add_days_to_time(int16_t days) -> time_t {
	time_t now = std::time(nullptr);
	struct tm* tm = localtime(&now);
	tm->tm_mday += days;
	return mktime(tm);
}

auto add_days_to_ticks(int64_t ticks, int16_t days) -> int64_t {
	// For expiration time increases
	return ticks + (days * 24 * 60 * 60);
}

}
}
}