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
#include "CurseDataProvider.h"
#include "Database.h"
#include "InitializeCommon.h"
#include "StringUtilities.h"
#include <algorithm>
#include <iomanip>
#include <iostream>

using Initializing::OutputWidth;
using StringUtilities::runFlags;

CurseDataProvider * CurseDataProvider::singleton = nullptr;

void CurseDataProvider::loadData() {
	std::cout << std::setw(OutputWidth) << std::left << "Initializing Curse Info... ";

	m_curseWords.clear();
	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM curse_data");

	for (soci::rowset<>::const_iterator i = rs.begin(); i != rs.end(); ++i) {
		soci::row const &row = *i;

		m_curseWords.push_back(row.get<string>("word"));
	}

	std::cout << "DONE" << std::endl;
}

bool CurseDataProvider::isCurseWord(const string &cmp) {
	string c = StringUtilities::removeSpaces(StringUtilities::toLower(cmp));
	bool curse = (m_curseWords.end() != std::find_if(m_curseWords.begin(), m_curseWords.end(),
		[&c](const string &s) -> bool {
			return c.find(s, 0) != string::npos;
		})
	);
	return curse;
}