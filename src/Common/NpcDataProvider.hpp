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
#pragma once

#include "Types.hpp"
#include <unordered_map>

struct NpcData {
	bool isMapleTv = false;
	bool isGuildRank = false;
	int32_t storageCost = 0;
};

class NpcDataProvider {
	SINGLETON(NpcDataProvider);
public:
	auto loadData() -> void;

	auto getStorageCost(int32_t npc) const -> int32_t;
	auto isMapleTv(int32_t npc) const -> bool;
	auto isGuildRank(int32_t npc) const -> bool;
	auto isValidNpcId(int32_t npc) const -> bool;
private:
	hash_map_t<int32_t, NpcData> m_data;
};