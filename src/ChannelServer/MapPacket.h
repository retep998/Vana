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

#include "Types.h"
#include <string>

using std::string;

class Mist;
class Player;
class PacketCreator;

namespace MapPacket {
	PacketCreator playerPacket(Player *player);
	void showPlayer(Player *player);
	void removePlayer(Player *player);
	void changeMap(Player *player);
	void portalBlocked(Player *player); // Blocked portals
	void showClock(Player *player, uint8_t hour, uint8_t min, uint8_t sec);
	void showTimer(int32_t mapid, int32_t sec); // Show Timer like in PQ
	void showTimer(Player *player, int32_t sec); // Show Timer like in PQ
	void forceMapEquip(Player *player);
	void setMusic(int32_t mapid, const string &musicname); // Set map music
	void sendSound(int32_t mapid, const string &soundname); // Send sound - clear/wrong/etc
	void sendEvent(int32_t mapid, const string &eventname); // Send event - clear/wrong/etc
	void sendEffect(int32_t mapid, const string &effectname); // Send effect - gate/etc
	void playPortalSoundEffect(Player *player);
	void showEventInstructions(int32_t mapid);
	void showMist(Player *player, Mist *mist);
	void spawnMist(int32_t mapid, Mist *mist);
	void removeMist(int32_t mapid, int32_t id);
	void instantWarp(Player *player, int8_t pid);
}
