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
#include <boost/tr1/memory.hpp>
#include <boost/tr1/unordered_map.hpp>

using std::tr1::shared_ptr;
using std::tr1::unordered_map;

class KeyMaps {
public:
	struct KeyMap;

	KeyMaps();

	void add(int32_t pos, KeyMap *map);
	void defaultMap();
	KeyMap * getKeyMap(int32_t pos);
	int32_t getMax();

	void load(int32_t charid);
	void save(int32_t charid);

	static const size_t size = 90;
private:
	unordered_map<int32_t, shared_ptr<KeyMap> > keyMaps;
	int32_t maxValue; // Cache max value
};

struct KeyMaps::KeyMap {
	KeyMap(int8_t type, int32_t action);
	int8_t type;
	int32_t action;
};

inline KeyMaps::KeyMaps() : maxValue(-1) { }

inline void KeyMaps::add(int32_t pos, KeyMap *map) {
	keyMaps[pos].reset(map);
	if (maxValue < pos) {
		maxValue = pos;
	}
}

inline KeyMaps::KeyMap * KeyMaps::getKeyMap(int32_t pos) {
	if (keyMaps.find(pos) != keyMaps.end()) {
		return keyMaps[pos].get();
	}
	return nullptr;
}

inline int32_t KeyMaps::getMax() {
	return maxValue;
}

inline KeyMaps::KeyMap::KeyMap(int8_t type, int32_t action) : type(type), action(action) { }
