/*
Copyright (C) 2008-2015 Vana Development Team

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
#include "Algorithm.hpp"
#include "Database.hpp"
#include "InitializeCommon.hpp"
#include "StringUtilities.hpp"
#include <algorithm>
#include <iomanip>
#include <iostream>

auto CurseDataProvider::loadData() -> void {
	std::cout << std::setw(Initializing::OutputWidth) << std::left << "Initializing Curse Info... ";

	m_curseWords.clear();
	auto &db = Database::getDataDb();
	auto &sql = db.getSession();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.makeTable("curse_data"));

	for (const auto &row : rs) {
		m_curseWords.push_back(row.get<string_t>("word"));
	}

	std::cout << "DONE" << std::endl;
}

auto CurseDataProvider::isCurseWord(const string_t &cmp) const -> bool {
	string_t c = StringUtilities::removeSpaces(StringUtilities::toLower(cmp));
	return ext::any_of(m_curseWords, [&c](const string_t &s) -> bool {
		return c.find(s, 0) != string_t::npos;
	});
}