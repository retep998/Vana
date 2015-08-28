/*
Copyright (C) 2008-2015 Vana Development Team

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

namespace Vana {
	class MysticDoor;
	class Party;
	class Player;

	namespace Packets {
		namespace Party {
			namespace Errors {
				enum : int8_t {
					PlayerHasParty2 = 0x09,
					BeginnerCannotCreateParty = 0x0A,
					PlayerHasNotJoinedParty = 0x0D,
					PlayerHasParty = 0x10,
					PartyFull = 0x11,
					DifferingChannel = 0x12,
					UnableToFind = 0x13,
					CannotKickUserInMap = 0x19,
					MemberMustBeCloseToPassLeader = 0x1C,
					AutomaticLeaderPassFailed = 0x1D,
					MemberMustBeOnSameChannelToPassLeader = 0x1E,
					GmCannotCreateParty = 0x20,
					UnableToFind2 = 0x21,
				};
			}
			namespace InviteErrors {
				enum : int8_t {
					TargetBlockingInvitations = 0x15,
					TargetAlreadyInvitedToSomeParty = 0x16,
					TargetDeniedInvitation = 0x17,
				};
			}

			PACKET(error, int8_t error);
			PACKET(inviteError, int8_t error, const string_t &target);
			PACKET(customError, const string_t &error = "");
			PACKET(createParty, Vana::Party *party, Vana::Player *leader);
			PACKET(joinParty, map_id_t targetMapId, Vana::Party *party, const string_t &player);
			PACKET(leaveParty, map_id_t targetMapId, Vana::Party *party, player_id_t playerId, const string_t &name, bool kicked);
			PACKET(invitePlayer, Vana::Party *party, const string_t &inviter);
			PACKET(disbandParty, Vana::Party *party);
			PACKET(setLeader, Vana::Party *party, player_id_t newLeader);
			PACKET(silentUpdate, map_id_t targetMapId, Vana::Party *party);
			PACKET(updateParty, map_id_t targetMapId, Vana::Party *party);
			PACKET(updateDoor, uint8_t zeroBasedPlayerIndex, ref_ptr_t<MysticDoor> door);
		}
	}
}