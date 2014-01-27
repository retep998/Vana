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
#include "PlayerVariables.h"
#include "Database.h"
#include "Player.h"

PlayerVariables::PlayerVariables(Player *p) :
	m_player(p)
{
	load();
}

auto PlayerVariables::save() -> void {
	soci::session &sql = Database::getCharDb();
	int32_t charId = m_player->getId();

	sql.once << "DELETE FROM character_variables WHERE character_id = :char", soci::use(charId, "char");

	if (m_variables.size() > 0) {
		string_t key = "";
		string_t value = "";

		soci::statement st = (sql.prepare
			<< "INSERT INTO character_variables "
			<< "VALUES (:char, :key, :value)",
			soci::use(charId, "char"),
			soci::use(key, "key"),
			soci::use(value, "value"));

		for (const auto &kvp : m_variables) {
			key = kvp.first;
			value = kvp.second;
			st.execute(true);
		}
	}
}

auto PlayerVariables::load() -> void {
	soci::rowset<> rs = (Database::getCharDb().prepare << "SELECT * FROM character_variables WHERE character_id = :char", soci::use(m_player->getId(), "char"));

	for (const auto &row : rs) {
		m_variables[row.get<string_t>("key")] = row.get<string_t>("value");
	}
}