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

struct DropInfo {
	DropInfo() : isMesos(false), itemId(0), minAmount(0), maxAmount(0), questId(0), chance(0) { }
	bool isMesos;
	int32_t itemId;
	int32_t minAmount;
	int32_t maxAmount;
	int16_t questId;
	uint32_t chance;
};
typedef vector<DropInfo> DropsInfo;

struct GlobalDrop {
	GlobalDrop() : isMesos(false), itemId(0), minAmount(0), maxAmount(0), minLevel(1), maxLevel(200), continent(-1), questId(0), chance(0) { }
	bool isMesos;
	int32_t itemId;
	int32_t minAmount;
	int32_t maxAmount;
	uint8_t minLevel;
	uint8_t maxLevel;
	int8_t continent;
	int16_t questId;
	uint32_t chance;
};
typedef vector<GlobalDrop> GlobalDrops;

class DropDataProvider : boost::noncopyable {
public:
	static DropDataProvider * Instance() {
		if (singleton == nullptr)
			singleton = new DropDataProvider();
		return singleton;
	}
	void loadData();

	bool hasDrops(int32_t objectId) { return (m_dropInfo.find(objectId) != m_dropInfo.end()); }
	DropsInfo getDrops(int32_t objectId) { return m_dropInfo[objectId]; }
	GlobalDrops * getGlobalDrops() { return (m_globalDrops.size() > 0 ? &m_globalDrops : nullptr); }
private:
	DropDataProvider() {}
	static DropDataProvider *singleton;

	void loadDrops();
	void loadGlobalDrops();

	unordered_map<int32_t, DropsInfo> m_dropInfo;
	GlobalDrops m_globalDrops;
};