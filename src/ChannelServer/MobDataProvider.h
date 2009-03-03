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
#ifndef MOBDATA_H
#define MOBDATA_H

#include "Types.h"
#include <boost/tr1/unordered_map.hpp>
#include <vector>

using std::vector;
using std::tr1::unordered_map;

struct MobAttackInfo {
	MobAttackInfo() : id(0), level(0), mpconsume(0), mpburn(0), mobid(-1), deadlyattack(false) { }
	int8_t id;
	int8_t level;
	uint8_t disease;
	uint8_t mpconsume;
	uint16_t mpburn;
	int32_t mobid;
	bool deadlyattack;
};

struct MobInfo {
	uint8_t level;
	uint32_t hp;
	uint32_t mp;
	uint32_t hprecovery;
	uint32_t mprecovery;
	int32_t exp;
	bool boss;
	bool canfreeze;
	bool canpoison;
	int8_t hpcolor;
	int8_t hpbgcolor;
	vector<int32_t> summon;
	vector<MobAttackInfo> skills;
};

class MobDataProvider {
public:
	static MobDataProvider * Instance() {
		if (singleton == 0)
			singleton = new MobDataProvider();
		return singleton;
	}
	void loadData();
	bool mobExists(int32_t mobid) {
		return mobinfo.find(mobid) != mobinfo.end();
	}
	MobInfo const getMobInfo(int32_t mobid) {
		return mobinfo[mobid];
	}

private:
	MobDataProvider() {}
	MobDataProvider(const MobDataProvider&);
	MobDataProvider& operator=(const MobDataProvider&);
	static MobDataProvider *singleton;

	unordered_map<int32_t, MobInfo> mobinfo;
};

#endif
