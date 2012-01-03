/*
Copyright (C) 2008-2012 Vana Development Team

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
#include <vector>

using std::string;
using std::vector;

class Player;

namespace SyncPacket {
	namespace PlayerPacket {
		void changeChannel(Player *info, uint16_t channel);
		void connectableEstablished(int32_t playerId);
		void connect(Player *player);
		void disconnect(int32_t playerId);
		void updateLevel(int32_t playerId, int32_t level);
		void updateJob(int32_t playerId, int32_t job);
		void updateMap(int32_t playerId, int32_t map);
	}
	namespace PartyPacket {
		void sync(int8_t type, int32_t playerId, int32_t target = 0);
	}
	namespace BuddyPacket {
		void buddyInvite(int32_t playerId, int32_t inviteeId);
		void buddyOnline(int32_t playerId, const vector<int32_t> &players, bool online);
	}
}