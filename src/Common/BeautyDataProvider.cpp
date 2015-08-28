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

namespace Vana {

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

	auto &db = Database::getDataDb();
	auto &sql = db.getSession();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.makeTable("character_skin_data") << " ORDER BY skinid ASC");

	for (const auto &row : rs) {
		m_skins.push_back(row.get<skin_id_t>("skinid"));
	}

	std::cout << "DONE" << std::endl;
}

auto BeautyDataProvider::loadHair() -> void {
	std::cout << std::setw(Initializing::OutputWidth) << std::left << "Initializing Hair... ";

	auto &db = Database::getDataDb();
	auto &sql = db.getSession();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.makeTable("character_hair_data") << " ORDER BY hairid ASC");

	for (const auto &row : rs) {
		gender_id_t genderId = GameLogicUtilities::getGenderId(row.get<string_t>("gender"));
		hair_id_t hair = row.get<hair_id_t>("hairid");
		auto &gender = genderId == Gender::Female ? m_female : m_male;
		gender.hair.push_back(hair);
	}

	std::cout << "DONE" << std::endl;
}

auto BeautyDataProvider::loadFaces() -> void {
	std::cout << std::setw(Initializing::OutputWidth) << std::left << "Initializing Faces... ";

	auto &db = Database::getDataDb();
	auto &sql = db.getSession();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.makeTable("character_face_data") << " ORDER BY faceid ASC");

	for (const auto &row : rs) {
		gender_id_t genderId = GameLogicUtilities::getGenderId(row.get<string_t>("gender"));
		face_id_t face = row.get<face_id_t>("faceid");
		auto &gender = genderId == Gender::Female ? m_female : m_male;
		gender.faces.push_back(face);
	}

	std::cout << "DONE" << std::endl;
}

auto BeautyDataProvider::getRandomSkin() const -> skin_id_t {
	return *Randomizer::select(m_skins);
}

auto BeautyDataProvider::getRandomHair(gender_id_t genderId) const -> hair_id_t {
	return *Randomizer::select(getGender(genderId).hair);
}

auto BeautyDataProvider::getRandomFace(gender_id_t genderId) const -> face_id_t {
	return *Randomizer::select(getGender(genderId).faces);
}

auto BeautyDataProvider::getSkins() const -> const vector_t<skin_id_t> & {
	return m_skins;
}

auto BeautyDataProvider::getHair(gender_id_t genderId) const -> const vector_t<hair_id_t> & {
	return getGender(genderId).hair;
}

auto BeautyDataProvider::getFaces(gender_id_t genderId) const -> const vector_t<face_id_t> & {
	return getGender(genderId).faces;
}

auto BeautyDataProvider::isValidHair(gender_id_t genderId, hair_id_t hair) const -> bool {
	const auto &gender = getGender(genderId);
	return ext::any_of(gender.hair, [hair](int32_t testHair) { return testHair == hair; });
}

auto BeautyDataProvider::isValidFace(gender_id_t genderId, face_id_t face) const -> bool {
	const auto &gender = getGender(genderId);
	return ext::any_of(gender.faces, [face](int32_t testFace) { return testFace == face; });
}

auto BeautyDataProvider::isValidSkin(skin_id_t skin) const -> bool {
	return ext::any_of(m_skins, [skin](skin_id_t testSkin) { return testSkin == skin; });
}

auto BeautyDataProvider::getGender(gender_id_t genderId) const -> const ValidLook & {
	return genderId == Gender::Female ? m_female : m_male;
}

}