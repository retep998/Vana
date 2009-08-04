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
#include "SkillDataProvider.h"
#include "Database.h"
#include "InitializeCommon.h"
#include "MiscUtilities.h"

using Initializing::outputWidth;

SkillDataProvider * SkillDataProvider::singleton = 0;

void SkillDataProvider::loadData() {
	std::cout << std::setw(outputWidth) << std::left << "Initializing Skills... ";

	mysqlpp::Query query = Database::getDataDB().query("SELECT * FROM skilldata");
	mysqlpp::UseQueryResult res = query.use();

	MYSQL_ROW skillRow;
	SkillLevelInfo level;
	maxlevels.clear();
	skills.clear();

	enum SkillRows {
		SkillId = 0,
		Level, MobCount, HitCount, Time, Mp,
		Hp, Damage, Item, ItemCount, BulletCon,
		MoneyCon, X, Y, Speed, Jump,
		Watk, Wdef, Matk, Mdef, Acc,
		Avoid, HpP, MpP, Prop, Morph,
		LTX, LTY, RBX, RBY, CoolTime
	};

	while (skillRow = res.fetch_raw_row()) {
		int32_t skillid = atoi(skillRow[SkillId]);
		uint8_t skilllevel = atoi(skillRow[Level]);

		level.mobcount = atoi(skillRow[MobCount]);
		level.hitcount = atoi(skillRow[HitCount]);
		level.time = atoi(skillRow[Time]);
		level.mp = atoi(skillRow[Mp]);
		level.hp = atoi(skillRow[Hp]);
		level.damage = atoi(skillRow[Damage]);
		level.item = atoi(skillRow[Item]);
		level.itemcount = atoi(skillRow[ItemCount]);
		level.bulletcon = atoi(skillRow[BulletCon]);
		level.moneycon = atoi(skillRow[MoneyCon]);
		level.x = atoi(skillRow[X]);
		level.y = atoi(skillRow[Y]);
		level.speed = atoi(skillRow[Speed]);
		level.jump = atoi(skillRow[Jump]);
		level.watk = atoi(skillRow[Watk]);
		level.wdef = atoi(skillRow[Wdef]);
		level.matk = atoi(skillRow[Matk]);
		level.mdef = atoi(skillRow[Mdef]);
		level.acc = atoi(skillRow[Acc]);
		level.avo = atoi(skillRow[Avoid]);
		level.hpP = atoi(skillRow[HpP]);
		level.mpP = atoi(skillRow[MpP]);
		level.prop = atoi(skillRow[Prop]);
		level.morph = atoi(skillRow[Morph]);
		level.lt = Pos(atoi(skillRow[LTX]), atoi(skillRow[LTY]));
		level.rb = Pos(atoi(skillRow[RBX]), atoi(skillRow[RBY]));
		level.cooltime = atoi(skillRow[CoolTime]);

		skills[skillid][skilllevel] = level;
		if (maxlevels.find(skillid) == maxlevels.end() || maxlevels[skillid] < skilllevel) {
			maxlevels[skillid] = skilllevel;
		}
	}

	query << "SELECT mobskills.*, mobskillsummons.mobid FROM mobskills LEFT JOIN mobskillsummons ON mobskills.level = mobskillsummons.level AND mobskills.skillid = 200 ORDER BY skillid ASC";
	res = query.use();

	uint8_t currentid = 0;
	uint8_t previousid = -1;
	uint8_t currentlevel = 0;
	uint8_t previouslevel = -1;
	bool haspreviouslevel = false;
	bool haspreviousskill = false;
	mobskills.clear();

	MobSkillLevelInfo moblevel;

	enum MobSkillRows {
		MobSkillId = 0,
		MobSkillLevel, MobTime, MobMp, MobX, MobY,
		MobProp, Count, Interval, MobLTX, MobRBX,
		MobLTY, MobRBY, MobHp, MobLimit, SummonEffect,
		SummonId
	};
	while (skillRow = res.fetch_raw_row()) {
		currentid = atoi(skillRow[MobSkillId]);
		currentlevel = atoi(skillRow[MobSkillLevel]);

		if ((currentid != previousid && haspreviousskill) || (currentlevel != previouslevel && haspreviousskill)) { // Add the items into the cache
			mobskills[previousid][previouslevel] = moblevel;
			moblevel.summons.clear();
		}

		moblevel.time = atoi(skillRow[MobTime]);
		moblevel.mp = atoi(skillRow[MobMp]);
		moblevel.x = atoi(skillRow[MobX]);
		moblevel.y = atoi(skillRow[MobY]);
		moblevel.prop = atoi(skillRow[MobProp]);
		moblevel.count = atoi(skillRow[Count]);
		moblevel.interval = atoi(skillRow[Interval]);
		moblevel.lt.x = atoi(skillRow[MobLTX]);
		moblevel.rb.x = atoi(skillRow[MobRBX]);
		moblevel.lt.y = atoi(skillRow[MobLTY]);
		moblevel.rb.y = atoi(skillRow[MobRBY]);
		moblevel.hp = atoi(skillRow[MobHp]);
		moblevel.limit = atoi(skillRow[MobLimit]);
		moblevel.summoneffect = atoi(skillRow[SummonEffect]);

		if (skillRow[SummonId] != 0) {
			moblevel.summons.push_back(atoi(skillRow[SummonId]));
		}

		previousid = currentid;
		previouslevel = currentlevel;
		haspreviouslevel = true;
		haspreviousskill = true;
	}
	if (haspreviouslevel) {
		mobskills[previousid][previouslevel] = moblevel;
	}

	std::cout << "DONE" << std::endl;
}

SkillLevelInfo * SkillDataProvider::getSkill(int32_t skill, uint8_t level) {
	if (skills.find(skill) != skills.end()) {
		if (skills[skill].find(level) != skills[skill].end()) {
			return &skills[skill][level];
		}
		return 0;
	}
	return 0;
}

MobSkillLevelInfo * SkillDataProvider::getMobSkill(uint8_t skill, uint8_t level) {
	if (mobskills.find(skill) != mobskills.end()) {
		if (mobskills[skill].find(level) != mobskills[skill].end()) {
			return &mobskills[skill][level];
		}
		return 0;
	}
	return 0;
}