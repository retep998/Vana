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
	DropInfo() : ismesos(false), itemid(0), minamount(0), maxamount(0), questid(0), chance(0) { }
	bool ismesos;
	int32_t itemid;
	int32_t minamount;
	int32_t maxamount;
	int16_t questid;
	uint32_t chance;
};
typedef vector<DropInfo> DropsInfo;

struct GlobalDrop {
	GlobalDrop() : ismesos(false), itemid(0), minamount(0), maxamount(0), minlevel(1), maxlevel(200), continent(-1), questid(0), chance(0) { }
	bool ismesos;
	int32_t itemid;
	int32_t minamount;
	int32_t maxamount;
	uint8_t minlevel;
	uint8_t maxlevel;
	int8_t continent;
	int16_t questid;
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

	bool hasDrops(int32_t oid) { return (dropdata.find(oid) != dropdata.end()); }
	DropsInfo getDrops(int32_t objectid) { return dropdata[objectid]; }
	GlobalDrops * getGlobalDrops() { return (globaldrops.size() > 0 ? &globaldrops : nullptr); }
private:
	DropDataProvider() {}
	static DropDataProvider *singleton;

	void loadDrops();
	void loadGlobalDrops();

	unordered_map<int32_t, DropsInfo> dropdata;
	GlobalDrops globaldrops;
};
