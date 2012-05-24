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
#include "ValidCharDataProvider.h"
#include "Database.h"
#include "GameConstants.h"
#include "GameLogicUtilities.h"
#include "InitializeCommon.h"
#include "StringUtilities.h"
#include <iomanip>
#include <iostream>

using Initializing::OutputWidth;
using StringUtilities::runFlags;

ValidCharDataProvider * ValidCharDataProvider::singleton = nullptr;

void ValidCharDataProvider::loadData() {
	std::cout << std::setw(OutputWidth) << std::left << "Initializing Char Info... ";

	loadForbiddenNames();
	loadCreationItems();

	std::cout << "DONE" << std::endl;
}

void ValidCharDataProvider::loadForbiddenNames() {
	m_forbiddenNames.clear();

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM character_forbidden_names");

	for (soci::rowset<>::const_iterator i = rs.begin(); i != rs.end(); ++i) {
		soci::row const &row = *i;

		m_forbiddenNames.push_back(row.get<string>("forbidden_name"));
	}
}

void ValidCharDataProvider::loadCreationItems() {
	for (int8_t i = 0; i < ValidClass::MaxClass; i++) {
		m_validItems[i].clear();
	}

	int8_t gender;
	int8_t classId;
	int32_t objectId;
	string className;
	ValidItems *items;

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM character_creation_data");

	for (soci::rowset<>::const_iterator i = rs.begin(); i != rs.end(); ++i) {
		soci::row const &row = *i;

		gender = GameLogicUtilities::getGenderId(row.get<string>("gender"));
		runFlags(row.get<opt_string>("character_type"), [&classId](const string &cmp) {
			if (cmp == "explorer") classId = ValidClass::Adventurer;
			else if (cmp == "cygnus") classId = ValidClass::Cygnus;
			else if (cmp == "aran") classId = ValidClass::Aran;
			else if (cmp == "evan") classId = ValidClass::Evan;
			else if (cmp == "mercedes") classId = ValidClass::Mercedes;
			else if (cmp == "resistance") classId = ValidClass::Resistance;
			else if (cmp == "demon_slayer") classId = ValidClass::DemonSlayer;
			else if (cmp == "ultimate_explorer") classId = ValidClass::UltimateExplorer;
		});

		objectId = row.get<int32_t>("objectid");
		items = getItems(gender, classId);

		runFlags(row.get<opt_string>("object_type"), [&items, &objectId](const string &cmp) {
			if (cmp == "face") items->faces.push_back(objectId);
			else if (cmp == "hair") items->hair.push_back(objectId);
			else if (cmp == "haircolor") items->haircolor.push_back(objectId);
			else if (cmp == "skin") items->skin.push_back(objectId);
			else if (cmp == "top") items->top.push_back(objectId);
			else if (cmp == "bottom") items->bottom.push_back(objectId);
			else if (cmp == "shoes") items->shoes.push_back(objectId);
			else if (cmp == "weapon") items->weapons.push_back(objectId);
		});
	}
}

bool ValidCharDataProvider::isForbiddenName(const string &cmp) {
	bool forbidden = false;
	for (size_t i = 0; i < m_forbiddenNames.size(); i++) {
		if (cmp.find(m_forbiddenNames[i], 0) != string::npos) {
			forbidden = true;
			break;
		}
	}
	return forbidden;
}

bool ValidCharDataProvider::isValidCharacter(int8_t gender, int32_t hair, int32_t haircolor, int32_t eyes, int32_t skin, int32_t top, int32_t bottom, int32_t shoes, int32_t weapon, int8_t classId) {
	if (gender != Gender::Male && gender != Gender::Female) {
		return false;
	}
	bool valid = isValidItem(hair, gender, classId, ValidItemType::Hair);
	if (valid) valid = isValidItem(haircolor, gender, classId, ValidItemType::HairColor);
	if (valid) valid = isValidItem(eyes, gender, classId, ValidItemType::Face);
	if (valid) valid = isValidItem(skin, gender, classId, ValidItemType::Skin);
	if (valid) valid = isValidItem(top, gender, classId, ValidItemType::Top);
	if (valid) valid = isValidItem(bottom, gender, classId, ValidItemType::Bottom);
	if (valid) valid = isValidItem(shoes, gender, classId, ValidItemType::Shoes);
	if (valid) valid = isValidItem(weapon, gender, classId, ValidItemType::Weapon);
	return valid;
}

bool ValidCharDataProvider::isValidItem(int32_t id, int8_t gender, int8_t classId, int8_t type) {
	ValidItems *items = getItems(gender, classId);

	if (items == nullptr) {
		return false;
	}

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
	if (!valid) {
		std::cout << " !!! Non-valid item: " << id << "; gender: " << (int16_t)gender << "; classId: " << (int16_t)classId << "; Type: " << (int16_t)type << std::endl;
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

ValidItems * ValidCharDataProvider::getItems(int8_t gender, int8_t classId) {
	if (classId < 0 || classId >= ValidClass::MaxClass) {
		throw std::exception("Can't get class items when the classId is incorrect");
	}

	ValidItems *vec = nullptr;
	if (gender == Gender::Male) {
		return &m_validItems[classId].male;
	}
	else if (gender == Gender::Female) {
		return &m_validItems[classId].female;
	}
	return vec;
}