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
#include "MobDataProvider.h"
#include "Database.h"
#include "InitializeCommon.h"
#include "MiscUtilities.h"
#include <iostream>
#include <string>

using MiscUtilities::atob;
using Initializing::outputWidth;
using std::string;

MobDataProvider * MobDataProvider::singleton = 0;

void MobDataProvider::loadData() {
	std::cout << std::setw(outputWidth) << std::left << "Initializing Mobs... ";
	mobinfo.clear();
	mysqlpp::Query query = Database::getDataDB().query("SELECT mobdata.*, mobsummondata.summonid FROM mobdata LEFT JOIN mobsummondata ON mobdata.mobid = mobsummondata.mobid ORDER BY mobdata.mobid ASC");
	mysqlpp::UseQueryResult res = query.use();

	MYSQL_ROW mobRow;
	MobInfo mob;
	enum MobData {
		MobId = 0,
		Level, Hp, Mp, HpRecovery, MpRecovery,
		SelfDestruct, Exp, Link, Buff, RemoveAfter,
		Boss, Undead, Flying, Friendly, PublicReward,
		ExplosiveReward, HpBar, HpBarBg, ElemAttr, Summon
	};

	while (mobRow = res.fetch_raw_row()) {
		int32_t mobid = atoi(mobRow[MobId]);

		if (mobinfo.find(mobid) == mobinfo.end()) {
			mob = MobInfo();
			mob.level = atoi(mobRow[Level]);
			mob.hp = atoi(mobRow[Hp]);
			mob.mp = atoi(mobRow[Mp]);
			mob.hprecovery = atoi(mobRow[HpRecovery]);
			mob.mprecovery = atoi(mobRow[MpRecovery]);
			mob.selfdestruction = atoi(mobRow[SelfDestruct]);
			mob.exp = atoi(mobRow[Exp]);
			mob.link = atoi(mobRow[Link]);
			mob.buff = atoi(mobRow[Buff]);
			mob.removeafter = atoi(mobRow[RemoveAfter]);

			mob.boss = atob(mobRow[Boss]);
			mob.undead = atob(mobRow[Undead]);
			mob.flying = atob(mobRow[Flying]);
			mob.friendly = atob(mobRow[Friendly]);
			mob.publicreward = atob(mobRow[PublicReward]);
			mob.explosivereward = atob(mobRow[ExplosiveReward]);

			mob.hpcolor = atoi(mobRow[HpBar]);
			mob.hpbgcolor = atoi(mobRow[HpBarBg]);

			string elemattr(mobRow[ElemAttr]);

			mob.canfreeze = (!mob.boss && elemattr.find("I2") == string::npos && elemattr.find("I1") == string::npos);
			mob.canpoison = (!mob.boss && elemattr.find("S2") == string::npos && elemattr.find("S1") == string::npos);

			mobinfo[mobid] = mob;
		}

		if (mobRow[Summon] != 0) {
			mobinfo[mobid].summon.push_back(atoi(mobRow[Summon]));
		}

	}

	query << "SELECT mobid, attackid, mpconsume, mpburn, disease, level, deadly FROM mobattackdata";
	res = query.use();
	MobAttackInfo mobattack;

	while (mobRow = res.fetch_raw_row()) {
		// Col0 : Mob ID
		//    1 : Attack ID
		//    2 : MP Consumption
		//    3 : MP Burn
		//    4 : Disease
		//    5 : Level
		//    6 : Deadly

		mobattack.id = atoi(mobRow[1]);
		mobattack.mpconsume = atoi(mobRow[2]);
		mobattack.mpburn = atoi(mobRow[3]);
		mobattack.disease = atoi(mobRow[4]);
		mobattack.level = atoi(mobRow[5]);
		mobattack.deadlyattack = atob(mobRow[6]);
		mobinfo[atoi(mobRow[0])].attacks.push_back(mobattack);
	}

	query << "SELECT mobid, skillid, level, effectAfter FROM mobskilldata";
	res = query.use();
	MobSkillInfo mobskill;

	while (mobRow = res.fetch_raw_row()) {
		// Col0 : Mob ID
		//    1 : Skill ID
		//    2 : Level
		//    3 : EffectAfter

		mobskill.skillid = atoi(mobRow[1]);
		mobskill.level = atoi(mobRow[2]);
		mobskill.effectAfter = atoi(mobRow[3]);
		mobinfo[atoi(mobRow[0])].skills.push_back(mobskill);
	}

	query << "SELECT * FROM banfields";
	res = query.use();
	banishinfo.clear();
	BanishField banish;

	while (mobRow = res.fetch_raw_row()) {
		// Col0 : Mob ID
		//    1 : Message
		//    2 : Field
		//    3 : Portal

		banish.message = mobRow[1];
		banish.field = atoi(mobRow[2]);
		banish.portal = mobRow[3];
		banishinfo[atoi(mobRow[0])] = banish;
	}

	std::cout << "DONE" << std::endl;
}

MobAttackInfo * MobDataProvider::getMobAttack(int32_t mobid, uint8_t type) {
	try {
		return &mobinfo[mobid].attacks.at(type);
	}
	catch (std::out_of_range) {

	}
	return 0;
}

MobSkillInfo * MobDataProvider::getMobSkill(int32_t mobid, uint8_t index) {
	try {
		return &mobinfo[mobid].skills.at(index);
	}
	catch (std::out_of_range) {

	}
	return 0;
}