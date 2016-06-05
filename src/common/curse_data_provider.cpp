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
#include "curse_data_provider.hpp"
#include "common/algorithm_temp.hpp"
#include "common/database_temp.hpp"
#include "common/initialize_common.hpp"
#include "common/string_utilities.hpp"
#include <algorithm>
#include <iomanip>
#include <iostream>

namespace vana {

auto curse_data_provider::load_data() -> void {
	std::cout << std::setw(initializing::output_width) << std::left << "Initializing Curse Info...";

	m_curse_words.clear();
	auto &db = database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table("curse_data"));

	for (const auto &row : rs) {
		m_curse_words.push_back(row.get<string>("word"));
	}

	std::cout << "DONE" << std::endl;
}

auto curse_data_provider::is_curse_word(const string &cmp) const -> bool {
	string c = utilities::str::remove_spaces(utilities::str::to_lower(cmp));
	return ext::any_of(m_curse_words, [&c](const string &s) -> bool {
		return c.find(s, 0) != string::npos;
	});
}

}