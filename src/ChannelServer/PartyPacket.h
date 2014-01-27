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

#include "Types.h"
#include <string>

class Party;
class Player;

namespace PartyPacket {
	namespace Errors {
		enum : int8_t {
			PlayerHasParty = 0x10,
			PartyFull = 0x11,
			DifferingChannel = 0x12
		};
	}

	auto error(Player *player, int8_t error) -> void;
	auto createParty(Player *packetTarget, Party *party) -> void;
	auto joinParty(Player *packetTarget, Party *party, const string_t &player) -> void;
	auto leaveParty(Player *packetTarget, Party *party, int32_t playerId, const string_t &name, bool kicked) -> void;
	auto invitePlayer(Player *packetTarget, Party *party, const string_t &inviter) -> void;
	auto disbandParty(Player *packetTarget, Party *party) -> void;
	auto setLeader(Player *packetTarget, Party *party, int32_t newLeader) -> void;
	auto silentUpdate(Player *packetTarget, Party *party) -> void;
}