/*
Copyright (C) 2008-2009 Vana Development Team

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
#ifndef DROPDATA_H
#define DROPDATA_H

#include "Types.h"
#include <boost/tr1/unordered_map.hpp>
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

class DropDataProvider {
public:
	static DropDataProvider * Instance() {
		if (singleton == 0)
			singleton = new DropDataProvider();
		return singleton;
	}
	void loadData();
	DropsInfo const getDrops(int32_t objectid) {
		return dropdata[objectid];
	}

private:
	DropDataProvider() {}
	DropDataProvider(const DropDataProvider&);
	DropDataProvider& operator=(const DropDataProvider&);
	static DropDataProvider *singleton;

	unordered_map<int32_t, DropsInfo> dropdata;
};

#endif
