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
#ifndef BUFFS_H
#define BUFFS_H

#include "GameConstants.h"
#include "Types.h"
#include <boost/tr1/unordered_map.hpp>
#include <boost/utility.hpp>
#include <vector>

using std::tr1::unordered_map;
using std::vector;

class Player;

struct Buff {
	Buff() : type(0), byte(0), value(0) { }
	uint8_t type;
	int8_t byte;
	int8_t value;
};

struct BuffInfo {
	BuffInfo() : itemval(0), hasmapval(false), hasmapentry(false), useval(false) { }
	Buff buff;
	int16_t itemval;
	bool hasmapval;
	bool hasmapentry;
	bool useval;
};

struct BuffMapInfo {
	BuffMapInfo() : useval(false) { }
	Buff buff;
	bool useval;
};

struct BuffAct {
	Act type;
	int8_t value;
	int32_t time;
};

struct SkillInfo {
	vector<BuffInfo> player;
	vector<BuffMapInfo> map;
	BuffAct act;
	bool bact;
};

struct MobAilmentInfo {
	vector<BuffInfo> mob;
	int16_t delay;
	BuffAct act;
	bool bact;
};

struct ActiveBuff {
	ActiveBuff() : hasmapbuff(false) {
		for (size_t i = 0; i < 8; i++) {
			types[i] = 0;
		}
	}
	uint8_t types[8];
	vector<int16_t> vals;
	bool hasmapbuff;
};

struct ActiveMapBuff {
	ActiveMapBuff() : debuff(false) {
		for (int8_t i = 0; i < 8; i++)
			typelist[i] = 0;
	}
	vector<uint8_t> bytes;
	vector<int8_t> types;
	vector<int16_t> values;
	vector<bool> usevals;
	uint8_t typelist[8];
	bool debuff;
};

struct MapEntryVals {
	MapEntryVals() : use(false), debuff(false), val(0), skill(0) { }
	bool use;
	bool debuff;
	int16_t val;
	int16_t skill;
};

struct MapEntryBuffs {
	MapEntryBuffs() : mountid(0), mountskill(0) {
		for (int8_t i = 0; i < 8; i++) {
			types[i] = 0;
		}
	}
	uint8_t types[8];
	unordered_map<int8_t, unordered_map<uint8_t, MapEntryVals> > values;

	int32_t mountid;
	int32_t mountskill;
};

typedef unordered_map<int8_t, unordered_map<uint8_t, int32_t> > ActiveBuffsByType; // Used to determine which buffs are affecting which bytes so they can be properly overwritten

class Buffs : boost::noncopyable {
public:
	static Buffs * Instance() {
		if (singleton == 0)
			singleton = new Buffs();
		return singleton;
	}
	void addItemInfo(int32_t itemid, const vector<uint8_t> &types, const vector<int8_t> &bytes, const vector<int16_t> &values);
	void addBuff(Player *player, int32_t itemid, int32_t time);
	bool addBuff(Player *player, int32_t skillid, uint8_t level, int16_t addedinfo);
	void endBuff(Player *player, int32_t skill);
	void doAct(Player *player, int32_t skillid, uint8_t level);
	void addDebuff(Player *player, uint8_t skillid, uint8_t level);
	void endDebuff(Player *player, uint8_t skillid);

	ActiveBuff parseBuffInfo(Player *player, int32_t skillid, uint8_t level);
	ActiveMapBuff parseBuffMapInfo(Player *player, int32_t skillid, uint8_t level);
	vector<Buff> parseBuffs(int32_t skillid, uint8_t level);
	ActiveBuff parseMobBuffInfo(Player *player, uint8_t skillid, uint8_t level);
	ActiveMapBuff parseMobBuffMapInfo(Player *player, uint8_t skillid, uint8_t level);
	vector<Buff> parseMobBuffs(uint8_t skillid);
private:
	Buffs();
	static Buffs *singleton;

	unordered_map<int32_t, SkillInfo> skillsinfo;
	unordered_map<uint8_t, MobAilmentInfo> mobskillsinfo;

	ActiveMapBuff parseBuffMapEntryInfo(Player *player, int32_t skillid, uint8_t level);
	ActiveMapBuff parseMobBuffMapEntryInfo(Player *player, uint8_t skillid, uint8_t level);
	int16_t getValue(int8_t value, int32_t skillid, uint8_t level);
	int16_t getMobSkillValue(int8_t value, uint8_t skillid, uint8_t level);
	int32_t parseMountInfo(Player *player, int32_t skillid, uint8_t level);
};

#endif
