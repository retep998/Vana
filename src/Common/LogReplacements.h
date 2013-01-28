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

#include "noncopyable.hpp"
#include "Types.h"
#include <string>
#include <unordered_map>

using std::string;
using std::unordered_map;

namespace Replacements {
	enum Replacements : int32_t {
		Long = 0x01, // Long versions of strings or padded digits
		Uppercase = Long << 1, // Primarily for AM/PM
		String = Long << 2, // End of flags

		Start = Long << 8,
		Id = Start << 1,
		Message = Start << 2,
		Time = Start << 3,
		Event = Start << 4,
		Origin = Start << 5,
		Year = Start << 6,
		StringMonth = Start << 7,
		IntegerMonth = Start << 8,
		StringDate = Start << 9,
		IntegerDate = Start << 10,
		Hour = Start << 11,
		MilitaryHour = Start << 12,
		Minute = Start << 13,
		Second = Start << 14,
		AmPm = Start << 15,
		TimeZone = Start << 16
	};
	const uint32_t RemoveFlagMask = 0xFFFFFFF8; // Take off the flag bits
	const uint32_t GetFlagMask = ~RemoveFlagMask; // Get only the flag bits
}

// Prevents them from being loaded a bunch of times, keeps them in memory for the life of the application
class LogReplacements : public boost::noncopyable {
public:
	typedef unordered_map<string, int32_t> map_t;

	static LogReplacements * Instance() {
		if (singleton == nullptr)
			singleton = new LogReplacements;
		return singleton;
	}
	const map_t & getMap() const { return m_replacementMap; }
private:
	LogReplacements();
	static LogReplacements *singleton;

	void add(const string &key, int32_t val);
	map_t m_replacementMap;
};