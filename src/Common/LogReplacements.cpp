/*
Copyright (C) 2008-2012 Vana Development Team

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
#include "LogReplacements.h"
#include <utility>

LogReplacements * LogReplacements::singleton = nullptr;

LogReplacements::LogReplacements()
{
	add("%yy", Replacements::Year);
	add("%YY", Replacements::Year | Replacements::Long);
	add("%mm", Replacements::IntegerMonth);
	add("%MM", Replacements::IntegerMonth | Replacements::Long);
	add("%oo", Replacements::StringMonth | Replacements::String);
	add("%OO", Replacements::StringMonth | Replacements::String | Replacements::Long);
	add("%dd", Replacements::IntegerDate);
	add("%DD", Replacements::IntegerDate | Replacements::Long);
	add("%aa", Replacements::StringDate | Replacements::String);
	add("%AA", Replacements::StringDate | Replacements::String | Replacements::Long);
	add("%hh", Replacements::Hour);
	add("%HH", Replacements::Hour | Replacements::Long);
	add("%mi", Replacements::MilitaryHour);
	add("%MI", Replacements::MilitaryHour | Replacements::Long);
	add("%ii", Replacements::Minute);
	add("%II", Replacements::Minute | Replacements::Long);
	add("%ss", Replacements::Second);
	add("%SS", Replacements::Second | Replacements::Long);
	add("%ww", Replacements::AmPm | Replacements::String | Replacements::Long);
	add("%WW", Replacements::AmPm | Replacements::String | Replacements::Long | Replacements::Uppercase);
	add("%qq", Replacements::AmPm | Replacements::String);
	add("%QQ", Replacements::AmPm | Replacements::String | Replacements::Uppercase);
	add("%zz", Replacements::TimeZone | Replacements::String);
	add("%id", Replacements::Id | Replacements::String);
	add("%t", Replacements::Time | Replacements::String);
	add("%e", Replacements::Event | Replacements::String);
	add("%orig", Replacements::Origin | Replacements::String);
	add("%msg", Replacements::Message | Replacements::String);
}

void LogReplacements::add(const string &key, int32_t val) {
	m_replacementMap.insert(std::make_pair(key, val));
}