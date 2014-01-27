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
#include "CurseDataProvider.hpp"
#include "Database.hpp"
#include "InitializeCommon.hpp"
#include "StringUtilities.hpp"
#include <algorithm>
#include <iomanip>
#include <iostream>

auto CurseDataProvider::loadData() -> void {
	std::cout << std::setw(Initializing::OutputWidth) << std::left << "Initializing Curse Info... ";

	m_curseWords.clear();
	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM curse_data");

	for (const auto &row : rs) {
		m_curseWords.push_back(row.get<string_t>("word"));
	}

	std::cout << "DONE" << std::endl;
}

auto CurseDataProvider::isCurseWord(const string_t &cmp) -> bool {
	string_t c = StringUtilities::removeSpaces(StringUtilities::toLower(cmp));
	bool curse = (std::end(m_curseWords) != std::find_if(std::begin(m_curseWords), std::end(m_curseWords),
		[&c](const string_t &s) -> bool {
			return c.find(s, 0) != string_t::npos;
		})
	);
	return curse;
}