/*
Copyright (C) 2008-2012 Vana Development Team

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

#include "GameObjects.h"
#include "noncopyable.hpp"
#include "PlayerObjects.h"
#include "Types.h"
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

using std::string;
using std::function;
using std::unordered_map;

class PacketCreator;
class PacketReader;
class Party;
class Player;

class PlayerDataProvider : boost::noncopyable {
public:
	static PlayerDataProvider * Instance() {
		if (singleton == nullptr)
			singleton = new PlayerDataProvider;
		return singleton;
	}

	void parseChannelConnectPacket(PacketReader &packet);

	// Online players
	void addPlayer(Player *player);
	void changeChannel(PacketReader &packet);
	void newConnectable(PacketReader &packet);
	void deleteConnectable(int32_t id);
	void removePlayer(Player *player);
	Player * getPlayer(int32_t id);
	Player * getPlayer(const string &name);
	void run(function<void (Player *)> func);
	void sendPacket(PacketCreator &packet, int32_t minGmLevel = 0);

	// Player data
	PlayerData * getPlayerData(int32_t id);
	void updatePlayer(PacketReader &packet);

	// Parties
	Party * getParty(int32_t id);
	void newParty(int32_t id, int32_t leaderId);
	void disbandParty(int32_t id);
	void switchPartyLeader(int32_t id, int32_t leaderId);
	void removePartyMember(int32_t id, int32_t playerId, bool kicked);
	void addPartyMember(int32_t id, int32_t playerId);
private:
	PlayerDataProvider() {}
	static PlayerDataProvider *singleton;

	unordered_map<int32_t, std::shared_ptr<PlayerData>> m_playerData;
	unordered_map<int32_t, std::shared_ptr<Party>> m_parties;
	unordered_map<int32_t, Player *> m_players;
	unordered_map<string, Player *> m_playersByName;
};