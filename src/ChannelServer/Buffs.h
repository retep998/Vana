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
#include "BuffDataProvider.h"
#include <boost/array.hpp>
#include <vector>

using std::vector;

class Player;

struct ActiveBuff {
	ActiveBuff() : hasmapbuff(false) {
		for (size_t i = 0; i < BuffBytes::ByteQuantity; i++) {
			types[i] = 0;
		}
	}
	boost::array<uint8_t, BuffBytes::ByteQuantity> types;
	vector<int16_t> vals;
	bool hasmapbuff;
};

struct ActiveMapBuff {
	ActiveMapBuff() : debuff(false) {
		for (int8_t i = 0; i < BuffBytes::ByteQuantity; i++)
			typelist[i] = 0;
	}
	vector<uint8_t> bytes;
	vector<int8_t> types;
	vector<int16_t> values;
	vector<bool> usevals;
	boost::array<uint8_t, BuffBytes::ByteQuantity> typelist;
	bool debuff;
};

struct MapEntryVals {
	MapEntryVals() : use(false), debuff(false), val(0), skill(0) { }
	bool use;
	bool debuff;
	int16_t val;
	int16_t skill;
};

namespace Buffs {
	void addBuff(Player *player, int32_t itemid, int32_t time);
	bool addBuff(Player *player, int32_t skillid, uint8_t level, int16_t addedinfo, int32_t mapmobid = 0);
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
	ActiveMapBuff parseBuffMapEntryInfo(Player *player, int32_t skillid, uint8_t level);
	ActiveMapBuff parseMobBuffMapEntryInfo(Player *player, uint8_t skillid, uint8_t level);
	int16_t getValue(int8_t value, int32_t skillid, uint8_t level);
	int16_t getMobSkillValue(int8_t value, uint8_t skillid, uint8_t level);
	int32_t parseMountInfo(Player *player, int32_t skillid, uint8_t level);
};
