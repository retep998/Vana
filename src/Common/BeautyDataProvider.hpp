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

#include "Types.hpp"
#include <vector>

struct ValidLook {
	auto clear() -> void {
		hair.clear();
		faces.clear();
	}

	vector_t<int32_t> hair;
	vector_t<int32_t> faces;
};

class BeautyDataProvider {
	SINGLETON(BeautyDataProvider);
public:
	auto loadData() -> void;

	auto getRandomSkin() const -> int8_t;
	auto getRandomHair(int8_t genderId) const -> int32_t;
	auto getRandomFace(int8_t genderId) const -> int32_t;

	auto getSkins() const -> const vector_t<int8_t> &;
	auto getHair(int8_t genderId) const -> const vector_t<int32_t> &;
	auto getFaces(int8_t genderId) const -> const vector_t<int32_t> &;

	auto isValidSkin(int8_t skin) const -> bool;
	auto isValidHair(int8_t genderId, int32_t hair) const -> bool;
	auto isValidFace(int8_t genderId, int32_t face) const -> bool;
private:
	auto loadSkins() -> void;
	auto loadHair() -> void;
	auto loadFaces() -> void;
	auto getGender(int8_t genderId) const -> const ValidLook &;

	ValidLook m_male;
	ValidLook m_female;
	vector_t<int8_t> m_skins;
};