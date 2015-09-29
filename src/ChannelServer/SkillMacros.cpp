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
#include "SkillMacros.hpp"
#include "Common/Database.hpp"
#include "Common/MiscUtilities.hpp"

namespace Vana {
namespace ChannelServer {

auto SkillMacros::load(player_id_t charId) -> void {
	auto &db = Database::getCharDb();
	auto &sql = db.getSession();
	soci::rowset<> rs = (sql.prepare << "SELECT s.* FROM " << db.makeTable("skill_macros") << " s WHERE s.character_id = :char", soci::use(charId, "char"));

	for (const auto &row : rs) {
		add(row.get<int8_t>("pos"), new SkillMacro(row.get<string_t>("name"), row.get<bool>("shout"), row.get<skill_id_t>("skill_1"), row.get<skill_id_t>("skill_2"), row.get<skill_id_t>("skill_3")));
	}
}

auto SkillMacros::save(player_id_t charId) -> void {
	static init_list_t<skill_id_t> nulls = {0};
	MiscUtilities::NullableMode nullMode = MiscUtilities::NullableMode::NullIfFound;

	int8_t i = 0;
	string_t name = "";
	bool shout = false;
	optional_t<skill_id_t> skill1 = 0;
	optional_t<skill_id_t> skill2 = 0;
	optional_t<skill_id_t> skill3 = 0;

	auto &db = Database::getCharDb();
	auto &sql = db.getSession();
	soci::statement st = (sql.prepare
		<< "REPLACE INTO " << db.makeTable("skill_macros") << " "
		<< "VALUES (:char, :key, :name, :shout, :skill1, :skill2, :skill3)",
		soci::use(charId, "char"),
		soci::use(i, "key"),
		soci::use(name, "name"),
		soci::use(shout, "shout"),
		soci::use(skill1, "skill1"),
		soci::use(skill2, "skill2"),
		soci::use(skill3, "skill3"));

	for (i = 0; i <= getMax(); i++) {
		SkillMacro *macro = getSkillMacro(i);
		if (macro != nullptr) {
			name = macro->name;
			shout = macro->shout;
			skill1 = MiscUtilities::getOptional(macro->skill1, nullMode, nulls);
			skill2 = MiscUtilities::getOptional(macro->skill2, nullMode, nulls);
			skill3 = MiscUtilities::getOptional(macro->skill3, nullMode, nulls);
			st.execute(true);
		}
	}
}

}
}