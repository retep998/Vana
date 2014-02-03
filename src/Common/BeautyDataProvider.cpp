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
#include "Algorithm.hpp"
#include "Database.hpp"
#include "GameConstants.hpp"
#include "GameLogicUtilities.hpp"
#include "InitializeCommon.hpp"
#include "Randomizer.hpp"
#include <algorithm>
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

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM " << Database::makeDataTable("character_skin_data") << " ORDER BY skinid ASC");

	for (const auto &row : rs) {
		m_skins.push_back(row.get<int8_t>("skinid"));
	}

	std::cout << "DONE" << std::endl;
}

auto BeautyDataProvider::loadHair() -> void {
	std::cout << std::setw(Initializing::OutputWidth) << std::left << "Initializing Hair... ";

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM " << Database::makeDataTable("character_hair_data") << " ORDER BY hairid ASC");

	for (const auto &row : rs) {
		int8_t genderId = GameLogicUtilities::getGenderId(row.get<string_t>("gender"));
		int32_t hair = row.get<int32_t>("hairid");
		auto &gender = genderId == Gender::Female ? m_female : m_male;
		gender.hair.push_back(hair);
	}

	std::cout << "DONE" << std::endl;
}

auto BeautyDataProvider::loadFaces() -> void {
	std::cout << std::setw(Initializing::OutputWidth) << std::left << "Initializing Faces... ";

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM " << Database::makeDataTable("character_face_data") << " ORDER BY faceid ASC");

	for (const auto &row : rs) {
		int8_t genderId = GameLogicUtilities::getGenderId(row.get<string_t>("gender"));
		int32_t face = row.get<int32_t>("faceid");
		auto &gender = genderId == Gender::Female ? m_female : m_male;
		gender.faces.push_back(face);
	}

	std::cout << "DONE" << std::endl;
}

auto BeautyDataProvider::getRandomSkin() const -> int8_t {
	return *Randomizer::select(m_skins);
}

auto BeautyDataProvider::getRandomHair(int8_t genderId) const -> int32_t {
	return *Randomizer::select(getGender(genderId).hair);
}

auto BeautyDataProvider::getRandomFace(int8_t genderId) const -> int32_t {
	return *Randomizer::select(getGender(genderId).faces);
}

auto BeautyDataProvider::getSkins() const -> const vector_t<int8_t> & {
	return m_skins;
}

auto BeautyDataProvider::getHair(int8_t genderId) const -> const vector_t<int32_t> & {
	return getGender(genderId).hair;
}

auto BeautyDataProvider::getFaces(int8_t genderId) const -> const vector_t<int32_t> & {
	return getGender(genderId).faces;
}

auto BeautyDataProvider::isValidHair(int8_t genderId, int32_t hair) const -> bool {
	const auto &gender = getGender(genderId);
	return ext::any_of(gender.hair, [hair](int32_t testHair) { return testHair == hair; });
}

auto BeautyDataProvider::isValidFace(int8_t genderId, int32_t face) const -> bool {
	const auto &gender = getGender(genderId);
	return ext::any_of(gender.faces, [face](int32_t testFace) { return testFace == face; });
}

auto BeautyDataProvider::isValidSkin(int8_t skin) const -> bool {
	return ext::any_of(m_skins, [skin](int8_t testSkin) { return testSkin == skin; });
}

auto BeautyDataProvider::getGender(int8_t genderId) const -> const ValidLook & {
	return genderId == Gender::Female ? m_female : m_male;
}