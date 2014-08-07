/*
Copyright (C) 2008-2014 Vana Development Team

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

#include "PacketBuilder.hpp"
#include "Types.hpp"
#include <string>

struct Point;
class Mist;
class MysticDoor;
class Player;

namespace MapPacket {
	PACKET(playerPacket, Player *player);
	PACKET(removePlayer, player_id_t playerId);
	PACKET(changeMap, Player *player, bool spawnByPosition, const Point &spawnPosition);
	PACKET(portalBlocked);
	PACKET(showClock, int8_t hour, int8_t min, int8_t sec);
	PACKET(showTimer, const seconds_t &sec);
	PACKET(forceMapEquip);
	PACKET(showEventInstructions);
	PACKET(spawnMist, Mist *mist, bool mapEntry);
	PACKET(removeMist, map_object_t id);
	PACKET(spawnDoor, ref_ptr_t<MysticDoor> door, bool isInsideTown, bool alreadyOpen);
	PACKET(removeDoor, ref_ptr_t<MysticDoor> door, bool isFade);
	PACKET(spawnPortal, ref_ptr_t<MysticDoor> door, map_id_t callingMap);
	PACKET(removePortal);
	PACKET(instantWarp, portal_id_t portalId);
	PACKET(boatDockUpdate, bool docked, int8_t shipKind);
	PACKET(changeWeather, bool adminWeather, item_id_t itemId = 0, const string_t &message = "");
}