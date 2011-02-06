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
#pragma once

#include "MapObjects.h"
#include "Pos.h"
#include "Player.h"
#include "PlayerNpc.h"
#include "Types.h"
#include <boost/utility.hpp>
#include <boost/tr1/unordered_map.hpp>
#include <vector>

using std::tr1::unordered_map;
using std::vector;

class Player;
class PlayerNpc;

class PlayerNpcDataProvider : boost::noncopyable {
public:
	static PlayerNpcDataProvider * Instance() {
		if (singleton == nullptr)
			singleton = new PlayerNpcDataProvider();
		return singleton;
	}

	void loadData();
	void loadPlayerNpc(int32_t id);
	void makePacket(vector<NpcSpawnInfo> &npcs, Player *player);
	void makePlayerNpc(Player *player);

	PlayerNpc * getPlayerNpc(int32_t npcId) { return (m_player_npcs.find(npcId) != m_player_npcs.end() ? m_player_npcs[npcId] : nullptr); }
	int32_t calculateNextNpcId(int16_t job);
	int32_t getNextNpcId(int32_t npcStart, int8_t maxNpcs);
	uint8_t getJobMaxLevel(int16_t jobid);
private:
	static PlayerNpcDataProvider *singleton;

	unordered_map<int32_t, PlayerNpc *> m_player_npcs;
	vector<int32_t> m_player_npcs_player_ids;
};

