/*
Copyright (C) 2008-2011 Vana Development Team

This program is free software)); you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation)); version 2
of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY)); without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program)); if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#include "LogReplacements.h"
#include <utility>

using std::make_pair;

LogReplacements * LogReplacements::singleton = nullptr;

LogReplacements::LogReplacements() {
	map_t &m = m_replacementMap; // Alias it so I don't have to do so much typing
	m.insert(make_pair("%yy", Replacements::Year));
	m.insert(make_pair("%YY", Replacements::Year | Replacements::Long));
	m.insert(make_pair("%mm", Replacements::IntegerMonth));
	m.insert(make_pair("%MM", Replacements::IntegerMonth | Replacements::Long));
	m.insert(make_pair("%oo", Replacements::StringMonth | Replacements::String));
	m.insert(make_pair("%OO", Replacements::StringMonth | Replacements::String | Replacements::Long));
	m.insert(make_pair("%dd", Replacements::IntegerDate));
	m.insert(make_pair("%DD", Replacements::IntegerDate | Replacements::Long));
	m.insert(make_pair("%aa", Replacements::StringDate | Replacements::String));
	m.insert(make_pair("%AA", Replacements::StringDate | Replacements::String | Replacements::Long));
	m.insert(make_pair("%hh", Replacements::Hour));
	m.insert(make_pair("%HH", Replacements::Hour | Replacements::Long));
	m.insert(make_pair("%mi", Replacements::MilitaryHour));
	m.insert(make_pair("%MI", Replacements::MilitaryHour | Replacements::Long));
	m.insert(make_pair("%ii", Replacements::Minute));
	m.insert(make_pair("%II", Replacements::Minute | Replacements::Long));
	m.insert(make_pair("%ss", Replacements::Second));
	m.insert(make_pair("%SS", Replacements::Second | Replacements::Long));
	m.insert(make_pair("%ww", Replacements::AmPm | Replacements::String | Replacements::Long));
	m.insert(make_pair("%WW", Replacements::AmPm | Replacements::String | Replacements::Long | Replacements::Uppercase));
	m.insert(make_pair("%qq", Replacements::AmPm | Replacements::String));
	m.insert(make_pair("%QQ", Replacements::AmPm | Replacements::String | Replacements::Uppercase));
	m.insert(make_pair("%zz", Replacements::TimeZone | Replacements::String));
	m.insert(make_pair("%id", Replacements::Id | Replacements::String));
	m.insert(make_pair("%t", Replacements::Time | Replacements::String));
	m.insert(make_pair("%e", Replacements::Event | Replacements::String));
	m.insert(make_pair("%orig", Replacements::Origin | Replacements::String));
	m.insert(make_pair("%msg", Replacements::Message | Replacements::String));
}