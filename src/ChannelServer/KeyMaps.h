/*
Copyright (C) 2008-2013 Vana Development Team

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
#include <memory>
#include <unordered_map>

using std::shared_ptr;
using std::unordered_map;

class KeyMaps {
public:
	struct KeyMap;

	KeyMaps();

	void add(int32_t pos, const KeyMap &map);
	void defaultMap();
	KeyMap * getKeyMap(int32_t pos);
	int32_t getMax();

	void load(int32_t charId);
	void save(int32_t charId);

	static const size_t size = 90;
private:
	unordered_map<int32_t, KeyMap> keyMaps;
	int32_t maxValue; // Cache max value
};

struct KeyMaps::KeyMap {
	KeyMap(int8_t type, int32_t action);
	KeyMap() {}
	int8_t type;
	int32_t action;
};

inline KeyMaps::KeyMaps() : maxValue(-1) { }

inline void KeyMaps::add(int32_t pos, const KeyMap &map) {
	keyMaps[pos] = map;
	if (maxValue < pos) {
		maxValue = pos;
	}
}

inline KeyMaps::KeyMap * KeyMaps::getKeyMap(int32_t pos) {
	auto kvp = keyMaps.find(pos);
	if (kvp != keyMaps.end()) {
		return &kvp->second;
	}
	return nullptr;
}

inline int32_t KeyMaps::getMax() {
	return maxValue;
}

inline KeyMaps::KeyMap::KeyMap(int8_t type, int32_t action) : type(type), action(action) { }