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
#include "npc.hpp"
#include "common/algorithm.hpp"
#include "common/database.hpp"
#include "common/game_logic_utilities.hpp"
#include "common/initialize_common.hpp"
#include "common/string_utilities.hpp"
#include <iomanip>
#include <iostream>

namespace vana {
namespace data {
namespace provider {

auto npc::load_data() -> void {
	std::cout << std::setw(initializing::output_width) << std::left << "Initializing NPCs... ";

	auto &db = database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table("npc_data"));

	for (const auto &row : rs) {
		data::type::npc_info npc;
		game_npc_id id = row.get<game_npc_id>("npcid");
		npc.storage_cost = row.get<game_mesos>("storage_cost");

		utilities::str::run_flags(row.get<opt_string>("flags"), [&npc](const string &cmp) {
			if (cmp == "maple_tv") npc.is_maple_tv = true;
			else if (cmp == "is_guild_rank") npc.is_guild_rank = true;
		});

		m_data[id] = npc;
	}

	std::cout << "DONE" << std::endl;
}

auto npc::get_storage_cost(game_npc_id npc) const -> game_mesos {
	return m_data.find(npc)->second.storage_cost;
}

auto npc::is_maple_tv(game_npc_id npc) const -> bool {
	return m_data.find(npc)->second.is_maple_tv;
}

auto npc::is_guild_rank(game_npc_id npc) const -> bool {
	return m_data.find(npc)->second.is_guild_rank;
}

auto npc::is_valid_npc_id(game_npc_id npc) const -> bool {
	return ext::is_element(m_data, npc);
}

}
}
}