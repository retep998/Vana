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
#pragma once

#include "Types.hpp"
#include "ValidClassData.hpp"
#include "ValidClassGenderData.hpp"
#include "ValidItemType.hpp"
#include <string>
#include <vector>

namespace vana {
	class valid_char_data_provider {
	public:
		auto load_data() -> void;

		auto is_forbidden_name(const string &cmp) const -> bool;
		auto is_valid_character(game_gender_id gender_id, game_hair_id hair, game_hair_id hair_color, game_face_id face, game_skin_id skin, game_item_id top, game_item_id bottom, game_item_id shoes, game_item_id weapon, int8_t class_id = adventurer) const -> bool;

		const static int8_t adventurer = 1;
		const static int8_t cygnus = 2;
	private:
		auto load_forbidden_names() -> void;
		auto load_creation_items() -> void;

		auto is_valid_item(int32_t id, const valid_class_data &items, valid_item_type type) const -> bool;
		auto get_items(game_gender_id gender_id, int8_t class_id) const -> const valid_class_data &;

		vector<string> m_forbidden_names;
		valid_class_gender_data m_adventurer;
		valid_class_gender_data m_cygnus;
	};
}