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
#include "KeyMaps.h"
#include "Database.h"

void KeyMaps::defaultMap() {
	add(2, KeyMap(4, 10));
	add(3, KeyMap(4, 12));
	add(4, KeyMap(4, 13));
	add(5, KeyMap(4, 18));
	add(6, KeyMap(4, 24));
	add(7, KeyMap(4, 21));
	add(16, KeyMap(4, 8));
	add(17, KeyMap(4, 5));
	add(18, KeyMap(4, 0));
	add(19, KeyMap(4, 4));
	add(23, KeyMap(4, 1));
	add(24, KeyMap(4, 25));
	add(25, KeyMap(4, 19));
	add(26, KeyMap(4, 14));
	add(27, KeyMap(4, 15));
	add(29, KeyMap(5, 52));
	add(31, KeyMap(4, 2));
	add(33, KeyMap(4, 26));
	add(34, KeyMap(4, 17));
	add(35, KeyMap(4, 11));
	add(37, KeyMap(4, 3));
	add(38, KeyMap(4, 20));
	add(40, KeyMap(4, 16));
	add(41, KeyMap(4, 23));
	add(43, KeyMap(4, 9));
	add(44, KeyMap(5, 50));
	add(45, KeyMap(5, 51));
	add(46, KeyMap(4, 6));
	add(48, KeyMap(4, 22));
	add(50, KeyMap(4, 7));
	add(56, KeyMap(5, 53));
	add(57, KeyMap(5, 54));
	add(59, KeyMap(6, 100));
	add(60, KeyMap(6, 101));
	add(61, KeyMap(6, 102));
	add(62, KeyMap(6, 103));
	add(63, KeyMap(6, 104));
	add(64, KeyMap(6, 105));
	add(65, KeyMap(6, 106));
}

void KeyMaps::load(int32_t charId) {
	soci::rowset<> rs = (Database::getCharDb().prepare << "SELECT k.* FROM keymap k WHERE k.character_id = :char", soci::use(charId, "char"));

	for (const auto &row : rs) {
		add(row.get<int32_t>("pos"), KeyMap(row.get<int8_t>("type"), row.get<int32_t>("action")));
	}
	if (getMax() == -1) {
		// No keymaps, set default map
		defaultMap();
		save(charId);
	}
}

void KeyMaps::save(int32_t charId) {
	size_t i = 0;
	int8_t type = 0;
	int32_t action = 0;

	soci::statement st = (Database::getCharDb().prepare
		<< "REPLACE INTO keymap "
		<< "VALUES (:char, :key, :type, :action)",
		soci::use(charId, "char"),
		soci::use(i, "key"),
		soci::use(type, "type"),
		soci::use(action, "action"));

	for (i = 0; i < KeyMaps::size; i++) {
		KeyMap *keymap = getKeyMap(i);
		if (keymap != nullptr) {
			type = keymap->type;
			action = keymap->action;
			st.execute(true);
		}
	}
}