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
#include "Inventory.h"
#include "MiscUtilities.h"
#include <iostream>
#include <string>

using MiscUtilities::atob;
using Initializing::outputWidth;
using std::string;

MobDataProvider * MobDataProvider::singleton = 0;

void MobDataProvider::loadData() {
	std::cout << std::setw(outputWidth) << std::left << "Initializing Mobs... ";
	mysqlpp::Query query = Database::getDataDB().query("SELECT mobdata.mobid, mobdata.level, mobdata.hp, mobdata.mp, mobdata.elemAttr, mobdata.hprecovery, mobdata.mprecovery, mobdata.selfdestruction, mobdata.exp, mobdata.boss, mobdata.hpcolor, mobdata.hpbgcolor, mobdata.undead, mobsummondata.summonid FROM mobdata LEFT JOIN mobsummondata ON mobdata.mobid=mobsummondata.mobid ORDER BY mobdata.mobid ASC");
	mysqlpp::UseQueryResult res = query.use();

	MYSQL_ROW mobRow;
	while ((mobRow = res.fetch_raw_row())) {
		// Col0 : Mob ID
		//    1 : Level
		//    2 : HP
		//    3 : MP
		//    4 : Elemental Attributes
		//    5 : HP Recovery
		//    6 : MP Recovery
		//    7 : Self-Destruction HP
		//    8 : EXP
		//    9 : Boss
		//   10 : HP Color
		//   11 : HP BG Color
		//   12 : Undead?
		//   13 : Mob Summon
		int32_t mobid = atoi(mobRow[0]);

		if (mobinfo.find(mobid) == mobinfo.end()) {
			MobInfo mob = MobInfo();
			mob.level = atoi(mobRow[1]);
			mob.hp = atoi(mobRow[2]);
			mob.mp = atoi(mobRow[3]);
			string elemattr(mobRow[4]);
			mob.hprecovery = atoi(mobRow[5]);
			mob.mprecovery = atoi(mobRow[6]);
			mob.selfdestruction = atoi(mobRow[7]);
			mob.exp = atoi(mobRow[8]);
			mob.boss = atob(mobRow[9]);
			mob.hpcolor = atoi(mobRow[10]);
			mob.hpbgcolor = atoi(mobRow[11]);
			mob.undead = atob(mobRow[12]);

			mob.canfreeze = (!mob.boss && elemattr.find("I2") == string::npos && elemattr.find("I1") == string::npos);
			mob.canpoison = (!mob.boss && elemattr.find("S2") == string::npos && elemattr.find("S1") == string::npos);

			mobinfo[mobid] = mob;
		}

		if (mobRow[13] != 0) {
			mobinfo[mobid].summon.push_back(atoi(mobRow[13]));
		}

	}

	query << "SELECT mobid, attackid, mpconsume, mpburn, disease, level, deadly FROM mobattackdata";
	res = query.use();

	while ((mobRow = res.fetch_raw_row())) {
		// Col0 : Mob ID
		//    1 : Attack ID
		//    2 : MP Consumption
		//    3 : MP Burn
		//    4 : Disease
		//    5 : Level
		//    6 : Deadly
		MobAttackInfo mobattack;
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

	while ((mobRow = res.fetch_raw_row())) {
		// Col0 : Mob ID
		//    1 : Skill ID
		//    2 : Level
		//    3 : EffectAfter
		MobSkillInfo mobskill;
		mobskill.skillid = atoi(mobRow[1]);
		mobskill.level = atoi(mobRow[2]);
		mobskill.effectAfter = atoi(mobRow[3]);
		mobinfo[atoi(mobRow[0])].skills.push_back(mobskill);
	}
	std::cout << "DONE" << std::endl;
}
