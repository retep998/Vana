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
#pragma once

#include "Types.h"
#include <boost/utility.hpp>
#include <string>
#include <vector>

using std::string;
using std::vector;

struct ValidItems {
	void clear() {
		hair.clear();
		haircolor.clear();
		faces.clear();
		skin.clear();
		top.clear();
		bottom.clear();
		shoes.clear();
		weapons.clear();
	}
	vector<int32_t> hair;
	vector<int32_t> haircolor;
	vector<int32_t> faces;
	vector<int32_t> skin;
	vector<int32_t> top;
	vector<int32_t> bottom;
	vector<int32_t> shoes;
	vector<int32_t> weapons;
};

namespace ValidItemType {
	enum {
		Face = 1,
		Hair = 2,
		HairColor = 3,
		Skin = 4,
		Top = 5,
		Bottom = 6,
		Shoes = 7,
		Weapon = 8
	};
}

struct ClassValidItems {
	void clear() {
		male.clear();
		female.clear();
	}
	ValidItems male;
	ValidItems female;
};

class ValidCharDataProvider : boost::noncopyable {
public:
	static ValidCharDataProvider * Instance() {
		if (singleton == nullptr)
			singleton = new ValidCharDataProvider();
		return singleton;
	}
	void loadData();

	bool isForbiddenName(const string &cmp);
	bool isValidCharacter(int8_t gender, int32_t hair, int32_t haircolor, int32_t eyes, int32_t skin, int32_t top, int32_t bottom, int32_t shoes, int32_t weapon, int8_t classId = Adventurer);

	const static int8_t Adventurer = 1;
	const static int8_t Cygnus = 2;
private:
	ValidCharDataProvider() {}
	static ValidCharDataProvider *singleton;

	void loadForbiddenNames();
	void loadCreationItems();

	bool isValidItem(int32_t id, int8_t gender, int8_t classId, int8_t type);
	bool iterateTest(int32_t id, vector<int32_t> *test);
	ValidItems * getItems(int8_t gender, int8_t classId);

	vector<string> m_forbiddenNames;
	ClassValidItems m_adventurer;
	ClassValidItems m_cygnus;
};
