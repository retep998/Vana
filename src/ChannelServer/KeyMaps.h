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
#pragma once

#include "Types.h"
#include <unordered_map>

class KeyMaps {
	NONCOPYABLE(KeyMaps);
public:
	struct KeyMap;

	KeyMaps() = default;

	auto add(int32_t pos, const KeyMap &map) -> void;
	auto defaultMap() -> void;
	auto getKeyMap(int32_t pos) -> KeyMap *;
	auto getMax() -> int32_t;

	auto load(int32_t charId) -> void;
	auto save(int32_t charId) -> void;

	static const size_t size = 90;
private:
	hash_map_t<int32_t, KeyMap> keyMaps;
	int32_t m_maxValue = -1; // Cache max value
};

struct KeyMaps::KeyMap {
	KeyMap(int8_t type, int32_t action);
	KeyMap() = default;

	int8_t type = 0;
	int32_t action = 0;
};

inline
auto KeyMaps::add(int32_t pos, const KeyMap &map) -> void {
	keyMaps[pos] = map;
	if (m_maxValue < pos) {
		m_maxValue = pos;
	}
}

inline
auto KeyMaps::getKeyMap(int32_t pos) -> KeyMaps::KeyMap * {
	auto kvp = keyMaps.find(pos);
	if (kvp != std::end(keyMaps)) {
		return &kvp->second;
	}
	return nullptr;
}

inline
auto KeyMaps::getMax() -> int32_t {
	return m_maxValue;
}

inline
KeyMaps::KeyMap::KeyMap(int8_t type, int32_t action) : type(type), action(action) { }