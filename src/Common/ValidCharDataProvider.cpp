/*
Copyright (C) 2008-2010 Vana Development Team

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
#include "ValidCharDataProvider.h"
#include "Database.h"
#include "GameConstants.h"
#include "GameLogicUtilities.h"
#include "InitializeCommon.h"
#include "StringUtilities.h"

using Initializing::outputWidth;
using StringUtilities::runFlags;

ValidCharDataProvider * ValidCharDataProvider::singleton = nullptr;

void ValidCharDataProvider::loadData() {
	std::cout << std::setw(outputWidth) << std::left << "Initializing Char Info... ";

	loadForbiddenNames();
	loadCreationItems();
	
	std::cout << "DONE" << std::endl;
}

void ValidCharDataProvider::loadForbiddenNames() {
	forbiddennames.clear();
	mysqlpp::Query query = Database::getDataDB().query("SELECT * FROM character_forbidden_names");
	mysqlpp::UseQueryResult res = query.use();
	string name;

	enum ForbiddenNameData {
		Name
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		name = row[Name];
		forbiddennames.push_back(name);
	}
}

namespace Functors {
	struct CreationItemFlags {
		void operator()(const string &cmp) {
			if (cmp == "face") items->faces.push_back(objectid);
			else if (cmp == "hair") items->hair.push_back(objectid);
			else if (cmp == "haircolor") items->haircolor.push_back(objectid);
			else if (cmp == "skin") items->skin.push_back(objectid);
			else if (cmp == "top") items->top.push_back(objectid);
			else if (cmp == "bottom") items->bottom.push_back(objectid);
			else if (cmp == "shoes") items->shoes.push_back(objectid);
			else if (cmp == "weapon") items->weapons.push_back(objectid);
		}
		ValidItems *items;
		int32_t objectid;
	};
}

void ValidCharDataProvider::loadCreationItems() {
	regmale.clear();
	regfemale.clear();
	cygnusmale.clear();
	cygnusfemale.clear();
	mysqlpp::Query query = Database::getDataDB().query("SELECT * FROM character_creation_data");
	mysqlpp::UseQueryResult res = query.use();
	int8_t gender;
	bool cygnus;
	int32_t objectid;
	ValidItems *items;

	using namespace Functors;

	enum CreationData {
		Id = 0,
		CharacterType, Gender, ObjectType, ObjectId
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		gender = GameLogicUtilities::getGenderId(row[Gender]);
		cygnus = (static_cast<string>(row[CharacterType]) == "cygnus");
		objectid = atoi(row[ObjectId]);
		items = getItems(gender, cygnus);

		CreationItemFlags whoo = {items, objectid};
		runFlags(row[ObjectType], whoo);
	}
}

bool ValidCharDataProvider::isForbiddenName(const string &cmp) {
	bool forbidden = false;
	for (size_t i = 0; i < forbiddennames.size(); i++) {
		if (cmp.find(forbiddennames[i], 0) != string::npos) {
			forbidden = true;
			break;
		}
	}
	return forbidden;
}

bool ValidCharDataProvider::isValidCharacter(int8_t gender, int32_t hair, int32_t haircolor, int32_t eyes, int32_t skin, int32_t top, int32_t bottom, int32_t shoes, int32_t weapon, bool cygnus) {
	if (gender != Gender::Male && gender != Gender::Female) {
		return false;
	}
	bool valid = isValidItem(hair, gender, cygnus, ValidItemType::Hair);
	if (valid) valid = isValidItem(haircolor, gender, cygnus, ValidItemType::HairColor);
	if (valid) valid = isValidItem(eyes, gender, cygnus, ValidItemType::Face);
	if (valid) valid = isValidItem(skin, gender, cygnus, ValidItemType::Skin);
	if (valid) valid = isValidItem(top, gender, cygnus, ValidItemType::Top);
	if (valid) valid = isValidItem(bottom, gender, cygnus, ValidItemType::Bottom);
	if (valid) valid = isValidItem(shoes, gender, cygnus, ValidItemType::Shoes);
	if (valid) valid = isValidItem(weapon, gender, cygnus, ValidItemType::Weapon);
	return valid;
}

bool ValidCharDataProvider::isValidItem(int32_t id, int8_t gender, bool cygnus, int8_t type) {
	ValidItems *items = getItems(gender, cygnus);

	if (items == nullptr)
		return false;

	bool valid = false;
	switch (type) {
		case ValidItemType::Face: valid = iterateTest(id, &(items->faces)); break;
		case ValidItemType::Hair: valid = iterateTest(id, &(items->hair)); break;
		case ValidItemType::HairColor: valid = iterateTest(id, &(items->haircolor)); break;
		case ValidItemType::Skin: valid = iterateTest(id, &(items->skin)); break;
		case ValidItemType::Top: valid = iterateTest(id, &(items->top)); break;
		case ValidItemType::Bottom: valid = iterateTest(id, &(items->bottom)); break;
		case ValidItemType::Shoes: valid = iterateTest(id, &(items->shoes)); break;
		case ValidItemType::Weapon: valid = iterateTest(id, &(items->weapons)); break;
	}
	return valid;
}

bool ValidCharDataProvider::iterateTest(int32_t id, vector<int32_t> *test) {
	for (size_t i = 0; i < test->size(); i++) {
		if (id == (*test)[i]) {
			return true;
		}
	}
	return false;
}

ValidItems * ValidCharDataProvider::getItems(int8_t gender, bool cygnus) {
	ValidItems *vec = nullptr;
	if (gender == Gender::Male) {
		if (!cygnus) vec = &regmale;
		else vec = &cygnusmale;
	}
	else if (gender == Gender::Female) {
		if (!cygnus) vec = &regfemale;
		else vec = &cygnusfemale;
	}
	return vec;
}