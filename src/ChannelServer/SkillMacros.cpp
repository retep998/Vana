/*
Copyright (C) 2008-2013 Vana Development Team

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
#include "SkillMacros.h"
#include "Database.h"
#include "MiscUtilities.h"

void SkillMacros::load(int32_t charId) {
	soci::rowset<> rs = (Database::getCharDb().prepare << "SELECT s.* FROM skill_macros s WHERE s.character_id = :char", soci::use(charId, "char"));

	for (const auto &row : rs) {
		add(row.get<int8_t>("pos"), new SkillMacro(row.get<string>("name"), row.get<bool>("shout"), row.get<int32_t>("skill1"), row.get<int32_t>("skill2"), row.get<int32_t>("skill3")));
	}
}

void SkillMacros::save(int32_t charId) {
	using MiscUtilities::getOptional;

	static int32_t nullsInt32[] = {0};
	MiscUtilities::NullableMode nulls = MiscUtilities::NullIfFound;

	int8_t i = 0;
	string name = "";
	bool shout = false;
	opt_int32_t skill1 = 0;
	opt_int32_t skill2 = 0;
	opt_int32_t skill3 = 0;

	soci::statement st = (Database::getCharDb().prepare
		<< "REPLACE INTO skill_macros "
		<< "VALUES (:char, :key, :name, :shout, :skill1, :skill2, :skill3)",
		soci::use(charId, "char"),
		soci::use(i, "key"),
		soci::use(name, "name"),
		soci::use(shout, "shout"),
		soci::use(skill1, "skill1"),
		soci::use(skill2, "skill2"),
		soci::use(skill3, "skill3"));

	for (i = 0; i < getMax(); i++) {
		SkillMacro *macro = getSkillMacro(i);
		if (macro != nullptr) {
			name = macro->name;
			shout = macro->shout;
			skill1 = getOptional(macro->skill1, nulls, nullsInt32);
			skill2 = getOptional(macro->skill2, nulls, nullsInt32);
			skill3 = getOptional(macro->skill3, nulls, nullsInt32);
			st.execute(true);
		}
	}
}