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
#include "key_maps.hpp"
#include "common/io/database.hpp"

namespace vana {
namespace channel_server {

auto key_maps::default_map() -> void {
	add(key_map_key::one, key_map{key_map_action::all_chat});
	add(key_map_key::two, key_map{key_map_action::party_chat});
	add(key_map_key::three, key_map{key_map_action::buddy_chat});
	add(key_map_key::four, key_map{key_map_action::guild_chat});
	add(key_map_key::five, key_map{key_map_action::alliance_chat});
	add(key_map_key::six, key_map{key_map_action::spouse_chat});
	add(key_map_key::q, key_map{key_map_action::quest_menu});
	add(key_map_key::w, key_map{key_map_action::world_map});
	add(key_map_key::e, key_map{key_map_action::equipment_menu});
	add(key_map_key::r, key_map{key_map_action::buddy_list});
	add(key_map_key::i, key_map{key_map_action::item_menu});
	add(key_map_key::o, key_map{key_map_action::party_search});
	add(key_map_key::p, key_map{key_map_action::party_list});
	add(key_map_key::bracket_left, key_map{key_map_action::shortcut});
	add(key_map_key::bracket_right, key_map{key_map_action::quick_slot});
	add(key_map_key::left_ctrl, key_map{key_map_action::attack});
	add(key_map_key::s, key_map{key_map_action::ability_menu});
	add(key_map_key::f, key_map{key_map_action::family_list});
	add(key_map_key::g, key_map{key_map_action::guild_list});
	add(key_map_key::h, key_map{key_map_action::whisper_chat});
	add(key_map_key::k, key_map{key_map_action::skill_menu});
	add(key_map_key::l, key_map{key_map_action::helper});
	add(key_map_key::quote, key_map{key_map_action::expand_chat});
	add(key_map_key::backtick, key_map{key_map_action::cash_shop});
	add(key_map_key::backslash, key_map{key_map_action::set_key});
	add(key_map_key::z, key_map{key_map_action::pick_up});
	add(key_map_key::x, key_map{key_map_action::sit});
	add(key_map_key::c, key_map{key_map_action::messenger});
	add(key_map_key::b, key_map{key_map_action::monster_book});
	add(key_map_key::m, key_map{key_map_action::minimap});
	add(key_map_key::left_alt, key_map{key_map_action::jump});
	add(key_map_key::space, key_map{key_map_action::npc_chat});
	add(key_map_key::f1, key_map{key_map_action::cockeyed});
	add(key_map_key::f2, key_map{key_map_action::happy});
	add(key_map_key::f3, key_map{key_map_action::sarcastic});
	add(key_map_key::f4, key_map{key_map_action::crying});
	add(key_map_key::f5, key_map{key_map_action::outraged});
	add(key_map_key::f6, key_map{key_map_action::shocked});
	add(key_map_key::f7, key_map{key_map_action::annoyed});
}

auto key_maps::load(game_player_id char_id) -> void {
	auto &db = vana::io::database::get_char_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare
		<< "SELECT k.* "
		<< "FROM " << db.make_table(vana::table::keymap) << " k "
		<< "WHERE k.character_id = :char",
		soci::use(char_id, "char"));

	for (const auto &row : rs) {
		add(row.get<int32_t>("pos"), key_map{static_cast<key_map_type>(row.get<int8_t>("type")), row.get<int32_t>("action")});
	}
	if (get_max() == -1) {
		// No keymaps, set default map
		default_map();
		save(char_id);
	}
}

auto key_maps::save(game_player_id char_id) -> void {
	size_t i = 0;
	int8_t type = 0;
	int32_t action = 0;

	auto &db = vana::io::database::get_char_db();
	auto &sql = db.get_session();

	sql.once
		<< "DELETE FROM " << db.make_table(vana::table::keymap) << " "
		<< "WHERE character_id = :char",
		soci::use(char_id, "char");

	soci::statement st = (sql.prepare
		<< "INSERT INTO " << db.make_table(vana::table::keymap) << " "
		<< "VALUES (:char, :key, :type, :action)",
		soci::use(char_id, "char"),
		soci::use(i, "key"),
		soci::use(type, "type"),
		soci::use(action, "action"));

	for (i = 0; i < key_maps::key_count; i++) {
		key_map *keymap = get_key_map(i);
		if (keymap != nullptr) {
			type = static_cast<int8_t>(keymap->type);
			action = keymap->action;
			st.execute(true);
		}
	}
}

}
}