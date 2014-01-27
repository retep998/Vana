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
#include "BeautyDataProvider.hpp"
#include "Database.hpp"
#include "GameConstants.hpp"
#include "GameLogicUtilities.hpp"
#include "InitializeCommon.hpp"
#include "Randomizer.hpp"
#include <iomanip>
#include <iostream>

auto BeautyDataProvider::loadData() -> void {
	loadSkins();

	m_male.clear();
	m_female.clear();

	loadHair();
	loadFaces();
}

auto BeautyDataProvider::loadSkins() -> void {
	std::cout << std::setw(Initializing::OutputWidth) << std::left << "Initializing Skins... ";
	m_skins.clear();

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM character_skin_data ORDER BY skinid ASC");

	for (const auto &row : rs) {
		m_skins.push_back(row.get<int8_t>("skinid"));
	}

	std::cout << "DONE" << std::endl;
}

auto BeautyDataProvider::loadHair() -> void {
	std::cout << std::setw(Initializing::OutputWidth) << std::left << "Initializing Hair... ";
	int8_t gender;
	int32_t hair;
	ValidLook *look;

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM character_hair_data ORDER BY hairid ASC");

	for (const auto &row : rs) {
		gender = GameLogicUtilities::getGenderId(row.get<string_t>("gender"));
		hair = row.get<int32_t>("hairid");
		look = getGender(gender);
		look->hair.push_back(hair);
	}

	std::cout << "DONE" << std::endl;
}

auto BeautyDataProvider::loadFaces() -> void {
	std::cout << std::setw(Initializing::OutputWidth) << std::left << "Initializing Faces... ";
	int8_t gender;
	int32_t face;
	ValidLook *look;

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM character_face_data ORDER BY faceid ASC");

	for (const auto &row : rs) {
		gender = GameLogicUtilities::getGenderId(row.get<string_t>("gender"));
		face = row.get<int32_t>("faceid");
		look = getGender(gender);
		look->faces.push_back(face);
	}

	std::cout << "DONE" << std::endl;
}

auto BeautyDataProvider::getRandomSkin() -> int8_t {
	return m_skins[Randomizer::rand<int32_t>(m_skins.size() - 1)];
}

auto BeautyDataProvider::getRandomHair(int8_t gender) -> int32_t {
	ValidLook *look = getGender(gender);
	return look->hair[Randomizer::rand<int32_t>(look->hair.size() - 1)];
}

auto BeautyDataProvider::getRandomFace(int8_t gender) -> int32_t {
	ValidLook *look = getGender(gender);
	return look->faces[Randomizer::rand<int32_t>(look->faces.size() - 1)];
}

auto BeautyDataProvider::getSkins() -> vector_t<int8_t> {
	return m_skins;
}

auto BeautyDataProvider::getHair(int8_t gender) -> vector_t<int32_t> {
	return getGender(gender)->hair;
}

auto BeautyDataProvider::getFaces(int8_t gender) -> vector_t<int32_t> {
	return getGender(gender)->faces;
}

auto BeautyDataProvider::isValidHair(int8_t gender, int32_t hair) -> bool {
	ValidLook *look = getGender(gender);
	bool valid = false;
	for (size_t i = 0; i < look->hair.size(); i++) {
		if (hair == look->hair[i]) {
			valid = true;
			break;
		}
	}
	return valid;
}

auto BeautyDataProvider::isValidFace(int8_t gender, int32_t face) -> bool {
	ValidLook *look = getGender(gender);
	bool valid = false;
	for (size_t i = 0; i < look->faces.size(); i++) {
		if (face == look->faces[i]) {
			valid = true;
			break;
		}
	}
	return valid;
}

auto BeautyDataProvider::isValidSkin(int8_t skin) -> bool {
	bool valid = false;
	for (size_t i = 0; i < m_skins.size(); i++) {
		if (skin == m_skins[i]) {
			valid = true;
			break;
		}
	}
	return valid;
}

auto BeautyDataProvider::getGender(int8_t gender) -> ValidLook * {
	if (gender == Gender::Female) {
		return &m_female;
	}
	return &m_male;
}