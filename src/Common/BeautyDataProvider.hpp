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

	vector_t<hair_id_t> hair;
	vector_t<face_id_t> faces;
};

class BeautyDataProvider {
	SINGLETON(BeautyDataProvider);
public:
	auto loadData() -> void;

	auto getRandomSkin() const -> skin_id_t;
	auto getRandomHair(gender_id_t genderId) const -> hair_id_t;
	auto getRandomFace(gender_id_t genderId) const -> face_id_t;

	auto getSkins() const -> const vector_t<skin_id_t> &;
	auto getHair(gender_id_t genderId) const -> const vector_t<hair_id_t> &;
	auto getFaces(gender_id_t genderId) const -> const vector_t<face_id_t> &;

	auto isValidSkin(skin_id_t skin) const -> bool;
	auto isValidHair(gender_id_t genderId, hair_id_t hair) const -> bool;
	auto isValidFace(gender_id_t genderId, face_id_t face) const -> bool;
private:
	auto loadSkins() -> void;
	auto loadHair() -> void;
	auto loadFaces() -> void;
	auto getGender(gender_id_t genderId) const -> const ValidLook &;

	ValidLook m_male;
	ValidLook m_female;
	vector_t<skin_id_t> m_skins;
};