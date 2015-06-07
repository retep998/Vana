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

#include "Types.hpp"
#include <string>
#include <vector>

struct ValidItems {
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

struct ClassValidItems {
	auto clear() -> void {
		male.clear();
		female.clear();
	}

	ValidItems male;
	ValidItems female;
};

enum class ValidItemType {
	Face = 1,
	Hair = 2,
	HairColor = 3,
	Skin = 4,
	Top = 5,
	Bottom = 6,
	Shoes = 7,
	Weapon = 8
};

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

	auto isValidItem(int32_t id, const ValidItems &items, ValidItemType type) const -> bool;
	auto getItems(gender_id_t genderId, int8_t classId) const -> const ValidItems &;

	vector_t<string_t> m_forbiddenNames;
	ClassValidItems m_adventurer;
	ClassValidItems m_cygnus;
};