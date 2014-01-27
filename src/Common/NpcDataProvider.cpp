/*
Copyright (C) 2008-2014 Vana Development Team

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
#include "NpcDataProvider.hpp"
#include "Database.hpp"
#include "GameConstants.hpp"
#include "GameLogicUtilities.hpp"
#include "InitializeCommon.hpp"
#include "StringUtilities.hpp"
#include <iomanip>
#include <iostream>

auto NpcDataProvider::loadData() -> void {
	std::cout << std::setw(Initializing::OutputWidth) << std::left << "Initializing NPCs... ";
	int32_t id;
	NpcData npc;

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM npc_data");

	for (const auto &row : rs) {
		npc = NpcData();

		id = row.get<int32_t>("npcid");
		StringUtilities::runFlags(row.get<opt_string_t>("flags"), [&npc](const string_t &cmp) {
			if (cmp == "maple_tv") npc.isMapleTv = true;
			else if (cmp == "is_guild_rank") npc.isGuildRank = true;
		});
		npc.storageCost = row.get<int32_t>("storage_cost");
		m_data[id] = npc;
	}

	std::cout << "DONE" << std::endl;
}