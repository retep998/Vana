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

#include "Types.hpp"
#include <string>
#include <vector>

class Player;
struct Rates;

namespace SyncPacket {
	namespace ConfigPacket {
		auto scrollingHeader(const string_t &message) -> void;
		auto resetRates() -> void;
		auto modifyRates(const Rates &rates) -> void;
	}
	namespace PlayerPacket {
		auto changeChannel(Player *info, uint16_t channel) -> void;
		auto connectableEstablished(int32_t playerId) -> void;
		auto connect(Player *player) -> void;
		auto disconnect(int32_t playerId) -> void;
		auto updateLevel(int32_t playerId, int32_t level) -> void;
		auto updateJob(int32_t playerId, int32_t job) -> void;
		auto updateMap(int32_t playerId, int32_t map) -> void;
	}
	namespace PartyPacket {
		auto sync(int8_t type, int32_t playerId, int32_t target = 0) -> void;
	}
	namespace BuddyPacket {
		auto buddyInvite(int32_t inviterId, int32_t inviteeId) -> void;
		auto buddyOnline(int32_t playerId, const vector_t<int32_t> &players, bool online) -> void;
	}
}