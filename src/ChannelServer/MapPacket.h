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
#ifndef MAPPACKET_H
#define MAPPACKET_H

#include <string>

using std::string;

class Player;
class PacketCreator;

namespace MapPacket {
	PacketCreator playerPacket(Player *player);
	void showPlayer(Player *player);
	void showPlayers(Player *player, vector <Player*> players);
	void removePlayer(Player *player);
	void changeMap(Player *player);
	void portalBlocked(Player *player); // Blocked portals
	void showClock(Player *player, unsigned char hour, unsigned char min, unsigned char sec);
	void showTimer(Player *player, int sec); // Show Timer like in PQ
	void makeApple(Player *player);
	void changeMusic(int mapid, const string &musicname); // Change map music
	void sendSound(int mapid, const string &soundname); // Send sound - clear/wrong/etc
	void sendEvent(int mapid, const string &eventname); // Send event - clear/wrong/etc
};

#endif
