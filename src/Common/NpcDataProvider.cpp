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
#include "NpcDataProvider.h"
#include "Database.h"
#include "GameConstants.h"
#include "GameLogicUtilities.h"
#include "InitializeCommon.h"
#include "StringUtilities.h"

using Initializing::outputWidth;
using StringUtilities::runFlags;

NpcDataProvider * NpcDataProvider::singleton = nullptr;

namespace Functors {
	struct FlagFunctor {
		void operator()(const string &cmp) {
			if (cmp == "maple_tv") npc->isMapleTv = true;
			else if (cmp == "is_guild_rank") npc->isGuildRank = true;
		}
		NpcData *npc;
	};
}

void NpcDataProvider::loadData() {
	std::cout << std::setw(outputWidth) << std::left << "Initializing NPCs... ";
	mysqlpp::Query query = Database::getDataDB().query("SELECT * FROM npc_data");
	mysqlpp::UseQueryResult res = query.use();
	int32_t id;
	NpcData npc;

	using namespace Functors;

	enum NpcFields {
		Id = 0,
		StorageCost, Flags
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		npc = NpcData();
		FlagFunctor func = {&npc};
		runFlags(row[Flags], func);
		id = atoi(row[Id]);

		npc.storageCost = atoi(row[StorageCost]);
		m_data[id] = npc;
	}

	std::cout << "DONE" << std::endl;
}