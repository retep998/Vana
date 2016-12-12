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

#include "InterHelper.hpp"
#include "PacketBuilder.hpp"
#include "PlayerObjects.hpp"
#include "Types.hpp"
#include <string>
#include <vector>

class Player;
struct Rates;

namespace SyncPacket {
	namespace ConfigPacket {
		PACKET(scrollingHeader, const string_t &message);
		PACKET(resetRates);
		PACKET(modifyRates, const Rates &rates);
	}
	namespace PlayerPacket {
		PACKET(changeChannel, Player *info, channel_id_t channel);
		PACKET(connectableEstablished, player_id_t playerId);
		PACKET(connect, const PlayerData &player, bool firstConnect);
		PACKET(disconnect, player_id_t playerId);
		PACKET(updatePlayer, const PlayerData &player, update_bits_t flags);
	}
	namespace PartyPacket {
		PACKET(sync, int8_t type, player_id_t playerId, int32_t target = 0);
	}
	namespace BuddyPacket {
		PACKET(buddyInvite, player_id_t inviterId, player_id_t inviteeId);
		PACKET(acceptBuddyInvite, player_id_t inviteeId, player_id_t inviterId);
		PACKET(removeBuddy, player_id_t listOwnerId, player_id_t removalId);
		PACKET(readdBuddy, player_id_t listOwnerId, player_id_t buddyId);
	}
}