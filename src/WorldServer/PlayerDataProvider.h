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
#include "LoopingId.h"
#include "Types.h"
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

using std::function;
using std::string;
using std::unordered_map;

class PacketCreator;
class Party;
class Player;
namespace soci {
	class row;
}

class PlayerDataProvider : boost::noncopyable {
public:
	static PlayerDataProvider * Instance() {
		if (singleton == nullptr)
			singleton = new PlayerDataProvider;
		return singleton;
	}
	void loadData();
	void getChannelConnectPacket(PacketCreator &packet);
	void getPlayerDataPacket(PacketCreator &packet, int32_t playerId);

	// Player info
	void initialPlayerConnect(int32_t id, uint16_t channel, ip_t ip);
	void playerConnect(Player *player, bool online = true);
	void playerDisconnect(int32_t id, int16_t channel = -1);
	void removeChannelPlayers(uint16_t channel);
	void loadPlayer(int32_t playerId);
	Player * getPlayer(const string &name, bool includeOffline = false);
	Player * getPlayer(int32_t id, bool includeOffline = false);
	int32_t getPlayerQuantity();

	// Channel changes
	void addPendingPlayer(int32_t id, uint16_t channelId);
	void removePendingPlayer(int32_t id);
	int16_t removePendingPlayerEarly(int32_t id);
	uint16_t getPendingPlayerChannel(int32_t id);

	// Parties
	int32_t getPartyId();
	void createParty(int32_t playerId);
	void addPartyMember(int32_t playerId);
	void removePartyMember(int32_t playerId, int32_t target);
	void removePartyMember(int32_t playerId);
	void setPartyLeader(int32_t playerId, int32_t leaderId);
	void disbandParty(int32_t id);
	Party * getParty(int32_t id);
private:
	PlayerDataProvider();
	static PlayerDataProvider *singleton;
	typedef unordered_map<int32_t, std::shared_ptr<Party>> PartyMap;
	typedef unordered_map<int32_t, std::shared_ptr<Player>> PlayerMap;

	void loadGuilds(int16_t worldId);
	void loadAlliances(int16_t worldId);
	void loadPlayers(int16_t worldId);
	void loadPlayer(const soci::row &row);
	void generatePlayerDataPacket(PacketCreator &packet, Player *player);

	unordered_map<int32_t, uint16_t> m_channelSwitches; // Channel changes
	LoopingId<int32_t> m_partyIds;
	PartyMap m_parties;
	PlayerMap m_players;
};