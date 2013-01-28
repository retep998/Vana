/*
Copyright (C) 2008-2013 Vana Development Team

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
#include <iomanip>
#include <iostream>

using Initializing::OutputWidth;
using StringUtilities::runFlags;

NpcDataProvider * NpcDataProvider::singleton = nullptr;

void NpcDataProvider::loadData() {
	std::cout << std::setw(OutputWidth) << std::left << "Initializing NPCs... ";
	int32_t id;
	NpcData npc;

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM npc_data");

	for (soci::rowset<>::const_iterator i = rs.begin(); i != rs.end(); ++i) {
		const soci::row &row = *i;
		npc = NpcData();

		id = row.get<int32_t>("npcid");
		runFlags(row.get<opt_string>("flags"), [&npc](const string &cmp) {
			if (cmp == "maple_tv") npc.isMapleTv = true;
			else if (cmp == "is_guild_rank") npc.isGuildRank = true;
		});
		npc.storageCost = row.get<int32_t>("storage_cost");
		m_data[id] = npc;
	}

	std::cout << "DONE" << std::endl;
}