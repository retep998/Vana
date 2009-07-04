/*
Copyright (C) 2008-2009 Vana Development Team

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
#include "InitializeCommon.h"
#include "MiscUtilities.h"
#include "Randomizer.h"

using MiscUtilities::atob;
using Initializing::outputWidth;

BeautyDataProvider * BeautyDataProvider::singleton = 0;

void BeautyDataProvider::loadData() {
	// Skin data
	std::cout << std::setw(outputWidth) << std::left << "Initializing Skins... ";
	skins.clear();
	mysqlpp::Query query = Database::getDataDB().query("SELECT * FROM skindata ORDER BY skinid ASC");
	mysqlpp::UseQueryResult res = query.use();
	MYSQL_ROW Row;
	while (Row = res.fetch_raw_row()) {
		// Col0 : Skin ID
		skins.push_back(atoi(Row[0]));
	}
	std::cout << "DONE" << std::endl;

	// Hair data
	std::cout << std::setw(outputWidth) << std::left << "Initializing Hair... ";
	femalehair.clear();
	malehair.clear();
	query << "SELECT * FROM hairdata ORDER BY hairid ASC";
	res = query.use();
	bool female;
	while (Row = res.fetch_raw_row()) {
		// Col0 : Hair ID
		//    1 : Gender
		female = atob(Row[1]);
		if (female) {
			femalehair.push_back(atoi(Row[0]));
		}
		else {
			malehair.push_back(atoi(Row[0]));
		}
	}
	std::cout << "DONE" << std::endl;

	// Face data
	std::cout << std::setw(outputWidth) << std::left << "Initializing Faces... ";
	femalefaces.clear();
	malefaces.clear();
	query << "SELECT * FROM facedata ORDER BY faceid ASC";
	res = query.use();
	while (Row = res.fetch_raw_row()) {
		// Col0 : Face ID
		//    1 : Gender
		female = atob(Row[1]);
		if (female) {
			femalefaces.push_back(atoi(Row[0]));
		}
		else {
			malefaces.push_back(atoi(Row[0]));
		}
	}
	std::cout << "DONE" << std::endl;
}

int8_t BeautyDataProvider::getRandomSkin() {
	return skins[Randomizer::Instance()->randInt(skins.size() - 1)];
}

int32_t BeautyDataProvider::getRandomHair(int8_t gender) {
	if (gender == 1) // Female
		return femalehair[Randomizer::Instance()->randInt(femalehair.size() - 1)];
	return malehair[Randomizer::Instance()->randInt(malehair.size() - 1)];
}

int32_t BeautyDataProvider::getRandomFace(int8_t gender) {
	if (gender == 1) // Female
		return femalefaces[Randomizer::Instance()->randInt(femalefaces.size() - 1)];
	return malefaces[Randomizer::Instance()->randInt(malefaces.size() - 1)];
}

vector<int8_t> BeautyDataProvider::getSkins() {
	return skins;
}

vector<int32_t> BeautyDataProvider::getHair(int8_t gender) {
	if (gender == 1)  // Female
		return femalehair;
	return malehair;
}

vector<int32_t> BeautyDataProvider::getFaces(int8_t gender) {
	if (gender == 1)  // Female
		return femalefaces;
	return malefaces;
}

bool BeautyDataProvider::isValidHair(int8_t gender, int32_t hair) {
	vector<int32_t> v = getHair(gender);
	bool valid = false;
	for (size_t i = 0; i < v.size(); i++) {
		if (hair == v[i]) {
			valid = true;
			break;
		}
	}
	return valid;
}

bool BeautyDataProvider::isValidFace(int8_t gender, int32_t face) {
	vector<int32_t> v = getFaces(gender);
	bool valid = false;
	for (size_t i = 0; i < v.size(); i++) {
		if (face == v[i]) {
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