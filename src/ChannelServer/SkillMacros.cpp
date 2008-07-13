/*
Copyright (C) 2008 Vana Development Team

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
#include "SkillMacros.h"
#include "MySQLM.h"

void SkillMacros::load(int charid) {
	mysqlpp::Query query = db.query();
	query << "SELECT * FROM skillmacros WHERE charid = " << mysqlpp::quote << charid;
	mysqlpp::StoreQueryResult res = query.store();
	for (size_t i = 0; i < res.num_rows(); ++i) {
		int pos = static_cast<int>(res[i]["pos"]);

		string name;
		res[i]["name"].to_string(name);

		bool shout = static_cast<bool>(res[i]["shout"]);
		int skill1 = static_cast<int>(res[i]["skill1"]);
		int skill2 = static_cast<int>(res[i]["skill2"]);
		int skill3 = static_cast<int>(res[i]["skill3"]);
		add(pos, new SkillMacro(name, shout, skill1, skill2, skill3));
	}
}

void SkillMacros::save(int charid) {
	mysqlpp::Query query = db.query();
	query << "REPLACE INTO skillmacros VALUES ";
	for (int i = 0; i <= getMax(); i++) {
		SkillMacro *macro = getSkillMacro(i);
		if (macro != 0) {
			query << "(" << mysqlpp::quote << charid << ", "
				<< mysqlpp::quote << i << ", "
				<< mysqlpp::quote << macro->name << ", "
				<< mysqlpp::quote << macro->shout << ", "
				<< mysqlpp::quote << macro->skill1 << ", "
				<< mysqlpp::quote << macro->skill2<< ", "
				<< mysqlpp::quote << macro->skill3 << ")";
			if (i != getMax()) {
				query << ",";
			}
		}
	}
	query.exec();
}
