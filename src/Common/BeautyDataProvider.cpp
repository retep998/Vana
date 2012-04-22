/*
Copyright (C) 2008-2012 Vana Development Team

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
#include "BeautyDataProvider.h"
#include "Database.h"
#include "GameConstants.h"
#include "GameLogicUtilities.h"
#include "InitializeCommon.h"
#include "Randomizer.h"
#include <iomanip>

using Initializing::OutputWidth;

BeautyDataProvider * BeautyDataProvider::singleton = nullptr;

void BeautyDataProvider::loadData() {
	loadSkins();

	m_male.clear();
	m_female.clear();

	loadHair();
	loadFaces();
}

void BeautyDataProvider::loadSkins() {
	std::cout << std::setw(OutputWidth) << std::left << "Initializing Skins... ";
	m_skins.clear();

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM character_skin_data ORDER BY skinid ASC");

	for (soci::rowset<>::const_iterator i = rs.begin(); i != rs.end(); ++i) {
		soci::row const &row = *i;

		m_skins.push_back(row.get<int8_t>("skinid"));
	}

	std::cout << "DONE" << std::endl;
}

void BeautyDataProvider::loadHair() {
	std::cout << std::setw(OutputWidth) << std::left << "Initializing Hair... ";
	int8_t gender;
	int32_t hair;
	ValidLook *look;

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM character_hair_data ORDER BY hairid ASC");

	for (soci::rowset<>::const_iterator i = rs.begin(); i != rs.end(); ++i) {
		soci::row const &row = *i;

		gender = GameLogicUtilities::getGenderId(row.get<string>("gender"));
		hair = row.get<int32_t>("hairid");
		look = getGender(gender);
		look->hair.push_back(hair);
	}

	std::cout << "DONE" << std::endl;
}

void BeautyDataProvider::loadFaces() {
	std::cout << std::setw(OutputWidth) << std::left << "Initializing Faces... ";
	int8_t gender;
	int32_t face;
	ValidLook *look;

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM character_face_data ORDER BY faceid ASC");

	for (soci::rowset<>::const_iterator i = rs.begin(); i != rs.end(); ++i) {
		soci::row const &row = *i;

		gender = GameLogicUtilities::getGenderId(row.get<string>("gender"));
		face = row.get<int32_t>("faceid");
		look = getGender(gender);
		look->faces.push_back(face);
	}

	std::cout << "DONE" << std::endl;
}

int8_t BeautyDataProvider::getRandomSkin() {
	return m_skins[Randomizer::Instance()->randInt(m_skins.size() - 1)];
}

int32_t BeautyDataProvider::getRandomHair(int8_t gender) {
	ValidLook *look = getGender(gender);
	return look->hair[Randomizer::Instance()->randInt(look->hair.size() - 1)];
}

int32_t BeautyDataProvider::getRandomFace(int8_t gender) {
	ValidLook *look = getGender(gender);
	return look->faces[Randomizer::Instance()->randInt(look->faces.size() - 1)];
}

vector<int8_t> BeautyDataProvider::getSkins() {
	return m_skins;
}

vector<int32_t> BeautyDataProvider::getHair(int8_t gender) {
	return getGender(gender)->hair;
}

vector<int32_t> BeautyDataProvider::getFaces(int8_t gender) {
	return getGender(gender)->faces;
}

bool BeautyDataProvider::isValidHair(int8_t gender, int32_t hair) {
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

bool BeautyDataProvider::isValidFace(int8_t gender, int32_t face) {
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

bool BeautyDataProvider::isValidSkin(int8_t skin) {
	bool valid = false;
	for (size_t i = 0; i < m_skins.size(); i++) {
		if (skin == m_skins[i]) {
			valid = true;
			break;
		}
	}
	return valid;
}

ValidLook * BeautyDataProvider::getGender(int8_t gender) {
	if (gender == Gender::Female) {
		return &m_female;
	}
	return &m_male;
}