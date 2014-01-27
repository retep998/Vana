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
#include "ValidCharDataProvider.h"
#include "Database.h"
#include "GameConstants.h"
#include "GameLogicUtilities.h"
#include "InitializeCommon.h"
#include "StringUtilities.h"
#include <iomanip>
#include <iostream>

auto ValidCharDataProvider::loadData() -> void {
	std::cout << std::setw(Initializing::OutputWidth) << std::left << "Initializing Char Info... ";

	loadForbiddenNames();
	loadCreationItems();

	std::cout << "DONE" << std::endl;
}

auto ValidCharDataProvider::loadForbiddenNames() -> void {
	m_forbiddenNames.clear();

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM character_forbidden_names");

	for (const auto &row : rs) {
		m_forbiddenNames.push_back(row.get<string_t>("forbidden_name"));
	}
}

auto ValidCharDataProvider::loadCreationItems() -> void {
	m_adventurer.clear();
	m_cygnus.clear();

	int8_t gender;
	int8_t classId;
	int32_t objectId;
	string_t className;
	ValidItems *items;

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM character_creation_data");

	for (const auto &row : rs) {
		gender = GameLogicUtilities::getGenderId(row.get<string_t>("gender"));
		StringUtilities::runEnum(row.get<string_t>("character_type"), [&classId](const string_t &cmp) {
			if (cmp == "regular") classId = Adventurer;
			else if (cmp == "cygnus") classId = Cygnus;
		});

		objectId = row.get<int32_t>("objectid");
		items = getItems(gender, classId);

		StringUtilities::runEnum(row.get<string_t>("object_type"), [&items, &objectId](const string_t &cmp) {
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

auto ValidCharDataProvider::isForbiddenName(const string_t &cmp) -> bool {
	string_t c = StringUtilities::removeSpaces(StringUtilities::toLower(cmp));
	bool forbidden = (std::end(m_forbiddenNames) != std::find_if(std::begin(m_forbiddenNames), std::end(m_forbiddenNames),
		[&c](const string_t &s) -> bool {
			return c.find(s, 0) != string_t::npos;
		})
	);
	return forbidden;
}

auto ValidCharDataProvider::isValidCharacter(int8_t gender, int32_t hair, int32_t haircolor, int32_t eyes, int32_t skin, int32_t top, int32_t bottom, int32_t shoes, int32_t weapon, int8_t classId) -> bool {
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

auto ValidCharDataProvider::isValidItem(int32_t id, int8_t gender, int8_t classId, int8_t type) -> bool {
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
	return valid;
}

auto ValidCharDataProvider::iterateTest(int32_t id, vector_t<int32_t> *test) -> bool {
	for (size_t i = 0; i < test->size(); i++) {
		if (id == (*test)[i]) {
			return true;
		}
	}
	return false;
}

auto ValidCharDataProvider::getItems(int8_t gender, int8_t classId) -> ValidItems * {
	ValidItems *vec = nullptr;
	if (gender == Gender::Male) {
		switch (classId) {
			case Adventurer: vec = &m_adventurer.male; break;
			case Cygnus: vec = &m_cygnus.male; break;
		}
	}
	else if (gender == Gender::Female) {
		switch (classId) {
			case Adventurer: vec = &m_adventurer.female; break;
			case Cygnus: vec = &m_cygnus.female; break;
		}
	}
	return vec;
}