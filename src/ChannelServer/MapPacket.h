/*
Copyright (C) 2008-2013 Vana Development Team

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
class PacketCreator;
class Player;

namespace MapPacket {
	PacketCreator playerPacket(Player *player);
	void showPlayer(Player *player);
	void removePlayer(Player *player);
	void changeMap(Player *player);
	void portalBlocked(Player *player); // Blocked portals
	void showClock(Player *player, int8_t hour, int8_t min, int8_t sec);
	void showTimer(int32_t mapId, const seconds_t &sec); // Show timer like in PQs
	void showTimer(Player *player, const seconds_t &sec); // Show timer like in PQs
	void forceMapEquip(Player *player);
	void showEventInstructions(int32_t mapId);
	void showMist(Player *player, Mist *mist);
	void spawnMist(int32_t mapId, Mist *mist);
	void removeMist(int32_t mapId, int32_t id);
	void instantWarp(Player *player, int8_t portalId);
	void changeWeather(int32_t mapId, bool adminWeather, int32_t itemId = 0, const string &message = "");
}