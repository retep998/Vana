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
#include "Algorithm.hpp"
#include "Database.hpp"
#include "GameConstants.hpp"
#include "GameLogicUtilities.hpp"
#include "InitializeCommon.hpp"
#include "StringUtilities.hpp"
#include <iomanip>
#include <iostream>

auto NpcDataProvider::loadData() -> void {
	std::cout << std::setw(Initializing::OutputWidth) << std::left << "Initializing NPCs... ";

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM " << Database::makeDataTable("npc_data"));

	for (const auto &row : rs) {
		NpcData npc;
		npc_id_t id = row.get<npc_id_t>("npcid");
		npc.storageCost = row.get<mesos_t>("storage_cost");

		StringUtilities::runFlags(row.get<opt_string_t>("flags"), [&npc](const string_t &cmp) {
			if (cmp == "maple_tv") npc.isMapleTv = true;
			else if (cmp == "is_guild_rank") npc.isGuildRank = true;
		});

		m_data[id] = npc;
	}

	std::cout << "DONE" << std::endl;
}

auto NpcDataProvider::getStorageCost(npc_id_t npc) const -> mesos_t {
	return m_data.find(npc)->second.storageCost;
}

auto NpcDataProvider::isMapleTv(npc_id_t npc) const -> bool {
	return m_data.find(npc)->second.isMapleTv;
}

auto NpcDataProvider::isGuildRank(npc_id_t npc) const -> bool {
	return m_data.find(npc)->second.isGuildRank;
}

auto NpcDataProvider::isValidNpcId(npc_id_t npc) const -> bool {
	return ext::is_element(m_data, npc);
}