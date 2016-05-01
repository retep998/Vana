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

namespace Vana {
	class ValidCharDataProvider {
	public:
		auto loadData() -> void;

		auto isForbiddenName(const string_t &cmp) const -> bool;
		auto isValidCharacter(gender_id_t genderId, hair_id_t hair, hair_id_t hairColor, face_id_t face, skin_id_t skin, item_id_t top, item_id_t bottom, item_id_t shoes, item_id_t weapon, int8_t classId = Adventurer) const -> bool;

		const static int8_t Adventurer = 1;
		const static int8_t Cygnus = 2;
	private:
		auto loadForbiddenNames() -> void;
		auto loadCreationItems() -> void;

		auto isValidItem(int32_t id, const ValidClassData &items, ValidItemType type) const -> bool;
		auto getItems(gender_id_t genderId, int8_t classId) const -> const ValidClassData &;

		vector_t<string_t> m_forbiddenNames;
		ValidClassGenderData m_adventurer;
		ValidClassGenderData m_cygnus;
	};
}