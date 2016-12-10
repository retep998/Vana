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
#include "common/data/initialize.hpp"
#include "common/io/database.hpp"
#include "common/util/string.hpp"
#include <iomanip>
#include <iostream>

namespace vana {
namespace data {
namespace provider {

auto npc::load_data() -> void {
	std::cout << std::setw(vana::data::initialize::output_width) << std::left << "Initializing NPCs... ";

	auto &db = vana::io::database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table(vana::data::table::npc_data));

	for (const auto &row : rs) {
		data::type::npc_info info;
		info.id = row.get<game_npc_id>("npcid");
		info.storage_cost = row.get<game_mesos>("storage_cost");

		vana::util::str::run_flags(row.get<opt_string>("flags"), [&info](const string &cmp) {
			if (cmp == "maple_tv") info.is_maple_tv = true;
			else if (cmp == "is_guild_rank") info.is_guild_rank = true;
		});

		m_data.push_back(info);
	}

	std::cout << "DONE" << std::endl;
}

auto npc::get_storage_cost(game_npc_id npc) const -> game_mesos {
	for (const auto &value : m_data) {
		if (value.id == npc) {
			return value.storage_cost;
		}
	}

	THROW_CODE_EXCEPTION(codepath_invalid_exception);
}

auto npc::is_maple_tv(game_npc_id npc) const -> bool {
	for (const auto &value : m_data) {
		if (value.id == npc) {
			return value.is_maple_tv;
		}
	}

	return false;
}

auto npc::is_guild_rank(game_npc_id npc) const -> bool {
	for (const auto &value : m_data) {
		if (value.id == npc) {
			return value.is_guild_rank;
		}
	}

	return false;
}

auto npc::is_valid_npc_id(game_npc_id npc) const -> bool {
	for (const auto &value : m_data) {
		if (value.id == npc) {
			return true;
		}
	}
	return false;
}

}
}
}