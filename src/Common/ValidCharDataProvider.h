/*
Copyright (C) 2008-2014 Vana Development Team

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

#include "Types.h"
#include <string>
#include <vector>

struct ValidItems {
	auto clear() -> void {
		hair.clear();
		haircolor.clear();
		faces.clear();
		skin.clear();
		top.clear();
		bottom.clear();
		shoes.clear();
		weapons.clear();
	}

	vector_t<int32_t> hair;
	vector_t<int32_t> haircolor;
	vector_t<int32_t> faces;
	vector_t<int32_t> skin;
	vector_t<int32_t> top;
	vector_t<int32_t> bottom;
	vector_t<int32_t> shoes;
	vector_t<int32_t> weapons;
};

struct ClassValidItems {
	auto clear() -> void {
		male.clear();
		female.clear();
	}

	ValidItems male;
	ValidItems female;
};

namespace ValidItemType {
	enum : int8_t {
		Face = 1,
		Hair = 2,
		HairColor = 3,
		Skin = 4,
		Top = 5,
		Bottom = 6,
		Shoes = 7,
		Weapon = 8
	};
}

class ValidCharDataProvider {
	SINGLETON(ValidCharDataProvider);
public:
	auto loadData() -> void;

	auto isForbiddenName(const string_t &cmp) -> bool;
	auto isValidCharacter(int8_t gender, int32_t hair, int32_t haircolor, int32_t eyes, int32_t skin, int32_t top, int32_t bottom, int32_t shoes, int32_t weapon, int8_t classId = Adventurer) -> bool;

	const static int8_t Adventurer = 1;
	const static int8_t Cygnus = 2;
private:
	auto loadForbiddenNames() -> void;
	auto loadCreationItems() -> void;

	auto isValidItem(int32_t id, int8_t gender, int8_t classId, int8_t type) -> bool;
	auto iterateTest(int32_t id, vector_t<int32_t> *test) -> bool;
	auto getItems(int8_t gender, int8_t classId) -> ValidItems *;

	vector_t<string_t> m_forbiddenNames;
	ClassValidItems m_adventurer;
	ClassValidItems m_cygnus;
};