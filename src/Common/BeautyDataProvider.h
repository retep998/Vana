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
#include <boost/tr1/unordered_map.hpp>
#include <boost/utility.hpp>
#include <vector>

using std::tr1::unordered_map;
using std::vector;

struct ValidLook {
	void clear() {
		hair.clear();
		faces.clear();
	}
	vector<int32_t> hair;
	vector<int32_t> faces;
};

class BeautyDataProvider : boost::noncopyable {
public:
	static BeautyDataProvider * Instance() {
		if (singleton == nullptr)
			singleton = new BeautyDataProvider();
		return singleton;
	}
	void loadData();

	int8_t getRandomSkin();
	int32_t getRandomHair(int8_t gender);
	int32_t getRandomFace(int8_t gender);

	vector<int8_t> getSkins();
	vector<int32_t> getHair(int8_t gender);
	vector<int32_t> getFaces(int8_t gender);

	bool isValidSkin(int8_t skin);
	bool isValidHair(int8_t gender, int32_t hair);
	bool isValidFace(int8_t gender, int32_t face);
private:
	BeautyDataProvider() {}
	static BeautyDataProvider *singleton;

	void loadSkins();
	void loadHair();
	void loadFaces();
	ValidLook * getGender(int8_t gender);

	ValidLook male;
	ValidLook female;
	vector<int8_t> skins;
};
