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

#include "GameObjects.h"
#include "LoopingId.h"
#include "Types.h"
#include <boost/shared_ptr.hpp>
#include <boost/tr1/functional.hpp>
#include <boost/tr1/unordered_map.hpp>
#include <string>

using std::string;
using std::tr1::function;
using std::tr1::unordered_map;

class PacketCreator;
class Party;
class Player;

class PlayerDataProvider : boost::noncopyable {
public:
	static PlayerDataProvider * Instance() {
		if (singleton == nullptr)
			singleton = new PlayerDataProvider;
		return singleton;
	}
	void loadData();
	void getChannelConnectPacket(PacketCreator &packet);

	// Player info
	void playerConnect(Player *player, bool online = true);
	void playerDisconnect(int32_t id, int16_t channel = -1);
	void removeChannelPlayers(uint16_t channel);
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
	typedef unordered_map<int32_t, boost::shared_ptr<Party>> PartyMap;
	typedef unordered_map<int32_t, boost::shared_ptr<Player>> PlayerMap;

	void loadGuilds(int16_t worldId);
	void loadAlliances(int16_t worldId);
	void loadPlayers(int16_t worldId);

	unordered_map<int32_t, uint16_t> m_channelSwitches; // Channel changes
	LoopingId<int32_t> m_partyIds;
	PartyMap m_parties;
	PlayerMap m_players;
};