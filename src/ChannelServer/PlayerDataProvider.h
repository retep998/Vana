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
#include "Types.h"
#include <boost/shared_ptr.hpp>
#include <boost/tr1/functional.hpp>
#include <boost/tr1/unordered_map.hpp>
#include <boost/utility.hpp>
#include <string>

using std::string;
using std::tr1::function;
using std::tr1::unordered_map;

class PacketCreator;
class PacketReader;
class Party;
class Player;

struct Guild {
	Guild(int32_t id, const string &name, const GuildLogo &logo, int32_t capacity, int32_t allianceid) :
	id(id),
	name(name),
	logo(logo),
	capacity(capacity),
	allianceid(allianceid) { };

	int32_t id;
	int32_t capacity;
	int32_t allianceid;
	string name;
	GuildLogo logo;
};

struct Alliance {
	Alliance(int32_t id, const string &name, int32_t capacity) :
	id(id),
	name(name),
	capacity(capacity) { };

	int32_t id;
	int32_t capacity;
	string name;
};

class PlayerDataProvider : boost::noncopyable {
public:
	static PlayerDataProvider * Instance() {
		if (singleton == nullptr)
			singleton = new PlayerDataProvider;
		return singleton;
	}

	// Incoming packets
	void parseIncomingPacket(PacketReader &packet);
	void removePacket(int32_t id);
	bool checkPlayer(int32_t id);
	PacketReader & getPacket(int32_t id);

	// Players
	void addPlayer(Player *player);
	void removePlayer(Player *player);
	Player * getPlayer(int32_t id);
	Player * getPlayer(const string &name);
	void run(function<void (Player *)> func);
	void sendPacket(PacketCreator &packet, int32_t minGmLevel = 0);

	// Guilds
	void addGuild(int32_t id, const string &name, const GuildLogo &logo, int32_t capacity, int32_t allianceid);
	Guild * getGuild(int32_t id);
	Guild * getGuild(const string &name);
	void addGuild(int32_t id, Guild *gi);
	void loadGuild(int32_t id);
	void unloadGuild(int32_t id);
	bool hasEmblem(int32_t id);

	// Alliances
	void addAlliance(int32_t id, const string &name, int32_t capacity);
	Alliance * getAlliance(int32_t id);
	void addAlliance(int32_t id, Alliance *alliance);
	void loadAlliance(int32_t id);
	void unloadAlliance(int32_t id);

	// Parties
	Party * getParty(int32_t id);
	void addParty(Party *party);
	void removeParty(int32_t id);
private:
	PlayerDataProvider() {};
	static PlayerDataProvider *singleton;

	unordered_map<int32_t, boost::shared_ptr<PacketReader> > m_packets;
	unordered_map<int32_t, Player *> m_players;
	unordered_map<int32_t, Guild *> m_guilds;
	unordered_map<int32_t, Alliance *> m_alliances;
	unordered_map<int32_t, Party *> m_parties;
	unordered_map<string, Player *> m_players_names; // Index of players by name
	unordered_map<string, Guild *> m_guilds_names;
};
