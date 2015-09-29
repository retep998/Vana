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
#pragma once

#include "Common/Types.hpp"
#include <vector>

namespace Vana {
	struct ValidClassData {
		auto clear() -> void {
			hair.clear();
			hairColor.clear();
			faces.clear();
			skin.clear();
			top.clear();
			bottom.clear();
			shoes.clear();
			weapons.clear();
		}

		vector_t<hair_id_t> hair;
		vector_t<hair_id_t> hairColor;
		vector_t<face_id_t> faces;
		vector_t<skin_id_t> skin;
		vector_t<item_id_t> top;
		vector_t<item_id_t> bottom;
		vector_t<item_id_t> shoes;
		vector_t<item_id_t> weapons;
	};
}