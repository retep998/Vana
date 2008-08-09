/*
Copyright (C) 2008 Vana Development Team

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
#ifndef PLAYERS_H
#define PLAYERS_H

#include <hash_map>
#include <string>
#include <boost/function.hpp>

using std::string;
using stdext::hash_map;

class PacketCreator;
class Player;
class ReadPacket;

class Players {
public:
	static Players * Instance() {
		if (singleton == 0)
			singleton = new Players;
		return singleton;
	}

	void addPlayer(Player *player);
	void removePlayer(Player *player);
	Player * getPlayer(int id);
	Player * getPlayer(const string &name);

	void run(boost::function<void (Player *)> func);
	void sendPacket(PacketCreator &packet);
private:
	Players() {};
	Players(const Players&);
	Players& operator=(const Players&);
	static Players *singleton;

	hash_map<int, Player *> m_players;
	hash_map<string, Player *> m_players_names; // Index of players by name
};

#endif
