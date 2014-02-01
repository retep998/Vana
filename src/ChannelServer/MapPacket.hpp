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

class Mist;
class Player;

namespace MapPacket {
	PACKET(playerPacket, Player *player);
	PACKET(removePlayer, int32_t playerId);
	PACKET(changeMap, Player *player);
	PACKET(portalBlocked);
	PACKET(showClock, int8_t hour, int8_t min, int8_t sec);
	PACKET(showTimer, const seconds_t &sec);
	PACKET(forceMapEquip);
	PACKET(showEventInstructions);
	PACKET(spawnMist, Mist *mist, bool mapEntry);
	PACKET(removeMist, int32_t id);
	PACKET(instantWarp, int8_t portalId);
	PACKET(changeWeather, bool adminWeather, int32_t itemId = 0, const string_t &message = "");
}