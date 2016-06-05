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
#include "ValidLookData.hpp"
#include <vector>

namespace vana {
	class beauty_data_provider {
	public:
		auto load_data() -> void;

		auto get_random_skin() const -> game_skin_id;
		auto get_random_hair(game_gender_id gender_id) const -> game_hair_id;
		auto get_random_face(game_gender_id gender_id) const -> game_face_id;

		auto get_skins() const -> const vector<game_skin_id> &;
		auto get_hair(game_gender_id gender_id) const -> const vector<game_hair_id> &;
		auto get_faces(game_gender_id gender_id) const -> const vector<game_face_id> &;

		auto is_valid_skin(game_skin_id skin) const -> bool;
		auto is_valid_hair(game_gender_id gender_id, game_hair_id hair) const -> bool;
		auto is_valid_face(game_gender_id gender_id, game_face_id face) const -> bool;
	private:
		auto load_skins() -> void;
		auto load_hair() -> void;
		auto load_faces() -> void;
		auto get_gender(game_gender_id gender_id) const -> const valid_look_data &;

		valid_look_data m_male;
		valid_look_data m_female;
		vector<game_skin_id> m_skins;
	};
}