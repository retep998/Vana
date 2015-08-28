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
#include "PlayerVariables.hpp"
#include "Database.hpp"
#include "Player.hpp"

namespace Vana {

PlayerVariables::PlayerVariables(Player *player) :
	m_player{player}
{
	load();
}

auto PlayerVariables::save() -> void {
	auto &db = Database::getCharDb();
	auto &sql = db.getSession();
	player_id_t charId = m_player->getId();

	sql.once << "DELETE FROM " << db.makeTable("character_variables") << " WHERE character_id = :char", soci::use(charId, "char");

	if (m_variables.size() > 0) {
		string_t key = "";
		string_t value = "";

		soci::statement st = (sql.prepare
			<< "INSERT INTO " << db.makeTable("character_variables") << " "
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
	auto &db = Database::getCharDb();
	auto &sql = db.getSession();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.makeTable("character_variables") << " WHERE character_id = :char", soci::use(m_player->getId(), "char"));

	for (const auto &row : rs) {
		m_variables[row.get<string_t>("key")] = row.get<string_t>("value");
	}
}

}