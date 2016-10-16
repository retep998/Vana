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
#include "skill_macros.hpp"
#include "common/io/database.hpp"
#include "common/util/misc.hpp"

namespace vana {
namespace channel_server {

auto skill_macros::load(game_player_id char_id) -> void {
	auto &db = vana::io::database::get_char_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT s.* FROM " << db.make_table(vana::table::skill_macros) << " s WHERE s.character_id = :char",
		soci::use(char_id, "char"));

	for (const auto &row : rs) {
		add(row.get<int8_t>("pos"), new skill_macro(row.get<string>("name"), row.get<bool>("shout"), row.get<game_skill_id>("skill_1"), row.get<game_skill_id>("skill_2"), row.get<game_skill_id>("skill_3")));
	}
}

auto skill_macros::save(game_player_id char_id) -> void {
	static init_list<game_skill_id> nulls = {0};
	vana::util::nullable_mode null_mode = vana::util::nullable_mode::null_if_found;

	int8_t i = 0;
	string name = "";
	bool shout = false;
	optional<game_skill_id> skill1 = 0;
	optional<game_skill_id> skill2 = 0;
	optional<game_skill_id> skill3 = 0;

	auto &db = vana::io::database::get_char_db();
	auto &sql = db.get_session();
	soci::statement st = (sql.prepare
		<< "REPLACE INTO " << db.make_table(vana::table::skill_macros) << " "
		<< "VALUES (:char, :key, :name, :shout, :skill1, :skill2, :skill3)",
		soci::use(char_id, "char"),
		soci::use(i, "key"),
		soci::use(name, "name"),
		soci::use(shout, "shout"),
		soci::use(skill1, "skill1"),
		soci::use(skill2, "skill2"),
		soci::use(skill3, "skill3"));

	for (i = 0; i <= get_max(); i++) {
		skill_macro *macro = get_skill_macro(i);
		if (macro != nullptr) {
			name = macro->name;
			shout = macro->shout;
			skill1 = vana::util::misc::get_optional(macro->skill1, null_mode, nulls);
			skill2 = vana::util::misc::get_optional(macro->skill2, null_mode, nulls);
			skill3 = vana::util::misc::get_optional(macro->skill3, null_mode, nulls);
			st.execute(true);
		}
	}
}

}
}