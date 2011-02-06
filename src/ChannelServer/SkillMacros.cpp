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
#include "SkillMacros.h"
#include "Database.h"

void SkillMacros::load(int32_t charid) {
	mysqlpp::Query query = Database::getCharDB().query();
	query << "SELECT * FROM skillmacros WHERE charid = " << charid;
	mysqlpp::StoreQueryResult res = query.store();
	for (size_t i = 0; i < res.num_rows(); ++i) {
		int8_t pos = static_cast<int8_t>(res[i]["pos"]);

		string name;
		res[i]["name"].to_string(name);

		bool shout = static_cast<bool>(res[i]["shout"]);
		int32_t skill1 = static_cast<int32_t>(res[i]["skill1"]);
		int32_t skill2 = static_cast<int32_t>(res[i]["skill2"]);
		int32_t skill3 = static_cast<int32_t>(res[i]["skill3"]);
		add(pos, new SkillMacro(name, shout, skill1, skill2, skill3));
	}
}

void SkillMacros::save(int32_t charid) {
	mysqlpp::Query query = Database::getCharDB().query();
	query << "REPLACE INTO skillmacros VALUES ";
	int8_t max = getMax();
	for (int8_t i = 0; i <= max; i++) {
		SkillMacro *macro = getSkillMacro(i);
		if (macro != nullptr) {
			query << "(" << charid << ", "
				<< (int16_t) i << ", "
				<< mysqlpp::quote << macro->name << ", "
				<< macro->shout << ", "
				<< macro->skill1 << ", "
				<< macro->skill2<< ", "
				<< macro->skill3 << ")";
			if (i != max)
				query << ",";
		}
	}
	query.exec();
}
