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
	namespace ChannelServer {
		class Player;

		namespace Packets {
			PACKET(playMusic, const string_t &music);
			PACKET(sendEvent, const string_t &id);
			PACKET(sendEffect, const string_t &effect);
			PACKET(playPortalSoundEffect);
			PACKET(sendFieldSound, const string_t &sound);
			PACKET(sendMinigameSound, const string_t &sound);
			SPLIT_PACKET(sendMobItemBuffEffect, player_id_t playerId, item_id_t itemId);
		}
	}
}