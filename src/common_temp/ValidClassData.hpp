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

#include "common_temp/Types.hpp"
#include <vector>

namespace vana {
	struct valid_class_data {
		auto clear() -> void {
			hair.clear();
			hair_color.clear();
			faces.clear();
			skin.clear();
			top.clear();
			bottom.clear();
			shoes.clear();
			weapons.clear();
		}

		vector<game_hair_id> hair;
		vector<game_hair_id> hair_color;
		vector<game_face_id> faces;
		vector<game_skin_id> skin;
		vector<game_item_id> top;
		vector<game_item_id> bottom;
		vector<game_item_id> shoes;
		vector<game_item_id> weapons;
	};
}