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

#include "Common/PacketBuilder.hpp"
#include "Common/SplitPacketBuilder.hpp"
#include "Common/Types.hpp"
#include <string>

namespace Vana {
	class Item;
	class PacketBuilder;

	namespace ChannelServer {
		class Pet;
		class Player;

		namespace Packets {
			namespace Pets {
				SPLIT_PACKET(petSummoned, player_id_t playerId, Pet *pet, bool kick = false, int8_t index = -1);
				SPLIT_PACKET(showChat, player_id_t playerId, Pet *pet, const string_t &message, int8_t act);
				SPLIT_PACKET(showMovement, player_id_t playerId, Pet *pet, unsigned char *buf, int32_t bufLen);
				PACKET(showAnimation, player_id_t playerId, Pet *pet, int8_t animation);
				PACKET(updatePet, Pet *pet, Item *petItem);
				SPLIT_PACKET(levelUp, player_id_t playerId, Pet *pet);
				SPLIT_PACKET(changeName, player_id_t playerId, Pet *pet);
				// TODO FIXME packet
				// This doesn't appear to be used anywhere, not sure if that's by mistake or not
				//auto showPet(ref_ptr_t<Player> player, Pet *pet) -> void;
				PACKET(updateSummonedPets, ref_ptr_t<Player> player);
				PACKET(blankUpdate);
				PACKET(addInfo, Pet *pet, Item *petItem);
			}
		}
	}
}