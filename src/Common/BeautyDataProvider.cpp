/*
Copyright (C) 2008-2011 Vana Development Team

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

using Initializing::outputWidth;

BeautyDataProvider * BeautyDataProvider::singleton = nullptr;

void BeautyDataProvider::loadData() {
	loadSkins();

	male.clear();
	female.clear();

	loadHair();
	loadFaces();
}

void BeautyDataProvider::loadSkins() {
	std::cout << std::setw(outputWidth) << std::left << "Initializing Skins... ";
	skins.clear();
	mysqlpp::Query query = Database::getDataDB().query("SELECT * FROM character_skin_data ORDER BY skinid ASC");
	mysqlpp::UseQueryResult res = query.use();

	enum SkinData {
		Id = 0
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		skins.push_back(atoi(row[Id]));
	}
	std::cout << "DONE" << std::endl;
}

void BeautyDataProvider::loadHair() {
	std::cout << std::setw(outputWidth) << std::left << "Initializing Hair... ";

	mysqlpp::Query query = Database::getDataDB().query("SELECT * FROM character_hair_data ORDER BY hairid ASC");
	mysqlpp::UseQueryResult res = query.use();
	int8_t gender;
	int32_t hair;
	ValidLook *yes;

	enum HairData {
		Id = 0,
		Gender
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		gender = GameLogicUtilities::getGenderId(row[Gender]);
		hair = atoi(row[Id]);
		yes = getGender(gender);
		yes->hair.push_back(hair);
	}

	std::cout << "DONE" << std::endl;
}

void BeautyDataProvider::loadFaces() {
	std::cout << std::setw(outputWidth) << std::left << "Initializing Faces... ";

	mysqlpp::Query query = Database::getDataDB().query("SELECT * FROM character_face_data ORDER BY faceid ASC");
	mysqlpp::UseQueryResult res = query.use();
	ValidLook *yes;
	int8_t gender;
	int32_t face;

	enum FaceData {
		Id = 0,
		Gender
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		gender = GameLogicUtilities::getGenderId(row[Gender]);
		face = atoi(row[Id]);
		yes = getGender(gender);
		yes->faces.push_back(face);
	}

	std::cout << "DONE" << std::endl;
}

int8_t BeautyDataProvider::getRandomSkin() {
	return skins[Randomizer::Instance()->randInt(skins.size() - 1)];
}

int32_t BeautyDataProvider::getRandomHair(int8_t gender) {
	ValidLook *yes = getGender(gender);
	return yes->hair[Randomizer::Instance()->randInt(yes->hair.size() - 1)];
}

int32_t BeautyDataProvider::getRandomFace(int8_t gender) {
	ValidLook *yes = getGender(gender);
	return yes->faces[Randomizer::Instance()->randInt(yes->faces.size() - 1)];
}

vector<int8_t> BeautyDataProvider::getSkins() {
	return skins;
}

vector<int32_t> BeautyDataProvider::getHair(int8_t gender) {
	return getGender(gender)->hair;
}

vector<int32_t> BeautyDataProvider::getFaces(int8_t gender) {
	return getGender(gender)->faces;
}

bool BeautyDataProvider::isValidHair(int8_t gender, int32_t hair) {
	ValidLook *yes = getGender(gender);
	bool valid = false;
	for (size_t i = 0; i < yes->hair.size(); i++) {
		if (hair == yes->hair[i]) {
			valid = true;
			break;
		}
	}
	return valid;
}

bool BeautyDataProvider::isValidFace(int8_t gender, int32_t face) {
	ValidLook *yes = getGender(gender);
	bool valid = false;
	for (size_t i = 0; i < yes->faces.size(); i++) {
		if (face == yes->faces[i]) {
			valid = true;
			break;
		}
	}
	return valid;
}

bool BeautyDataProvider::isValidSkin(int8_t skin) {
	bool valid = false;
	for (size_t i = 0; i < skins.size(); i++) {
		if (skin == skins[i]) {
			valid = true;
			break;
		}
	}
	return valid;
}

ValidLook * BeautyDataProvider::getGender(int8_t gender) {
	if (gender == Gender::Female)  // Female
		return &female;
	return &male;
}