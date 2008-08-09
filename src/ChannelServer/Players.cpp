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
#include "Players.h"
#include "PacketCreator.h"
#include "Player.h"
#include "StringUtilities.h"

Players * Players::singleton = 0;

void Players::addPlayer(Player *player) {
	m_players[player->getPlayerid()] = player;
	m_players_names[StringUtilities::toUpper(player->getName())] = player; // Store in upper case for easy non-case-sensitive search
}

void Players::removePlayer(Player *player) {
	m_players.erase(player->getPlayerid());
	m_players_names.erase(player->getName());
}

Player * Players::getPlayer(int id) {
	return (m_players.find(id) == m_players.end()) ? 0 : m_players[id];
}

Player * Players::getPlayer(const string &name) {
	string upCaseName = StringUtilities::toUpper(name);
	return (m_players_names.find(upCaseName) == m_players_names.end()) ? 0 : m_players_names[upCaseName];
}

void Players::run(boost::function<void (Player *)> func) {
	for (hash_map<int, Player *>::iterator iter = m_players.begin(); iter != m_players.end(); iter++) {
		func(iter->second);
	}
}

void Players::sendPacket(PacketCreator &packet) {
	for (hash_map<int, Player *>::iterator iter = m_players.begin(); iter != m_players.end(); iter++) {
		packet.send(iter->second);
	}
}
