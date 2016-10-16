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
#include "valid_char.hpp"
#include "common/algorithm.hpp"
#include "common/constant/gender.hpp"
#include "common/data/initialize.hpp"
#include "common/io/database.hpp"
#include "common/util/game_logic/player.hpp"
#include "common/util/string.hpp"
#include <iomanip>
#include <iostream>
#include <stdexcept>

namespace vana {
namespace data {
namespace provider {

auto valid_char::load_data() -> void {
	std::cout << std::setw(vana::data::initialize::output_width) << std::left << "Initializing Char Info... ";

	load_forbidden_names();
	load_creation_items();

	std::cout << "DONE" << std::endl;
}

auto valid_char::load_forbidden_names() -> void {
	m_forbidden_names.clear();

	auto &db = vana::io::database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table(vana::data::table::character_forbidden_names));

	for (const auto &row : rs) {
		m_forbidden_names.push_back(row.get<string>("forbidden_name"));
	}
}

auto valid_char::load_creation_items() -> void {
	m_adventurer.clear();
	m_cygnus.clear();

	auto &db = vana::io::database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table(vana::data::table::character_creation_data));

	for (const auto &row : rs) {
		game_gender_id gender_id = vana::util::game_logic::player::get_gender_id(row.get<string>("gender"));
		int32_t object_id = row.get<int32_t>("objectid");
		int8_t class_id = -1;

		vana::util::str::run_enum(row.get<string>("character_type"), [&class_id](const string &cmp) {
			if (cmp == "regular") class_id = adventurer;
			else if (cmp == "cygnus") class_id = cygnus;
		});

		auto &items = gender_id == constant::gender::male ?
			(class_id == adventurer ? m_adventurer.male : m_cygnus.male) :
			(class_id == adventurer ? m_adventurer.female : m_cygnus.female);

		vana::util::str::run_enum(row.get<string>("object_type"), [&items, &object_id](const string &cmp) {
			if (cmp == "face") items.faces.push_back(object_id);
			else if (cmp == "hair") items.hair.push_back(object_id);
			else if (cmp == "haircolor") items.hair_color.push_back(object_id);
			else if (cmp == "skin") items.skin.push_back(static_cast<game_skin_id>(object_id));
			else if (cmp == "top") items.top.push_back(object_id);
			else if (cmp == "bottom") items.bottom.push_back(object_id);
			else if (cmp == "shoes") items.shoes.push_back(object_id);
			else if (cmp == "weapon") items.weapons.push_back(object_id);
		});
	}
}

auto valid_char::is_forbidden_name(const string &cmp) const -> bool {
	string c = vana::util::str::remove_spaces(vana::util::str::to_lower(cmp));
	return ext::any_of(m_forbidden_names, [&c](const string &s) -> bool {
		return c.find(s, 0) != string::npos;
	});
}

auto valid_char::is_valid_character(game_gender_id gender_id, game_hair_id hair, game_hair_id hair_color, game_face_id face, game_skin_id skin, game_item_id top, game_item_id bottom, game_item_id shoes, game_item_id weapon, int8_t class_id) const -> bool {
	if (gender_id != constant::gender::male && gender_id != constant::gender::female) {
		return false;
	}

	auto &items = get_items(gender_id, class_id);
	bool valid = is_valid_item(hair, items, data::type::valid_item_type::hair);
	if (valid) valid = is_valid_item(hair_color, items, data::type::valid_item_type::hair_color);
	if (valid) valid = is_valid_item(face, items, data::type::valid_item_type::face);
	if (valid) valid = is_valid_item(skin, items, data::type::valid_item_type::skin);
	if (valid) valid = is_valid_item(top, items, data::type::valid_item_type::top);
	if (valid) valid = is_valid_item(bottom, items, data::type::valid_item_type::bottom);
	if (valid) valid = is_valid_item(shoes, items, data::type::valid_item_type::shoes);
	if (valid) valid = is_valid_item(weapon, items, data::type::valid_item_type::weapon);
	return valid;
}

auto valid_char::is_valid_item(int32_t id, const valid_class_data &items, data::type::valid_item_type type) const -> bool {
	auto id_test = [id](int32_t test) -> bool { return id == test; };
	switch (type) {
		case data::type::valid_item_type::face: return ext::any_of(items.faces, id_test);
		case data::type::valid_item_type::hair: return ext::any_of(items.hair, id_test);
		case data::type::valid_item_type::hair_color: return ext::any_of(items.hair_color, id_test);
		case data::type::valid_item_type::skin: return ext::any_of(items.skin, [id = static_cast<game_skin_id>(id)](game_skin_id test) -> bool { return id == test; });
		case data::type::valid_item_type::top: return ext::any_of(items.top, id_test);
		case data::type::valid_item_type::bottom: return ext::any_of(items.bottom, id_test);
		case data::type::valid_item_type::shoes: return ext::any_of(items.shoes, id_test);
		case data::type::valid_item_type::weapon: return ext::any_of(items.weapons, id_test);
	}
	THROW_CODE_EXCEPTION(not_implemented_exception, "valid_item_type");
}

auto valid_char::get_items(game_gender_id gender_id, int8_t class_id) const -> const valid_class_data & {
	return gender_id == constant::gender::male ?
		(class_id == adventurer ? m_adventurer.male : m_cygnus.male) :
		(class_id == adventurer ? m_adventurer.female : m_cygnus.female);
}

}
}
}