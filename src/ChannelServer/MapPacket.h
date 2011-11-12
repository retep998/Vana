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

class Door;
class Mist;
class PacketCreator;
class Player;
struct Pos;
struct Kite;

namespace MapPacket {
	PacketCreator playerPacket(Player *player);
	void showPlayer(Player *player);
	void removePlayer(Player *player);
	void changeMap(Player *player);
	void portalBlocked(Player *player); // Blocked portals
	void showClock(Player *player, int8_t hour, int8_t min, int8_t sec);
	void showTimer(int32_t mapid, int32_t sec); // Show timer like in PQs
	void showTimer(Player *player, int32_t sec); // Show Timer like in PQs
	void forceMapEquip(Player *player);
	void showEventInstructions(int32_t mapid);
	void showMist(Player *player, Mist *mist);
	void spawnMist(int32_t mapid, Mist *mist);
	void removeMist(int32_t mapid, int32_t id);
	void instantWarp(Player *player, int8_t pid);
	void changeWeather(int32_t mapid, bool adminWeather, int32_t itemid = 0, const string &message = "");
	void changeWeatherPlayer(Player *player, bool adminWeather, int32_t itemid = 0, const string &message = "");
	void spawnDoor(Door *door);
	void showDoor(Player *player, Door *door, bool istown);
	void showDoorDisappear(Player *player, Door *door);
	void removeDoor(Door *door, bool displace);
	void spawnPortal(int32_t source, int32_t destination, const Pos &srcpos, const Pos &dstpos);
	void removePortal(int32_t source, int32_t destination);
	void spawnKite(Kite &kite);
	void spawnKite(Player *player, Kite &kite);
	void despawnKite(Kite &kite, int8_t reason);
};
