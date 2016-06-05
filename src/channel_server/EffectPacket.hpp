/*
Copyright (C) 2008-2016 Vana Development Team

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

#include "common/PacketBuilder.hpp"
#include "common/SplitPacketBuilder.hpp"
#include "common/Types.hpp"
#include <string>

namespace vana {
	namespace channel_server {
		class player;

		namespace packets {
			PACKET(play_music, const string &music);
			PACKET(send_event, const string &id);
			PACKET(send_effect, const string &effect);
			PACKET(play_portal_sound_effect);
			PACKET(send_field_sound, const string &sound);
			PACKET(send_minigame_sound, const string &sound);
			SPLIT_PACKET(send_mob_item_buff_effect, game_player_id player_id, game_item_id item_id);
		}
	}
}